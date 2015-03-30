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

#include "GeneralFunc.h"
extern int RANDOMSEED;

void ReadInputFromFiles(FIFO<int> *pIn,int Sz[2],const char *name)
{
  FILE *file;

  if((file=fopen(name,"r"))==NULL)
    throw std::runtime_error ("file open error\n");

  int *pReadIn=new int[(Sz[0]*Sz[1])];
  for(int i=0;i<(Sz[0]*Sz[1]);i++)
  {
    if(fscanf(file,"%d",(pReadIn+i)) == EOF)
    {
      if(ferror(file) != 0)
      throw std::runtime_error ("file read error\n");
    }

  }
  fclose(file);

  if(Sz[0]==1)
  {bool flag = (*pIn).Write(pReadIn);}
  else
  {
   int **pReadMatrix=new int*[Sz[0]];
   for(int r=0;r<Sz[0];r++){*(pReadMatrix+r)=new int[Sz[1]];}

   for(int r=0;r<Sz[0];r++)
   {
     for(int c=0;c<Sz[1];c++)
     {
       *(*(pReadMatrix+r)+c)=*(pReadIn+r*Sz[1]+c);
     }
   }
   bool flag = (*pIn).Write(pReadMatrix);
   for(int r=0;r<Sz[0];r++){delete[] *(pReadMatrix+r);}
   delete[] pReadMatrix;
  }

  delete[] pReadIn;
}


void ReadInputFromFiles(FIFO<float> *pIn,int Sz[2],const char *name)
{
  FILE *file;

  if((file=fopen(name,"r"))==NULL)
    throw std::runtime_error ("file open error\n");

  float *pReadIn=new float[(Sz[0]*Sz[1])];
  for(int i=0;i<(Sz[0]*Sz[1]);i++)
  {
    if(fscanf(file,"%f",(pReadIn+i)) == EOF)
    {
      if(ferror(file) != 0)
      throw std::runtime_error ("file read error\n");
    }

  }
  fclose(file);

  if(Sz[0]==1)
  {bool flag = (*pIn).Write(pReadIn);}
  else
  {
   float **pReadMatrix=new float*[Sz[0]];
   for(int r=0;r<Sz[0];r++){*(pReadMatrix+r)=new float[Sz[1]];}

   for(int r=0;r<Sz[0];r++)
   {
     for(int c=0;c<Sz[1];c++)
     {
       *(*(pReadMatrix+r)+c)=*(pReadIn+r*Sz[1]+c);
     }
   }
   bool flag = (*pIn).Write(pReadMatrix);
   for(int r=0;r<Sz[0];r++){delete[] *(pReadMatrix+r);}
   delete[] pReadMatrix;
  }

  delete[] pReadIn;
}

void ReadInputFromFiles(FIFO<complex<float> > *pIn,int Sz[2],const char *nameReal,const char *nameImag)
{
  FILE *file;

  if((file=fopen(nameReal,"r"))==NULL)
    throw std::runtime_error ("file open error\n");

  float *pReadInReal=new float[(Sz[0]*Sz[1])];
  for(int i=0;i<(Sz[0]*Sz[1]);i++)
  {
    if(fscanf(file,"%f",(pReadInReal+i)) == EOF)
    {
      if(ferror(file) != 0)
      throw std::runtime_error ("file read error\n");
    }

  }
  fclose(file);

  if((file=fopen(nameImag,"r"))==NULL)
    throw std::runtime_error ("file open error\n");

  float *pReadInImag=new float[(Sz[0]*Sz[1])];
  for(int i=0;i<(Sz[0]*Sz[1]);i++)
  {
    if(fscanf(file,"%f",(pReadInImag+i)) == EOF)
    {
      if(ferror(file) != 0)
      throw std::runtime_error ("file read error\n");
    }

  }
  fclose(file);

  if(Sz[0]==1)
  {
    complex<float> *pReadIn=new complex<float>[(Sz[0]*Sz[1])];
    for(int i=0;i<(Sz[0]*Sz[1]);i++)
    {*(pReadIn+i)=complex<float>((*(pReadInReal+i)),(*(pReadInImag+i)));}
    bool flag = (*pIn).Write(pReadIn);
    delete[] pReadIn;
  }
  else
  {
   complex<float> **pReadMatrix=new complex<float>*[Sz[0]];
   for(int r=0;r<Sz[0];r++){*(pReadMatrix+r)=new complex<float>[Sz[1]];}

   for(int r=0;r<Sz[0];r++)
   {
     for(int c=0;c<Sz[1];c++)
     {
       *(*(pReadMatrix+r)+c)=complex<float>((*(pReadInReal+r*Sz[1]+c)),(*(pReadInImag+r*Sz[1]+c)));
     }
   }
   bool flag = (*pIn).Write(pReadMatrix);

   for(int r=0;r<Sz[0];r++){delete[] *(pReadMatrix+r);}
   delete[] pReadMatrix;
  }

  delete[] pReadInReal;
  delete[] pReadInImag;
}



