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
#include "SCFDMAModulation.h"

SCFDMAModulation::SCFDMAModulation(UserPara* pUser)
{
PSFlag= (*pUser).ProcessingStatusFlag;
BufFlag=(*pUser).BufferSizeFlag;
NIFFT = (*pUser).NIFFT;
CPLen = (*pUser).CPLen;
NumULSymbSF=(*pUser).NumULSymbSF;
NumLayer=(*pUser).NumLayer;

pInpData = new complex<float>* [NumLayer*NumULSymbSF];
pOutData = new complex<float>* [NumLayer*NumULSymbSF];

for(int nsym = 0; nsym<NumLayer*NumULSymbSF;nsym++)
{
    *(pInpData+nsym)=new complex<float>[NIFFT];
    *(pOutData+nsym)=new complex<float>[(NIFFT+CPLen)];
}

complex<float> *in;
out = new complex<float>[NIFFT];
ifftplan=fftwf_plan_dft_1d(NIFFT,reinterpret_cast<fftwf_complex*>(in),reinterpret_cast<fftwf_complex*>(out),FFTW_BACKWARD,FFTW_ESTIMATE);

//////////////////////// Calc In/Out buffer size//////////////////////////
InBufSz[0]=NumULSymbSF*NumLayer; InBufSz[1]=NIFFT;
OutBufSz[0]=NumLayer*NumULSymbSF; OutBufSz[1]=(NIFFT+CPLen);
if(BufFlag)
{
  cout<<"SCFDMA Modulation"<<endl;
  cout<<"Input buffer size is "<<"{"<<InBufSz[0]<<" , "<<InBufSz[1]<<"}"<<"  complex<float>"<<endl;
  cout<<"Output buffer size is "<<"{"<<OutBufSz[0]<<" , "<<OutBufSz[1]<<"}"<<"  complex<float>"<<endl; 
}
else
{}
//////////////////////End of clac in/out buffer size//////////////////////

}

void SCFDMAModulation::SCFDMAModulating(FIFO<complex<float> > *pInpBuf, FIFO<complex<float> > *pOutBuf)
{
//cout<<"SCFDMAModulating"<<endl;
if(PSFlag)
{cout<<"SCFDMAModulating"<<endl;}

bool ReadFlag = (*pInpBuf).Read(pInpData);
if(ReadFlag)
{
///////////////////// Start SCFDMA modulating /////////////////

    for(int nlayer=0;nlayer<NumLayer;nlayer++)
    {
        for(int nsym=0;nsym<NumULSymbSF;nsym++)
        {
            int idx=nlayer*NumULSymbSF+nsym; 

//            fftwf_execute_dft(ifftplan,reinterpret_cast<fftwf_complex*>((*(pInpData+idx))),reinterpret_cast<fftwf_complex*>((*(pOutData+idx)+CPLen)));
fftwf_execute_dft(ifftplan,reinterpret_cast<fftwf_complex*>((*(pInpData+idx))),reinterpret_cast<fftwf_complex*>(out));

            for(int n=0;n<NIFFT;n++)
            {
            *(*(pOutData+idx)+CPLen+n)=(*(out+n))/sqrt((float)NIFFT);
            }
 

            for(int n=0;n<CPLen;n++)
            {
                *(*(pOutData+idx)+n)=*(*(pOutData+idx)+n+NIFFT);
            }
        }
    }

bool WriteFlag = (*pOutBuf).Write(pOutData);

if(PSFlag)
{cout<<"SCFDMAModulation completed"<<endl;}
////////////////////End SCFDMA modulating /////////////////////
}
else
{cout<<"unable to read input buffer"<<endl;}


}

SCFDMAModulation::~SCFDMAModulation()
{
for(int nsym=0;nsym<NumLayer*NumULSymbSF;nsym++)
{
    delete[] *(pInpData+nsym);
    delete[] *(pOutData+nsym);
}

delete[] pInpData;
delete[] pOutData;
fftwf_destroy_plan(ifftplan);
delete[] out;
}
