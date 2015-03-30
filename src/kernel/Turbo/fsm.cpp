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
#include "fsm.h"
#include <iostream>
using namespace std;

fsm::fsm(void)
{
	d_I=0;
	d_S=0;
	d_O=0;
}

fsm::fsm(const fsm &FSM)
{
	d_I = FSM.I();
	d_S = FSM.S();
	d_O = FSM.O();
	pd_NS = new int[d_I*d_S];
	for(int i=0;i<d_I*d_S;i++)
	{
		*(pd_NS+i) = *(FSM.pNS()+i);
	}
	pd_OS = new int[d_I*d_S];
	for(int i =0;i<d_I*d_S;i++)
	{
		*(pd_OS+i)=*(FSM.pOS()+i);
	}
	
	
	generate_PS_PI();
	generate_TM();
}

fsm::fsm(int I, int S, int O, const int *pNS, const int *pOS)
{
	d_I = I;
	d_S = S;
	d_O = O;
	pd_NS = new int[d_I*d_S];
	for(int i=0;i<d_I*d_S;i++)
	{
		*(pd_NS+i) = *(pNS+i);
	}
	pd_OS = new int[d_I*d_S];
	for(int i =0;i<d_I*d_S;i++)
	{
		*(pd_OS+i)=*(pOS+i);
	}

	generate_PS_PI();
	generate_TM();
}

fsm::fsm(const char *name) 
{
  FILE *fsmfile;

  if((fsmfile=fopen(name,"r"))==NULL) 
    throw std::runtime_error ("fsm::fsm(const char *name): file open error\n");
 

  if(fscanf(fsmfile,"%d %d %d\n",&d_I,&d_S,&d_O) == EOF) {
    if(ferror(fsmfile) != 0)
      throw std::runtime_error ("fsm::fsm(const char *name): file read error\n");
  }
  
  pd_NS = new int[d_I*d_S];
  pd_OS = new int[d_I*d_S];

  for(int i=0;i<d_S;i++) {
    for(int j=0;j<d_I;j++) {
      if(fscanf(fsmfile,"%d",(pd_NS+i*d_I+j)) == EOF) {
	if(ferror(fsmfile) != 0)
	  throw std::runtime_error ("fsm::fsm(const char *name): file read error\n");
      }
    }
  }
  for(int i=0;i<d_S;i++) {
    for(int j=0;j<d_I;j++) {
      if(fscanf(fsmfile,"%d",(pd_OS+i*d_I+j)) == EOF) {
	if(ferror(fsmfile) != 0)
	  throw std::runtime_error ("fsm::fsm(const char *name): file read error\n");
      }
    }
  }


 
  generate_PS_PI();
  generate_TM();
}




void fsm::generate_PS_PI()
{
	pd_PS = new int*[d_S];
	pd_PI = new int*[d_S];
	for(int i=0;i<d_S;i++)
	{
		*(pd_PS+i) = new int[d_I*d_S];
		*(pd_PI+i) = new int[d_I*d_S];
		int j=0;
		for(int ii=0;ii<d_S;ii++)
		{
			for(int jj=0;jj<d_I;jj++)
			{
				if(*(pd_NS+ii*d_I+jj)!=i) continue;
				*(*(pd_PS+i)+j)=ii;
				*(*(pd_PI+i)+j)=jj;
				j++;
			}
		}
		int *pStmp = new int[j];
		int *pItmp = new int[j];
		for(int t=0;t<j;t++)
		{
			*(pStmp+t) = *(*(pd_PS+i)+t);
			*(pItmp+t) = *(*(pd_PI+i)+t);
		}
		delete[] *(pd_PS+i);
		delete[] *(pd_PI+i);
		*(pd_PS+i)= new int[j];
		*(pd_PI+i)= new int[j];
		for(int t=0;t<j;t++)
		{
			*(*(pd_PS+i)+t) = *(pStmp+t);
			*(*(pd_PI+i)+t) = *(pItmp+t);
		}
		delete[] pStmp;
		delete[] pItmp;
	}
	
}

void fsm::generate_TM()
{
	pd_TMi = new int[d_S*d_S];
	pd_TMl = new int[d_S*d_S];

	for(int i=0;i<d_S*d_S;i++)
	{
		*(pd_TMi+i) = -1;
		*(pd_TMl+i) = d_S;
		if(i/d_S == i%d_S)
		{*(pd_TMl+i) = 0;}
	}

	for(int s=0;s<d_S;s++)
	{
		bool done=false;
		int attemps = 0;
		while(done==false&&attemps<d_S-1)
		{
			done = find_es(s);
			attemps++;
		}
		if(done==false)
		{
			cout<<"fsm::generate_TM(): FSM appears to be disconnected"<<endl;
			cout<<"state "<<s<<" cannot be reached from all other states"<<endl;
		}
	}
}

bool fsm::find_es(int es)
{
	bool done = true;
	for(int s=0;s<d_S;s++)
	{
	if(*(pd_TMl+s*d_S+es)<d_S)
		continue;
	int minl = d_S;
	int mini = -1;
	for(int i=0;i<d_I;i++)
	{
		if((1+(*(pd_TMl+((*(pd_NS+s*d_I+i))*d_S+es))))<minl)
		{
			minl = (1+(*(pd_TMl+((*(pd_NS+s*d_I+i))*d_S+es))));
			mini=i;
		}
	}
	if(mini!=-1)
	{
		*(pd_TMl+s*d_S+es)=minl;
		*(pd_TMi+s*d_S+es)=mini;
	}
	else
	{done=false;}
	}
	return done;
}

fsm::~fsm(void)
{
	delete[] pd_NS;

	delete[] pd_OS;

	for(int i=0;i<d_S;i++)
	{
		delete[] *(pd_PS+i);
		delete[] *(pd_PI+i);
	}
	delete[] pd_PS;
	delete[] pd_PI;

	delete[] pd_TMi;
	delete[] pd_TMl;

}
