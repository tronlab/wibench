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
#include "TurboEncoder_c.h"
extern fsm FSM;
extern Interleaver_lte<int,int> EInter;

TurboEncoder_c::TurboEncoder_c(UserPara* pUser)
{
PSFlag = (*pUser).ProcessingStatusFlag;
BufFlag=(*pUser).BufferSizeFlag;
Rate=3;
DataLength=(*pUser).DataLength;
if((DataLength%6144)==0){NumBlock=DataLength/6144;}
else{NumBlock=(DataLength-(DataLength%6144))/6144+1;}
pLengthSet=new int[NumBlock];
for(int nblock=0;nblock<NumBlock-1;nblock++)
{
 *(pLengthSet+nblock)=6144;
}
if(NumBlock==1){*(pLengthSet+NumBlock-1)=DataLength;}
else{*(pLengthSet+NumBlock-1)=DataLength%6144;}

piSeq=new int[DataLength];
pcSeq=new int*[Rate];
for(int r=0;r<Rate;r++)
{
 *(pcSeq+r)=new int[((NumBlock-1)*(6144+4)+1*((*(pLengthSet+NumBlock-1))+4))];
}
///////////////Calc In/Out buffer size/////////////////
InBufSz[0]=1;    InBufSz[1] = DataLength;
OutBufSz[0]=Rate;OutBufSz[1]= ((NumBlock-1)*(6144+4)+1*((*(pLengthSet+NumBlock-1))+4));
if(BufFlag)
{
  cout<<"TurboEncoder"<<endl;
  cout<<"Input buffer size is "<<"{"<<InBufSz[0]<<" , "<<InBufSz[1]<<"}"<<"  int"<<endl;
  cout<<"Output buffer size is"<<"{"<<OutBufSz[0]<<" , "<<OutBufSz[1]<<"}"<<"  int"<<endl; 
}
else
{}
////////////////End clac in/out buffer size///////////////

}


int TurboEncoder_c::maxNum(int a,int b)
{if(a>=b){return a;}else{return b;}}

int TurboEncoder_c::maxElen(int num,int *p,int pos,int space)
{
        int mamm = *(p+0);
        for(int i=0;i<num;i++)
        {mamm = maxNum(mamm,*(p+i*space+pos));}
        return mamm;
}

