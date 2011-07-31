TARGET = dsound.dll

SRCS := $(wildcard *.cpp)
OBJS := $(SRCS:.cpp=.o)
HEADERS := $(wildcard *.hpp) $(wildcard *.h)

all: $(TARGET)

LIBS := -ldxguid -static-libstdc++ -static-libgcc -lrsound -L. dsound.def

CXXFLAGS += -O3 -g -I. -Wall -ansi -pedantic

CXX = g++

LDFLAGS += -Wl,--add-stdcall-alias -Wl,--enable-stdcall-fixup -lm

%.o: %.cpp $(HEADERS)
	$(CXX) -c -o $@ $< $(CXXFLAGS)

$(TARGET): $(OBJS)
	$(CXX) -o $@ $(OBJS) $(LIBS) $(LDFLAGS) -shared

clean:
	rm -f $(OBJS)
	rm -f $(TARGET)

.PHONY: clean
