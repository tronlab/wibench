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
#include "Channel.h"
extern int RANDOMSEED;
Channel::Channel(BSPara* pBS)
{
MaxTxAnt=2;
MaxRxAnt=2;
PSFlag = (*pBS).ProcessingStatusFlag;
BufFlag=(*pBS).BufferSizeFlag;
NumULSymbSF=(*pBS).NumULSymbSF;
NumLayerPerUser=(*pBS).NumLayerPerUser;
NumRxAntenna = (*pBS).NumRxAntenna;
NIFFT=(*pBS).NIFFT;
CPLen=(*pBS).CPLen;

ChannelLen=8;
ChannelType=1;
AWGNFlag=false;

FadingSigma=1.0;
AWGNSigma=1.0;

EVAAmplitude[0]=4.2865;EVAAmplitude[1]=0.3508;EVAAmplitude[2]=0.4467; EVAAmplitude[3]=0.2512; EVAAmplitude[4]=0.1429; 

pPCSI=new complex<float>** [MaxTxAnt];
for(int i=0;i<MaxTxAnt;i++){*(pPCSI+i)=new complex<float>*[MaxRxAnt];}
for(int i=0;i<MaxTxAnt;i++){for(int j=0;j<MaxRxAnt;j++){*(*(pPCSI+i)+j)=new complex<float>[NIFFT];}}

//////////////////////// Calc In/Out buffer size//////////////////////////
InBufSz[0]=NumLayerPerUser*NumULSymbSF;InBufSz[1]=(NIFFT+CPLen);
OutBufSz[0]=NumRxAntenna*NumULSymbSF;OutBufSz[1]=(NIFFT+CPLen);
if(BufFlag)
{
  cout<<"Channel"<<endl;
  cout<<"Input buffer size is "<<"{"<<InBufSz[0]<<" , "<<InBufSz[1]<<"}"<<"  complex<float>"<<endl;
  cout<<"Output buffer size is "<<"{"<<OutBufSz[0]<<" , "<<OutBufSz[1]<<"}"<<"  complex<float>"<<endl; 
}
else
{}
//////////////////////End of clac in/out buffer size//////////////////////

}

void Channel::ApplyFading()
{
  switch(ChannelType)
  {
  case 0:ApplyPredefinedChannel();break;
  case 1:ApplyGaussianRandomChannel();break;
  case 2:ApplyEVA();break;
  default: ApplyDeltaChannel();break;
  }
}

void Channel::ApplyGaussianRandomChannel()
{

complex<float> *** pGRC = new complex<float>**[CurNumLayer];
for(int ntx=0;ntx<CurNumLayer;ntx++){*(pGRC+ntx)=new complex<float>*[NumRxAntenna];}
for(int ntx=0;ntx<CurNumLayer;ntx++){for(int nrx=0;nrx<NumRxAntenna;nrx++){*(*(pGRC+ntx)+nrx)=new complex<float>[ChannelLen];}}
//////////////////// Generate Channel //////////////////////////////
int seed = -RANDOMSEED-CurNumLayer-NumRxAntenna;
for(int ntx=0;ntx<CurNumLayer;ntx++)
{
  for(int nrx=0;nrx<NumRxAntenna;nrx++)
  {
    complex<float> *ptmp=new complex<float>[ChannelLen];
    float gsum=0.0;
    for(int l=0;l<ChannelLen;l++)
    {
      float v = (float) gauss1(&seed);
      float vi= (float)gauss1(&seed);
      *(ptmp+l)=complex<float>(v,vi);
      gsum+=pow(abs(*(ptmp+l)),2.0);  
    }
    float gmean = gsum/((float)ChannelLen);
    for(int l=0;l<ChannelLen;l++)
    {*(*(*(pGRC+ntx)+nrx)+l)=(*(ptmp+l))/(sqrt(((float)ChannelLen)*(gmean)));}
    delete[] ptmp;
  }
}
/////////////////END Generate Channel //////////////////////////////

////////////////// cconv Channel //////////////////////////
for(int nrx=0;nrx<NumRxAntenna;nrx++)
{
  for(int ntx=0;ntx<CurNumLayer;ntx++)
  {
   int Ldim=NumULSymbSF*(NIFFT+CPLen);
   complex<float>* poV=new complex<float>[Ldim];

   CircularConvolution<int,complex<float> >(Ldim,ChannelLen,(*(VpInpData+ntx)),(*(*(pGRC+ntx)+nrx)),poV);


   for(int idx=0;idx<Ldim;idx++){(*(*(VpOutData+nrx)+idx))+=(*(poV+idx));}

   delete[] poV;
  }
}

//////////////////END cconv Channel/////////////////////////



////////////////// Regenerate Perfect CSI for GR Channel/////////////////
complex<float> *in,*out;
in=new complex<float>[NIFFT];
out=new complex<float>[NIFFT];
fftwf_plan fftplan=fftwf_plan_dft_1d(NIFFT,reinterpret_cast<fftwf_complex*>(in),reinterpret_cast<fftwf_complex*>(out),FFTW_FORWARD,FFTW_ESTIMATE);
for(int i=0;i<CurNumLayer;i++)
{
  for(int j=0;j<NumRxAntenna;j++)
  {
    for(int m=0;m<ChannelLen;m++){*(in+m)=(*(*(*(pGRC+i)+j)+m));}
    for(int m=ChannelLen;m<NIFFT;m++){*(in+m)=complex<float>(0.0,0.0);}
    fftwf_execute_dft(fftplan,reinterpret_cast<fftwf_complex*>(in),reinterpret_cast<fftwf_complex*>(out));
    for(int n=0;n<NIFFT;n++){*(*(*(pPCSI+i)+j)+n)=(*(out+n));}
  }
}

fftwf_destroy_plan(fftplan);
delete[] in;
delete[] out;
/////////////////END Perfect CSI for GR Channel////////////////

 
for(int ntx=0;ntx<CurNumLayer;ntx++){for(int nrx=0;nrx<NumRxAntenna;nrx++){delete[] *(*(pGRC+ntx)+nrx);}}
for(int ntx=0;ntx<CurNumLayer;ntx++){delete[] *(pGRC+ntx);}
delete[] pGRC;

}

