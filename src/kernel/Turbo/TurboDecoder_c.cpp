/*
 * Copyright (c) 2013 The Regents of The University of Michigan
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met: redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer;
 * redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution;
 * neither the name of the copyright holders nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Authors: Yajing Chen
 *          Achilleas Anastasopoulos 
 *          Qi Zheng
 */
#include "TurboDecoder_c.h"
extern fsm FSM;

extern Interleaver_lte<int,float> DInter;

static const float INF = 1.0e9;
float min(float a,float b){return a <= b ? a : b; }
float min_star(float a,float b){return ( a<= b ? a : b)-log(1+exp(a <= b ? a-b : b-a));}


TurboDecoder_c::TurboDecoder_c(BSPara* pBS)
{
PSFlag = (*pBS).ProcessingStatusFlag;
BufFlag=(*pBS).BufferSizeFlag;
DataLength = (*pBS).DataLengthPerUser;

Rate=3;

/////////////////////// Init Parameters ///////////////////////
ST10 = 0;ST1K = 0;
ST20 = 0;ST2K = 0;
iterations = 5;

p2mymin= min;
D0 = 1;D1 = 1;D2 = 1;

/////////////////////END Init Parameters///////////////////////
  LastBlockLen=(DataLength%6144);
  NumBlock=(DataLength-LastBlockLen)/6144+1;
  pLengthSet=new int[NumBlock];
  for(int nBlock=0;nBlock<NumBlock-1;nBlock++)
  {
   *(pLengthSet+nBlock)=6144;
  }
  *(pLengthSet+NumBlock-1)=LastBlockLen;

  pLLR = new float[(NumBlock-1)*(6144*Rate+12)+1*(LastBlockLen*Rate+12)];
  pData = new int[DataLength];

//////////////////////// Calc In/Out buffer size//////////////////////////
InBufSz[0]=1;InBufSz[1]=(NumBlock-1)*(6144*Rate+12)+1*(LastBlockLen*Rate+12);
OutBufSz[0]=1;OutBufSz[1]=DataLength;
if(BufFlag)
{
  cout<<"Turbo Decoder"<<endl;
  cout<<"Input buffer size is "<<"{"<<InBufSz[0]<<" , "<<InBufSz[1]<<"}"<<"  float"<<endl;
  cout<<"Output buffer size is "<<"{"<<OutBufSz[0]<<" , "<<OutBufSz[1]<<"}"<<"  int"<<endl; 
}
else
{}
//////////////////////End of clac in/out buffer size//////////////////////

}

//***************** siso function *****************//
void TurboDecoder_c::siso_algorithm_bb(int I,int S,int O,
                                        int *pNS,
                                        int *pOS,
                                        int **pPS,
                                        int **pPI,
                                        int K,
                                        int S0,int SK,
                                        bool POSTI,bool POSTO,
                                        float (*p2mymin)(float,float),
                                        float *priori,float *prioro,float *post)
{
        float norm,mm,minm;
        float *alpha = new float[S*(K+1)];
        float *beta  = new float[S*(K+1)];

        //***********Initial State Setting***********//
        if(S0<0)
        {for(int i=0;i<S;i++){*(alpha+0*S+i)=0.0;}}
        else
        {
                for(int i=0;i<S;i++){*(alpha+0*S+i)=INF;}
                *(alpha+0*S+S0)=0.0;
        }

        //***********forward recursion***********//
        for(int k=0;k<K;k++)
        {
                norm = INF;
                for(int j=0;j<S;j++)
                {
                        minm = INF;
                        for(int i=0;i<I;i++)
                        {
                                int inp_v=(*(*(pPI+j)+i));
                                int outp_v=(*(pOS+((*(*(pPS+j)+i))*I+(inp_v))));
                                mm = *(alpha+(k*S+(*(*(pPS+j)+i)))) + (1-inp_v)*(*(priori+(k))) + (1-outp_v)*(*(prioro+(k)));
                                minm = (*p2mymin)(minm,mm);
                        }
                        *(alpha+(k+1)*S+j) = minm;
                        if(minm<norm) {norm=minm;}
                }
                for(int j=0;j<S;j++)
                {
                        *(alpha+(k+1)*S+j) -= norm;
                }
        }

        //***********final state setting***********//
        if(SK<0)
        {for(int i=0;i<S;i++){*(beta+K*S+i)=0.0;}}
        else
        {for(int i=0;i<S;i++){*(beta+K*S+i)=INF;}
         *(beta+K*S+SK)=0.0;
        }
        //***********backward recursion***********//
        for(int k=K-1;k>=0;k--)
        {
                norm = INF;
                for(int j=0;j<S;j++)
                {
                        minm = INF;
                        for(int i=0;i<I;i++)
                        {
                                int i0 = j*I+i;
                                mm = (*(beta+((k+1)*S+(*(pNS+i0)))))+(1-i)*(*(priori+k))+(1-(*(pOS+i0)))*(*(prioro+k));
                                minm = (*p2mymin)(minm,mm);
                        }
                        *(beta+k*S+j)=minm;
                        if(minm<norm) norm = minm;
                }
                for(int j=0;j<S;j++)
                {*(beta+k*S+j) -= norm;}
        }

        //*********** Input Output Combining ***********//
        if(POSTI)
        {
                //*********** Only Input Combining ***********//
                for(int k=0;k<K;k++)
                {
                        norm = INF;
                        float m_tmp[2];
                        for(int i=0;i<I;i++)
                        {
                                minm = INF;
                                for(int j=0;j<S;j++)
                                {
                                        int oo=(*(pOS+j*I+i));
                                        mm = *(alpha+k*S+j)+(1-oo)*(*(prioro+k))+(*(beta+(k+1)*S+(*(pNS+j*I+i))));
                                        minm = (*p2mymin)(minm,mm);
                                }
                                m_tmp[i]=minm;
                        
                                if(minm<norm) norm = minm;
                        }
                        for(int i=0;i<I;i++)
                        {
                                m_tmp[i]-=norm;
                                
                        }
                        *(post+k)=m_tmp[0]-m_tmp[1];
                }
        }
        //********** Delete []**********//
        delete[] alpha;
        delete[] beta;
}


