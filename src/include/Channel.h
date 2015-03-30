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
#include <ctime>
#include "gauss.h"
#include "fftw3.h"
#include "BSPara.h"
#include "FIFO.h"
#include "matrix.h"
#include <iostream>
#include <cmath>
using namespace std;
//////////////////// Single User ///////////////////////
class Channel
{
private:
bool PSFlag; //processing status flag
bool BufFlag;

int ChannelLen;
int MaxTxAnt;
int MaxRxAnt;

int NumULSymbSF;

int NIFFT;
int CPLen;
int NumLayerPerUser;

int NumRxAntenna;

int SpatialCorrFactor; // 0: low  correlation   1: medium correlation   2: high correlation
float R_alpha;
float R_beta;

int ChannelType;
bool AWGNFlag;

float FadingSigma;
float AWGNSigma;

complex<float>*** pPC;

complex<float>*** pPCSI;

int CurNumLayer;
complex<float>** pInpData;
complex<float>** pOutData;

complex<float>** VpInpData;
complex<float>** VpOutData;
FIFO<complex<float> >* VpOutBuf; 
void ApplyFading(void);
void ApplyGaussianRandomChannel(void);
float EVAAmplitude[5];
void ApplyEVA(void);
void ApplyPredefinedChannel(void);
void ApplyDeltaChannel(void);
void ApplyAWGN(complex<float>** pInMatrix);

public:
int InBufSz[2];
int OutBufSz[2];

  Channel(BSPara* pBS);
  void ApplyChannel(FIFO<complex<float> >* pInpBuf,FIFO<complex<float> >* pOutBuf);
  void ApplyChannel(FIFO<complex<float> >* pInpBuf,FIFO<complex<float> >* pOutBuf,float AWGNSigma);
  complex<float>*** GetpPCSI(void)const{return pPCSI;}
  float GetAWGNNo(void) const {return 2.0*pow(AWGNSigma,2.0);};
  ~Channel(void);
};
