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
#include "TransformPrecoder.h"

TransformPrecoder::TransformPrecoder(UserPara* pUser)
{
PSFlag = (*pUser).ProcessingStatusFlag;
BufFlag=(*pUser).BufferSizeFlag;
QAMLen=(*pUser).QAMLen;
NumULSymbSF=(*pUser).NumULSymbSF;
MDFT=(*pUser).MDFT;
NumLayer=(*pUser).NumLayer;

pQAMSeq = new complex<float>[NumLayer*QAMLen];
pDataMatrix=new complex<float>*[NumLayer*(NumULSymbSF-2)];
for(int nsym=0;nsym<(NumULSymbSF-2)*NumLayer;nsym++)
{*(pDataMatrix+nsym)=new complex<float>[MDFT];}
complex<float> *in,*out;
fftplan=fftwf_plan_dft_1d(MDFT,reinterpret_cast<fftwf_complex*>(in),reinterpret_cast<fftwf_complex*>(out),FFTW_FORWARD,FFTW_ESTIMATE);

//////////////////////// Calc In/Out buffer size//////////////////////////
InBufSz[0]=1; InBufSz[1]=NumLayer*QAMLen;
OutBufSz[0]=NumLayer*(NumULSymbSF-2); OutBufSz[1]=MDFT;
if(BufFlag)
{
  cout<<"Transform Precoder"<<endl;
  cout<<"Input buffer size is "<<"{"<<InBufSz[0]<<" , "<<InBufSz[1]<<"}"<<"  complex<float>"<<endl;
  cout<<"Output buffer size is "<<"{"<<OutBufSz[0]<<" , "<<OutBufSz[1]<<"}"<<"  complex<float>"<<endl; 
}
else
{}
//////////////////////End of clac in/out buffer size//////////////////////


}

void TransformPrecoder::TransformPrecoding(FIFO<complex<float> > *pInpBuf, FIFO<complex<float> > *pOutBuf) 
{

if(PSFlag)
{cout<<"TransformPrecoding"<<endl;}
bool ReadFlag = (*pInpBuf).Read(pQAMSeq);
if(ReadFlag)
{
///////////// Start transform precoding /////////////////
for(int nlayer=0;nlayer<NumLayer;nlayer++)
{
    for(int nsym=0;nsym<NumULSymbSF-2;nsym++)
    {

       int idx = nlayer*(MDFT*(NumULSymbSF-2))+nsym*MDFT;
       int symIdx=nlayer*(NumULSymbSF-2)+nsym;

      fftwf_execute_dft(fftplan,reinterpret_cast<fftwf_complex*>((pQAMSeq+idx)),reinterpret_cast<fftwf_complex*>((*(pDataMatrix+symIdx))));
       for(int m=0;m<MDFT;m++)
       {(*(*(pDataMatrix+symIdx)+m))/=sqrt((float)MDFT);}
    }
}

bool WriteFlag = (*pOutBuf).Write(pDataMatrix);

if(PSFlag)
{cout<<"TransformPrecode completed"<<endl;}


//////////////End transform precoding////////////////////
}
else
{cout<<"unable to read input buffer"<<endl;}

}

TransformPrecoder::~TransformPrecoder()
{
delete[] pQAMSeq;
for(int nsym=0;nsym<(NumULSymbSF-2)*NumLayer;nsym++){delete[] *(pDataMatrix+nsym);}
delete[] pDataMatrix;
fftwf_destroy_plan(fftplan);

}
