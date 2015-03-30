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
#include "SubblockInterleaver_lte.h"
#include <iostream>

using namespace std;

template<class U, class T>
SubblockInterleaver_lte<U,T>::SubblockInterleaver_lte(void)
{
Rate=3;
C_sb=32;
DummyValue = (T)1000000;
int arr[32]={0,16,8,24,4,20,12,28,2,18,10,26,6,22,14,30,1,17,9,25,5,21,13,29,3,19,11,27,7,23,15,31};
for(int i=0;i<32;i++){InterColumnPattern[i]=arr[i];}
}

template<class U, class T>
void SubblockInterleaver_lte<U,T>::SubblockInterleaving(U SeqLen,T **pInpMtr,T **pOutMtr)
{
D=SeqLen;
R_sb=(int)(ceil((((double)D)/((double)C_sb))));
K_pi=R_sb*C_sb;
NumDummy=K_pi-D;
int OutIdx;
//////////////////// Interleaving for i=0,1 ///////////////////////
T **pInterMatrix=new T*[R_sb];
for(int r=0;r<R_sb;r++){*(pInterMatrix+r)=new T[C_sb];}
T *VpInpSeq;
T *VpOutSeq;
for(int StrIdx=0;StrIdx<(Rate-1);StrIdx++)
{
  VpInpSeq=*(pInpMtr+StrIdx);
  VpOutSeq=*(pOutMtr+StrIdx);
  //////////////// write into matrix //////////////////
  for(int r=0;r<R_sb;r++)
  {
    for(int c=0;c<C_sb;c++)
    {
      int k=r*C_sb+c;
      if(k<NumDummy){*(*(pInterMatrix+r)+c)=DummyValue;}
      else{*(*(pInterMatrix+r)+c)=*(VpInpSeq+(k-NumDummy));}
    }
  }
  ////////////// END write into matrix ////////////////
  ////////////// Read out matrix ///////////////////////
  OutIdx=0;
  for(int c=0;c<C_sb;c++)
  {
    int col = InterColumnPattern[c];
    for(int r=0;r<R_sb;r++)
    {
      int k=col*R_sb+r;
      T v = *(*(pInterMatrix+r)+col);
      if(v==DummyValue){}
      else{*(VpOutSeq+OutIdx)=v;OutIdx++;}  
    }
  }  
  ////////////// END read out matrix////////////////////
}
for(int r=0;r<R_sb;r++){delete[] *(pInterMatrix+r);}
delete[] pInterMatrix;
////////////////////END Interleaving for i=0,1 ////////////////////
//////////////////// Interleaving for i=2 ///////////////////////
int *Pi=new int[K_pi];
T *pInterSeq=new T[K_pi];
VpInpSeq=*(pInpMtr+(Rate-1));
VpOutSeq=*(pOutMtr+(Rate-1));
for(int k=0;k<NumDummy;k++)
{*(pInterSeq+k)=DummyValue;}
for(int k=NumDummy;k<K_pi;k++)
{*(pInterSeq+k)=*(VpInpSeq+(k-NumDummy));}
//////////////// Pi //////////////////
for(int k=0;k<K_pi;k++)
{
  int idxP=(int)(floor((((double)(k))/((double)(R_sb)))));
  int idx = ((InterColumnPattern[idxP])+(C_sb*(k%R_sb))+1)%K_pi;
  *(Pi+k)=idx;
}
///////////////END Pi/////////////////
/////////////// Interleaving ////////////////////
OutIdx=0;
for(int k=0;k<K_pi;k++)
{
  T v = *(pInterSeq+(*(Pi+k)));
  if(v==DummyValue){}
  else{*(VpOutSeq+OutIdx)=v;OutIdx++;}
}
////////////////END interleaving/////////////////
delete[] pInterSeq;
delete[] Pi;
////////////////////END Interleaving for i=2 ////////////////////

}



