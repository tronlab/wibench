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
#include "TxRateMatch.h"

TxRateMatch::TxRateMatch(UserPara* pUser)
{
PSFlag = (*pUser).ProcessingStatusFlag;
BufFlag=(*pUser).BufferSizeFlag;
Rate=3;
DataLength=(*pUser).DataLength;

NumBlock=(DataLength-(DataLength%6144))/6144+1;
pLengthSet=new int[NumBlock];
for(int nblock=0;nblock<NumBlock-1;nblock++)
{
 *(pLengthSet+nblock)=6144;
}
*(pLengthSet+NumBlock-1)=DataLength%6144;


NumLayer=(*pUser).NumLayer;
NInfoBits=(*pUser).NInfoBits;

NumExtraBits=NumLayer*NInfoBits-((NumBlock-1)*(6144*Rate+12)+1*((*(pLengthSet+NumBlock-1))*Rate+12));
pcMatrix=new int*[Rate];
for(int r=0;r<Rate;r++){*(pcMatrix+r)=new int[((NumBlock-1)*(6144+4)+1*((*(pLengthSet+NumBlock-1))+4))];}
pcSeq = new int[NumLayer*NInfoBits];

EncLen=3*DataLength+12;
pEncS=new int[EncLen];

//////////////////////// Calc In/Out buffer size//////////////////////////
InBufSz[0]=Rate;InBufSz[1]=((NumBlock-1)*(6144+4)+1*((*(pLengthSet+NumBlock-1))+4));
OutBufSz[0]=1;  OutBufSz[1]=NumLayer*NInfoBits;
if(BufFlag)
{
  cout<<"TxRateMatch"<<endl;
  cout<<"Input buffer size is "<<"{"<<InBufSz[0]<<" , "<<InBufSz[1]<<"}"<<"  int"<<endl;
  cout<<"Output buffer size is "<<"{"<<OutBufSz[0]<<" , "<<OutBufSz[1]<<"}"<<"  int"<<endl; 
}
else
{}
//////////////////////End of clac in/out buffer size//////////////////////

}


void TxRateMatch::TxRateMatching(FIFO<int> *pInpBuf,FIFO<int> *pOutBuf)
{
if(PSFlag)
{cout<<"TxRateMatching"<<endl;}

bool ReadFlag = (*pInpBuf).Read(pcMatrix);
if(ReadFlag)
{
////////////// Start Rate Matching ///////////

 int EncSK=(*(pLengthSet+0))+4;
 for(int i=0;i<EncSK;i++)
 {
  for(int r=0;r<Rate;r++)
  {
   *(pEncS+i*Rate+r)=*(*(pcMatrix+r)+i);
  }
 }

 for(int nBlock=0;nBlock<NumBlock;nBlock++)
 {
  iSeqLength=*(pLengthSet+nBlock);
  InpBlockShift = nBlock*(6144+4);
  OutBlockShift = nBlock*(6144*Rate+12);
 /////////////////// TxRateMatch this block /////////////////////
  
////////////////////// Subblock Interleaver //////////////////////////
  int **pInMatrix = new int*[Rate];
  int **pOutMatrix = new int*[Rate];
  for(int r=0;r<Rate;r++)
  {
    *(pInMatrix+r)=new int[iSeqLength+4];
    *(pOutMatrix+r)=new int[iSeqLength+4];
    for(int i=0;i<iSeqLength+4;i++)
    {*(*(pInMatrix+r)+i)=*(*(pcMatrix+r)+InpBlockShift+i);}
  }
  SbInterleaver.SubblockInterleaving((iSeqLength+4),pInMatrix,pOutMatrix);
  for(int r=0;r<Rate;r++)
  {
   for(int i=0;i<iSeqLength+4;i++)
   {*(*(pcMatrix+r)+InpBlockShift+i)=*(*(pOutMatrix+r)+i);}
  }
  for(int r=0;r<Rate;r++)
  {
    delete[] *(pInMatrix+r);
    delete[] *(pOutMatrix+r);
  }
  delete[] pInMatrix;
  delete[] pOutMatrix;
//////////////////// END Subblock Interleaver ////////////////////////

  for(int i=0;i<iSeqLength+4;i++)
  {
   *(pcSeq+OutBlockShift+3*i+0)=*(*(pcMatrix+0)+InpBlockShift+i);
   *(pcSeq+OutBlockShift+3*i+1)=*(*(pcMatrix+1)+InpBlockShift+i);
   *(pcSeq+OutBlockShift+3*i+2)=*(*(pcMatrix+2)+InpBlockShift+i);
  }

 ///////////////////END TxRateMatch this block /////////////////////
 }
 for(int i=0;i<NumExtraBits;i++)
 {
  *(pcSeq+((NumBlock-1)*(6144*Rate+12)+1*((*(pLengthSet+NumBlock-1))*Rate+12))+i)=0;
 }

bool WriteFlag = (*pOutBuf).Write(pcSeq);

if(PSFlag)
{cout<<"TxRateMatch completed"<<endl;}

///////////End Rate Match ////////////////////
}
else
{cout<<"unable to read input buffer"<<endl;}

}

TxRateMatch::~TxRateMatch()
{
delete[] pLengthSet;

for(int r=0;r<Rate;r++){delete[] *(pcMatrix+r);}
delete[] pcMatrix;

delete[] pcSeq;

delete[] pEncS;
}
