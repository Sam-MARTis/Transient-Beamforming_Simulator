CXX := g++
CXXFLAGS := -Wall -Wextra -std=c++20 -O2 -Iimgui -Iimgui-sfml 
LIBS := -lsfml-graphics -lsfml-window -lsfml-system -lGL


TARGET := sfml_imgui_test
SRC := main.cpp \
       imgui/imgui.cpp \
       imgui/imgui_widgets.cpp \
       imgui/imgui_draw.cpp \
       imgui/imgui_tables.cpp \
       imgui-sfml/imgui-SFML.cpp

OBJ := $(SRC:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

rebuild: clean all

.PHONY: all clean rebuild
