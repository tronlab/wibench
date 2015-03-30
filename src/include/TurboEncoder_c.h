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
 *          Achilleas Anastasopoulos 
 *          Qi Zheng
 */
#pragma once
#include <complex>
#include <iostream>
#include "fsm.h"
#include "Interleaver_lte.h"
#include "UserPara.h"
#include "FIFO.h"


using namespace std;

class TurboEncoder_c
{
private:
bool PSFlag;
bool BufFlag;
int InpBlockShift;
int OutBlockShift;
int NumBlock;
int DataLength;
int* pLengthSet;

int Rate;
int iSeqLength;

int* piSeq;
int** pcSeq;

int maxNum(int a,int b);
int maxElen(int num,int *p,int pos,int space);
public:
int InBufSz[2];
int OutBufSz[2];

  TurboEncoder_c(UserPara* pUser);
  void TurboEncoding(FIFO<int> *pInpBuf,FIFO<int> *pOutBuf);
  ~TurboEncoder_c(void);  
};