template<class U, class T>
void SubblockInterleaver_lte<U,T>::SubblockDeInterleaving(U SeqLen,T **pInpMtr, T **pOutMtr)
{
D=SeqLen;
R_sb=(int)(ceil((((double)D)/((double)C_sb))));
K_pi=R_sb*C_sb;
NumDummy=K_pi-D;
int InIdx;
int OutIdx;
//////////////////// DeInterleaving for i=0,1 ///////////////////////
T **pInterMatrix=new T*[R_sb];
for(int r=0;r<R_sb;r++){*(pInterMatrix+r)=new T[C_sb];}

T *VpInpSeq;
T *VpOutSeq;
for(int StrIdx=0;StrIdx<(Rate-1);StrIdx++)
{
  VpInpSeq=*(pInpMtr+StrIdx);
  VpOutSeq=*(pOutMtr+StrIdx);
//////////////////// Write DeInter Matrix//////////////////////
  InIdx=0;
  //////////////// write into matrix //////////////////
  for(int r=0;r<R_sb;r++)
  {
    for(int c=0;c<C_sb;c++)
    {
      int k=r*C_sb+c;
      if(k<NumDummy){*(*(pInterMatrix+r)+c)=DummyValue;}
      else{*(*(pInterMatrix+r)+c)=(T)0;}
    }
  }
  ////////////// END write into matrix ////////////////
  ////////////// Read out matrix ///////////////////////
  for(int c=0;c<C_sb;c++)
  {
    int col = InterColumnPattern[c];
    for(int r=0;r<R_sb;r++)
    {
      int k=col*R_sb+r;
      T v = *(*(pInterMatrix+r)+col);
      if(v==DummyValue){}
      else
      {
      *(*(pInterMatrix+r)+col)=*(VpInpSeq+InIdx);      
      InIdx++;
      }  
    }
  }  
  ////////////// END read out matrix////////////////////
////////////////////END Write DeInter Matrix//////////////////////
//////////////////// Read Out DeInter Matrix//////////////////////
  OutIdx=0;
  for(int r=0;r<R_sb;r++)
  {
     for(int c=0;c<C_sb;c++)
     {
       T v = *(*(pInterMatrix+r)+c);
       if(v==DummyValue){}
       else
       {
        *(VpOutSeq+OutIdx)=*(*(pInterMatrix+r)+c);
        OutIdx++;
       }
     }
  }
///////////////////END Read Out DeInter Matrix////////////////////
}
for(int r=0;r<R_sb;r++){delete[] *(pInterMatrix+r);}
delete[] pInterMatrix;

////////////////////END DeInterleaving for i=0,1 ////////////////////
//////////////////// DeInterleaving for i=2 ///////////////////////
int *Pi=new int[K_pi];

T *pInterSeq=new T[K_pi];
VpInpSeq=*(pInpMtr+(Rate-1));
VpOutSeq=*(pOutMtr+(Rate-1));
for(int k=0;k<NumDummy;k++)
{*(pInterSeq+k)=DummyValue;}
//////////////// Pi & DePi//////////////////
for(int k=0;k<K_pi;k++)
{
  int idxP=(int)(floor((((double)(k))/((double)(R_sb)))));
  int idx = ((InterColumnPattern[idxP])+(C_sb*(k%R_sb))+1)%K_pi;
  *(Pi+k)=idx;
}
///////////////END Pi & DePi/////////////////
/////////////// DeInterleaving ////////////////////
InIdx=0;
for(int k=0;k<K_pi;k++)
{
  T v = *(pInterSeq+(*(Pi+k)));
  if(v==DummyValue){}
  else{*(pInterSeq+(*(Pi+k)))=*(VpInpSeq+InIdx);InIdx++;}
}
OutIdx=0;
for(int k=NumDummy;k<K_pi;k++)
{*(VpOutSeq+OutIdx)=*(pInterSeq+k);OutIdx++;}
////////////////END DeInterleaving/////////////////
delete[] pInterSeq;
delete[] Pi;

////////////////////END DeInterleaving for i=2 ////////////////////
}


template<class U, class T>
SubblockInterleaver_lte<U,T>::~SubblockInterleaver_lte(void)
{}

template SubblockInterleaver_lte<int,int>::SubblockInterleaver_lte(void);
template void SubblockInterleaver_lte<int,int>::SubblockInterleaving(int SeqLen,int **pInpMtr,int **pOutMtr);
template void SubblockInterleaver_lte<int,int>::SubblockDeInterleaving(int SeqLen,int **pInpMtr,int **pOutMtr);
template SubblockInterleaver_lte<int,int>::~SubblockInterleaver_lte(void);

template SubblockInterleaver_lte<int,float>::SubblockInterleaver_lte(void);
template void SubblockInterleaver_lte<int,float>::SubblockInterleaving(int SeqLen,float **pInpMtr,float **pOutMtr);
template void SubblockInterleaver_lte<int,float>::SubblockDeInterleaving(int SeqLen,float **pInpMtr,float **pOutMtr);
template SubblockInterleaver_lte<int,float>::~SubblockInterleaver_lte(void);

template SubblockInterleaver_lte<int,double>::SubblockInterleaver_lte(void);
template void SubblockInterleaver_lte<int,double>::SubblockInterleaving(int SeqLen,double **pInpMtr,double **pOutMtr);
template void SubblockInterleaver_lte<int,double>::SubblockDeInterleaving(int SeqLen,double **pInpMtr,double **pOutMtr);
template SubblockInterleaver_lte<int,double>::~SubblockInterleaver_lte(void);
