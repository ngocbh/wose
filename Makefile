CXX = g++-8
LDIR =libs/icu/lib/
IDIR =libs/icu/include
CFLAGS = -std=c++11 -Wall -O3 -msse2 -I${IDIR} -L${LDIR}


BIN = ./bin/wose_train
.PHONY: clean all

all: ./bin $(BIN)

./bin/wose_train: ./src/main.cpp ./src/utils/*.h ./src/data_processing/*.h ./src/classification/*.h

./bin:
	mkdir -p bin

LDFLAGS= -lm -licuio -licui18n -licuuc -licudata -Wno-unused-result -Wno-sign-compare -Wno-unused-variable -Wno-parentheses -Wno-format 
$(BIN) :
	$(CXX) $(CFLAGS) $(LDFLAGS) -o $@ $(filter %.cpp %.o %.c, $^)
$(OBJ) :
	$(CXX) -c $(CFLAGS) -o $@ $(firstword $(filter %.cpp %.c, $^) )

clean :
	rm -rf bin