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


#include "TransformPreDecMain.h"
int RANDOMSEED;
#define TransformDec

#ifdef TransformPre
int main()
{
cout<<"Transform Precoder"<<endl;
BSPara BS;
BS.initBSPara();
UserPara User(&BS);
TransformPrecoder TP(&User);
FIFO<complex<float> > TPIn(1,TP.InBufSz);
FIFO<complex<float> > TPOut(1,TP.OutBufSz);
//ReadInputFromFiles(&TPIn,(TP.InBufSz),"TransformPrecoderInputReal","TransformPrecoderInputImag");
//GeneRandomInput(&TPIn,TP.InBufSz,"TransformPrecoderRandomInputReal","TransformPrecoderRandomInputImag");
GeneRandomInput(&TPIn,TP.InBufSz);
TP.TransformPrecoding(&TPIn,&TPOut);
//WriteOutputToFiles(&TPOut,(TP.OutBufSz),"TransformPrecoderOutputReal","TransformPrecoderOutputImag");
//WriteOutputToFiles(&TPOut,(TP.OutBufSz),"TransformPrecoderRandomOutputReal","TransformPrecoderRandomOutputImag");
ReadOutput(&TPOut,(TP.OutBufSz));

return 0;
}
#endif

#ifdef TransformDec
int main()
{
cout<<"Transform Decoder"<<endl;
BSPara BS;
BS.initBSPara();
UserPara User(&BS);
TransformDecoder TD(&BS);
FIFO<complex<float> > TDIn(1,TD.InBufSz);
FIFO<complex<float> > TDOut(1,TD.OutBufSz);
//ReadInputFromFiles(&TDIn,(TD.InBufSz),"TransformDecoderInputReal","TransformDecoderInputImag");
//GeneRandomInput(&TDIn,TD.InBufSz,"TransformDecoderRandomInputReal","TransformDecoderRandomInputImag");
GeneRandomInput(&TDIn,TD.InBufSz);
TD.TransformDecoding(&TDIn,&TDOut);
//WriteOutputToFiles(&TDOut,(TD.OutBufSz),"TransformDecoderOutputReal","TransformDecoderOutputImag");
//WriteOutputToFiles(&TDOut,(TD.OutBufSz),"TransformDecoderRandomOutputReal","TransformDecoderRandomOutputImag");
ReadOutput(&TDOut,(TD.OutBufSz));
return 0;
}
#endif



