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
#include "Interleaver_lte.h"
#include <iostream>
using namespace std;
template <class U, class T>
Interleaver_lte<U,T>::Interleaver_lte(void)
{
	int Kvalue;
//*********************START Build parameter_table*********************//
	for(int i=0;i<60;i++)
	{
		Kvalue = 40+i*8;
		paraTable[i][0] = Kvalue;
	}
	for(int i=0+60;i<32+60;i++)
	{
		Kvalue = Kvalue+16;
		paraTable[i][0] = Kvalue;
	}
	for(int i=0+60+32;i<32+60+32;i++)
	{
		Kvalue = Kvalue+32;
		paraTable[i][0] = Kvalue;
	}
	for(int i=0+60+32+32;i<188;i++)
	{
		Kvalue = Kvalue+64;
		paraTable[i][0] = Kvalue;
	}
//*************************** Set f1********************************//
	paraTable[0][1] = 3;paraTable[1][1] = 7;paraTable[2][1] = 19;paraTable[3][1] = 7;
	paraTable[4][1] = 7;paraTable[5][1] = 11;paraTable[6][1] = 5;paraTable[7][1] = 11;
	paraTable[8][1] = 7;paraTable[9][1] = 41;paraTable[10][1] = 103;paraTable[11][1] = 15;
	paraTable[12][1] = 9;paraTable[13][1] = 17;paraTable[14][1] = 9;paraTable[15][1] = 21;
	paraTable[16][1] = 101;paraTable[17][1] = 21;paraTable[18][1] = 57;paraTable[19][1] = 23;
	paraTable[20][1] = 13;paraTable[21][1] = 27;paraTable[22][1] = 11;paraTable[23][1] = 27;
	paraTable[24][1] = 85;paraTable[25][1] = 29;paraTable[26][1] = 33;paraTable[27][1] = 15;
	paraTable[28][1] = 17;paraTable[29][1] = 33;paraTable[30][1] = 103;paraTable[31][1] = 19;
	paraTable[32][1] = 19;paraTable[33][1] = 37;paraTable[34][1] = 19;paraTable[35][1] = 21;
	paraTable[36][1] = 21;paraTable[37][1] = 115;paraTable[38][1] = 193;paraTable[39][1] = 21;
	paraTable[40][1] = 133;paraTable[41][1] = 81;paraTable[42][1] = 45;paraTable[43][1] = 23;
	paraTable[44][1] = 243;paraTable[45][1] = 151;paraTable[46][1] = 155;paraTable[47][1] = 25;
	paraTable[48][1] = 51;paraTable[49][1] = 47;paraTable[50][1] = 91;paraTable[51][1] = 29;
	paraTable[52][1] = 29;paraTable[53][1] = 247;paraTable[54][1] = 29;paraTable[55][1] = 89;
	paraTable[56][1] = 91;paraTable[57][1] = 157;paraTable[58][1] = 55;paraTable[59][1] = 31;
	paraTable[60][1] = 17;paraTable[61][1] = 35;paraTable[62][1] = 227;paraTable[63][1] = 65;
	paraTable[64][1] = 19;paraTable[65][1] = 37;paraTable[66][1] = 41;paraTable[67][1] = 39;
	paraTable[68][1] = 185;paraTable[69][1] = 43;paraTable[70][1] = 21;paraTable[71][1] = 155;
	paraTable[72][1] = 79;paraTable[73][1] = 139;paraTable[74][1] = 23;paraTable[75][1] = 217;
	paraTable[76][1] = 25;paraTable[77][1] = 17;paraTable[78][1] = 127;paraTable[79][1] = 25;
	paraTable[80][1] = 239;paraTable[81][1] = 17;paraTable[82][1] = 137;paraTable[83][1] = 215;
	paraTable[84][1] = 29;paraTable[85][1] = 15;paraTable[86][1] = 147;paraTable[87][1] = 29;
	paraTable[88][1] = 59;paraTable[89][1] = 65;paraTable[90][1] = 55;paraTable[91][1] = 31;
	paraTable[92][1] = 17;paraTable[93][1] = 171;paraTable[94][1] = 67;paraTable[95][1] = 35;
	paraTable[96][1] = 19;paraTable[97][1] = 39;paraTable[98][1] = 19;paraTable[99][1] = 199;
	paraTable[100][1] = 21;paraTable[101][1] = 211;paraTable[102][1] = 21;paraTable[103][1] = 43;
	paraTable[104][1] = 149;paraTable[105][1] = 45;paraTable[106][1] = 49;paraTable[107][1] = 71;
	paraTable[108][1] = 13;paraTable[109][1] = 17;paraTable[110][1] = 25;paraTable[111][1] = 183;
	paraTable[112][1] = 55;paraTable[113][1] = 127;paraTable[114][1] = 27;paraTable[115][1] = 29;
	paraTable[116][1] = 29;paraTable[117][1] = 57;paraTable[118][1] = 45;paraTable[119][1] = 31;
	paraTable[120][1] = 59;paraTable[121][1] = 185;paraTable[122][1] = 113;paraTable[123][1] = 31;
	paraTable[124][1] = 17;paraTable[125][1] = 171;paraTable[126][1] = 209;paraTable[127][1] = 253;
	paraTable[128][1] = 367;paraTable[129][1] = 265;paraTable[130][1] = 181;paraTable[131][1] = 39;
	paraTable[132][1] = 27;paraTable[133][1] = 127;paraTable[134][1] = 143;paraTable[135][1] = 43;
	paraTable[136][1] = 29;paraTable[137][1] = 45;paraTable[138][1] = 157;paraTable[139][1] = 47;
	paraTable[140][1] = 13;paraTable[141][1] = 111;paraTable[142][1] = 443;paraTable[143][1] = 51;
	paraTable[144][1] = 51;paraTable[145][1] = 451;paraTable[146][1] = 257;paraTable[147][1] = 57;
	paraTable[148][1] = 313;paraTable[149][1] = 271;paraTable[150][1] = 179;paraTable[151][1] = 331;
	paraTable[152][1] = 363;paraTable[153][1] = 375;paraTable[154][1] = 127;paraTable[155][1] = 31;
	paraTable[156][1] = 33;paraTable[157][1] = 43;paraTable[158][1] = 33;paraTable[159][1] = 477;
	paraTable[160][1] = 35;paraTable[161][1] = 233;paraTable[162][1] = 357;paraTable[163][1] = 337;
	paraTable[164][1] = 37;paraTable[165][1] = 71;paraTable[166][1] = 71;paraTable[167][1] = 37;
	paraTable[168][1] = 39;paraTable[169][1] = 127;paraTable[170][1] = 39;paraTable[171][1] = 39;
	paraTable[172][1] = 31;paraTable[173][1] = 113;paraTable[174][1] = 41;paraTable[175][1] = 251;
	paraTable[176][1] = 43;paraTable[177][1] = 21;paraTable[178][1] = 43;paraTable[179][1] = 45;
	paraTable[180][1] = 45;paraTable[181][1] = 161;paraTable[182][1] = 89;paraTable[183][1] = 323;
	paraTable[184][1] = 47;paraTable[185][1] = 23;paraTable[186][1] = 47;paraTable[187][1] = 263;
//***************************End Set f1********************************//

//*************************** Set f2********************************//
	paraTable[0][2] = 10;paraTable[1][2] = 12;paraTable[2][2] = 42;paraTable[3][2] = 16;
	paraTable[4][2] = 18;paraTable[5][2] = 20;paraTable[6][2] = 22;paraTable[7][2] = 24;
	paraTable[8][2] = 26;paraTable[9][2] = 84;paraTable[10][2] = 90;paraTable[11][2] = 32;
	paraTable[12][2] = 34;paraTable[13][2] = 108;paraTable[14][2] = 38;paraTable[15][2] = 120;
	paraTable[16][2] = 84;paraTable[17][2] = 44;paraTable[18][2] = 46;paraTable[19][2] = 48;
	paraTable[20][2] = 50;paraTable[21][2] = 52;paraTable[22][2] = 36;paraTable[23][2] = 56;
	paraTable[24][2] = 58;paraTable[25][2] = 60;paraTable[26][2] = 62;paraTable[27][2] = 32;
	paraTable[28][2] = 198;paraTable[29][2] = 68;paraTable[30][2] = 210;paraTable[31][2] = 36;
	paraTable[32][2] = 74;paraTable[33][2] = 76;paraTable[34][2] = 78;paraTable[35][2] = 120;
	paraTable[36][2] = 82;paraTable[37][2] = 84;paraTable[38][2] = 86;paraTable[39][2] = 44;
	paraTable[40][2] = 90;paraTable[41][2] = 46;paraTable[42][2] = 94;paraTable[43][2] = 48;
	paraTable[44][2] = 98;paraTable[45][2] = 40;paraTable[46][2] = 102;paraTable[47][2] = 52;
	paraTable[48][2] = 106;paraTable[49][2] = 72;paraTable[50][2] = 110;paraTable[51][2] = 168;
	paraTable[52][2] = 114;paraTable[53][2] = 58;paraTable[54][2] = 118;paraTable[55][2] = 180;
	paraTable[56][2] = 122;paraTable[57][2] = 62;paraTable[58][2] = 84;paraTable[59][2] = 64;
	paraTable[60][2] = 66;paraTable[61][2] = 68;paraTable[62][2] = 420;paraTable[63][2] = 96;
	paraTable[64][2] = 74;paraTable[65][2] = 76;paraTable[66][2] = 234;paraTable[67][2] = 80;
	paraTable[68][2] = 82;paraTable[69][2] = 252;paraTable[70][2] = 86;paraTable[71][2] = 44;
	paraTable[72][2] = 120;paraTable[73][2] = 92;paraTable[74][2] = 94;paraTable[75][2] = 48;
	paraTable[76][2] = 98;paraTable[77][2] = 80;paraTable[78][2] = 102;paraTable[79][2] = 52;
	paraTable[80][2] = 106;paraTable[81][2] = 48;paraTable[82][2] = 110;paraTable[83][2] = 112;
	paraTable[84][2] = 114;paraTable[85][2] = 58;paraTable[86][2] = 118;paraTable[87][2] = 60;
	paraTable[88][2] = 122;paraTable[89][2] = 124;paraTable[90][2] = 84;paraTable[91][2] = 64;
	paraTable[92][2] = 66;paraTable[93][2] = 204;paraTable[94][2] = 140;paraTable[95][2] = 72;
	paraTable[96][2] = 74;paraTable[97][2] = 76;paraTable[98][2] = 78;paraTable[99][2] = 240;
	paraTable[100][2] = 82;paraTable[101][2] = 252;paraTable[102][2] = 86;paraTable[103][2] = 88;
	paraTable[104][2] = 60;paraTable[105][2] = 92;paraTable[106][2] = 846;paraTable[107][2] = 48;
	paraTable[108][2] = 28;paraTable[109][2] = 80;paraTable[110][2] = 102;paraTable[111][2] = 104;
	paraTable[112][2] = 954;paraTable[113][2] = 96;paraTable[114][2] = 110;paraTable[115][2] = 112;
	paraTable[116][2] = 114;paraTable[117][2] = 116;paraTable[118][2] = 354;paraTable[119][2] = 120;
	paraTable[120][2] = 610;paraTable[121][2] = 124;paraTable[122][2] = 420;paraTable[123][2] = 64;
	paraTable[124][2] = 66;paraTable[125][2] = 136;paraTable[126][2] = 420;paraTable[127][2] = 216;
	paraTable[128][2] = 444;paraTable[129][2] = 456;paraTable[130][2] = 468;paraTable[131][2] = 80;
	paraTable[132][2] = 164;paraTable[133][2] = 504;paraTable[134][2] = 172;paraTable[135][2] = 88;
	paraTable[136][2] = 300;paraTable[137][2] = 92;paraTable[138][2] = 188;paraTable[139][2] = 96;
	paraTable[140][2] = 28;paraTable[141][2] = 240;paraTable[142][2] = 204;paraTable[143][2] = 104;
	paraTable[144][2] = 212;paraTable[145][2] = 192;paraTable[146][2] = 220;paraTable[147][2] = 336;
	paraTable[148][2] = 228;paraTable[149][2] = 232;paraTable[150][2] = 236;paraTable[151][2] = 120;
	paraTable[152][2] = 244;paraTable[153][2] = 248;paraTable[154][2] = 168;paraTable[155][2] = 64;
	paraTable[156][2] = 130;paraTable[157][2] = 264;paraTable[158][2] = 134;paraTable[159][2] = 408;
	paraTable[160][2] = 138;paraTable[161][2] = 280;paraTable[162][2] = 142;paraTable[163][2] = 480;
	paraTable[164][2] = 146;paraTable[165][2] = 444;paraTable[166][2] = 120;paraTable[167][2] = 152;
	paraTable[168][2] = 462;paraTable[169][2] = 234;paraTable[170][2] = 158;paraTable[171][2] = 80;
	paraTable[172][2] = 96;paraTable[173][2] = 902;paraTable[174][2] = 166;paraTable[175][2] = 336;
	paraTable[176][2] = 170;paraTable[177][2] = 86;paraTable[178][2] = 174;paraTable[179][2] = 176;
	paraTable[180][2] = 178;paraTable[181][2] = 120;paraTable[182][2] = 182;paraTable[183][2] = 184;
	paraTable[184][2] = 186;paraTable[185][2] = 94;paraTable[186][2] = 190;paraTable[187][2] = 480;
//*************************** Set f2********************************//
//********************* END Build parameter_table *********************//
	
}

