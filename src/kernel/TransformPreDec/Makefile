include ../../../common/makefile.config

SRC = TransformDecoder.cpp  TransformPrecoder.cpp  TransformPreDecMain.cpp 
SRC += $(UTIL_DIR)/BSPara.cpp $(UTIL_DIR)/FIFO.cpp $(UTIL_DIR)/gauss.cpp $(UTIL_DIR)/matrix.cpp $(UTIL_DIR)/UserPara.cpp $(UTIL_DIR)/GeneralFunc.cpp 

EXE = TPTD.out

CXXFLAGS += -lfftw3f

all: 
	$(CXX) $(SRC) $(CXXFLAGS) -o $(EXE)
	cp $(EXE) $(KERNEL_BUILD_DIR)	

clean:
	rm -rf $(EXE) *.o
