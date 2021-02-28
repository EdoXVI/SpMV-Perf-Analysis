FLAGS=-std=c++11 -pthread -o2
BIN_FOLDER=bin
SRC_FOLDER=src
INCLUDE=$(SRC_FOLDER)/options.hpp $(SRC_FOLDER)/utils.hpp
COMPILER=clang++

.PHONY: spmv

all: \
	spmv

spmv: $(INCLUDE) $(SRC_FOLDER)/spmv.cpp
	$(COMPILER) $(SRC_FOLDER)/spmv.cpp $(FLAGS) -o $(BIN_FOLDER)/spmv