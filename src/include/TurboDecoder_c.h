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
#include <cmath>
#include <stdexcept>
#include <complex>
#include <iostream>
#include "fsm.h"
#include "Interleaver_lte.h"
#include "UserPara.h"
#include "BSPara.h"
#include "FIFO.h"

using namespace std;


float min(float a,float b);
float min_star(float a,float b);


class TurboDecoder_c
{
private:
bool PSFlag;
bool BufFlag;

int Rate;
int DataLength;
int LastBlockLen;
int *pLengthSet;

int NumBlock;
int iSeqLength;
int InpBlockShift;
int OutBlockShift;

float* pLLR;
int* pData;

//*******Parameters*******//
int ST10; int ST1K;
int ST20; int ST2K;
int iterations;

float (*p2mymin)(float,float);
int D0;int D1; int D2;

//***************** siso function *****************//

void siso_algorithm_bb(int I,int S,int O,
                                        int *pNS,
                                        int *pOS,
                                        int **pPS,
                                        int **pPI,
                                        int K,
                                        int S0,int SK,
                                        bool POSTI,bool POSTO,
                                        float (*p2mymin)(float,float),
                                        float *priori,float *prioro,float *post);
//**************END siso function *****************//

//*************** pccc function ***************//
void pccc_decoder_bb(
                                        const fsm &FSM,
                                        int ST10, int ST1K, 
                                        int ST20, int ST2K,
                                        int D0,
                                        int D1,
                                        int D2,
                                        Interleaver_lte<int,float> &DInter, int blocklength, int iterations,
                                        float (*p2mymin)(float,float),
                                        float *cprioro, int *data);
//**************END pccc function**************//


public:
int InBufSz[2];
int OutBufSz[2];

  TurboDecoder_c(BSPara* pBS);
  void TurboDecoding(FIFO<float>* pInpBuf,FIFO<int>* pOutBuf);
  ~TurboDecoder_c();
  
};