//***************************** Start Interleaving Function ************************************//
template<class U, class T>
void Interleaver_lte<U,T>::Interleaving(U SeqLen,T *pInpSeq,T *pOutSeq)
{
//************************ Permutation Pattern *****************************//
K = SeqLen;
int f1,f2;
bool continueflag = 1;
for(int idx = 187;idx>=0&&continueflag;idx--)
{
	if(paraTable[idx][0]==SeqLen)
	{
		f1 = paraTable[idx][1];
		f2 = paraTable[idx][2];
		continueflag = 0;
	}
	else
	{}
}

pPi = new int[K];
long long tmp;
for(int idx=0;idx<K;idx++)
{
	tmp=((((long long)idx)*((long long)f1))+((((long long)idx)*((long long)idx))*((long long)f2)));
	*(pPi+idx)=(int)(tmp%((long long)K));
}

//*********************End Permutation Pattern *****************************//

//********************* Start Interleaving ****************************//
for(int idx=0;idx<K;idx++)
{
	*(pOutSeq+idx) = *(pInpSeq+(*(pPi+idx)));
}

//*********************** End  Interleaving **************************//
//************ Delete pointers ***************//
delete[] pPi;
}
//******************************* End Interleaving Function ************************************//


//**************************** START DeInterleaving Function ********************************//
template<class U,class T>
void Interleaver_lte<U,T>::DeInterleaving(U SeqLen,T *pInpSeq, T *pOutDeSeq)
{
//************************ Permutation Pattern *****************************//
K = SeqLen;
int f1,f2;
bool continueflag = 1;
for(int idx = 187;idx>=0&&continueflag;idx--)
{
	if(paraTable[idx][0]==SeqLen)
	{
		f1 = paraTable[idx][1];
		f2 = paraTable[idx][2];
		continueflag = 0;
	}
	else
	{}
}

pPi = new int[K];
long long tmp;
for(int idx=0;idx<K;idx++)
{
	tmp=((((long long)idx)*((long long)f1))+((((long long)idx)*((long long)idx))*((long long)f2)));
	*(pPi+idx)=(int)(tmp%((long long)K));
}

//*********************End Permutation Pattern *****************************//

//************************ DePermutation Pattern *****************************//
pdePi = new int[K];
for(int idx=0;idx<K;idx++)
{
	*(pdePi+(*(pPi+idx)))=idx;
}
//*********************End DePermutation Pattern *****************************//

//****************** Start deInterleaving **************************//
for(int idx=0;idx<K;idx++)
{
	*(pOutDeSeq+idx) = *(pInpSeq+(*(pdePi+idx)));
}
//****************** End deInterleaving ****************************//
//************ Delete pointers ***************//
delete[] pPi;
delete[] pdePi;
}
//**************************** End DeInterleaving Function **********************************//


