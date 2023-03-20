.DEFAULT_GOAL := main

CXX = g++-12

CXXFLAGS =-fopenmp -O3 -std=c++17 -Wno-deprecated-declarations
TARGET =main
INCLUDES =-I ./include -I /usr/local/include
LDFLAGS =-L /usr/local/lib -lemon
OBJECTS = \
	graph.cpp \
	auction.cpp \
	$(TARGET).cpp

all: 
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $(TARGET) $(OBJECTS)

lemon:
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(LDFLAGS) -o $(TARGET) $(OBJECTS)

.cpp.o: 
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJECTS)

message:
	echo "Executable: $(TARGET) has been created"