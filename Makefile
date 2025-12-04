

CXX := g++
CXXFLAGS := -Wall -Wno-everything -std=c++20 -O2 \
            -Iimgui \
            -Iimgui-sfml \
            -I/usr/local/include


LDFLAGS := -L/usr/local/lib
LIBS := -lsfml-graphics -lsfml-window -lsfml-system -lGL -lpthread -ldl

TARGET := fluid_sim

SRC := main.cpp \
       imgui/imgui.cpp \
       imgui/imgui_widgets.cpp \
       imgui/imgui_draw.cpp \
       imgui/imgui_tables.cpp \
       imgui-sfml/imgui-SFML.cpp \
       aux-functions.cpp \
       initializations.cpp \
	display-functions.cpp \
       core-sim-functions.cpp

DEPS := constants.hpp aux-functions.hpp initializations.hpp display-functions.hpp core-sim-functions.hpp

OBJ := $(SRC:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

%.o: %.cpp $(DEPS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

rebuild: clean all

.PHONY: all clean rebuild
