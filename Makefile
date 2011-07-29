TARGET = dsound.dll

SRCS := $(wildcard *.cpp)
OBJS := $(SRCS:.cpp=.o)

all: $(TARGET)

LIBS := -ldxguid -static-libstdc++ -static-libgcc -lrsound -L.

CXXFLAGS += -O3 -g -I. -Wall -ansi -pedantic

CXX = g++

%.o: %.cpp
	$(CXX) -c -o $@ $< $(CXXFLAGS)

$(TARGET): $(OBJS)
	$(CXX) -o $@ $(OBJS) $(LIBS) -shared

clean:
	rm -f $(OBJS)
	rm -f $(TARGET)

.PHONY: clean
