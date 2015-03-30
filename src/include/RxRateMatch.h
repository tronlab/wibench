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
#include <cmath>
#include "BSPara.h"
#include "FIFO.h"
#include "SubblockInterleaver_lte.h"
#include <iostream>

using namespace std;
class RxRateMatch
{
private:
bool PSFlag;
bool BufFlag;
int NumULSymbSF;

int MDFT;
int MQAM;
int NumLayer;

int DataLength;

int Rate;

SubblockInterleaver_lte<int,float> SbDeInterleaver;

int* pLengthSet;
int LastBlockLen;
int NumBlock;
int EncDataLen;


float* pLLRin;
float* pLLRout;
int *pHD;
public:
int InBufSz[2];
int OutBufSz[2];

  RxRateMatch(BSPara* pBS);
  void RxRateMatching(FIFO<float>* pInpBuf,FIFO<float>* pOutBuf);
  int* GetpHD(void) const {return pHD;}
  ~RxRateMatch(void);
};
