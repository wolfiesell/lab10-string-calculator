CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2
TARGET = string_calculator
SRC = string_calculator.cpp

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)