//////////////////// Random Generate Input and Write to Files ////////////////////////

void GeneRandomInput(FIFO<int> *pIn,int Sz[2],const char *name)
{
   FILE *fptr=NULL;
   int sd = -111;
   if(Sz[0]==1)
   {
     int *pRandom=new int[Sz[1]];

     fptr = fopen(name,"w+");
     for(int i=0;i<Sz[1];i++)
     {
       sd-=i;
       float v=(float)gauss1(&sd);
       if(v>0){*(pRandom+i)=1;}
       else{*(pRandom+i)=0;}
       fprintf(fptr,"%d\t",*(pRandom+i));
     }
     fclose(fptr);
     bool flag = (*pIn).Write(pRandom);

     delete[] pRandom;
   }
   else
   {
     int **pRandom = new int*[Sz[0]];
     for(int i=0;i<Sz[0];i++){*(pRandom+i)=new int[Sz[1]];}

     fptr = fopen(name,"w+");
     for(int i=0;i<Sz[0];i++)
     {
       for(int j=0;j<Sz[1];j++)
       {
         sd-=(i+j);
         float v=(float)gauss1(&sd);
         if(v>0){*(*(pRandom+i)+j)=1;}
         else{*(*(pRandom+i)+j)=0;}
         fprintf(fptr,"%d\t",*(*(pRandom+i)+j));
       }
       fprintf(fptr,"\n");
     }
     fclose(fptr);
     bool flag = (*pIn).Write(pRandom);

     for(int i=0;i<Sz[0];i++){delete[] *(pRandom+i);}
     delete[] pRandom;
   }
}


void GeneRandomInput(FIFO<float> *pIn,int Sz[2],const char *name)
{
    FILE *fptr=NULL;
    int sd = -111;
    if(Sz[0]==1)
    {
      float *pRandom=new float[Sz[1]];
 
      fptr = fopen(name,"w+");
      for(int i=0;i<Sz[1];i++)
      {
        sd-=i;
        *(pRandom+i)=(float)gauss1(&sd);
        fprintf(fptr,"%f\t",*(pRandom+i));
      }
      fclose(fptr);
      bool flag = (*pIn).Write(pRandom);
 
      delete[] pRandom;
    }
    else
    {
      float **pRandom = new float*[Sz[0]];
      for(int i=0;i<Sz[0];i++){*(pRandom+i)=new float[Sz[1]];}
 
      fptr = fopen(name,"w+");
      for(int i=0;i<Sz[0];i++)
      {
        for(int j=0;j<Sz[1];j++)
        {
          sd-=(i+j);
          *(*(pRandom+i)+j)=(float)gauss1(&sd);
          fprintf(fptr,"%f\t",*(*(pRandom+i)+j));
        }
        fprintf(fptr,"\n");
      }
      fclose(fptr);
      bool flag = (*pIn).Write(pRandom);
 
      for(int i=0;i<Sz[0];i++){delete[] *(pRandom+i);}
      delete[] pRandom;
    }
 }


