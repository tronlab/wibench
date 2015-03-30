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

#include "ScrambDescrambMain.h"
int RANDOMSEED;
#define Descramb

#ifdef Scramb
int main()
{
cout<<"Scrambler"<<endl;
BSPara BS;
BS.initBSPara();
UserPara User(&BS);
Scramble SCRB(&User);
FIFO<int> SCRBIn(1,SCRB.InBufSz);
FIFO<int> SCRBOut(1,SCRB.OutBufSz);
//ReadInputFromFiles(&SCRBIn,(SCRB.InBufSz),"ScrambleInput");
//GeneRandomInput(&SCRBIn,SCRB.InBufSz,"ScrambleRandomInput");
GeneRandomInput(&SCRBIn,SCRB.InBufSz);
SCRB.Scrambling(&SCRBIn,&SCRBOut);
//WriteOutputToFiles(&SCRBOut,(SCRB.OutBufSz),"ScrambleOutput");
//WriteOutputToFiles(&SCRBOut,(SCRB.OutBufSz),"ScrambleRandomOutput");
ReadOutput(&SCRBOut,(SCRB.OutBufSz));
return 0;
}
#endif

#ifdef Descramb
int main()
{
cout<<"Descrambler"<<endl;
BSPara BS;
BS.initBSPara();
UserPara User(&BS);
Descramble DSCRB(&BS);
FIFO<float> DSCRBIn(1,DSCRB.InBufSz);
FIFO<float> DSCRBOut(1,DSCRB.OutBufSz);
//ReadInputFromFiles(&DSCRBIn,DSCRB.InBufSz,"DescrambleInput");
//GeneRandomInput(&DSCRBIn,DSCRB.InBufSz,"DescrambleRandomInput");
GeneRandomInput(&DSCRBIn,DSCRB.InBufSz);
DSCRB.Descrambling(&DSCRBIn,&DSCRBOut);
//WriteOutputToFiles(&DSCRBOut,(DSCRB.OutBufSz),"DescrambleOutput");
//WriteOutputToFiles(&DSCRBOut,(DSCRB.OutBufSz),"DescrambleRandomOutput");
ReadOutput(&DSCRBOut,(DSCRB.OutBufSz));
return 0;
}
#endif