/////////////////////// EVA //////////////////////////
void Channel::ApplyEVA()
{

complex<float> *** pEVA = new complex<float>**[CurNumLayer];
for(int ntx=0;ntx<CurNumLayer;ntx++){*(pEVA+ntx)=new complex<float>*[NumRxAntenna];}
for(int ntx=0;ntx<CurNumLayer;ntx++){for(int nrx=0;nrx<NumRxAntenna;nrx++){*(*(pEVA+ntx)+nrx)=new complex<float>[ChannelLen];}}
//////////////////// Generate Channel //////////////////////////////
int seed = -RANDOMSEED-CurNumLayer-NumRxAntenna;
for(int ntx=0;ntx<CurNumLayer;ntx++)
{
  for(int nrx=0;nrx<NumRxAntenna;nrx++)
  {
    complex<float> *ptmp=new complex<float>[ChannelLen];
    float gsum=0.0;
    for(int l=0;l<ChannelLen;l++)
    {
      float v = (float) gauss1(&seed);
      float vi= (float)gauss1(&seed);
      *(ptmp+l)=(complex<float>(v,vi))*EVAAmplitude[l];
      gsum+=pow(abs(*(ptmp+l)),2.0);
    }
    float gmean = gsum/((float)ChannelLen);
    for(int l=0;l<ChannelLen;l++)
    {*(*(*(pEVA+ntx)+nrx)+l)=(*(ptmp+l))/(sqrt(((float)ChannelLen)*(gmean)));}
    delete[] ptmp;
  }
}
/////////////////END Generate Channel //////////////////////////////

////////////////// cconv Channel //////////////////////////
for(int nrx=0;nrx<NumRxAntenna;nrx++)
{
  for(int ntx=0;ntx<CurNumLayer;ntx++)
  {
   int Ldim=NumULSymbSF*(NIFFT+CPLen);
   complex<float>* poV=new complex<float>[Ldim];

   CircularConvolution<int,complex<float> >(Ldim,ChannelLen,(*(VpInpData+ntx)),(*(*(pEVA+ntx)+nrx)),poV);


   for(int idx=0;idx<Ldim;idx++){(*(*(VpOutData+nrx)+idx))+=(*(poV+idx));}

   delete[] poV;
  }
}

//////////////////END cconv Channel/////////////////////////



////////////////// Regenerate Perfect CSI for EVA Channel/////////////////
complex<float> *in,*out;
in=new complex<float>[NIFFT];
out=new complex<float>[NIFFT];
fftwf_plan fftplan=fftwf_plan_dft_1d(NIFFT,reinterpret_cast<fftwf_complex*>(in),reinterpret_cast<fftwf_complex*>(out),FFTW_FORWARD,FFTW_ESTIMATE);
for(int i=0;i<CurNumLayer;i++)
{
  for(int j=0;j<NumRxAntenna;j++)
  {
    for(int m=0;m<ChannelLen;m++){*(in+m)=(*(*(*(pEVA+i)+j)+m));}
    for(int m=ChannelLen;m<NIFFT;m++){*(in+m)=complex<float>(0.0,0.0);}
    fftwf_execute_dft(fftplan,reinterpret_cast<fftwf_complex*>(in),reinterpret_cast<fftwf_complex*>(out));
    for(int n=0;n<NIFFT;n++)
    {*(*(*(pPCSI+i)+j)+n)=(*(out+n));}
  }
}

fftwf_destroy_plan(fftplan);
delete[] in;delete[] out;
/////////////////END Perfect CSI for EVA Channel////////////////


for(int ntx=0;ntx<CurNumLayer;ntx++){for(int nrx=0;nrx<NumRxAntenna;nrx++){delete[] *(*(pEVA+ntx)+nrx);}}
for(int ntx=0;ntx<CurNumLayer;ntx++){delete[] *(pEVA+ntx);}
delete[] pEVA;

}

