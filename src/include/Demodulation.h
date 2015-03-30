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
#pragma once
#include <complex>
#include <cmath>
#include "BSPara.h"
#include "FIFO.h"
#include <iostream>
#include <string>
#include <fstream>
using namespace std;

class Demodulation
{
private:
bool PSFlag;
bool BufFlag;

bool SCFDMAFlag;

float** pBPSKtable;
float** pQPSKtable;
float** pQAM16table;
float** pQAM64table;

int NumULSymbSF;
int NumRxAntenna;

int MDFT;
int MQAM;
int NumLayer;


int NInfoBits;
int QAMLen;
int BitsPerSymb;
float** pTable;
complex<float>* pDecQAMSeq;
float* pLLR;

complex<float> ***VpEqW;
complex<float> ***VpHdm;
complex<float> ***pW;
float *pSCFactor;
complex<float> *pSC_Cntx;
float min(float* pV,int len);

public:
int InBufSz[2];
int OutBufSz[2];

  Demodulation(BSPara* pBS);
  void Demodulating(FIFO<complex<float> >* pInpBuf,FIFO<float>* pOutBuf,float awgnSigma);
  void Demodulating(FIFO<complex<float> >* pInpBuf,FIFO<float>* pOutBuf,complex<float>*** pEqW,complex<float>*** pHdm,float awgnSigma);
  ~Demodulation(void);
};
