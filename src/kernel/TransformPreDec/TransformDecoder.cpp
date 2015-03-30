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
 *          Qi Zheng
 */
#include "TransformDecoder.h"

TransformDecoder::TransformDecoder(BSPara* pBS)
{
PSFlag = (*pBS).ProcessingStatusFlag;
BufFlag=(*pBS).BufferSizeFlag;

NumULSymbSF=(*pBS).NumULSymbSF;
QAM=(*pBS).MQAMPerUser;
MDFT=(*pBS).MDFTPerUser;
NumLayer=(*pBS).NumLayerPerUser;

SymbNum=(NumULSymbSF-2)*(NumLayer);
QAMLen = MDFT*(NumULSymbSF-2);

pDataMatrix=new complex<float>*[SymbNum];
for(int s=0;s<SymbNum;s++){*(pDataMatrix+s)=new complex<float>[MDFT];}
pDecQAMSeq = new complex<float>[QAMLen*NumLayer];  

complex<float> *in,*out;     
ifftplan=fftwf_plan_dft_1d(MDFT,reinterpret_cast<fftwf_complex*>(in),reinterpret_cast<fftwf_complex*>(out),FFTW_BACKWARD,FFTW_ESTIMATE);

//////////////////////// Calc In/Out buffer size//////////////////////////
InBufSz[0]=SymbNum;InBufSz[1]=MDFT;
OutBufSz[0]=1;OutBufSz[1]=QAMLen*NumLayer;
if(BufFlag)
{
  cout<<"Transform Decoder"<<endl;
  cout<<"Input buffer size is "<<"{"<<InBufSz[0]<<" , "<<InBufSz[1]<<"}"<<"  complex<float>"<<endl;
  cout<<"Output buffer size is "<<"{"<<OutBufSz[0]<<" , "<<OutBufSz[1]<<"}"<<"  complex<float>"<<endl; 
}
else
{}
//////////////////////End of clac in/out buffer size//////////////////////

}

void TransformDecoder::TransformDecoding(FIFO<complex<float> >* pInpBuf,FIFO<complex<float> >* pOutBuf)
{
 
if(PSFlag)
{cout<<"TransformDecoding"<<endl;} 
    bool ReadFlag = (*pInpBuf).Read(pDataMatrix);
    if(ReadFlag)
    {

        for(int nlayer=0;nlayer<NumLayer;nlayer++)
        {
            for(int nsym=0;nsym<NumULSymbSF-2;nsym++)
            {
                int idx = nlayer*(MDFT*(NumULSymbSF-2))+nsym*MDFT;
                int symIdx=nlayer*(NumULSymbSF-2)+nsym;
               
               fftwf_execute_dft(ifftplan,reinterpret_cast<fftwf_complex*>((*(pDataMatrix+symIdx))),reinterpret_cast<fftwf_complex*>((pDecQAMSeq+idx)));

               for(int m=0;m<MDFT;m++)
               {*(pDecQAMSeq+idx+m)=(*(pDecQAMSeq+idx+m))/sqrt((float)MDFT);}                              
            }
        }
        bool WriteFlag = (*pOutBuf).Write(pDecQAMSeq);
 //       if(WriteFlag){cout<<"successfully written!"<<endl;}else{}
        if(PSFlag)
        {cout<<"TransformDecode completed"<<endl;}
    }
    else
    {cout<<"fail to read data from previous buffer"<<endl;}
    
}

TransformDecoder::~TransformDecoder()
{
    fftwf_destroy_plan(ifftplan);
    delete[] pDecQAMSeq;
    for(int s=0;s<SymbNum;s++){delete[] *(pDataMatrix+s);}
    delete[] pDataMatrix;
}
