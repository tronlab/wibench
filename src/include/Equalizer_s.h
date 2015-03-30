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
#include "UserPara.h"
#include "BSPara.h"
#include "FIFO.h"
#include "matrix.h"
#include "fftw3.h"
#include <iostream>
#include <cmath>


using namespace std;

class Equalizer
{
private:
bool PSFlag;
bool BufFlag;

int NumULSymbSF;
int MDFTPerUser;
int NumLayerPerUser;

int NumRxAntenna;
int EstimationMethod;

UserPara* VpUser;
complex<float>*** pDMRS;
FIFO<complex<float> >* VpInpBuf;
FIFO<complex<float> >* VpOutBuf;

complex<float>** pInpData;
complex<float>** pOutData;


complex<float> ***VpCSI;
complex<float> ***pEqW;
complex<float> ***pHdm;

void FDLSEstimation(complex<float>** pXt,complex<float>** pXtDagger,complex<float>** pYt,complex<float>** pHTranspose,int NumLayer);

void FDLSEqualization(complex<float>** pHTranspose,int m,int NumLayer);
void FDMMSEEqualization(complex<float>** pHTranspose,int m,int NumLayer,float No);

void LSFreqDomain(void);

public:
int InBufSz[2];
int OutBufSz[2];

  Equalizer(BSPara* pBS, UserPara* pUser);
  void Equalizing(FIFO<complex<float> >* pInpBuf,FIFO<complex<float> >* pOutBuf);
  void Equalizing(FIFO<complex<float> >* pInpBuf,FIFO<complex<float> >* pOutBuf,complex<float>***pPCSI,float AWGNNo);
  complex<float> ***GetpEqW(void) const {return pEqW;}
  complex<float> ***GetpHdm(void) const {return pHdm;}
  ~Equalizer(void);
};