void GeneRandomInput(FIFO<complex<float> > *pIn,int Sz[2],const char *nameReal,const char *nameImag)
{
    FILE *fptr_real=NULL;
    FILE *fptr_imag=NULL;
    int sd = -111;
    if(Sz[0]==1)
    {
      complex<float> *pRandom=new complex<float>[Sz[1]];

      fptr_real = fopen(nameReal,"w+");
      fptr_imag = fopen(nameImag,"w+");
      for(int i=0;i<Sz[1];i++)
      {
        sd-=i;
        float vr = (float)gauss1(&sd);
        sd-=222;
        float vi = (float)gauss1(&sd);
        *(pRandom+i)=complex<float>(vr,vi);
        fprintf(fptr_real,"%f\t",(*(pRandom+i)).real());
        fprintf(fptr_imag,"%f\t",(*(pRandom+i)).imag());
      }
      fclose(fptr_real);
      fclose(fptr_imag);
      bool flag = (*pIn).Write(pRandom);

      delete[] pRandom;
    }
    else
    {
      complex<float> **pRandom = new complex<float>*[Sz[0]];
      for(int i=0;i<Sz[0];i++){*(pRandom+i)=new complex<float>[Sz[1]];}

      fptr_real = fopen(nameReal,"w+");
      fptr_imag = fopen(nameImag,"w+");
      for(int i=0;i<Sz[0];i++)
      {
        for(int j=0;j<Sz[1];j++)
        {
          sd-=(i+j);
          float vr = (float)gauss1(&sd);
          sd-=333;
          float vi = (float)gauss1(&sd);
          *(*(pRandom+i)+j)=complex<float>(vr,vi);
          fprintf(fptr_real,"%f\t",(*(*(pRandom+i)+j)).real());
          fprintf(fptr_imag,"%f\t",(*(*(pRandom+i)+j)).imag());
        }
        fprintf(fptr_real,"\n");
        fprintf(fptr_imag,"\n");
      }
      fclose(fptr_real);
      fclose(fptr_imag);
      bool flag = (*pIn).Write(pRandom);
      
      for(int i=0;i<Sz[0];i++){delete[] *(pRandom+i);}
      delete[] pRandom;
    }
 }


////////////////////END Random Generate Input and Write to Files ////////////////////////

///////////////////// Random Generate Input but NOT write to Files ///////////////////////
void GeneRandomInput(FIFO<int> *pIn,int Sz[2])
{
   int sd = -111;
   if(Sz[0]==1)
   {
     int *pRandom=new int[Sz[1]];

     for(int i=0;i<Sz[1];i++)
     {
       sd-=i;
       float v=(float)gauss1(&sd);
       if(v>0){*(pRandom+i)=1;}
       else{*(pRandom+i)=0;}
     }
     bool flag = (*pIn).Write(pRandom);

     delete[] pRandom;
   }
   else
   {
     int **pRandom = new int*[Sz[0]];
     for(int i=0;i<Sz[0];i++){*(pRandom+i)=new int[Sz[1]];}

     for(int i=0;i<Sz[0];i++)
     {
       for(int j=0;j<Sz[1];j++)
       {
         sd-=(i+j);
         float v=(float)gauss1(&sd);
         if(v>0){*(*(pRandom+i)+j)=1;}
         else{*(*(pRandom+i)+j)=0;}
       }
     }
     
     bool flag = (*pIn).Write(pRandom);

     for(int i=0;i<Sz[0];i++){delete[] *(pRandom+i);}
     delete[] pRandom;
   }
}

void GeneRandomInput(FIFO<float> *pIn,int Sz[2])
{
    int sd = -111;
    if(Sz[0]==1)
    {
      float *pRandom=new float[Sz[1]];
      for(int i=0;i<Sz[1];i++)
      {
        sd-=i;
        *(pRandom+i)=(float)gauss1(&sd);
      }
      bool flag = (*pIn).Write(pRandom);

      delete[] pRandom;
    }
    else
    {
      float **pRandom = new float*[Sz[0]];
      for(int i=0;i<Sz[0];i++){*(pRandom+i)=new float[Sz[1]];}

      for(int i=0;i<Sz[0];i++)
      {
        for(int j=0;j<Sz[1];j++)
        {
          sd-=(i+j);
          *(*(pRandom+i)+j)=(float)gauss1(&sd);
        }
      }
      bool flag = (*pIn).Write(pRandom);

      for(int i=0;i<Sz[0];i++){delete[] *(pRandom+i);}
      delete[] pRandom;
    }
 }


