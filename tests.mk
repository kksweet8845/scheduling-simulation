SRC_DIR = ./
TESTCASE_DIR = ./testcase1/
TEST_DIR = ./unit_test/
CC = gcc
CFLAGS += -std=gnu99 -Wall -g -I./include

SRC = \
	task \
	resource \


# for task case
SRC_C = $(addsuffix .c, $(SRC))
OBJ	  = $(patsubst %.c, %.o, $(SRC_C))

TESTCASE = \
	config \
	task_set \

TESTCASE_SRC = $(wildcard $(TESTCASE_DIR)*.c)
TESTCASE_OBJ = $(patsubst $.c, $.o, $(TESTCASE_SRC))


TEST_SRC 	= $(wildcard $(TEST_DIR)*.c)
OBJ			+= $(TEST_SRC:%.c=%.o)


TEST_CASE = \
	task2suspend
.PHONY: clean


$(TESTCASE) : $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@



$(TEST_CASE) : $(OBJ) $(TEST_OBJ)
	@echo $(SRC)
	$(CC) $(CFLAGS) $^ -o $@



#include all .d files
-include $(DEP)


%.o : %.c
	$(CC) $(CFLAGS) -MMD -c $< -o $@
