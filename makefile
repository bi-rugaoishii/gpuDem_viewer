CXX = g++
CXXFLAGS =  -Wall -Wextra -O2 -DVIEWER_SHADER_PATH=\"$(CURDIR)\/\"

# Paths to external dependencies
INCLUDES = -Iinclude -Iexternal/imgui -Iexternal/imgui/backends  -Iexternal/ImGuiFileDialog
LIBS = -lglfw -ldl -lGL 

# All source files
CPP_SRC = main.cpp \
          Shader.cpp \
          Sphere.cpp \
          Camera.cpp \
		  CameraController.cpp \
		  PositionLoader.cpp \
		  GuiManager.cpp \
		  StlMeshLoader.cpp\
		  Color.cpp\
		  Mesh.cpp\
		  PlaneRenderer.cpp\
		  CubeMesh.cpp\
		  external/ImGuiFileDialog/ImGuiFileDialog.cpp \
		  external/imgui/imgui.cpp \
		  external/imgui/imgui_draw.cpp \
		  external/imgui/imgui_tables.cpp \
		  external/imgui/imgui_widgets.cpp \
		  external/imgui/backends/imgui_impl_glfw.cpp \
		  external/imgui/backends/imgui_impl_opengl3.cpp

C_SRC = external/glad/glad.c

SRC = $(CPP_SRC) $(C_SRC)

# Object files (place in obj/ directory)
OBJ = $(SRC:%.cpp=obj/%.o)
	OBJ := $(OBJ:%.c=obj/%.o)

# Output executable
TARGET = bin/viewer

# Default target
all: $(TARGET)

# Link
$(TARGET): $(OBJ)
	@mkdir -p bin
	$(CXX) -o $@ $^ $(LIBS)

# Compile .cpp to .o
obj/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Compile .c to .o
obj/%.o: %.c
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Clean
clean:
	rm -rf obj bin
