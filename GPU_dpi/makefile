
# Author     KMS - Martin Dubois, ing.
# Copyright  (C) 2019 KMS. All rights reserved.
# Product    GPU-dpi
# File       GPU_dpi/makefile

# CODE REVIEW  2019-06-11  KMS - Martin Dubois, ing.

include ../User.mk

INCLUDES = -I ../Includes $(INCLUDE_IMPORT) -I $(CUDA_FOLDER)/include

LIBRARIES = ../Libraries/CodeGen.a $(KMS_LIB_A)

OUTPUT = ../Binaries/libGPU-dpi.so

SOURCES =   AdapterConfig.cpp      \
            CallbackCaller.cpp     \
            Code.cpp		       \
            EventProcessor.cpp     \
            FileWriter_PCAP.cpp    \
            FileWriter.cpp         \
            SetupTool_Internal.cpp \
            SetupTool.cpp          \
            Status.cpp             \
            System_Internal.cpp    \
            System.cpp             \
            Utils.cpp

# ===== Rules / Regles ======================================================

.cpp.o:
	g++ -fvisibility=hidden -c $(COMPILE_FLAGS) -o $@ $(INCLUDES) $<

# ===== Macros ==============================================================

OBJECTS = $(SOURCES:.cpp=.o)

# ===== Targets ==============================================================

$(OUTPUT) : $(OBJECTS) $(LIBRARIES)
	g++ -L $(CUDA_FOLDER)/lib64 -l cudart -shared -o $@ $^

depend:
	makedepend -Y $(INCLUDES) $(SOURCES)
	
# DO NOT DELETE