void GeneRandomInput(FIFO<complex<float> > *pIn,int Sz[2])
{
    int sd = -111;
    if(Sz[0]==1)
    {
      complex<float> *pRandom=new complex<float>[Sz[1]];

      for(int i=0;i<Sz[1];i++)
      {
        sd-=i;
        float vr = (float)gauss1(&sd);
        sd-=222;
        float vi = (float)gauss1(&sd);
        *(pRandom+i)=complex<float>(vr,vi);
      }
      bool flag = (*pIn).Write(pRandom);

      delete[] pRandom;
    }
    else
    {
      complex<float> **pRandom = new complex<float>*[Sz[0]];
      for(int i=0;i<Sz[0];i++){*(pRandom+i)=new complex<float>[Sz[1]];}

      for(int i=0;i<Sz[0];i++)
      {
        for(int j=0;j<Sz[1];j++)
        {
          sd-=(i+j);
          float vr = (float)gauss1(&sd);
          sd-=333;
          float vi = (float)gauss1(&sd);
          *(*(pRandom+i)+j)=complex<float>(vr,vi);
        }
      }
      bool flag = (*pIn).Write(pRandom);

      for(int i=0;i<Sz[0];i++){delete[] *(pRandom+i);}
      delete[] pRandom;
    }

}
/////////////////////END Random Generate Input but NOT write to Files ///////////////////////


////////////////////// Read Output and Write Output to Files ////////////////////////
void WriteOutputToFiles(FIFO<int> *pOut,int Sz[2],const char *name)
{
  FILE *fptr=NULL;
  if(Sz[0]==1)
  {
    int *pW=new int[Sz[1]];
    bool flag = (*pOut).Read(pW);
    fptr = fopen(name,"w+");
    for(int i=0;i<Sz[1];i++)
    {
      fprintf(fptr,"%d\t",*(pW+i));
    }
    fclose(fptr);
    delete[] pW;
  }
  else
  {
    int **pW=new int*[Sz[0]];
    for(int i=0;i<Sz[0];i++){*(pW+i)=new int[Sz[1]];}

    bool flag = (*pOut).Read(pW);
    fptr = fopen(name,"w+");
    for(int i=0;i<Sz[0];i++)
    {
      for(int j=0;j<Sz[1];j++)
      {
       fprintf(fptr,"%d\t",*(*(pW+i)+j));
      }
      fprintf(fptr,"\n");
    }
    fclose(fptr);

    for(int i=0;i<Sz[0];i++){delete[] *(pW+i);}
    delete[] pW;
  }
}



void WriteOutputToFiles(FIFO<float> *pOut,int Sz[2],const char *name)
{
  FILE *fptr=NULL;
  if(Sz[0]==1)
  {
    float *pW=new float[Sz[1]];
    bool flag = (*pOut).Read(pW);
    fptr = fopen(name,"w+");
    for(int i=0;i<Sz[1];i++)
    {
      fprintf(fptr,"%f\t",*(pW+i));
    }
    fclose(fptr);
    delete[] pW;
  }
  else
  {
    float **pW=new float*[Sz[0]];
    for(int i=0;i<Sz[0];i++){*(pW+i)=new float[Sz[1]];}

    bool flag = (*pOut).Read(pW);
    fptr = fopen(name,"w+");
    for(int i=0;i<Sz[0];i++)
    {
      for(int j=0;j<Sz[1];j++)
      {
       fprintf(fptr,"%f\t",*(*(pW+i)+j));
      }
      fprintf(fptr,"\n");
    }
    fclose(fptr);

    for(int i=0;i<Sz[0];i++){delete[] *(pW+i);}
    delete[] pW;
  }
}



