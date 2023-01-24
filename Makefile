.DEFAULT_GOAL := main

CXX = g++-12

CXXFLAGS =-fopenmp -O3
TARGET =main
INCLUDES =-I ./include
OBJECTS = \
	graph.cpp \
	$(TARGET).cpp

all: 
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $(TARGET) $(OBJECTS)

.cpp.o: 
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJECTS)

message:
	echo "Executable: $(TARGET) has been created"