//**************END siso function *****************//


//*************** pccc function ***************//

void TurboDecoder_c::pccc_decoder_bb(
                                        const fsm &FSM,
                                        int ST10, int ST1K,
                                        int ST20, int ST2K,
                                        int D0,
                                        int D1,
                                        int D2,
                                        Interleaver_lte<int,float> &DInter, int blocklength, int iterations,
                                        float (*p2mymin)(float,float),
                                        float *cprioro, int *data)
{

        //********* Allocate space for priori0 *****************//
        float *priori0= new float[(blocklength)];
        //********* Allocate space for priori, prioro and posti of FSM1
        float *priori1 = new float[(blocklength+3)];
        float *prioro1 = new float[(blocklength+3)];
        float *posti1 = new float[(blocklength+3)];
        float *priori1tail = new float[3];
        //********* Allocate space for priori, prioro and posti of FSM2
        float *priori2 = new float[(blocklength+3)];
        float *prioro2 = new float[(blocklength+3)];
        float *posti2 = new float[(blocklength+3)];
        float *priori2tail = new float[3];

        for(int k=0;k<blocklength;k++)
        {
                *(priori0+k)=*(cprioro+k*(D0+D1+D2));
                *(prioro1+k)=*(cprioro+k*(D0+D1+D2)+D0);
                *(prioro2+k)=*(cprioro+k*(D0+D1+D2)+D0+D1);
        }

        for(int k=blocklength;k<blocklength+3;k++)
        {
                *(priori1tail+k-blocklength)=*(cprioro+blocklength*(D0+D1+D2)+(k-blocklength)*(D0+D1));
                *(prioro1+k)=*(cprioro+blocklength*(D0+D1+D2)+(k-blocklength)*(D0+D1)+D1);
                *(priori2tail+k-blocklength)=*(cprioro+blocklength*(D0+D1+D2)+3*2+(k-blocklength)*(D0+D2));
                *(prioro2+k)=*(cprioro+blocklength*(D0+D1+D2)+3*2+(k-blocklength)*(D0+D2)+D2);
        }
        //*************** END generate prioro1 and prioro2********************//
        //*********** Initialize priori1 and priori2 ***********//
        for(int i = 0;i<(blocklength);i++)
        {
                *(priori1+i)=*(priori0+i);
        }
        for(int i = blocklength;i<(blocklength+3);i++)
        {
                *(priori1+i)=*(priori1tail+i-blocklength);
        }

        //**********END Initialize priori1 and priori2 **********//

        //***************START iterations***************//
        for(int iter=0;iter<iterations;iter++)
        {
                //*********** siso1 ***********//
                siso_algorithm_bb(FSM.I(),FSM.S(),FSM.O(),
                                        FSM.pNS(),
                                        FSM.pOS(),
                                        FSM.pPS(),
                                        FSM.pPI(),
                                        blocklength+3,
                                        ST10,ST1K,
                                        true,false,
                                        p2mymin,
                                        priori1,prioro1,posti1);

                //********* END siso1 *********//
                //********* posti1 INTERLEAVING => priori2 *********//
                //********* posti1 + priori0 INTERLEAVING => priori2 *********//
                float *ptmp = new float[blocklength];
                float *pItmp= new float[blocklength];

                        for(int k=0;k<blocklength;k++)
                        {*(ptmp+k) = *(posti1+k)+*(priori0+k);}
                        DInter.Interleaving(blocklength,ptmp,pItmp);
                        for(int k=0;k<blocklength;k++)
                        {*(priori2+k) = *(pItmp+k);}
                delete[] ptmp;
                delete[] pItmp;

                //*********Tail bits do not be interleaved*********//

                        for(int k=blocklength;k<(blocklength+3);k++)
                        { *(priori2+k)=*(priori2tail+(k-blocklength));/**(posti1+(FSM1.I())*k+i);*/}
                //********END posti1 INTERLEAVING=> priori2 ********//
                //*********** siso2 ***********//
                siso_algorithm_bb(FSM.I(),FSM.S(),FSM.O(),
                                        FSM.pNS(),
                                        FSM.pOS(),
                                        FSM.pPS(),
                                        FSM.pPI(),
                                        blocklength+3,
                                        ST20,ST2K,
                                        true,false,
                                        p2mymin,
                                        priori2,prioro2,posti2);
                //********* END siso2 *********//
                //********* posti2 DEINTERLEAVING + priori0=> priori1 *********//
                float *pt = new float[blocklength];
                float *pDt= new float[blocklength];

                        for(int k=0;k<blocklength;k++)
                        {*(pt+k) = *(posti2+k);}
                        DInter.DeInterleaving(blocklength,pt,pDt);

                        for(int k=0;k<blocklength;k++)
                        {*(priori1+k)=(*(pDt+k))+(*(priori0+k));}
                delete[] pt;
                delete[] pDt;
                //*********Tail bits do not be deinterleaved*********//
                        for(int k=blocklength;k<(blocklength+3);k++)
                        {*(priori1+k)=*(priori1tail+k-blocklength);}
                //********END posti2 DEINTERLEAVING=> priori1 ********//
        }
        //*************** END iterations ***************//

        //*********** Generate Hard Decisions ***********//
        for(int k=0;k<blocklength;k++)
        {
               float mini;                

               (*(posti1+k)) = (*(priori1+k))+(*(posti1+k));
                
                if((*(posti1+k))<0)
                {mini=0;}
                else
                {mini=1;}
                (*(data+k))=(int)mini;
        }
        //**********END Generate Hard Decisions *********//

        //********** Delete [] ***********//
        delete[] priori0;
        delete[] priori1;
        delete[] prioro1;
        delete[] posti1;
        delete[] priori2;
        delete[] prioro2;
        delete[] posti2;
        delete[] priori1tail;
        delete[] priori2tail;


}


