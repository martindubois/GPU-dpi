
# Author     KMS - Martin Dubois, ing.
# Copyright  (C) 2019 KMS. All right reserved.
# Product    GPU-dpi
# File       User.mk

# CODE REVIEW  2019-06-11  KMS - Martin Dubois, ing.

CUDA_FOLDER = /usr/local/cuda-10.0
IMPORT_FOLDER = ../Import

# COMPILE_FLAGS = -fpic -ggdb -O2
COMPILE_FLAGS = -DNDEBUG -fpic -ggdb -O2

BINARY_IMPORT = -L $(IMPORT_FOLDER)/Binaries
INCLUDE_IMPORT = -I $(IMPORT_FOLDER)/Includes
KMS_LIB_A = $(IMPORT_FOLDER)/Libraries/KmsLib.a
