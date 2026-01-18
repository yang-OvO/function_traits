CXXFLAGS = -std=c++17 -Wall -Wextra
SOURCES=$(wildcard *.cpp)
PROGRAMS=$(SOURCES:.cpp=)

all: $(PROGRAMS)

.PHONY: clean
clean:
	rm $(PROGRAMS)

