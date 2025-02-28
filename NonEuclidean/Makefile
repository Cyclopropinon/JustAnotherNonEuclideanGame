CXX = c++
# Kompilierung mit c++11 und c++23 wegen geschwindigkeit + aktualität
CXXFLAGS_11 = -O3 -march=native -std=c++11
CXXFLAGS_23 = -O3 -march=native -std=c++23
LINK = c++
LINKFLAGS = -lm -lSDL2 -lSDL2main -lGLEW

UNAME := $(shell uname)

ifeq ($(UNAME), Linux)
    LINKFLAGS += -lGL
endif

ifeq ($(UNAME), Darwin)
    LINKFLAGS += -framework OpenGL
endif

EXE = NonEuclidean

# Vorerst müssen alle globale Header C++11 kompatibel sein
HEADERS = Camera.h Collider.h Engine.h Floorplan.h FrameBuffer.h GameHeader.h \
    Ground.h House.h Input.h Mesh.h Object.h Physical.h Pillar.h PillarRoom.h \
    Player.h Portal.h Resources.h Scene.h Shader.h Sky.h Sphere.h Statue.h \
    Texture.h Timer.h Tunnel.h Vector.h \
    Levels.h Level1.h Level2.h Level3.h Level4.h Level5.h Level6.h Level7.h

SRC_11 = Camera.cpp Collider.cpp Engine.cpp Engine_Win32.cpp Engine_SDL2.cpp \
    FrameBuffer.cpp Input.cpp Level1.cpp Level2.cpp Level3.cpp Level4.cpp \
    Level5.cpp Level6.cpp Main.cpp Mesh.cpp Object.cpp Physical.cpp Player.cpp \
    Portal.cpp Resources.cpp Shader.cpp Texture.cpp

SRC_23 = Level7.cpp

OBJS_11 = $(SRC_11:.cpp=.o)
OBJS_23 = $(SRC_23:.cpp=.o)

.PHONY: all clean

all: $(EXE)

clean:
	rm -fv $(OBJS_11) $(OBJS_23)

$(EXE): $(OBJS_11) $(OBJS_23)
	$(LINK) $^ $(LINKFLAGS) -o $(EXE)

$(OBJS_11): %.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS_11) -c $< -o $@

$(OBJS_23): %.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS_23) -c $< -o $@
