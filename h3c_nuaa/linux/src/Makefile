CC=g++
CFLAGS=-c -Wall
LDFLAGS=-lpthread -lpcap -lcrypto
SOURCES=App.cpp HuaweiNetwork.cpp HWNetworkFedora.cpp \
IniFileProcessor.cpp IniValue.cpp NetworkInterface.cpp \
ProfileManager.cpp runner.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=h3c

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@
