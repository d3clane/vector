CC := g++

CFLAGS = -D _DEBUG -ggdb3 -std=c++17 -O0 -Wall -Wextra -Weffc++ \
		   -Wc++14-compat -Wmissing-declarations -Wcast-align -Wcast-qual -Wchar-subscripts 		  \
		   -Wconversion -Wctor-dtor-privacy -Wempty-body -Wfloat-equal      \
		   -Wformat-nonliteral -Wformat-security -Wformat=2 \
		   -Wnon-virtual-dtor -Woverloaded-virtual -Wpacked -Wpointer-arith -Winit-self \
		   -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo		  \
		   -Wstrict-overflow=2 \
		   -Wsuggest-override -Wswitch-default -Wswitch-enum 		  \
		   -Wundef -Wunreachable-code -Wunused -Wvariadic-macros   \
		   -Wno-missing-field-initializers -Wno-narrowing -Wno-old-style-cast -Wno-varargs 			  \
		   -Wstack-protector -fcheck-new -fsized-deallocation -fstack-protector -fstrict-overflow 	  \
		   -fno-omit-frame-pointer -Wlarger-than=8192 -Wstack-protector  						  \
		   -fPIE -Werror=vla									  

OUT_O_DIR := build
COMMONINC := -I./include -I./
LIB_INC   := -isystem/opt/homebrew/Cellar/sfml/2.6.1/include
LIB_LINK  := -L/opt/homebrew/Cellar/sfml/2.6.1/lib -lsfml-graphics -lsfml-window -lsfml-system

LDFLAGS   := $(LIB_LINK)

PROGRAM_DIR  := $(OUT_O_DIR)/bin
PROGRAM_NAME := molecules.out

TESTS = ./Tests

ROOT_DIR:=$(shell dirname $(realpath $(firstword $(MAKEFILE_LIST))))

override CFLAGS += $(COMMONINC)
override CFLAGS += $(LIB_INC)

CPPSRC = src/Exceptions.cpp src/main.cpp
		 

CPPOBJ := $(addprefix $(OUT_O_DIR)/,$(CPPSRC:.cpp=.o))
DEPS = $(CPPOBJ:.o=.d)

.PHONY: all
all: $(PROGRAM_DIR)/$(PROGRAM_NAME)

$(PROGRAM_DIR)/$(PROGRAM_NAME): $(CPPOBJ)
	@mkdir -p $(@D)
	$(CC) $^ -o $@ $(LDFLAGS)

$(CPPOBJ) : $(OUT_O_DIR)/%.o : %.cpp
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

$(DEPS) : $(OUT_O_DIR)/%.d : %.cpp
	@mkdir -p $(@D)
	$(CC) -E $(CFLAGS) $< -MM -MT $(@:.d=.o) > $@

#
#TESTFILES=$(wildcard $(TESTS)/*.dat)
#
#.PHONY: testrun
#testrun: $(TESTFILES)

#.PHONY: $(TESTFILES)
#$(TESTFILES): $(OUT_O_DIR)/LC.x
#	@$(ROOT_DIR)/runtest.sh $@ $(OUT_O_DIR)/LC.x

.PHONY: clean cleanAll
clean:
	rm -rf $(CPPOBJ) $(DEPS) $(OUT_O_DIR)/*.x $(OUT_O_DIR)/*.log

cleanAll: clean
	rm -rf $(PROGRAM_DIR)/$(PROGRAM_NAME)

NODEPS = clean

ifeq (0, $(words $(findstring $(MAKECMDGOALS), $(NODEPS))))
include $(DEPS)
endif