/////////////////////END EVA//////////////////////////

void Channel::ApplyDeltaChannel()
{
cout<<"Delta Channel"<<endl;
for(int nrx=0;nrx<NumRxAntenna;nrx++)
{
  for(int ntx=0;ntx<CurNumLayer;ntx++)
  {
   int Ldim=NumULSymbSF*(NIFFT+CPLen);
   for(int idx=0;idx<Ldim;idx++){(*(*(VpOutData+nrx)+idx))+=(*(*(VpInpData+ntx)+idx));}
  }
}
}

void Channel::ApplyPredefinedChannel()
{}

void Channel::ApplyAWGN(complex<float>** pInMatrix)
{
int seed = -RANDOMSEED-NumULSymbSF;
for(int i=0;i<NumRxAntenna*NumULSymbSF;i++)
{
  for(int j=0;j<(NIFFT+CPLen);j++)
  {
   float vr=(float)gauss1(&seed)*AWGNSigma;
   float vi=(float)gauss1(&seed)*AWGNSigma;
   (*(*(pInMatrix+i)+j))+=complex<float>(vr,vi);
  }
}
}

void Channel::ApplyChannel(FIFO<complex<float> >* pInpBuf,FIFO<complex<float> >* pOutBuf)
{
if(PSFlag)
{cout<<"Applying Channel"<<endl;}
VpOutBuf=pOutBuf;
int InpDataRow=NumLayerPerUser;
pInpData= new complex<float>*[InpDataRow];
for(int i=0;i<InpDataRow;i++){*(pInpData+i)=new complex<float>[NumULSymbSF*(NIFFT+CPLen)];}
pOutData= new complex<float>*[NumRxAntenna];
for(int i=0;i<NumRxAntenna;i++){*(pOutData+i)=new complex<float>[NumULSymbSF*(NIFFT+CPLen)];}
/////////////////////////////////////////////////////////////////////////////////////////////////

  CurNumLayer=NumLayerPerUser;
  VpInpData=pInpData;
  complex<float>** ptmpBuf=new complex<float>*[CurNumLayer*NumULSymbSF];
  for(int s=0;s<CurNumLayer*NumULSymbSF;s++){*(ptmpBuf+s)=new complex<float>[(NIFFT+CPLen)];}
  bool ReadFlag=(*pInpBuf).Read(ptmpBuf);

  for(int layer=0;layer<CurNumLayer;layer++)
  {
    for(int ns=0;ns<NumULSymbSF;ns++)
    {
      for(int n=0;n<NIFFT+CPLen;n++)
      {*(*(VpInpData+layer)+ns*(NIFFT+CPLen)+n)=*(*(ptmpBuf+layer*NumULSymbSF+ns)+n);}
    }
  }
  for(int s=0;s<CurNumLayer*NumULSymbSF;s++){delete[] *(ptmpBuf+s);}
  delete[] ptmpBuf;
 
  VpOutData=pOutData;
  if(ReadFlag)
  {
    ApplyFading();
  }
  else
  {}

complex<float>** pTmpBuf=new complex<float>*[NumRxAntenna*NumULSymbSF];
for(int sn=0;sn<NumRxAntenna*NumULSymbSF;sn++){*(pTmpBuf+sn)=new complex<float>[NIFFT+CPLen];}
for(int nrx=0;nrx<NumRxAntenna;nrx++)
{
  for(int nsymb=0;nsymb<NumULSymbSF;nsymb++)
  {
   int SymbIdx=nrx*NumULSymbSF+nsymb;
   for(int n=0;n<NIFFT+CPLen;n++)
   {
    int loc=nsymb*(NIFFT+CPLen)+n;
    *(*(pTmpBuf+SymbIdx)+n)=(*(*(pOutData+nrx)+loc));
   }
  }
}
if(AWGNFlag)
{ApplyAWGN(pTmpBuf);}
else{}
bool WriteFlag=(*VpOutBuf).Write(pTmpBuf);
for(int sn=0;sn<NumRxAntenna*NumULSymbSF;sn++){delete[] *(pTmpBuf+sn);}
delete[] pTmpBuf;

///////////////////////////////////////////////////////////////////////////////////////////////////////
for(int i=0;i<InpDataRow;i++){delete[] *(pInpData+i);}
delete[] pInpData;
for(int i=0;i<NumRxAntenna;i++){delete[] *(pOutData+i);}
delete[] pOutData;
if(PSFlag)
{cout<<"Channel completed"<<endl;}

}

void Channel::ApplyChannel(FIFO<complex<float> >* pInpBuf,FIFO<complex<float> >* pOutBuf,float awgnSigma)
{

AWGNSigma=awgnSigma;
AWGNFlag=true;
ApplyChannel(pInpBuf,pOutBuf);
}

Channel::~Channel()
{
for(int i=0;i<MaxTxAnt;i++){for(int j=0;j<MaxRxAnt;j++){delete[] *(*(pPCSI+i)+j);}}
for(int i=0;i<MaxTxAnt;i++){delete[] *(pPCSI+i);}
delete[] pPCSI;
}
