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
#include "Scramble.h"
Scramble::Scramble(UserPara* pUser)
{
PSFlag = (*pUser).ProcessingStatusFlag;
BufFlag=(*pUser).BufferSizeFlag;
NumLayer=(*pUser).NumLayer;
NInfoBits=(*pUser).NInfoBits;

int INITM[31]={1,1,0,1,0,0,1,1,1,0,0,1,0,0,1,1,0,1,1,0,0,0,1,1,1,1,0,1,1,1,0};
for(int i=0;i<31;i++){n_init[i]=INITM[i];}

pInpSeq=new int[NumLayer*NInfoBits];
pScrambSeq=new int[NumLayer*NInfoBits];
pOutSeq=new int[NumLayer*NInfoBits];

int Mpn=NumLayer*NInfoBits;
/////////////////////Generate ScrambSeq///////////////////////
int Nc=1600;
int* px1=new int[(Mpn+Nc)];
int* px2=new int[(Mpn+Nc)];

for(int n=0;n<31;n++){*(px1+n)=0;*(px2+n)=n_init[n];} *(px1+0)=1;
for(int n=0;n<Mpn+Nc-31;n++)
{
*(px1+n+31)=((*(px1+n+3))+(*(px1+n)))%2;
*(px2+n+31)=((*(px2+n+3))+(*(px2+n+2))+(*(px2+n+1))+(*(px2+n)))%2;
}
for(int n=0;n<Mpn;n++){*(pScrambSeq+n)=((*(px1+n+Nc))+(*(px2+n+Nc)))%2;}

delete[] px1;
delete[] px2;
/////////////////////END Generate ScrambSeq///////////////////////
//////////////////////// Calc In/Out buffer size//////////////////////////
InBufSz[0]=1; InBufSz[1]=NumLayer*NInfoBits;
OutBufSz[0]=1;OutBufSz[1]=NumLayer*NInfoBits;
if(BufFlag)
{
  cout<<"Scramble"<<endl;
  cout<<"Input buffer size is "<<"{"<<InBufSz[0]<<" , "<<InBufSz[1]<<"}"<<"  int"<<endl;
  cout<<"Output buffer size is "<<"{"<<OutBufSz[0]<<" , "<<OutBufSz[1]<<"}"<<"  int"<<endl; 
}
else
{}
//////////////////////End of clac in/out buffer size//////////////////////

}

void Scramble::Scrambling(FIFO<int> *pInpBuf,FIFO<int> *pOutBuf)
{
if(PSFlag)
{cout<<"Scrambling"<<endl;}

int Mpn=NumLayer*NInfoBits;
 
bool ReadFlag = (*pInpBuf).Read(pInpSeq);
if(ReadFlag)
{
////////////////////////Scrambling////////////////////////////
for(int n=0;n<Mpn;n++)
{
*(pOutSeq+n)=((*(pInpSeq+n))+(*(pScrambSeq+n)))%2;
}
////////////////////////END Scrambling////////////////////////////
bool WriteFlag = (*pOutBuf).Write(pOutSeq);
if(PSFlag)
{cout<<"Scramble completed"<<endl;}

}
else
{cout<<"unable to read input buffer"<<endl;}

}

Scramble::~Scramble()
{
delete[] pInpSeq;
delete[] pScrambSeq;
delete[] pOutSeq;
}
