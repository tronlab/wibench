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
#include "RxRateMatch.h"

RxRateMatch::RxRateMatch(BSPara* pBS)
{
PSFlag = (*pBS).ProcessingStatusFlag;
BufFlag=(*pBS).BufferSizeFlag;

NumULSymbSF=(*pBS).NumULSymbSF;

MDFT=(*pBS).MDFTPerUser;
MQAM=(*pBS).MQAMPerUser;
NumLayer=(*pBS).NumLayerPerUser;
DataLength=(*pBS).DataLengthPerUser;

Rate=3;

pLLRin=new float[(NumLayer*(NumULSymbSF-2)*MDFT*((int)((log((double)MQAM))/(log(2.0)))))];
  
LastBlockLen=(DataLength%6144);
NumBlock=(DataLength-LastBlockLen)/6144+1;
EncDataLen=(NumBlock-1)*(6144*Rate+12)+1*(LastBlockLen*Rate+12);
pLLRout=new float[EncDataLen];

pLengthSet=new int[NumBlock];
for(int nblock=0;nblock<NumBlock-1;nblock++)
{
 *(pLengthSet+nblock)=6144;
}
*(pLengthSet+NumBlock-1)=DataLength%6144;

pHD = new int[EncDataLen];
//////////////////////// Calc In/Out buffer size//////////////////////////
InBufSz[0]=1;InBufSz[1]=(NumLayer*(NumULSymbSF-2)*MDFT*((int)((log((double)MQAM))/(log(2.0)))));
OutBufSz[0]=1;OutBufSz[1]=EncDataLen;
if(BufFlag)
{
  cout<<"RxRateMatch"<<endl;
  cout<<"Input buffer size is "<<"{"<<InBufSz[0]<<" , "<<InBufSz[1]<<"}"<<"  float"<<endl;
  cout<<"Output buffer size is "<<"{"<<OutBufSz[0]<<" , "<<OutBufSz[1]<<"}"<<"  float"<<endl; 
}
else
{}
//////////////////////End of clac in/out buffer size//////////////////////

}

void RxRateMatch::RxRateMatching(FIFO<float>* pInpBuf,FIFO<float>* pOutBuf)
{
if(PSFlag)
{cout<<"RxRateMatching"<<endl;}
  int iSeqLength;
  int OutBlockShift;
 

  bool ReadFlag = (*pInpBuf).Read(pLLRin);
  if(ReadFlag)
  {
    for(int nBlock=0;nBlock<NumBlock;nBlock++)
    {
     iSeqLength=*(pLengthSet+nBlock);
     OutBlockShift = nBlock*(6144*Rate+12);
////////////////////// Subblock DeInterleaver //////////////////////////
     float **pInMatrix = new float*[Rate];
     float **pOutMatrix = new float*[Rate];
     for(int r=0;r<Rate;r++)
     {
       *(pInMatrix+r)=new float[iSeqLength+4];
       *(pOutMatrix+r)=new float[iSeqLength+4];
     }



  ////////////////////// Read into pInMatrix //////////////////
    for(int i=0;i<iSeqLength+4;i++)
    {
      for(int r=0;r<Rate;r++)
      {
      *(*(pInMatrix+r)+i)=*(pLLRin+OutBlockShift+Rate*i+r);
      }
    }  

  //////////////////// END Read into pInMatrix ////////////////

   SbDeInterleaver.SubblockDeInterleaving((iSeqLength+4),pInMatrix,pOutMatrix);

  ////////////////////// Write back to pLLRin //////////////////
   for(int i=0;i<iSeqLength+4;i++)
   {
     for(int r=0;r<Rate;r++)
     {
      *(pLLRin+OutBlockShift+Rate*i+r)=*(*(pOutMatrix+r)+i);
     }
   }  
  //////////////////// END Write back to pLLRin ////////////////

     for(int r=0;r<Rate;r++)
     {
       delete[] *(pInMatrix+r);
       delete[] *(pOutMatrix+r);
     }
     delete[] pInMatrix;
     delete[] pOutMatrix;


//////////////////// END Subblock DeInterleaver ////////////////////////
   }

    for(int i=0;i<EncDataLen;i++)
    {
      if((*(pLLRin+i))<0){*(pHD+i)=0;}
      else{*(pHD+i)=1;}
    }

    for(int i=0;i<EncDataLen;i++){*(pLLRout+i)=*(pLLRin+i);}
    bool WriteFlag = (*pOutBuf).Write(pLLRout);
 //   if(WriteFlag){cout<<"successfully written!"<<endl;}else{}
    if(PSFlag)
    {cout<<"RxRateMatch completed"<<endl;}
  }
  else
  {cout<<"fail to read data from previous buffer"<<endl;}

}

RxRateMatch::~RxRateMatch()
{
delete[] pLengthSet;  
delete[] pLLRin;
delete[] pLLRout;
delete[] pHD;
}
