include ../../../common/makefile.config

FILE_LIST =  ModDemod/Demodulation.cpp ScrambDescramb/Descramble.cpp Equalizer/Equalizer_s.cpp Turbo/fsm.cpp Turbo/Interleaver_lte.cpp ModDemod/Modulation.cpp RateMatcher/RxRateMatch.cpp SCFDMA/SCFDMADemodulation.cpp SCFDMA/SCFDMAModulation.cpp ScrambDescramb/Scramble.cpp RateMatcher/SubblockInterleaver_lte.cpp SubCarrierMapDemap/SubCarrierDemap.cpp SubCarrierMapDemap/SubCarrierMap.cpp TransformPreDec/TransformDecoder.cpp TransformPreDec/TransformPrecoder.cpp Turbo/TurboDecoder_c.cpp Turbo/TurboEncoder_c.cpp RateMatcher/TxRateMatch.cpp

SRC = LTESys.cpp 
SRC +=$(CHANNEL_DIR)/Channel.cpp 
SRC += $(UTIL_DIR)/BSPara.cpp $(UTIL_DIR)/gauss.cpp $(UTIL_DIR)/GeneralFunc.cpp $(UTIL_DIR)/FIFO.cpp $(UTIL_DIR)/matrix.cpp $(UTIL_DIR)/UserPara.cpp 
SRC += $(foreach file, $(FILE_LIST),$(KERNEL_DIR)/$(file))

EXE = ltesys.out

CXXFLAGS += -lfftw3f

all: 
	$(CXX) $(SRC) $(CXXFLAGS)  -o $(EXE)
	cp $(EXE) $(APP_BUILD_DIR)

clean:
	rm -rf $(EXE) *.o 
