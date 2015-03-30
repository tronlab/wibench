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
#include "SCFDMADemodulation.h"

SCFDMADemodulation::SCFDMADemodulation(BSPara* pBS)
{
PSFlag = (*pBS).ProcessingStatusFlag;
BufFlag=(*pBS).BufferSizeFlag;
NumULSymbSF=(*pBS).NumULSymbSF;

NIFFT=(*pBS).NIFFT;
CPLen=(*pBS).CPLen;
NumRxAntenna=(*pBS).NumRxAntenna;

SymNum = NumULSymbSF*NumRxAntenna;
pInpData=new complex<float>*[SymNum];
pOutData=new complex<float>*[SymNum];
for(int s=0;s<SymNum;s++)
{
  *(pInpData+s)=new complex<float>[(NIFFT+CPLen)];
  *(pOutData+s)=new complex<float>[NIFFT];
}


complex<float> *in,*out;
fftplan=fftwf_plan_dft_1d(NIFFT,reinterpret_cast<fftwf_complex*>(in),reinterpret_cast<fftwf_complex*>(out),FFTW_FORWARD,FFTW_ESTIMATE);

//////////////////////// Calc In/Out buffer size//////////////////////////
InBufSz[0]=NumRxAntenna*NumULSymbSF;InBufSz[1]=(NIFFT+CPLen);
OutBufSz[0]=NumULSymbSF*NumRxAntenna; OutBufSz[1]=NIFFT;
if(BufFlag)
{
  cout<<"SCFDMA Demodulation"<<endl;
  cout<<"Input buffer size is "<<"{"<<InBufSz[0]<<" , "<<InBufSz[1]<<"}"<<"  complex<float>"<<endl;
  cout<<"Output buffer size is "<<"{"<<OutBufSz[0]<<" , "<<OutBufSz[1]<<"}"<<"  complex<float>"<<endl; 
}
else
{}
//////////////////////End of clac in/out buffer size//////////////////////

}

void SCFDMADemodulation::SCFDMADemodulating(FIFO<complex<float> >* pInpBuf,FIFO<complex<float> >* pOutBuf)
{
   if(PSFlag)
   {cout<<"SCFDMADemodulating"<<endl;}
   bool ReadFlag = (*pInpBuf).Read(pInpData);
   if(ReadFlag)
   {
       for(int nrx=0;nrx<NumRxAntenna;nrx++)
       {

           for(int nsym=0;nsym<NumULSymbSF;nsym++)
           {

           int SymIdx = nrx*NumULSymbSF+nsym;

           fftwf_execute_dft(fftplan,reinterpret_cast<fftwf_complex*>((*(pInpData+SymIdx)+CPLen)),reinterpret_cast<fftwf_complex*>((*(pOutData+SymIdx))));
              for(int n=0;n<NIFFT;n++)
              {(*(*(pOutData+SymIdx)+n))/=sqrt((float)NIFFT);}     
           }
       }

       bool WriteFlag = (*pOutBuf).Write(pOutData);
   

 //      if(WriteFlag){cout<<"successfully written!"<<endl;}   
     if(PSFlag)
     {cout<<"SCFDMADemodulation completed"<<endl;} 
   }
   else
   {cout<<"fail to read data from previous buffer"<<endl;}

}

SCFDMADemodulation::~SCFDMADemodulation()
{
fftwf_destroy_plan(fftplan);
   for(int s=0;s<SymNum;s++)
   {
    delete[] *(pInpData+s);
    delete[] *(pOutData+s);
   }
delete[] pInpData;
delete[] pOutData;
}