AdapterConfig.o: Component.h ../Import/Includes/KmsBase.h
AdapterConfig.o: ../Import/Includes/SafeAPI.h
AdapterConfig.o: ../Import/Includes/WindowsToLinux.h Code.h
AdapterConfig.o: ../Includes/GPU_dpi/Types.h AdapterConfig.h
AdapterConfig.o: ../Includes/GPU_dpi/Status.h ../Includes/GPU_dpi/GPU_dpi.h
CallbackCaller.o: Component.h ../Import/Includes/KmsBase.h
CallbackCaller.o: ../Import/Includes/SafeAPI.h
CallbackCaller.o: ../Import/Includes/WindowsToLinux.h
CallbackCaller.o: ../Import/Includes/KmsLib/Exception.h
CallbackCaller.o: ../Import/Includes/OpenNet/Buffer.h
CallbackCaller.o: ../Import/Includes/OpenNet/Status.h
CallbackCaller.o: ../Import/Includes/OpenNet/OpenNet.h CallbackCaller.h
CallbackCaller.o: ../Import/Includes/OpenNet/Adapter.h
CallbackCaller.o: ../Import/Includes/OpenNet/Processor.h
CallbackCaller.o: ../Import/Includes/OpenNet/StatisticsProvider.h
CallbackCaller.o: ../Import/Includes/OpenNetK/Adapter_Types.h
CallbackCaller.o: ../Includes/GPU_dpi/Types.h EventProcessor.h
Code.o: Component.h ../Import/Includes/KmsBase.h ../Import/Includes/SafeAPI.h
Code.o: ../Import/Includes/WindowsToLinux.h Code.h
Code.o: ../Includes/GPU_dpi/Types.h
EventProcessor.o: Component.h ../Import/Includes/KmsBase.h
EventProcessor.o: ../Import/Includes/SafeAPI.h
EventProcessor.o: ../Import/Includes/WindowsToLinux.h
EventProcessor.o: ../Import/Includes/OpenNet/Buffer.h
EventProcessor.o: ../Import/Includes/OpenNet/Status.h
EventProcessor.o: ../Import/Includes/OpenNet/OpenNet.h EventProcessor.h
EventProcessor.o: ../Import/Includes/OpenNet/Adapter.h
EventProcessor.o: ../Import/Includes/OpenNet/Processor.h
EventProcessor.o: ../Import/Includes/OpenNet/StatisticsProvider.h
EventProcessor.o: ../Import/Includes/OpenNetK/Adapter_Types.h
FileWriter_PCAP.o: Component.h ../Import/Includes/KmsBase.h
FileWriter_PCAP.o: ../Import/Includes/SafeAPI.h
FileWriter_PCAP.o: ../Import/Includes/WindowsToLinux.h PCap.h
FileWriter_PCAP.o: FileWriter_PCAP.h FileWriter.h
FileWriter_PCAP.o: ../Import/Includes/OpenNet/Adapter.h
FileWriter_PCAP.o: ../Import/Includes/OpenNet/Processor.h
FileWriter_PCAP.o: ../Import/Includes/OpenNet/OpenNet.h
FileWriter_PCAP.o: ../Import/Includes/OpenNet/Status.h
FileWriter_PCAP.o: ../Import/Includes/OpenNet/StatisticsProvider.h
FileWriter_PCAP.o: ../Import/Includes/OpenNetK/Adapter_Types.h
FileWriter_PCAP.o: EventProcessor.h ../Import/Includes/OpenNet/Buffer.h
FileWriter.o: Component.h ../Import/Includes/KmsBase.h
FileWriter.o: ../Import/Includes/SafeAPI.h
FileWriter.o: ../Import/Includes/WindowsToLinux.h
FileWriter.o: ../Import/Includes/KmsLib/Exception.h
FileWriter.o: ../Import/Includes/OpenNet/Buffer.h
FileWriter.o: ../Import/Includes/OpenNet/Status.h
FileWriter.o: ../Import/Includes/OpenNet/OpenNet.h FileWriter.h
FileWriter.o: ../Import/Includes/OpenNet/Adapter.h
FileWriter.o: ../Import/Includes/OpenNet/Processor.h
FileWriter.o: ../Import/Includes/OpenNet/StatisticsProvider.h
FileWriter.o: ../Import/Includes/OpenNetK/Adapter_Types.h EventProcessor.h
SetupTool_Internal.o: Component.h ../Import/Includes/KmsBase.h
SetupTool_Internal.o: ../Import/Includes/SafeAPI.h
SetupTool_Internal.o: ../Import/Includes/WindowsToLinux.h
SetupTool_Internal.o: ../Import/Includes/KmsLib/Exception.h Utils.h
SetupTool_Internal.o: ../Import/Includes/OpenNet/Status.h
SetupTool_Internal.o: ../Import/Includes/OpenNet/OpenNet.h
SetupTool_Internal.o: ../Includes/GPU_dpi/Status.h
SetupTool_Internal.o: ../Includes/GPU_dpi/GPU_dpi.h SetupTool_Internal.h
SetupTool_Internal.o: ../Import/Includes/OpenNet/SetupTool.h
SetupTool_Internal.o: ../Includes/GPU_dpi/SetupTool.h
SetupTool.o: Component.h ../Import/Includes/KmsBase.h
SetupTool.o: ../Import/Includes/SafeAPI.h ../Import/Includes/WindowsToLinux.h
SetupTool.o: ../Includes/GPU_dpi/SetupTool.h ../Includes/GPU_dpi/Status.h
SetupTool.o: ../Includes/GPU_dpi/GPU_dpi.h SetupTool_Internal.h
SetupTool.o: ../Import/Includes/OpenNet/SetupTool.h
SetupTool.o: ../Import/Includes/OpenNet/Status.h
SetupTool.o: ../Import/Includes/OpenNet/OpenNet.h
Status.o: Component.h ../Import/Includes/KmsBase.h
Status.o: ../Import/Includes/SafeAPI.h ../Import/Includes/WindowsToLinux.h
Status.o: ../Includes/GPU_dpi/Status.h ../Includes/GPU_dpi/GPU_dpi.h
System_Internal.o: Component.h ../Import/Includes/KmsBase.h
System_Internal.o: ../Import/Includes/SafeAPI.h
System_Internal.o: ../Import/Includes/WindowsToLinux.h
System_Internal.o: ../Import/Includes/KmsLib/Exception.h
System_Internal.o: ../Import/Includes/KmsLib/FileHandle.h
System_Internal.o: ../Import/Includes/KmsLib/ThreadBase.h
System_Internal.o: ../Import/Includes/OpenNet/Adapter.h
System_Internal.o: ../Import/Includes/OpenNet/Processor.h
System_Internal.o: ../Import/Includes/OpenNet/OpenNet.h
System_Internal.o: ../Import/Includes/OpenNet/Status.h
System_Internal.o: ../Import/Includes/OpenNet/StatisticsProvider.h
System_Internal.o: ../Import/Includes/OpenNetK/Adapter_Types.h
System_Internal.o: ../Import/Includes/OpenNet/Buffer.h
System_Internal.o: ../Import/Includes/OpenNet/Function.h
System_Internal.o: ../Import/Includes/OpenNet/SourceCode.h
System_Internal.o: ../Import/Includes/OpenNet/Kernel.h ../Common/Filter.h
System_Internal.o: ../Includes/GPU_dpi/Types.h AdapterConfig.h
System_Internal.o: ../Includes/GPU_dpi/Status.h ../Includes/GPU_dpi/GPU_dpi.h
System_Internal.o: CallbackCaller.h EventProcessor.h FileWriter_PCAP.h
System_Internal.o: FileWriter.h Utils.h System_Internal.h
System_Internal.o: ../Import/Includes/KmsLib/DebugLog.h
System_Internal.o: ../Import/Includes/KmsLib/TextFile.h
System_Internal.o: ../Import/Includes/OpenNet/System.h
System_Internal.o: ../Includes/GPU_dpi/System.h Code.h
System.o: Component.h ../Import/Includes/KmsBase.h
System.o: ../Import/Includes/SafeAPI.h ../Import/Includes/WindowsToLinux.h
System.o: ../Import/Includes/KmsLib/Exception.h System_Internal.h
System.o: ../Import/Includes/KmsLib/DebugLog.h
System.o: ../Import/Includes/KmsLib/TextFile.h
System.o: ../Import/Includes/OpenNet/Function.h
System.o: ../Import/Includes/OpenNet/SourceCode.h
System.o: ../Import/Includes/OpenNet/OpenNet.h
System.o: ../Import/Includes/OpenNet/Status.h
System.o: ../Import/Includes/OpenNet/System.h
System.o: ../Import/Includes/OpenNetK/Adapter_Types.h
System.o: ../Includes/GPU_dpi/System.h ../Includes/GPU_dpi/Status.h
System.o: ../Includes/GPU_dpi/GPU_dpi.h ../Includes/GPU_dpi/Types.h
System.o: CallbackCaller.h ../Import/Includes/OpenNet/Adapter.h
System.o: ../Import/Includes/OpenNet/Processor.h
System.o: ../Import/Includes/OpenNet/StatisticsProvider.h EventProcessor.h
System.o: ../Import/Includes/OpenNet/Buffer.h Code.h FileWriter.h
Utils.o: Utils.h ../Import/Includes/OpenNet/Status.h
Utils.o: ../Import/Includes/OpenNet/OpenNet.h ../Includes/GPU_dpi/Status.h
Utils.o: ../Includes/GPU_dpi/GPU_dpi.h
