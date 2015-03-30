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
#include "Equalizer_s.h"

Equalizer::Equalizer(BSPara* pBS,UserPara* pUser)
{
PSFlag = (*pBS).ProcessingStatusFlag;
BufFlag=(*pBS).BufferSizeFlag;

NumULSymbSF=(*pBS).NumULSymbSF;
MDFTPerUser=(*pBS).MDFTPerUser;
NumLayerPerUser=(*pBS).NumLayerPerUser;

NumRxAntenna = (*pBS).NumRxAntenna;
EstimationMethod = (*pBS).EstimationMethod;

VpUser = pUser;

  pEqW=new complex<float>**[MDFTPerUser];
  for(int m=0;m<MDFTPerUser;m++)
  {
    *(pEqW+m)=new complex<float>*[NumLayerPerUser];
    for(int l=0;l<NumLayerPerUser;l++)
    {
     *(*(pEqW+m)+l)=new complex<float>[NumRxAntenna];
    }
  }


   pHdm=new complex<float> **[MDFTPerUser];
   for(int m=0;m<MDFTPerUser;m++)
   {
      *(pHdm+m)=new complex<float> *[NumRxAntenna];
      for(int nrx=0;nrx<NumRxAntenna;nrx++)
      {
        *(*(pHdm+m)+nrx)=new complex<float>[NumLayerPerUser];
      }
   }


  pInpData = new complex<float>* [NumULSymbSF*NumLayerPerUser];
  pOutData = new complex<float>* [(NumULSymbSF-2)*NumLayerPerUser];
  for(int s=0;s<NumULSymbSF*NumLayerPerUser;s++){*(pInpData+s)=new complex<float>[MDFTPerUser];}
  for(int s=0;s<(NumULSymbSF-2)*NumLayerPerUser;s++){*(pOutData+s)=new complex<float>[MDFTPerUser];}

//////////////////////// Calc In/Out buffer size//////////////////////////
InBufSz[0]=NumULSymbSF*NumLayerPerUser;InBufSz[1]=MDFTPerUser;
OutBufSz[0]=(NumULSymbSF-2)*NumLayerPerUser;OutBufSz[1]=MDFTPerUser;
if(BufFlag)
{
  cout<<"Equalizer"<<endl;
  cout<<"Input buffer size is "<<"{"<<InBufSz[0]<<" , "<<InBufSz[1]<<"}"<<"  complex<float>"<<endl;
  cout<<"Output buffer size is "<<"{"<<OutBufSz[0]<<" , "<<OutBufSz[1]<<"}"<<"  complex<float>"<<endl; 
}
else
{}
//////////////////////End of clac in/out buffer size//////////////////////

}

void Equalizer::FDLSEstimation(complex<float>** pXt,complex<float>** pXtDagger,complex<float>** pYt,complex<float>** pHTranspose,int NumLayer)
{
//////////////////// Freq Domain Estimate HTranspose ////////////////////
   complex<float>** pXDX=new complex<float>*[NumLayer];
   for(int layer=0;layer<NumLayer;layer++){*(pXDX+layer)=new complex<float>[NumLayer];}
   MatrixProd<int,complex<float> >(NumLayer,2,NumLayer,pXtDagger,pXt,pXDX);

   complex<float>** pInvXDX=new complex<float>*[NumLayer];
   for(int layer=0;layer<NumLayer;layer++){*(pInvXDX+layer)=new complex<float>[NumLayer];}
   MatrixInv<int,complex<float> >(NumLayer,pXDX,pInvXDX);

   complex<float>** pXDY=new complex<float>*[NumLayer];
   for(int layer=0;layer<NumLayer;layer++){*(pXDY+layer)=new complex<float>[NumRxAntenna];}
   MatrixProd<int,complex<float> >(NumLayer,2,NumRxAntenna,pXtDagger,pYt,pXDY);

   MatrixProd<int,complex<float> >(NumLayer,NumLayer,NumRxAntenna,pInvXDX,pXDY,pHTranspose);

   for(int layer=0;layer<NumLayer;layer++){delete[] *(pXDX+layer);delete[] *(pInvXDX+layer);delete[] *(pXDY+layer);}
   delete[] pXDX; delete[] pInvXDX; delete[] pXDY;  
//////////////////// END Freq Domain Estimate HTranspose ////////////////////
}


