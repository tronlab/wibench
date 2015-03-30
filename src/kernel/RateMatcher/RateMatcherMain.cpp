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

#include "RateMatcherMain.h"
#define RxRateM
int RANDOMSEED;
#ifdef TxRateM
int main()
{
cout<<"Tx RateMatcher"<<endl;
BSPara BS;
BS.initBSPara();
UserPara User(&BS);
TxRateMatch TxRM(&User);
FIFO<int> TxRMIn(1,TxRM.InBufSz);
FIFO<int> TxRMOut(1,TxRM.OutBufSz);
//ReadInputFromFiles(&TxRMIn,(TxRM.InBufSz),"TxRateMatchInput");
//GeneRandomInput(&TxRMIn,TxRM.InBufSz,"TxRateMatchRandomInput");
GeneRandomInput(&TxRMIn,TxRM.InBufSz);
TxRM.TxRateMatching(&TxRMIn,&TxRMOut);
//WriteOutputToFiles(&TxRMOut,(TxRM.OutBufSz),"TxRateMatchRandomOutput");
ReadOutput(&TxRMOut,(TxRM.OutBufSz));
return 0;
}
#endif

#ifdef RxRateM
int main()
{
cout<<"Rx RateMatcher"<<endl;
BSPara BS;
BS.initBSPara();
UserPara User(&BS);
RxRateMatch RxRM(&BS);
FIFO<float> RxRMIn(1,RxRM.InBufSz);
FIFO<float> RxRMOut(1,RxRM.OutBufSz);
//ReadInputFromFiles(&RxRMIn,(RxRM.InBufSz),"RxRateMatchInput");
//GeneRandomInput(&RxRMIn,RxRM.InBufSz,"RxRateMatchRandomInput");
GeneRandomInput(&RxRMIn,RxRM.InBufSz);
RxRM.RxRateMatching(&RxRMIn,&RxRMOut);
//WriteOutputToFiles(&RxRMOut,(RxRM.OutBufSz),"RxRateMatchRandomOutput");
ReadOutput(&RxRMOut,(RxRM.OutBufSz));
return 0;
}
#endif