template<class U,class T>
Interleaver_lte<U,T>::~Interleaver_lte(void)
{
}

template	Interleaver_lte<int,int>::Interleaver_lte(void);
template	void Interleaver_lte<int,int>::Interleaving(int SeqLen,int *pInpSeq,int *pOutSeq);
template	void Interleaver_lte<int,int>::DeInterleaving(int SeqLen,int *pInpSeq, int *pOutDeSeq);
template	Interleaver_lte<int,int>::~Interleaver_lte(void);

template	Interleaver_lte<int,float>::Interleaver_lte(void);
template	void Interleaver_lte<int,float>::Interleaving(int SeqLen,float *pInpSeq,float *pOutSeq);
template	void Interleaver_lte<int,float>::DeInterleaving(int SeqLen,float *pInpSeq, float *pOutDeSeq);
template	Interleaver_lte<int,float>::~Interleaver_lte(void);


template	Interleaver_lte<int,double>::Interleaver_lte(void);
template	void Interleaver_lte<int,double>::Interleaving(int SeqLen,double *pInpSeq,double *pOutSeq);
template	void Interleaver_lte<int,double>::DeInterleaving(int SeqLen,double *pInpSeq,double *pOutDeSeq);
template	Interleaver_lte<int,double>::~Interleaver_lte(void);