void Equalizer::FDLSEqualization(complex<float>** pHTranspose,int m,int NumLayer)
{
//////////////////// Freq Domain Equalize received Data /////////////////
   complex<float>** pH=new complex<float>*[NumRxAntenna];
   for(int nrx=0;nrx<NumRxAntenna;nrx++){*(pH+nrx)=new complex<float>[NumLayer];}
   complex<float>** pHDagger=new complex<float>*[NumLayer];
   for(int layer=0;layer<NumLayer;layer++){*(pHDagger+layer)=new complex<float>[NumRxAntenna];}
   complex<float>** pHDH=new complex<float>*[NumLayer];
   for(int layer=0;layer<NumLayer;layer++){*(pHDH+layer)=new complex<float>[NumLayer];}
   complex<float>** pInvHDH=new complex<float>*[NumLayer];
   for(int layer=0;layer<NumLayer;layer++){*(pInvHDH+layer)=new complex<float>[NumLayer];}

   complex<float>* pHDY=new complex<float>[NumLayer];

   for(int nrx=0;nrx<NumRxAntenna;nrx++)
   {
     for(int layer=0;layer<NumLayer;layer++)
     {
     *(*(pH+nrx)+layer)=*(*(pHTranspose+layer)+nrx);
     *(*(pHDagger+layer)+nrx)=conj((*(*(pHTranspose+layer)+nrx)));
     }
   }
   MatrixProd<int,complex<float> >(NumLayer,NumRxAntenna,NumLayer,pHDagger,pH,pHDH);
   MatrixInv<int,complex<float> >(NumLayer,pHDH,pInvHDH);

   ////////////////// Equalizing Data /////////////////
   for(int nSymb=0;nSymb<NumULSymbSF-2;nSymb++)
   {
    complex<float>* pYData=new complex<float>[NumRxAntenna];
    for(int nrx=0;nrx<NumRxAntenna;nrx++)
    {
     int IDX=(NumULSymbSF-2)*nrx+nSymb+2*NumRxAntenna;
     *(pYData+nrx)=*(*(pInpData+IDX)+m);
    }
    MatrixProd<int,complex<float> >(NumLayer,NumRxAntenna,1,pHDagger,pYData,pHDY);

    complex<float>* pXData=new complex<float>[NumLayer];
    MatrixProd<int,complex<float> >(NumLayer,NumLayer,1,pInvHDH,pHDY,pXData);

    /////////////////////// Get EqW ////////////////////////
    complex<float> **pW = new complex<float>*[NumLayer];
    for(int layer=0;layer<NumLayer;layer++){*(pW+layer)=new complex<float>[NumRxAntenna];}
    MatrixProd<int,complex<float> >(NumLayer,NumLayer,1,pInvHDH,pHDagger,pW);
    for(int layer=0;layer<NumLayer;layer++)
    {
      for(int nrx=0;nrx<NumRxAntenna;nrx++)
      {*(*(*(pEqW+m)+layer)+nrx)=*(*(pW+layer)+nrx);}
    }   
    for(int layer=0;layer<NumLayer;layer++){delete[] *(pW+layer);}
    delete[] pW;
    ////////////////////////END Get EqW/////////////////////
    //////////////////////// Get pHdm ////////////////////////
    for(int nrx=0;nrx<NumRxAntenna;nrx++)
    {
      for(int layer=0;layer<NumLayer;layer++)
      {*(*(*(pHdm+m)+nrx)+layer)=*(*(pH+nrx)+layer);}
    }
    /////////////////////// END Get pHdm /////////////////////
    for(int layer=0;layer<NumLayer;layer++)
    {
     int IDX = (NumULSymbSF-2)*layer+nSymb;
     *(*(pOutData+IDX)+m)=*(pXData+layer);
    }

    delete[] pYData;
    delete[] pXData;
   }
   //////////////// END Equalizing Data ///////////////  

   for(int nrx=0;nrx<NumRxAntenna;nrx++){delete[] *(pH+nrx);}
   delete[] pH;
   for(int layer=0;layer<NumLayer;layer++)
   {delete[] *(pHDagger+layer);delete[] *(pHDH+layer);delete[] *(pInvHDH+layer);}
   delete[] pHDagger;
   delete[] pHDH;
   delete[] pInvHDH;

   delete[] pHDY;
//////////////////// END Freq Domain Equalize received Data//////////////////

}


