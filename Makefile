# Makefile for scripting language project; intended for GCC
PROJECT		:= lits

RUN_ARGS	:=

CXX			:= g++
CXXFLAGS	:= -std=c++14 -c
LDFLAGS		:= # -static # does not work due to school computer issues
LDLIBS		:=

output_file	:= bin/$(PROJECT)
cpp_files	:= $(wildcard *.cpp)
hpp_files	:= $(wildcard *.hpp)
obj_files	:= $(cpp_files:%.cpp=bin/%.o)

all: build run

build: gen_bin $(output_file)

run:
	@echo "Running $(PROJECT)..."
	@./"$(output_file)" $(RUN_ARGS)

rebuild: clean build

gen_bin:
	@test -d bin || mkdir bin

clean:
	@echo "Cleaning binaries..."
	@test -d bin && rm -r bin

$(output_file): $(obj_files) $(hpp_files)
	@echo "Building $(output_file)..."
	@$(CXX) $(LDFLAGS) $(LDLIBS) $(obj_files) -o $@

bin/%.o: %.cpp
	@echo "Building $@..."
	@$(CXX) $(CXXFLAGS) $< -o $@

.PHONY: all build run clean rebuild gen_bin
