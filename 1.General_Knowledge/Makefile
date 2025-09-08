CUR_DIR := .
INC_DIR := $(CUR_DIR)/inc 
SRC_DIR := $(CUR_DIR)/src
BIN_DIR := $(CUR_DIR)/bin
OBJ_DIR := $(CUR_DIR)/obj
LIB_DIR := $(CUR_DIR)/lib
LIB_STATIC_DIR := $(LIB_DIR)/static
LIB_SHARED_DIR := $(LIB_DIR)/shared
OBJ_STATIC_DIR := $(OBJ_DIR)/static
OBJ_SHARED_DIR := $(OBJ_DIR)/shared

LIB_NAME := strutils

all: static shared

static: create_obj 
	ar rcs $(LIB_STATIC_DIR)/libstrutils.a $(OBJ_STATIC_DIR)/bstrutils.o
	gcc   $(OBJ_DIR)/main.o  -L$(LIB_STATIC_DIR) -l$(LIB_NAME) -o $(BIN_DIR)/main_static

shared: create_obj
	gcc -shared $(OBJ_SHARED_DIR)/bstrutils.o -o $(LIB_SHARED_DIR)/lib$(LIB_NAME).so
	gcc  $(OBJ_DIR)/main.o -L$(LIB_SHARED_DIR) -l$(LIB_NAME) -o $(BIN_DIR)/main_shared

clean:
	rm -rf $(OBJ_DIR)/*.o
	rm -rf $(OBJ_SHARED_DIR)/*.o
	rm -rf $(OBJ_STATIC_DIR)/*.o
	rm -rf $(LIB_STATIC_DIR)/*.a
	rm -rf $(LIB_SHARED_DIR)/*.so
	rm -rf $(BIN_DIR)/main_static
	rm -rf $(BIN_DIR)/main_shared

create_obj: 
	gcc -c       $(SRC_DIR)/main.c        -o $(OBJ_DIR)/main.o -I $(INC_DIR)
	gcc -c       $(SRC_DIR)/bstrutils.c    -o $(OBJ_STATIC_DIR)/bstrutils.o -I $(INC_DIR)
	gcc -c -fPIC $(SRC_DIR)/bstrutils.c    -o $(OBJ_SHARED_DIR)/bstrutils.o -I $(INC_DIR)