void WriteOutputToFiles(FIFO<complex<float> > *pOut,int Sz[2],const char *nameReal,const char *nameImag)
{
  FILE *fptr_real=NULL;
  FILE *fptr_imag=NULL;
  if(Sz[0]==1)
  {
    complex<float> *pW=new complex<float>[Sz[1]];
    bool flag = (*pOut).Read(pW);
    fptr_real = fopen(nameReal,"w+");
    fptr_imag = fopen(nameImag,"w+");
    for(int i=0;i<Sz[1];i++)
    {
      fprintf(fptr_real,"%f\t",(*(pW+i)).real());
      fprintf(fptr_imag,"%f\t",(*(pW+i)).imag());
    }
    fclose(fptr_real);
    fclose(fptr_imag);
    delete[] pW;
  }
  else
  {
    complex<float> **pW=new complex<float>*[Sz[0]];
    for(int i=0;i<Sz[0];i++){*(pW+i)=new complex<float>[Sz[1]];}

    bool flag = (*pOut).Read(pW);
    fptr_real = fopen(nameReal,"w+");
    fptr_imag = fopen(nameImag,"w+");
    for(int i=0;i<Sz[0];i++)
    {
      for(int j=0;j<Sz[1];j++)
      {
       fprintf(fptr_real,"%f\t",(*(*(pW+i)+j)).real());
       fprintf(fptr_imag,"%f\t",(*(*(pW+i)+j)).imag());
      }
      fprintf(fptr_real,"\n");
      fprintf(fptr_imag,"\n");
    }
    fclose(fptr_real);
    fclose(fptr_imag);

    for(int i=0;i<Sz[0];i++){delete[] *(pW+i);}
    delete[] pW;
  }
}

//////////////////////END Read Output and Write Output to Files ////////////////////////

///////////////////////// Read Output /////////////////////////////
void ReadOutput(FIFO<int> *pOut,int Sz[])
{
  if(Sz[0]==1)
  {
    int *pW=new int[Sz[1]];
    bool flag = (*pOut).Read(pW);
    delete[] pW;
  }
  else
  {
    int **pW=new int*[Sz[0]];
    for(int i=0;i<Sz[0];i++){*(pW+i)=new int[Sz[1]];}

    bool flag = (*pOut).Read(pW);
    
    for(int i=0;i<Sz[0];i++){delete[] *(pW+i);}
    delete[] pW;
  }
}


void ReadOutput(FIFO<float> *pOut,int Sz[])
{
  if(Sz[0]==1)
  {
    float *pW=new float[Sz[1]];
    bool flag = (*pOut).Read(pW);
    delete[] pW;
  }
  else
  {
    float **pW=new float*[Sz[0]];
    for(int i=0;i<Sz[0];i++){*(pW+i)=new float[Sz[1]];}

    bool flag = (*pOut).Read(pW);
    
    for(int i=0;i<Sz[0];i++){delete[] *(pW+i);}
    delete[] pW;
  }
}


void ReadOutput(FIFO<complex<float> > *pOut,int Sz[])
{
  if(Sz[0]==1)
  {
    complex<float> *pW=new complex<float>[Sz[1]];
    bool flag = (*pOut).Read(pW);
    delete[] pW;
  }
  else
  {
    complex<float> **pW=new complex<float>*[Sz[0]];
    for(int i=0;i<Sz[0];i++){*(pW+i)=new complex<float>[Sz[1]];}

    bool flag = (*pOut).Read(pW);

    for(int i=0;i<Sz[0];i++){delete[] *(pW+i);}
    delete[] pW;
  }
}
//////////////////////////END Read Output//////////////////////////

void GenerateLTEChainInput(FIFO<int> *pDataSource, int DataK, int *pTxDS)
{
 int sd = -RANDOMSEED;

 for(int i=0;i<DataK;i++)
 {
    sd-=i;
    float v=(float)gauss1(&sd);
    if(v>0){*(pTxDS+i)=1;}
    else{*(pTxDS+i)=0;}
 }

 (*pDataSource).Write(pTxDS);
}

void ReadLTEChainOutput(FIFO<int> *pFileSink, int *pRxFS)
{
 (*pFileSink).Read(pRxFS);
}





