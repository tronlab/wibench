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
#include "Modulation.h"

Modulation::Modulation(UserPara* pUser)
{
PSFlag = (*pUser).ProcessingStatusFlag;
BufFlag=(*pUser).BufferSizeFlag;
////////////////// Init all Modulation Table /////////////////////
pBPSKtable=new float*[2];
*(pBPSKtable+0)=new float[2];*(pBPSKtable+1)=new float[2];
pBPSKtable[0][0]=+1.0;pBPSKtable[0][1]=+1.0;
pBPSKtable[1][0]=-1.0;pBPSKtable[1][1]=-1.0;
for(int r=0;r<2;r++)
{
  for(int c=0;c<2;c++)
  {pBPSKtable[r][c]/=sqrt(2.0);}
}

pQPSKtable=new float*[4];
for(int i=0;i<4;i++){*(pQPSKtable+i)=new float[2];}
pQPSKtable[0][0]=+1.0;pQPSKtable[0][1]=+1.0;
pQPSKtable[1][0]=+1.0;pQPSKtable[1][1]=-1.0;
pQPSKtable[2][0]=-1.0;pQPSKtable[2][1]=+1.0;
pQPSKtable[3][0]=-1.0;pQPSKtable[3][1]=-1.0;
for(int r=0;r<4;r++)
{
  for(int c=0;c<2;c++)
  {pQPSKtable[r][c]/=sqrt(2.0);}
}

pQAM16table=new float*[16];
for(int i=0;i<16;i++){*(pQAM16table+i)=new float[2];}
pQAM16table[0][0]= +1.0; pQAM16table[0][1]=+1.0;
pQAM16table[1][0]= +1.0; pQAM16table[1][1]=+3.0;
pQAM16table[2][0]= +3.0; pQAM16table[2][1]=+1.0;
pQAM16table[3][0]= +3.0; pQAM16table[3][1]=+3.0;
pQAM16table[4][0]= +1.0; pQAM16table[4][1]=-1.0;
pQAM16table[5][0]= +1.0; pQAM16table[5][1]=-3.0;
pQAM16table[6][0]= +3.0; pQAM16table[6][1]=-1.0;
pQAM16table[7][0]= +3.0; pQAM16table[7][1]=-3.0;
pQAM16table[8][0]= -1.0; pQAM16table[8][1]=+1.0;
pQAM16table[9][0]= -1.0; pQAM16table[9][1]=+3.0;
pQAM16table[10][0]=-3.0;pQAM16table[10][1]=+1.0;
pQAM16table[11][0]=-3.0;pQAM16table[11][1]=+3.0;
pQAM16table[12][0]=-1.0;pQAM16table[12][1]=-1.0;
pQAM16table[13][0]=-1.0;pQAM16table[13][1]=-3.0;
pQAM16table[14][0]=-3.0;pQAM16table[14][1]=-1.0;
pQAM16table[15][0]=-3.0;pQAM16table[15][1]=-3.0;
for(int r=0;r<16;r++)
{
  for(int c=0;c<2;c++)
  {pQAM16table[r][c]/=sqrt(10.0);}
}

pQAM64table=new float*[64];
for(int i=0;i<64;i++){*(pQAM64table+i)=new float[2];}
pQAM64table[0][0]= +3.0; pQAM64table[0][1]= +3.0;
pQAM64table[1][0]= +3.0; pQAM64table[1][1]= +1.0;
pQAM64table[2][0]= +1.0; pQAM64table[2][1]= +3.0;
pQAM64table[3][0]= +1.0; pQAM64table[3][1]= +1.0;
pQAM64table[4][0]= +3.0; pQAM64table[4][1]= +5.0;
pQAM64table[5][0]= +3.0; pQAM64table[5][1]= +7.0;
pQAM64table[6][0]= +1.0; pQAM64table[6][1]= +5.0;
pQAM64table[7][0]= +1.0; pQAM64table[7][1]= +7.0;

pQAM64table[8][0]= +5.0; pQAM64table[8][1]= +3.0;
pQAM64table[9][0]= +5.0; pQAM64table[9][1]= +1.0;
pQAM64table[10][0]=+7.0; pQAM64table[10][1]=+3.0;
pQAM64table[11][0]=+7.0; pQAM64table[11][1]=+1.0;
pQAM64table[12][0]=+5.0; pQAM64table[12][1]=+5.0;
pQAM64table[13][0]=+5.0; pQAM64table[13][1]=+7.0;
pQAM64table[14][0]=+7.0; pQAM64table[14][1]=+5.0;
pQAM64table[15][0]=+7.0; pQAM64table[15][1]=+7.0;

pQAM64table[16][0]=+3.0; pQAM64table[16][1]=-3.0;
pQAM64table[17][0]=+3.0; pQAM64table[17][1]=-1.0;
pQAM64table[18][0]=+1.0; pQAM64table[18][1]=-3.0;
pQAM64table[19][0]=+1.0; pQAM64table[19][1]=-1.0;
pQAM64table[20][0]=+3.0; pQAM64table[20][1]=-5.0;
pQAM64table[21][0]=+3.0; pQAM64table[21][1]=-7.0;
pQAM64table[22][0]=+1.0; pQAM64table[22][1]=-5.0;
pQAM64table[23][0]=+1.0; pQAM64table[23][1]=-7.0;

pQAM64table[24][0]=+5.0; pQAM64table[24][1]=-3.0;
pQAM64table[25][0]=+5.0; pQAM64table[25][1]=-1.0;
pQAM64table[26][0]=+7.0; pQAM64table[26][1]=-3.0;
pQAM64table[27][0]=+7.0; pQAM64table[27][1]=-1.0;
pQAM64table[28][0]=+5.0; pQAM64table[28][1]=-5.0;
pQAM64table[29][0]=+5.0; pQAM64table[29][1]=-7.0;
pQAM64table[30][0]=+7.0; pQAM64table[30][1]=-5.0;
pQAM64table[31][0]=+7.0; pQAM64table[31][1]=-7.0;

pQAM64table[32][0]=-3.0; pQAM64table[32][1]=+3.0;
pQAM64table[33][0]=-3.0; pQAM64table[33][1]=+1.0;
pQAM64table[34][0]=-1.0; pQAM64table[34][1]=+3.0;
pQAM64table[35][0]=-1.0; pQAM64table[35][1]=+1.0;
pQAM64table[36][0]=-3.0; pQAM64table[36][1]=+5.0;
pQAM64table[37][0]=-3.0; pQAM64table[37][1]=+7.0;
pQAM64table[38][0]=-1.0; pQAM64table[38][1]=+5.0;
pQAM64table[39][0]=-1.0; pQAM64table[39][1]=+7.0;

pQAM64table[40][0]=-5.0; pQAM64table[40][1]=+3.0;
pQAM64table[41][0]=-5.0; pQAM64table[41][1]=+1.0;
pQAM64table[42][0]=-7.0; pQAM64table[42][1]=+3.0;
pQAM64table[43][0]=-7.0; pQAM64table[43][1]=+1.0;
pQAM64table[44][0]=-5.0; pQAM64table[44][1]=+5.0;
pQAM64table[45][0]=-5.0; pQAM64table[45][1]=+7.0;
pQAM64table[46][0]=-7.0; pQAM64table[46][1]=+5.0;
pQAM64table[47][0]=-7.0; pQAM64table[47][1]=+7.0;

pQAM64table[48][0]=-3.0; pQAM64table[48][1]=-3.0;
pQAM64table[49][0]=-3.0; pQAM64table[49][1]=-1.0;
pQAM64table[50][0]=-1.0; pQAM64table[50][1]=-3.0;
pQAM64table[51][0]=-1.0; pQAM64table[51][1]=-1.0;
pQAM64table[52][0]=-3.0; pQAM64table[52][1]=-5.0;
pQAM64table[53][0]=-3.0; pQAM64table[53][1]=-7.0;
pQAM64table[54][0]=-1.0; pQAM64table[54][1]=-5.0;
pQAM64table[55][0]=-1.0; pQAM64table[55][1]=-7.0;

pQAM64table[56][0]=-5.0; pQAM64table[56][1]=-3.0;
pQAM64table[57][0]=-5.0; pQAM64table[57][1]=-1.0;
pQAM64table[58][0]=-7.0; pQAM64table[58][1]=-3.0;
pQAM64table[59][0]=-7.0; pQAM64table[59][1]=-1.0;
pQAM64table[60][0]=-5.0; pQAM64table[60][1]=-5.0;
pQAM64table[61][0]=-5.0; pQAM64table[61][1]=-7.0;
pQAM64table[62][0]=-7.0; pQAM64table[62][1]=-5.0;
pQAM64table[63][0]=-7.0; pQAM64table[63][1]=-7.0;
for(int r=0;r<64;r++)
{
  for(int c=0;c<2;c++)
  {pQAM64table[r][c]/=sqrt(42.0);}
}


////////////////END Init all Modulation Table ////////////////////

MQAM=(*pUser).MQAM;
NumLayer=(*pUser).NumLayer;
NInfoBits=(*pUser).NInfoBits;
QAMLen=(*pUser).QAMLen;

pBitsSeq=new int[NumLayer*NInfoBits];
BitsPerSymb=NInfoBits/QAMLen;
pQAMSeq=new complex<float>[NumLayer*QAMLen];
   switch(MQAM)
   {
    case 2: pTable=pBPSKtable;break;
    case 4: pTable=pQPSKtable;break;
    case 16:pTable=pQAM16table;break;
    case 64:pTable=pQAM64table;break;
    default: cout<<"Invalid modulation scheme!"<<endl;break;
   }

//////////////////////// Calc In/Out buffer size//////////////////////////
InBufSz[0]=1; InBufSz[1]=NumLayer*NInfoBits;
OutBufSz[0]=1;OutBufSz[1]=NumLayer*QAMLen;
if(BufFlag)
{
  cout<<"Modulation"<<endl;
  cout<<"Input buffer size is "<<"{"<<InBufSz[0]<<" , "<<InBufSz[1]<<"}"<<"  int"<<endl;
  cout<<"Output buffer size is "<<"{"<<OutBufSz[0]<<" , "<<OutBufSz[1]<<"}"<<"  complex<float>"<<endl; 
}
else
{}
//////////////////////End of clac in/out buffer size//////////////////////



}


