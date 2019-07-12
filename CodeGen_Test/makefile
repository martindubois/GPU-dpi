
# Author     KMS - Martin Dubois, ing.
# Copyright  (C) 2019 KMS. All rights reserved.
# Product    GPU-dpi
# File       CodeGen_Test/makefile

# CODE REVIEW  2019-06-11  KMS - Martin Dubois, ing.

include ../User.mk

INCLUDES = -I ../Includes $(INCLUDE_IMPORT)

LIBRARIES = ../Libraries/CodeGen.a $(KMS_LIB_A)

OUTPUT = ../Binaries/CodeGen_Test

SOURCES =   CodeGen_Test.cpp \
            Filter.cpp       \
            TestCase.cpp

# ===== Rules / Regles ======================================================

.cpp.o:
	g++ -c $(COMPILE_FLAGS) -o $@ $(INCLUDES) $<

# ===== Macros ==============================================================

OBJECTS = $(SOURCES:.cpp=.o)

# ===== Targets ==============================================================

$(OUTPUT) : $(OBJECTS) $(LIBRARIES)
	g++ -o $@ $^

depend:
	makedepend -Y $(INCLUDES) $(SOURCES)
	
# DO NOT DELETE

CodeGen_Test.o: Component.h ../Import/Includes/KmsBase.h
CodeGen_Test.o: ../Import/Includes/SafeAPI.h
CodeGen_Test.o: ../Import/Includes/WindowsToLinux.h
CodeGen_Test.o: ../Import/Includes/KmsTest.h
Filter.o: Component.h ../Import/Includes/KmsBase.h
Filter.o: ../Import/Includes/SafeAPI.h ../Import/Includes/WindowsToLinux.h
Filter.o: ../Import/Includes/KmsTest.h TestCase.h ../Common/Filter.h
Filter.o: ../Includes/GPU_dpi/Types.h
TestCase.o: Component.h ../Import/Includes/KmsBase.h
TestCase.o: ../Import/Includes/SafeAPI.h ../Import/Includes/WindowsToLinux.h
TestCase.o: ../Import/Includes/KmsTest.h TestCase.h ../Common/Filter.h
TestCase.o: ../Includes/GPU_dpi/Types.h
