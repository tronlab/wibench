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

#include "SCFDMAMain.h"
int RANDOMSEED;
#define SCFDMADemod

#ifdef SCFDMAMod
int main()
{
cout<<"SCFDMA Modulation"<<endl;
BSPara BS;
BS.initBSPara();
UserPara User(&BS);
SCFDMAModulation SCFM(&User);
FIFO<complex<float> > SCFMIn(1,SCFM.InBufSz);
FIFO<complex<float> > SCFMOut(1,SCFM.OutBufSz);
//ReadInputFromFiles(&SCFMIn,(SCFM.InBufSz),"SCFDMAModulationInputReal","SCFDMAModulationInputImag");
//GeneRandomInput(&SCFMIn,SCFM.InBufSz,"SCFDMAModulationRandomInputReal","SCFDMAModulationRandomInputImag");
GeneRandomInput(&SCFMIn,SCFM.InBufSz);
SCFM.SCFDMAModulating(&SCFMIn,&SCFMOut);
//WriteOutputToFiles(&SCFMOut,(SCFM.OutBufSz),"SCFDMAModulationOutputReal","SCFDMAModulationOutputImag");
ReadOutput(&SCFMOut,(SCFM.OutBufSz));
return 0;
}
#endif

#ifdef SCFDMADemod
int main()
{
cout<<"SCFDMA Demodulation"<<endl;
BSPara BS;
BS.initBSPara();
UserPara User(&BS);
SCFDMADemodulation SCFD(&BS);
FIFO<complex<float> > SCFDIn(1,SCFD.InBufSz);
FIFO<complex<float> > SCFDOut(1,SCFD.OutBufSz);
//ReadInputFromFiles(&SCFDIn,(SCFD.InBufSz),"SCFDMADemodulationInputReal","SCFDMADemodulationInputImag");
//GeneRandomInput(&SCFDIn,SCFD.InBufSz,"SCFDMADemodulationRandomInputReal","SCFDMADemodulationRandomInputImag");
GeneRandomInput(&SCFDIn,SCFD.InBufSz);
SCFD.SCFDMADemodulating(&SCFDIn,&SCFDOut);
//WriteOutputToFiles(&SCFDOut,(SCFD.OutBufSz),"SCFDMADemodulationOutputReal","SCFDMADemodulationOutputImag");
ReadOutput(&SCFDOut,(SCFD.OutBufSz));
return 0;
}
#endif
