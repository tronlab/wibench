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
#include "SubCarrierMap.h"

SubCarrierMap::SubCarrierMap(UserPara* pUser)
{
PSFlag = (*pUser).ProcessingStatusFlag;
BufFlag=(*pUser).BufferSizeFlag;
NIFFT = (*pUser).NIFFT;
NumULSymbSF=(*pUser).NumULSymbSF;
MDFT = (*pUser).MDFT;
NumLayer=(*pUser).NumLayer;
SCLoc = (*pUser).SCLoc;

DMRSSymbPos[0]=(*pUser).DMRSSymbPos[0];
DMRSSymbPos[1]=(*pUser).DMRSSymbPos[1];

VpDMRS = (*pUser).pDMRS;

pInpData=new complex<float>* [(NumULSymbSF-2)*NumLayer];
for(int i=0;i<(NumULSymbSF-2)*NumLayer;i++){*(pInpData+i)=new complex<float>[MDFT];}
pOutData=new complex<float>* [NumULSymbSF*NumLayer];
for(int i=0;i<NumULSymbSF*NumLayer;i++){*(pOutData+i)=new complex<float>[NIFFT];}
for(int i=0;i<NumULSymbSF*NumLayer;i++)
{
  for(int j=0;j<NIFFT;j++)
  {*(*(pOutData+i)+j)=complex<float>(1.0,0.0);}
}

//////////////////////// Calc In/Out buffer size//////////////////////////
InBufSz[0]=(NumULSymbSF-2)*NumLayer; InBufSz[1]=MDFT;
OutBufSz[0]=NumULSymbSF*NumLayer; OutBufSz[1]=NIFFT;
if(BufFlag)
{
  cout<<"SubCarrierMap"<<endl;
  cout<<"Input buffer size is "<<"{"<<InBufSz[0]<<" , "<<InBufSz[1]<<"}"<<"  complex<float>"<<endl;
  cout<<"Output buffer size is "<<"{"<<OutBufSz[0]<<" , "<<OutBufSz[1]<<"}"<<"  complex<float>"<<endl; 
}
else
{}
//////////////////////End of clac in/out buffer size//////////////////////

}

void SubCarrierMap::SubCarrierMapping(FIFO<complex<float> > *pInpBuf, FIFO<complex<float> > *pOutBuf)
{
if(PSFlag)
{cout<<"SubCarrierMapping"<<endl;}
bool ReadFlag = (*pInpBuf).Read(pInpData);
if(ReadFlag)
{
//////////////// Start subcarrier mapper //////////////
for(int nlayer=0;nlayer<NumLayer;nlayer++)
{
  int DMRSslot=0;
  for(int nsym=0;nsym<NumULSymbSF;nsym++)
  {

    int SymIdx = nlayer*NumULSymbSF+nsym;
    if(nsym==DMRSSymbPos[DMRSslot])
    {

      for(int n=SCLoc;n<SCLoc+MDFT;n++)
      {
      *(*(pOutData+SymIdx)+n)=*(*(*(VpDMRS+DMRSslot)+nlayer)+n-SCLoc);
      }
      DMRSslot++;
    }
    else
    {

      for(int n=SCLoc;n<SCLoc+MDFT;n++)
      {
      *(*(pOutData+SymIdx)+n)=*(*(pInpData+nlayer*(NumULSymbSF-2)+nsym-DMRSslot)+n-SCLoc);
      }
    }
  }
}

bool WriteFlag = (*pOutBuf).Write(pOutData);
if(PSFlag)
{cout<<"SubCarrierMap completed"<<endl;}

/////////////End subcarrier mapper ////////////////////
}
else
{cout<<"unable to read input buffer"<<endl;}

}

SubCarrierMap::~SubCarrierMap()
{
for(int i=0;i<(NumULSymbSF-2)*NumLayer;i++){delete[] *(pInpData+i);}
delete[] pInpData;
for(int i=0;i<NumULSymbSF*NumLayer;i++){delete[] *(pOutData+i);}
delete[] pOutData;
}
