
# Author     KMS - Martin Dubois, ing.
# Copyright  (C) 2019 KMS. All rights reserved.
# Product    GPU-dpi
# File       CodeGen/makefile

# CODE REVIEW  2019-06-11  KMS - Martin Dubois, ing.

include ../User.mk

INCLUDES = -I ../Includes $(INCLUDE_IMPORT)

OUTPUT = ../Libraries/CodeGen.a

SOURCES =   Filter_Internal.cpp \
            Filter.cpp          \
            Phase0.cpp		    \
            Phase1.cpp          \
            Phase2.cpp          \
            Phase3.cpp          \
            SearchElement.cpp   \
            SearchList.cpp      \
            Token.cpp

# ===== Rules / Regles ======================================================

.cpp.o:
	g++ -c $(COMPILE_FLAGS) -o $@ $(INCLUDES) $<

# ===== Macros ==============================================================

OBJECTS = $(SOURCES:.cpp=.o)

# ===== Targets ==============================================================

$(OUTPUT) : $(OBJECTS) $(LIBRARIES)
	ar r $@ $^
	ranlib $@

depend:
	makedepend -Y $(INCLUDES) $(SOURCES)
	
# DO NOT DELETE

Filter_Internal.o: Component.h ../Import/Includes/KmsBase.h
Filter_Internal.o: ../Import/Includes/SafeAPI.h
Filter_Internal.o: ../Import/Includes/WindowsToLinux.h Filter_Internal.h
Filter_Internal.o: ../Common/Filter.h ../Includes/GPU_dpi/Types.h
Filter.o: Component.h ../Import/Includes/KmsBase.h
Filter.o: ../Import/Includes/SafeAPI.h ../Import/Includes/WindowsToLinux.h
Filter.o: ../Common/Filter.h ../Includes/GPU_dpi/Types.h Phase0.h Phase1.h
Filter.o: Phase2.h Phase3.h Token.h Filter_Internal.h
Phase0.o: Component.h ../Import/Includes/KmsBase.h
Phase0.o: ../Import/Includes/SafeAPI.h ../Import/Includes/WindowsToLinux.h
Phase0.o: Token.h Phase0.h
Phase1.o: Component.h ../Import/Includes/KmsBase.h
Phase1.o: ../Import/Includes/SafeAPI.h ../Import/Includes/WindowsToLinux.h
Phase1.o: Token.h Phase1.h
Phase2.o: Component.h ../Import/Includes/KmsBase.h
Phase2.o: ../Import/Includes/SafeAPI.h ../Import/Includes/WindowsToLinux.h
Phase2.o: Token.h Phase2.h
Phase3.o: Component.h ../Import/Includes/KmsBase.h
Phase3.o: ../Import/Includes/SafeAPI.h ../Import/Includes/WindowsToLinux.h
Phase3.o: Filter_Internal.h ../Common/Filter.h ../Includes/GPU_dpi/Types.h
Phase3.o: SearchList.h SearchElement.h ../Import/Includes/OpenNetK/RegEx.h
Phase3.o: ../Includes/GPU_dpi/Status.h ../Includes/GPU_dpi/GPU_dpi.h Token.h
Phase3.o: Phase3.h
SearchElement.o: Component.h ../Import/Includes/KmsBase.h
SearchElement.o: ../Import/Includes/SafeAPI.h
SearchElement.o: ../Import/Includes/WindowsToLinux.h Filter_Internal.h
SearchElement.o: ../Common/Filter.h ../Includes/GPU_dpi/Types.h
SearchElement.o: SearchElement.h ../Import/Includes/OpenNetK/RegEx.h
SearchElement.o: ../Includes/GPU_dpi/Status.h ../Includes/GPU_dpi/GPU_dpi.h
SearchList.o: Component.h ../Import/Includes/KmsBase.h
SearchList.o: ../Import/Includes/SafeAPI.h
SearchList.o: ../Import/Includes/WindowsToLinux.h Filter_Internal.h
SearchList.o: ../Common/Filter.h ../Includes/GPU_dpi/Types.h SearchList.h
SearchList.o: SearchElement.h ../Import/Includes/OpenNetK/RegEx.h
SearchList.o: ../Includes/GPU_dpi/Status.h ../Includes/GPU_dpi/GPU_dpi.h
Token.o: Component.h ../Import/Includes/KmsBase.h
Token.o: ../Import/Includes/SafeAPI.h ../Import/Includes/WindowsToLinux.h
Token.o: Token.h
