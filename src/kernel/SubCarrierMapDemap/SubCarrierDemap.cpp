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
#include "SubCarrierDemap.h"

SubCarrierDemap::SubCarrierDemap(BSPara* pBS)
{
PSFlag = (*pBS).ProcessingStatusFlag;
BufFlag=(*pBS).BufferSizeFlag;

NumULSymbSF=(*pBS).NumULSymbSF;
NIFFT=(*pBS).NIFFT;
DMRSSymbPos[0]=(*pBS).DMRSSymbPos[0];
DMRSSymbPos[1]=(*pBS).DMRSSymbPos[1];

SCLocPattern=(*pBS).SCLocPattern;
MDFTPerUser=(*pBS).MDFTPerUser;

NumRxAntenna = (*pBS).NumRxAntenna;

pInpData=new complex<float>*[NumULSymbSF*NumRxAntenna];
for(int s=0;s<NumULSymbSF*NumRxAntenna;s++){*(pInpData+s)=new complex<float>[NIFFT];}

pOutData = new complex<float>*[NumRxAntenna*NumULSymbSF];
for(int s=0;s<NumRxAntenna*NumULSymbSF;s++){*(pOutData+s)=new complex<float>[MDFTPerUser];}

//////////////////////// Calc In/Out buffer size//////////////////////////
InBufSz[0]=NumULSymbSF*NumRxAntenna; InBufSz[1]=NIFFT;
OutBufSz[0]=NumRxAntenna*NumULSymbSF; OutBufSz[1]=MDFTPerUser;
if(BufFlag)
{
  cout<<"SubCarrierDemap"<<endl;
  cout<<"Input buffer size is "<<"{"<<InBufSz[0]<<" , "<<InBufSz[1]<<"}"<<"  complex<float>"<<endl;
  cout<<"Output buffer size is "<<"{"<<OutBufSz[0]<<" , "<<OutBufSz[1]<<"}"<<"  complex<float>"<<endl; 
}
else
{}
//////////////////////End of clac in/out buffer size//////////////////////

}


void SubCarrierDemap::SubCarrierDemapping(FIFO<complex<float> >* pInpBuf,FIFO<complex<float> >* pOutBuf)
{
if(PSFlag)
{cout<<"SubCarrierDemapping"<<endl;}
bool ReadFlag = (*pInpBuf).Read(pInpData);
if(ReadFlag)
{
  
  int MDFT = MDFTPerUser;
  int SCLoc=SCLocPattern;

//////////// Get DMRS /////////////

  for(int nrs=0;nrs<2;nrs++)
  {
    int DMRSPos=DMRSSymbPos[nrs];
    for(int nrx=0;nrx<NumRxAntenna;nrx++)
    {

      int SymIdxIn = nrx*NumULSymbSF+DMRSPos;
      int SymIdxOut= nrx*2+nrs;

      for(int n=0;n<MDFT;n++){*(*(pOutData+SymIdxOut)+n)=*(*(pInpData+SymIdxIn)+(n+SCLoc));}
    }
  }
///////////END Get DMRS////////////
//////////// Get Data /////////////

for(int nrx=0;nrx<NumRxAntenna;nrx++)
{
  int SymOutIdx=0;
  for(int nsym=0;nsym<NumULSymbSF;nsym++)
  {
    int SymIdxIn = NumULSymbSF*nrx+nsym;
    if(((nsym==DMRSSymbPos[0])||(nsym==DMRSSymbPos[1])))
    {}
    else
    {
    int SymOutT=(NumULSymbSF-2)*nrx+SymOutIdx+NumRxAntenna*2;

    for(int n=0;n<MDFT;n++){*(*(pOutData+SymOutT)+n)=*(*(pInpData+SymIdxIn)+(n+SCLoc));}
    SymOutIdx++;
    }
  }
}
////////////END Get Data///////////
  bool WriteFlag = (*(pOutBuf)).Write(pOutData);
//  if(WriteFlag){cout<<"successfully written!"<<endl;}else{}
  if(PSFlag)
  {cout<<"SubCarrierDemap completed"<<endl;}
}
else
{cout<<"fail to read data from previous buffer"<<endl;}

}

SubCarrierDemap::~SubCarrierDemap()
{
  for(int s=0;s<NumRxAntenna*NumULSymbSF;s++){*(pOutData+s);}
  delete[] pOutData;

  for(int s=0;s<NumULSymbSF*NumRxAntenna;s++){delete[] *(pInpData+s);}
  delete[] pInpData;

}
