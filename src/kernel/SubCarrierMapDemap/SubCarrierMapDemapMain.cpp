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

#include "SubCarrierMapDemapMain.h"
int RANDOMSEED;
#define SCMapper

#ifdef SCMapper
int main()
{
cout<<"SubCarrier Mapper"<<endl;
BSPara BS;
BS.initBSPara();
UserPara User(&BS);
SubCarrierMap SCM(&User);
FIFO<complex<float> > SCMIn(1,SCM.InBufSz);
FIFO<complex<float> > SCMOut(1,SCM.OutBufSz);
//ReadInputFromFiles(&SCMIn,(SCM.InBufSz),"SubCarrierMapInputReal","SubCarrierMapInputImag");
//GeneRandomInput(&SCMIn,SCM.InBufSz,"SubCarrierMapRandomInputReal","SubCarrierMapRandomInputImag");
GeneRandomInput(&SCMIn,SCM.InBufSz);
SCM.SubCarrierMapping(&SCMIn,&SCMOut);
//WriteOutputToFiles(&SCMOut,(SCM.OutBufSz),"SubCarrierMapRandomOutputReal","SubCarrierMapRandomOutputImag");
ReadOutput(&SCMOut,(SCM.OutBufSz));
return 0;
}
#endif

#ifdef SCDemapper
int main()
{
cout<<"SubCarrier Demapper"<<endl;
BSPara BS;
BS.initBSPara();
UserPara User(&BS);
SubCarrierDemap SCD(&BS);
FIFO<complex<float> > SCDIn(1,SCD.InBufSz);
FIFO<complex<float> > SCDOut(1,SCD.OutBufSz);
//ReadInputFromFiles(&SCDIn,(SCD.InBufSz),"SubCarrierDemapInputReal","SubCarrierDemapInputImag");
//GeneRandomInput(&SCDIn,SCD.InBufSz,"SubCarrierDemapRandomInputReal","SubCarrierDemapRandomInputImag");
GeneRandomInput(&SCDIn,SCD.InBufSz);
SCD.SubCarrierDemapping(&SCDIn,&SCDOut);
//WriteOutputToFiles(&SCDOut,(SCD.OutBufSz),"SubCarrierDemapRandomOutputReal","SubCarrierDemapRandomOutputImag");
ReadOutput(&SCDOut,(SCD.OutBufSz));
return 0;
}
#endif