/////////////////////////Frequency Domain MMSE Equalization///////////////////////////////////
////////////////////////////////////////////////////////////
void Equalizer::FDMMSEEqualization(complex<float>** pHTranspose,int m,int NumLayer,float No)
{
//////////////////// Freq Domain Equalize received Data /////////////////
   complex<float>** pH=new complex<float>*[NumRxAntenna];
   for(int nrx=0;nrx<NumRxAntenna;nrx++){*(pH+nrx)=new complex<float>[NumLayer];}
   complex<float>** pHDagger=new complex<float>*[NumLayer];
   for(int layer=0;layer<NumLayer;layer++){*(pHDagger+layer)=new complex<float>[NumRxAntenna];}
   complex<float>** pHHD=new complex<float>*[NumRxAntenna];
   for(int nrx=0;nrx<NumRxAntenna;nrx++){*(pHHD+nrx)=new complex<float>[NumRxAntenna];}
   complex<float>** pInvHHDN=new complex<float>*[NumRxAntenna];
   for(int nrx=0;nrx<NumRxAntenna;nrx++){*(pInvHHDN+nrx)=new complex<float>[NumRxAntenna];}

   complex<float>** pEo=new complex<float>*[NumLayer];
   for(int layer=0;layer<NumLayer;layer++){*(pEo+layer)=new complex<float>[NumRxAntenna];}

   for(int nrx=0;nrx<NumRxAntenna;nrx++)
   {
     for(int layer=0;layer<NumLayer;layer++)
     {
     *(*(pH+nrx)+layer)=*(*(pHTranspose+layer)+nrx);
     *(*(pHDagger+layer)+nrx)=conj((*(*(pHTranspose+layer)+nrx)));
     }
   }

   MatrixProd<int,complex<float> >(NumRxAntenna,NumLayer,NumRxAntenna,pH,pHDagger,pHHD);

   for(int nrx=0;nrx<NumRxAntenna;nrx++)
   {(*(*(pHHD+nrx)+nrx))+=No;}   

   MatrixInv<int,complex<float> >(NumRxAntenna,pHHD,pInvHHDN);
   MatrixProd<int,complex<float> >(NumLayer,NumRxAntenna,NumRxAntenna,pHDagger,pInvHHDN,pEo);
    /////////////////////// Get EqW ////////////////////////
    for(int layer=0;layer<NumLayer;layer++)
    {
      for(int nrx=0;nrx<NumRxAntenna;nrx++)
      {*(*(*(pEqW+m)+layer)+nrx)=*(*(pEo+layer)+nrx);}
    }   
    ////////////////////////END Get EqW/////////////////////
    //////////////////////// Get pHdm ////////////////////////
    for(int nrx=0;nrx<NumRxAntenna;nrx++)
    {
      for(int layer=0;layer<NumLayer;layer++)
      {*(*(*(pHdm+m)+nrx)+layer)=*(*(pH+nrx)+layer);}
    }
    /////////////////////// END Get pHdm /////////////////////

   ////////////////// Equalizing Data /////////////////
   for(int nSymb=0;nSymb<NumULSymbSF-2;nSymb++)
   {
    complex<float>* pYData=new complex<float>[NumRxAntenna];
    for(int nrx=0;nrx<NumRxAntenna;nrx++)
    {
     int IDX=(NumULSymbSF-2)*nrx+nSymb+2*NumRxAntenna;
     *(pYData+nrx)=*(*(pInpData+IDX)+m);
    }

    complex<float>* pXData=new complex<float>[NumLayer];
    MatrixProd<int,complex<float> >(NumLayer,NumRxAntenna,1,pEo,pYData,pXData);


    for(int layer=0;layer<NumLayer;layer++)
    {
     int IDX = (NumULSymbSF-2)*layer+nSymb;
     *(*(pOutData+IDX)+m)=*(pXData+layer);
    }

    delete[] pYData;
    delete[] pXData;
   }
   //////////////// END Equalizing Data ///////////////  

   for(int nrx=0;nrx<NumRxAntenna;nrx++){delete[] *(pH+nrx); delete[] *(pHHD+nrx); delete[] *(pInvHHDN+nrx);}
   delete[] pH; delete[] pHHD; delete[] pInvHHDN;
   for(int layer=0;layer<NumLayer;layer++)
   {delete[] *(pHDagger+layer);delete[] *(pEo+layer);}
   delete[] pHDagger;delete[] pEo;
//////////////////// END Freq Domain Equalize received Data//////////////////

}

