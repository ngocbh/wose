CXX = g++-8
LDIR =libs/icu/lib/
IDIR =libs/icu/include
CFLAGS = -std=c++11 -Wall -O3 -msse2


BIN = ./bin/wose_train ./bin/wose_segment
.PHONY: clean all

all: ./bin $(BIN)

./bin/wose_train: ./src/main.cpp ./src/utils/*.h ./src/data_processing/*.h ./src/classification/*.h ./src/frequent_pattern_mining/*.h ./src/segmentation/*.h
./bin/wose_segment: ./src/segment.cpp ./src/utils/*.h ./src/data_processing/*.h ./src/classification/*.h ./src/frequent_pattern_mining/*.h ./src/segmentation/*.h

./bin:
	mkdir -p bin

LDFLAGS= -lm -Wno-unused-result -Wno-sign-compare -Wno-unused-variable -Wno-parentheses -Wno-format 
$(BIN) :
	$(CXX) $(CFLAGS) $(LDFLAGS) -o $@ $(filter %.cpp %.o %.c, $^)
$(OBJ) :
	$(CXX) -c $(CFLAGS) -o $@ $(firstword $(filter %.cpp %.c, $^) )

clean :
	rm -rf bin