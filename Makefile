CC = g++
DEL = rm -f

SYS = $(shell uname)

DBG_FLAGS = -g

ifeq ($(SYS), Linux)
	CMP_FLAGS = -I"/usr/include/SDL2" -I"enet\include" $(DBG_FLAGS)
	LNK_FLAGS = -L"enet" -lSDL2main -lSDL2 -lSDL2_ttf -lSDL2_image -lenet64
	EXEC_EXT = 
else
ifeq ($(findstring MINGW32, $(SYS)), MINGW32)
	CMP_FLAGS = -I"C:\MinGW\include\SDL2" -I"enet\include" $(DBG_FLAGS)
	LNK_FLAGS = -L"C:\MinGW\lib" -L"enet" -lmingw32 -lSDL2main -lSDL2 -lSDL2_ttf -lSDL2_image -lenet64 -lws2_32 -lwinmm
	EXEC_EXT = .exe
endif
endif

# Util objects

HEADS_utils/geometry := utils/geometry

# Game objects

GAME_DATA := game/game_data utils/serialization
GAME_INTERFACES := game/game_observer game/game_view game/player_interface $(GAME_DATA)

HEADS_game/game_data := $(GAME_DATA)
HEADS_game/powerups := game/powerups $(GAME_DATA)
HEADS_game/game_logic := game/game_logic $(GAME_DATA)
HEADS_game/game_geometry := game/game_geometry game/game_logic game/collision_grid $(GAME_DATA)
HEADS_game/cheese_maker := game/cheese_maker
HEADS_game/game := game/game game/powerups game/game_geometry game/game_logic game/cheese_maker game/collision_grid $(GAME_INTERFACES)

# GUI objects

HEADS_gui/texts := gui/texts
HEADS_gui/images := gui/images
HEADS_gui/gui_utils := gui/gui_utils gui/keyset
HEADS_gui/game_drawer := gui/game_drawer gui/images gui/texts gui/gui_utils gui/colors gui/keyset game/game_logic $(GAME_INTERFACES) utils/geometry
HEADS_gui/game_gui := gui/game_gui gui/game_drawer gui/images gui/texts gui/gui_utils gui/colors gui/keyset game/game_logic $(GAME_INTERFACES)

# Network objects

HEADS_network/network := network/network
HEADS_network/client := network/client network/network
HEADS_network/server := network/server network/network

# executables

HEADS_test_main := gui/game_gui gui/game_drawer gui/texts gui/gui_utils gui/colors gui/keyset game/game game/powerups game/game_logic game/cheese_maker game/game_geometry game/collision_grid gui/images $(GAME_INTERFACES)

OBJECTS_test := game/game_data game/game_logic game/game game/powerups gui/gui_utils gui/texts gui/game_drawer gui/game_gui utils/geometry game/cheese_maker game/game_geometry gui/images test_main

EXECUTABLES := test

# rules
OBJECTS = $(OBJECTS_test)

OBJECTS := $(addprefix build/,$(addsuffix .o,$(OBJECTS)))
EXECUTABLES := $(addprefix build/,$(addsuffix $(EXEC_EXT),$(EXECUTABLES)))

all: $(EXECUTABLES)

clear:
	$(DEL) $(OBJECTS)

clear_all: clear
	$(DEL) $(EXECUTABLES)

.SECONDEXPANSION:
$(EXECUTABLES): build/%$(EXEC_EXT): $$(addprefix build/,$$(addsuffix .o,$$(OBJECTS_$$*)))
	$(CC) $(CMP_FLAGS) $^ -o $@ $(LNK_FLAGS)
	
$(OBJECTS): build/%.o: src/%.cpp $$(addprefix src/,$$(addsuffix .h,$$(HEADS_$$*)))
	$(CC) $(CMP_FLAGS) -c $< -o $@