////////////////////////////////////////////////////////////
/////////////////////////END Frequency Domain MMSE Equalization///////////////////////////////////
void Equalizer::LSFreqDomain(void)
{
  int MDFT=MDFTPerUser;
  int NumLayer=NumLayerPerUser;
  pDMRS=(*VpUser).GetpDMRS();
  bool ReadFlag = (*VpInpBuf).Read(pInpData);
  if(ReadFlag)
  {
  
    for(int m=0;m<MDFT;m++)
    {
/////////////////////////// Xt XtDagger//////////////////////////
    complex<float>** pXt=new complex<float>*[2];
    for(int slot=0;slot<2;slot++){*(pXt+slot)=new complex<float>[NumLayer];}
    complex<float>** pXtDagger = new complex<float>* [NumLayer];
    for(int layer=0;layer<NumLayer;layer++){*(pXtDagger+layer)=new complex<float>[2];}
    for(int slot=0;slot<2;slot++)
    {
      for(int layer=0;layer<NumLayer;layer++)
      {
      *(*(pXt+slot)+layer)=*(*(*(pDMRS+slot)+layer)+m);
      *(*(pXtDagger+layer)+slot)=conj((*(*(*(pDMRS+slot)+layer)+m)));
      }
    }
///////////////////////////END Xt XtDagger//////////////////////////
/////////////////////////// Yt //////////////////////////
    complex<float>** pYt=new complex<float>*[2];
    for(int slot=0;slot<2;slot++){*(pYt+slot)=new complex<float>[NumRxAntenna];}
    for(int slot=0;slot<2;slot++)
    {
      for(int nrx=0;nrx<NumRxAntenna;nrx++)
      {
      *(*(pYt+slot)+nrx)=*(*(pInpData+(nrx*2+slot))+m);
      }
    }
/////////////////////////// END Yt //////////////////////////
////////////////////////// HTranspose////////////////////////
   complex<float>** pHTranspose=new complex<float>*[NumLayer];
   for(int layer=0;layer<NumLayer;layer++){*(pHTranspose+layer)=new complex<float>[NumRxAntenna];}
//////////////////////////END HTranspose/////////////////////

//////////////////// Freq Domain Estimate HTranspose ////////////////////
   FDLSEstimation(pXt,pXtDagger,pYt,pHTranspose,NumLayer);
//////////////////// END Freq Domain Estimate HTranspose ////////////////////

//////////////////// Freq Domain Equalize received Data /////////////////
   FDLSEqualization(pHTranspose,m,NumLayer); 
//////////////////// END Freq Domain Equalize received Data//////////////////

/////////////////////////////////Delete///////////////////////////////////////    
/////////////////////////// Xt XtDagger//////////////////////////
    for(int slot=0;slot<2;slot++){delete[] *(pXt+slot);}
    delete[] pXt;
    for(int layer=0;layer<NumLayer;layer++){delete[] *(pXtDagger+layer);}
    delete[] pXtDagger;
///////////////////////////END Xt XtDagger//////////////////////////
/////////////////////////// Yt //////////////////////////
    for(int slot=0;slot<2;slot++){delete[] *(pYt+slot);}
    delete[] pYt; 
/////////////////////////// END Yt //////////////////////////
////////////////////////// HTranspose////////////////////////
   for(int layer=0;layer<NumLayer;layer++){delete[]  *(pHTranspose+layer);}
   delete[] pHTranspose;
//////////////////////////END HTranspose/////////////////////   
    }
   bool WriteFlag=(*VpOutBuf).Write(pOutData);
//   if(WriteFlag){cout<<"successfully written!"<<endl;}
   if(PSFlag)
   {cout<<"Equalization completed"<<endl;}
  }
  else
  {cout<<"fail to read data from previous buffer"<<endl;}

}

