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
#include "LTESys.h"

////////Global Variable//////////
int RANDOMSEED;

fsm FSM("rsc.txt");
Interleaver_lte<int,int> EInter;
Interleaver_lte<int,float> DInter;
////////End Global Variable//////////

using namespace std;
int main()
{
BSPara BS;
BS.initBSPara();
UserPara User(&BS);


int DataK=BS.DataLengthPerUser;
int LastK=DataK%6144;
int NumBlock=(DataK-LastK)/6144+1;
int HDLen = (NumBlock-1)*(6144*3+12)+1*(LastK*3+12);



float AWGNSigmaArray[NSTD];
for(int i=0;i<NSTD;i++)
{
float x = 10.0+0.5*((float)i);
AWGNSigmaArray[i]=sqrt((1.50/((log((float)((User).MQAM)))/(log(2.0))))*(pow(10.0,(-x/10.0))));
cout<<AWGNSigmaArray[i]<<endl;
}

float PE[NSTD],PB[NSTD];
float HPE[NSTD],HPB[NSTD];

///////////////////// construct the kernels ////////////////////
TurboEncoder_c TxTbE(&User);
TxRateMatch TxRM(&User);
Scramble TxSCRB(&User);
Modulation TxM(&User);
TransformPrecoder TxTP(&User);
SubCarrierMap TxSCM(&User);
SCFDMAModulation TxSCFM(&User);
Channel TxCRx(&BS);
SCFDMADemodulation RxSCFD(&BS);
SubCarrierDemap RxSCD(&BS);
Equalizer RxE(&BS,&User);
TransformDecoder RxTD(&BS);
Demodulation RxD(&BS);
Descramble RxDSCRB(&BS);
RxRateMatch RxRM(&BS);
TurboDecoder_c RxTbD(&BS);
//////////////////// Completed construction of the kernels/////////////////

////////////////// allocate the FIFOs /////////////////////////
int TxDSsz[2]={1,DataK};

FIFO<int> TxDataSource(1,TxDSsz);

FIFO<int> TxTbEBufRM(1,TxTbE.OutBufSz);

FIFO<int> TxRMBufSCRB(1,TxRM.OutBufSz);

FIFO<int> TxSCRBBufM(1,TxSCRB.OutBufSz);

FIFO<complex<float> > TxMBufTP(1,TxM.OutBufSz);

FIFO<complex<float> > TxTPBufSCM(1,TxTP.OutBufSz);

FIFO<complex<float> > TxSCMBufSCFM(1,TxSCM.OutBufSz);

FIFO<complex<float> > TxFileSink(1,TxSCFM.OutBufSz);


FIFO<complex<float> > RxDataSource(1,TxCRx.OutBufSz);

FIFO<complex<float> > RxSCFDBufSCD(1,RxSCFD.OutBufSz);

FIFO<complex<float> > RxSCDBufE(1,RxSCD.OutBufSz);

FIFO<complex<float> > RxEBufTD(1,RxE.OutBufSz);

FIFO<complex<float> > RxTDBufD(1,RxTD.OutBufSz);

FIFO<float> RxDBufDSCRB(1,RxD.OutBufSz);

FIFO<float> RxDSCRBBufRM(1,RxDSCRB.OutBufSz);

FIFO<float> RxRMBufTbD(1,RxRM.OutBufSz);

FIFO<int> RxFileSink(1,RxTbD.OutBufSz);
//////////////////End of allocation of FIFOs /////////////////////////

FILE *fptr = NULL;
int* pTxDS=new int[DataK];
int *pHDDS=new int[HDLen];
int *pHDFS=new int[HDLen];
int* pRxFS=new int[DataK];

for(int nsnr=0;nsnr<NSTD;nsnr++)
{

int PacketErr=0;
int BitErr=0;

int HPacketErr=0;
int HBitErr=0;

int nrun=0;

while((nrun<MAXRUN))
{
////////////////////////// Run Subframe //////////////////////////////////
 RANDOMSEED=(nrun+1)*(nsnr+2);

 GenerateLTEChainInput(&TxDataSource,DataK,pTxDS);

 TxTbE.TurboEncoding(&TxDataSource,&TxTbEBufRM);

 TxRM.TxRateMatching(&TxTbEBufRM,&TxRMBufSCRB);




 TxSCRB.Scrambling(&TxRMBufSCRB,&TxSCRBBufM);
 
 TxM.Modulating(&TxSCRBBufM,&TxMBufTP);

 TxTP.TransformPrecoding(&TxMBufTP,&TxTPBufSCM);

 TxSCM.SubCarrierMapping(&TxTPBufSCM,&TxSCMBufSCFM);

 TxSCFM.SCFDMAModulating(&TxSCMBufSCFM,&TxFileSink);

 
 TxCRx.ApplyChannel(&TxFileSink,&RxDataSource,(AWGNSigmaArray[nsnr]));

 RxSCFD.SCFDMADemodulating(&RxDataSource,&RxSCFDBufSCD);
// RxSCFD.SCFDMADemodulating(&TxFileSink,&RxSCFDBufSCD);

 RxSCD.SubCarrierDemapping(&RxSCFDBufSCD,&RxSCDBufE);

// RxE.Equalizing(&RxSCDBufE,&RxEBufTD,TxCRx.GetpPCSI(),TxCRx.GetAWGNNo());
 RxE.Equalizing(&RxSCDBufE,&RxEBufTD);

 RxTD.TransformDecoding(&RxEBufTD,&RxTDBufD);
// RxD.Demodulating(&RxTDBufD,&RxDBufDSCRB,RxE.GetpEqW(),RxE.GetpHdm(),(AWGNSigmaArray[nsnr]));

 RxD.Demodulating(&RxTDBufD,&RxDBufDSCRB,(AWGNSigmaArray[nsnr]));
// RxD.Demodulating(&RxTDBufD,&RxDBufDSCRB,((float)0.0));

 RxDSCRB.Descrambling(&RxDBufDSCRB,&RxDSCRBBufRM);

 RxRM.RxRateMatching(&RxDSCRBBufRM,&RxRMBufTbD);
 RxTbD.TurboDecoding(&RxRMBufTbD,&RxFileSink);


 ReadLTEChainOutput(&RxFileSink,pRxFS);

 for(int i=0;i<HDLen;i++)
 {*(pHDDS+i)=*((TxRM.GetpEncS())+i);}


 for(int i=0;i<HDLen;i++)
 {
 *(pHDFS+i)=*(RxRM.GetpHD()+i);
 }


 int NumErrBit=0;
 for(int i=0;i<DataK;i++)
 {
  if(*(pTxDS+i)==*(pRxFS+i)){}
  else{NumErrBit++;}
 }
 cout<<"Num of error bits="<<NumErrBit<<endl;

 int HDErrBit=0;
 for(int i=0;i<HDLen;i++)
 {
  if(*(pHDDS+i)==*(pHDFS+i)){}
  else{HDErrBit++;}
 } 
 cout<<"Num of HD error bits="<<HDErrBit<<endl;

 if(NumErrBit==0){}
 else{PacketErr++;BitErr+=NumErrBit;}

 if(HDErrBit==0){}
 else{HPacketErr++;HBitErr+=HDErrBit;}

 nrun++;

////////////////////////// END Run Subframe/////////////////////////////////
}
  
  PE[nsnr] = ((float)(PacketErr))/((float)(nrun));
  PB[nsnr] = ((float)(BitErr))/((float)(nrun*DataK));

  HPE[nsnr] = ((float)(HPacketErr))/((float)(nrun));
  HPB[nsnr] = ((float)(HBitErr))/((float)(nrun*(DataK*3+12)));
 
   /*          
                fptr = NULL;
                fptr = fopen("sigma.txt","a+");
                if(fptr!=NULL)
                {
                fprintf(fptr,"%f ",AWGNSigmaArray[nsnr]);
                fprintf(fptr,",");
                fclose(fptr);
                }

                fptr = NULL;
                fptr = fopen("PacketErrorProb.txt","a+");
                if(fptr!=NULL)
                {
                fprintf(fptr,"%f ",PE[nsnr]);
                fprintf(fptr,",");
                fclose(fptr);
                }

                fptr = NULL;
                fptr = fopen("BitErrProb.txt","a+");
                if(fptr!=NULL)
                {
                fprintf(fptr,"%f ",PB[nsnr]);
                fprintf(fptr,",");
                fclose(fptr);
                }

                fptr = NULL;
                fptr = fopen("HDPacketErrorProb.txt","a+");
                if(fptr!=NULL)
                {
                fprintf(fptr,"%f ",HPE[nsnr]);
                fprintf(fptr,",");
                fclose(fptr);
                }

                fptr = NULL;
                fptr = fopen("HDBitErrProb.txt","a+");
                if(fptr!=NULL)
                {
                fprintf(fptr,"%f ",HPB[nsnr]);
                fprintf(fptr,",");
                fclose(fptr);
                }
 */ 
             
}

 delete[] pTxDS;
 delete[] pHDDS;
 delete[] pHDFS;
 delete[] pRxFS;

return 0;
}
