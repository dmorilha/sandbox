# Copyright (c) 2026 Daniel Morilha

C_FLAGS += -MMD
CXX_FLAGS += -MMD
CXX_FLAGS += -std=c++20

#debug 
CC_FLAGS += -DDEBUG
CC_FLAGS += -Og
CC_FLAGS += -fno-omit-frame-pointer
CC_FLAGS += -g

CXX_FLAGS += -DDEBUG
CXX_FLAGS += -Og
CXX_FLAGS += -fno-omit-frame-pointer
CXX_FLAGS += -g

#release
#CC_FLAGS += -DNDEBUG
#CC_FLAGS += -O2
#CXX_FLAGS += -DNDEBUG
#CXX_FLAGS += -O2

TARGET = tic-tac-toe
HEADERS = $(wildcard *.h)
SOURCES = $(wildcard *.cc)
OBJECTS = $(patsubst %.cc,%.o,$(SOURCES))
DEPENDENCIES = $(patsubst %.o,%.d,$(OBJECTS))

$(TARGET): $(OBJECTS) $(HEADERS)
	$(CXX) $(CXX_FLAGS) $(LD_FLAGS) $(LIBS) -o $@ $(OBJECTS);

%.o : %.cc %.h
	$(CXX) $(CXX_FLAGS) -c -o $@ $<;

%.o : %.cc
	$(CXX) $(CXX_FLAGS) -c -o $@ $<;

-include $(DEPENDENCIES)

clean:
	rm -v $(OBJECTS) $(TARGET) $(DEPENDENCIES);
