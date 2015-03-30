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
#include "BSPara.h"
/////////// Single User Base Station Parameters/////////////////

BSPara::BSPara()
{}

void BSPara::initBSPara(void)
{
NumULSymbSF=14;
NIFFT=128;
CPLen=9;
RSU=0;
RSV=0;

DMRSSymbPos[0]=3;
DMRSSymbPos[1]=10;

SCLocPattern=1;
MDFTPerUser=75;
MQAMPerUser=16;
NumLayerPerUser=2;
DataLengthPerUser=2368;

EstimationMethod=0;
NumRxAntenna=2;

ProcessingStatusFlag=false;
BSParameterPrintFlag=false;
BufferSizeFlag=false;
if(BSParameterPrintFlag)
{
cout<<"single user Uplink LTE parameter:"<<endl;
cout<<"Number of symbols per subframe="<<NumULSymbSF<<endl;
cout<<"SCFDMA IFFT length="<<NIFFT<<"   Cyclic Prefix length="<<CPLen<<endl;
cout<<"DFT length="<<MDFTPerUser<<endl;
switch(MQAMPerUser)
{
case 4:cout<<"Modulation scheme is QPSK"<<endl;break;
case 16:cout<<"Modulation scheme is 16QAM"<<endl;break;
case 64:cout<<"Modulation scheme is 64QAM"<<endl;break;
default:cout<<"Invalid modulation scheme"<<endl;
}

cout<<"Number of Transmitter antennas="<<NumLayerPerUser<<endl;
cout<<"Number of Receiver antennas="<<NumRxAntenna<<endl;
}

}

BSPara::~BSPara()
{
}