void Equalizer::Equalizing(FIFO<complex<float> >* pInpBuf,FIFO<complex<float> >* pOutBuf)
{
if(PSFlag)
{cout<<"Equalizing"<<endl;}
VpInpBuf=pInpBuf;
VpOutBuf=pOutBuf;
LSFreqDomain();
}


void Equalizer::Equalizing(FIFO<complex<float> >* pInpBuf,FIFO<complex<float> >* pOutBuf,complex<float>***pPCSI,float AWGNNo)
{
if(PSFlag)
{cout<<"Equalizing"<<endl;}
VpInpBuf=pInpBuf;
VpOutBuf=pOutBuf;
VpCSI=pPCSI;
float No=AWGNNo;

  int MDFT=MDFTPerUser;
  int NumLayer=NumLayerPerUser;

  int SCLoc = (*VpUser).SCLoc;

  bool ReadFlag = (*VpInpBuf).Read(pInpData);

  if(ReadFlag)
  {

    for(int m=0;m<MDFT;m++)
    {
////////////////////////// HTranspose////////////////////////
   complex<float>** pHTranspose=new complex<float>*[NumLayer];
   for(int layer=0;layer<NumLayer;layer++){*(pHTranspose+layer)=new complex<float>[NumRxAntenna];}
   
   for(int layer=0;layer<NumLayer;layer++)
   {
     for(int nrx=0;nrx<NumRxAntenna;nrx++)
     {*(*(pHTranspose+layer)+nrx)=*(*(*(VpCSI+layer)+nrx)+(m+SCLoc));}
   }
//////////////////////////END HTranspose/////////////////////

//////////////////// Freq Domain Equalize received Data /////////////////
   FDMMSEEqualization(pHTranspose,m,NumLayer,No);
//////////////////// END Freq Domain Equalize received Data//////////////////


/////////////////////////////////Delete///////////////////////////////////////    
////////////////////////// HTranspose////////////////////////
   for(int layer=0;layer<NumLayer;layer++){delete[]  *(pHTranspose+layer);}
   delete[] pHTranspose;
//////////////////////////END HTranspose/////////////////////   
    }
   bool WriteFlag=(*VpOutBuf).Write(pOutData);
//   if(WriteFlag){cout<<"successfully written!"<<endl;}
    if(PSFlag)
    {cout<<"Equalization completed"<<endl;}
  }
  else
  {cout<<"fail to read data from previous buffer"<<endl;}

}

Equalizer::~Equalizer()
{

  for(int m=0;m<MDFTPerUser;m++)
  {
    for(int l=0;l<NumLayerPerUser;l++)
    {
     delete[] *(*(pEqW+m)+l);
    }

    for(int nrx=0;nrx<NumRxAntenna;nrx++)
    {
     delete[] *(*(pHdm+m)+nrx);
    }
  }

  for(int m=0;m<MDFTPerUser;m++)
  {
    delete[] *(pEqW+m);
    delete[] *(pHdm+m);
  }

delete[] pEqW;
delete[] pHdm;

  for(int s=0;s<NumULSymbSF*NumLayerPerUser;s++){delete[] *(pInpData+s);}
  delete[] pInpData;
  for(int s=0;s<(NumULSymbSF-2)*NumLayerPerUser;s++){delete[] *(pOutData+s);}
  delete[] pOutData;


}