void TurboEncoder_c::TurboEncoding(FIFO<int> *pInpBuf, FIFO<int> *pOutBuf)
{
if(PSFlag)
{cout<<"TurboEncoding"<<endl;}

bool ReadFlag = (*pInpBuf).Read(piSeq);
if(ReadFlag)
{
/////////////////////////////////Start encoding////////////////////////////////

 for(int nBlock=0;nBlock<NumBlock;nBlock++)
 {
  iSeqLength=*(pLengthSet+nBlock);
  InpBlockShift=nBlock*6144;
  OutBlockShift=nBlock*(6144+4);
  ////////////////// Start Encoding this block///////////////////

        int cs;         //current state
        int ns;         //next state
        int isym;       //input symbol
        int osym;       //output symbol

        for(int i=0;i<iSeqLength;i++)
        {
                *(*(pcSeq+0)+OutBlockShift+i) = *(piSeq+InpBlockShift+i);
        }

        //********** rsc1 **********//

        cs=0;   //start from 0 state
        for(int i=0;i<iSeqLength;i++)
        {
                isym = *(piSeq+InpBlockShift+i);
                ns  = *(FSM.pNS()+((FSM.I())*cs+isym));
                osym = *(FSM.pOS()+((FSM.I())*cs+isym));
                *(*(pcSeq+1)+OutBlockShift+i)=osym; 
                cs = ns;
        }

        //**********Adding termination**********//
        int ends = cs; // ending state of Turbo saved in ends
        int maxLen = maxElen(FSM.S(),FSM.pTMl(),0,FSM.S());
        int *ptaili = new int[maxLen];
        for(int i=0;i<maxLen;i++)
        {*(ptaili+i)=0;}

        int numidx=0;
        while(cs!=0&&numidx<maxLen)
        {
                *(ptaili+numidx)=*(FSM.pTMi()+cs*FSM.S()+0);

                cs = *(FSM.pNS()+((FSM.I())*cs+(*(ptaili+numidx))));

                numidx++;
        }
        cs = ends; //start from last state to zero-state
        
        int *pT1=new int[6];
        for(int i=0;i<maxLen;i++)
        {
                isym = *(ptaili+i);
                 ns  = *(FSM.pNS()+((FSM.I())*cs+isym));
                osym = *(FSM.pOS()+((FSM.I())*cs+isym));
                *(pT1+2*i+0)=isym;
                *(pT1+2*i+1)=osym;
                cs = ns;
        }

        for(int i=0;i<2;i++)
        {
            *(*(pcSeq+0)+OutBlockShift+iSeqLength+i) =  *(pT1+i*Rate+0);
            *(*(pcSeq+1)+OutBlockShift+iSeqLength+i) =  *(pT1+i*Rate+1);
            *(*(pcSeq+2)+OutBlockShift+iSeqLength+i) =  *(pT1+i*Rate+2);      
        }        

        delete[] pT1;
        int MaxLen1 = maxLen;
        delete[] ptaili;
        cs=-1;ns=-1;isym=-1;osym=-1;
        //*********END rsc1*********//

        //********* piSeq INTERLEAVING => piSeq2 *********//
        int *piSeq2 = new int[iSeqLength];
        EInter.Interleaving(iSeqLength,(piSeq+InpBlockShift),piSeq2);
        //******* END piSeq INTERLEAVING => piSeq2 *******//

        //********** rsc2 **********//

        cs = 0; //start from 0 state
        for(int i=0;i<iSeqLength;i++)
        {
                isym = *(piSeq2+i);
                ns  = *(FSM.pNS()+((FSM.I())*cs+isym));
                osym = *(FSM.pOS()+((FSM.I())*cs+isym));
                *(*(pcSeq+2)+OutBlockShift+i)=osym;
                cs = ns;
        }

        delete[] piSeq2;
        //**********Adding termination**********//
        ends = cs;
        maxLen = maxElen(FSM.S(),FSM.pTMl(),0,FSM.S());
        int *ptail2i = new int[maxLen];
        for(int i=0;i<maxLen;i++)
        {*(ptail2i+i)=0;}
        numidx=0;
        while(cs!=0&&numidx<maxLen)
        {
                *(ptail2i+numidx)=*(FSM.pTMi()+cs*FSM.S()+0);
                cs = *(FSM.pNS()+((FSM.I())*cs+(*(ptail2i+numidx))));
                numidx++;
        }
        cs = ends; //start from last state to zero-state
       
        int *pT2=new int[6];
        for(int i=0;i<maxLen;i++)
        {
                isym = *(ptail2i+i);
                ns  = *(FSM.pNS()+((FSM.I())*cs+isym));
                osym = *(FSM.pOS()+((FSM.I())*cs+isym));
                *(pT2+2*i+0)=isym;
                *(pT2+2*i+1)=osym;
                cs = ns;
        }
 
        for(int i=0;i<2;i++)
        {
            *(*(pcSeq+0)+OutBlockShift+iSeqLength+2+i) =  *(pT2+i*Rate+0);
            *(*(pcSeq+1)+OutBlockShift+iSeqLength+2+i) =  *(pT2+i*Rate+1);
            *(*(pcSeq+2)+OutBlockShift+iSeqLength+2+i) =  *(pT2+i*Rate+2);      
        }  
      
        delete[] pT2;
        delete[] ptail2i;
        //*********END rsc2*********//

  //////////////////End Encoding this Block//////////////////////
 }

///////////////////////////////// End encoding ///////////////////////////////////
bool WriteFlag = (*pOutBuf).Write(pcSeq);
if(PSFlag)
{cout<<"TurboEncode completed"<<endl;}

}
else
{cout<<"Unable to read input buffer"<<endl;}
}

TurboEncoder_c::~TurboEncoder_c()
{
delete[] pLengthSet;
delete[] piSeq;
for(int r=0;r<Rate;r++){delete[] *(pcSeq+r);}
delete[] pcSeq;
}