//**************END pccc function**************//


void TurboDecoder_c::TurboDecoding(FIFO<float>* pInpBuf,FIFO<int>* pOutBuf)
{
if(PSFlag)
{cout<<"TurboDecoding"<<endl;}

  bool ReadFlag = (*pInpBuf).Read(pLLR);
  if(ReadFlag)
  {

    for(int nBlock=0;nBlock<NumBlock;nBlock++)
    {
     iSeqLength=*(pLengthSet+nBlock); 
     InpBlockShift=nBlock*(6144*Rate+12);
     OutBlockShift=nBlock*6144;

     ////////////////////// Start Decoding this block ///////////////////////
        pccc_decoder_bb(
                        FSM,
                        ST10, ST1K,
                        ST20, ST2K,
                        D0,
                        D1,
                        D2,
                        DInter, iSeqLength, iterations,
                        p2mymin,
                        (pLLR+InpBlockShift),(pData+OutBlockShift));
     //////////////////////END Start Decoding this block ///////////////////////
    }

    bool WriteFlag = (*pOutBuf).Write(pData);
 //   if(WriteFlag){cout<<"successfully written!"<<endl;}else{}
    if(PSFlag)
    {cout<<"TurboDecode completed"<<endl;}
  }
  else
  {cout<<"fail to read data from previous buffer"<<endl;}

}


TurboDecoder_c::~TurboDecoder_c()
{
  delete[] pLLR;
  delete[] pData;
  delete[] pLengthSet;

}