void Modulation::Modulating(FIFO<int> *pInpBuf, FIFO<complex<float> > *pOutBuf)
{
if(PSFlag)
{cout<<"Modulating"<<endl;}

bool ReadFlag = (*pInpBuf).Read(pBitsSeq);
if(ReadFlag)
{
///////////// Modulating ////////////
 float I,Q;
 for(int nsymb=0;nsymb<QAMLen*NumLayer;nsymb++)
 {
  int Idx=0;
  for(int b=0;b<BitsPerSymb;b++)
  {
   Idx+=((*(pBitsSeq+nsymb*BitsPerSymb+b))*(pow(2.0,(float)(BitsPerSymb-1-b))));
  }
  I = *(*(pTable+Idx)+0);
  Q = *(*(pTable+Idx)+1);
  *(pQAMSeq+nsymb)=complex<float>(I,Q);
 }

bool WriteFlag = (*pOutBuf).Write(pQAMSeq);

if(PSFlag)
{cout<<"Modulation completed"<<endl;}

///////////End Modulating////////////
}
else
{cout<<"unable to read input buffer"<<endl;}

}

Modulation::~Modulation()
{
delete[] pBPSKtable;
delete[] pQPSKtable;
delete[] pQAM16table;
delete[] pQAM64table;
delete[] pBitsSeq;
delete[] pQAMSeq;
}
