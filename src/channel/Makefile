C = gcc
CC = g++
UTILPATH = ../Util/
INCPATH = ../INC/
SRC = $(UTILPATH)BSPara.cpp $(UTILPATH)FIFO.cpp $(UTILPATH)gauss.cpp $(UTILPATH)matrix.cpp $(UTILPATH)UserPara.cpp $(UTILPATH)GeneralFunc.cpp ChannelMain.cpp Channel.cpp 
EXE = Ch.out

all: $(SRC)
	$(CC) $(SRC) -I $(INCPATH) -o $(EXE) -lfftw3f -O3

clean:
	rm $(EXE) -rf
