# Compiler and flags
CXX = g++
CXXFLAGS = -Wall -g -Iinclude

# Build target executable
TARGET = myfind

# Source and object files
SRCS = $(wildcard *.cpp)
OBJS = $(patsubst %.cpp,bin/o/%.o,$(SRCS))

# Create bin and bin/o directories if they don't exist
$(shell mkdir -p bin/o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

bin/o/%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf bin/o/*.o $(TARGET)

run: $(TARGET)
	./$(TARGET)
	
fullrun: all run