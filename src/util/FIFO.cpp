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
#include "FIFO.h"
using namespace std;

template<class T>
FIFO<T>::FIFO()
{}

template<class T>
FIFO<T>::FIFO(int maxdata,int datachunksize[2])
{
	pWrite=0;
	pRead=0;
	MaxDataChunk=maxdata;
	NumDataChunk=0;
	DataChunkSize[0]=datachunksize[0];
	DataChunkSize[1]=datachunksize[1];


	switch(DataChunkSize[0])
	{
	case 1:
		p1DDataBuffer=new T[DataChunkSize[1]*MaxDataChunk];
		break;
	default:
		p2DDataBuffer=new T*[DataChunkSize[0]];
		for(int i=0;i<DataChunkSize[0];i++)
		{*(p2DDataBuffer+i)=new T[DataChunkSize[1]*MaxDataChunk];}
		break;
	}

}



template<class T>
bool FIFO<T>::Write(T* p1DData)
{



		if(NumDataChunk<MaxDataChunk)
		{
			for(int idx=0;idx<DataChunkSize[1];idx++)
			{*(p1DDataBuffer+pWrite*DataChunkSize[1]+idx)=*(p1DData+idx);}
			pWrite++;
			pWrite%=MaxDataChunk;
			NumDataChunk++;
			return true;
		}
		else
		{cout<<"Overflow"<<endl;return false;}

	
}

template<class T>
bool FIFO<T>::Write(T** p2DData)
{



	if(NumDataChunk<MaxDataChunk)
	{
			for(int d1=0;d1<DataChunkSize[0];d1++)
			{
				for(int idx=0;idx<DataChunkSize[1];idx++)
				{*(*(p2DDataBuffer+d1)+pWrite*DataChunkSize[1]+idx)=*(*(p2DData+d1)+idx);}
			}
		
			pWrite++;
			pWrite%=MaxDataChunk;
			NumDataChunk++;
			return true;
	}
	else
	{cout<<"Overflow"<<endl;return false;}

}

template<class T>
bool FIFO<T>::Read(T* pr1DData)
{

		if(NumDataChunk>0)
		{
			for(int idx=0;idx<DataChunkSize[1];idx++)
			{*(pr1DData+idx)=*(p1DDataBuffer+pRead*DataChunkSize[1]+idx);}
			pRead++;
			pRead%=MaxDataChunk;
			NumDataChunk--;
			return true;
		}
		else
		{cout<<"Underflow"<<endl;return false;}


}

template<class T>
bool FIFO<T>::Read(T** pr2DData)
{




		if(NumDataChunk>0)
		{

			for(int d1=0;d1<DataChunkSize[0];d1++)
			{
				for(int idx=0;idx<DataChunkSize[1];idx++)
				{*(*(pr2DData+d1)+idx)=*(*(p2DDataBuffer+d1)+pRead*DataChunkSize[1]+idx);}
			}

			pRead++;
			pRead%=MaxDataChunk;
			NumDataChunk--;
			return true;
		}
		else
		{cout<<"Underflow"<<endl;return false;}


}

template<class T>
FIFO<T>::~FIFO(void)
{

	switch(DataChunkSize[0])
	{
	case 1:
		delete[] p1DDataBuffer;
		break;
	default:	
		for(int i=0;i<DataChunkSize[0];i++)
		{delete[] *(p2DDataBuffer+i);}
		delete[] p2DDataBuffer;
		break;
	}

}



template FIFO<int>::FIFO(void);
template FIFO<int>::FIFO(int maxdata,int datachunksize[2]);
template bool FIFO<int>::Write(int* p1DData);
template bool FIFO<int>::Write(int** p2DData);
template bool FIFO<int>::Read(int* pr1DData);
template bool FIFO<int>::Read(int** pr2DData);
template FIFO<int>::~FIFO(void);

template FIFO<double>::FIFO(void);
template FIFO<double>::FIFO(int maxdata,int datachunksize[2]);
template bool FIFO<double>::Write(double* p1DData);
template bool FIFO<double>::Write(double** p2DData);
template bool FIFO<double>::Read(double* pr1DData);
template bool FIFO<double>::Read(double** pr2DData);
template FIFO<double>::~FIFO(void);

template FIFO<float>::FIFO(void);
template FIFO<float>::FIFO(int maxdata,int datachunksize[2]);
template bool FIFO<float>::Write(float* p1DData);
template bool FIFO<float>::Write(float** p2DData);
template bool FIFO<float>::Read(float* pr1DData);
template bool FIFO<float>::Read(float** pr2DData);
template FIFO<float>::~FIFO(void);

template FIFO<complex<double> >::FIFO(void);
template FIFO<complex<double> >::FIFO(int maxdata,int datachunksize[2]);
template bool FIFO<complex<double> >::Write(complex<double>* p1DData);
template bool FIFO<complex<double> >::Write(complex<double>** p2DData);
template bool FIFO<complex<double> >::Read(complex<double>* pr1DData);
template bool FIFO<complex<double> >::Read(complex<double>** pr2DData);
template FIFO<complex<double> >::~FIFO(void);

template FIFO<complex<float> >::FIFO(void);
template FIFO<complex<float> >::FIFO(int maxdata,int datachunksize[2]);
template bool FIFO<complex<float> >::Write(complex<float>* p1DData);
template bool FIFO<complex<float> >::Write(complex<float>** p2DData);
template bool FIFO<complex<float> >::Read(complex<float>* pr1DData);
template bool FIFO<complex<float> >::Read(complex<float>** pr2DData);
template FIFO<complex<float> >::~FIFO(void);

