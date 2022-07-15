CC = g++
DEL = rm -f

CMP_FLAGS = -I"C:\MinGW\include\SDL2" -g
LNK_FLAGS = -L"C:\MinGW\lib" -lmingw32 -lSDL2main -lSDL2

# Util objects

HEADS_utils/geometry := utils/geometry

# Game objects

GAME_DATA := game/game_data utils/serialization
GAME_INTERFACES := game/game_observer game/game_view game/player_interface $(GAME_INTERFACES)

HEADS_game/game_data := $(GAME_DATA)
HEADS_game/game_logic := game/game_logic $(GAME_DATA)
HEADS_game/game := game/game game/game_logic $(GAME_INTERFACES)

# GUI objects

HEADS_gui/gui_utils := gui/gui_utils
HEADS_gui/game_drawer := gui/game_drawer gui/gui_utils gui/colors $(GAME_INTERFACES) utils/geometry
HEADS_gui/game_gui := gui/game_gui gui/game_drawer gui/gui_utils gui/colors $(GAME_INTERFACES)

# executables


OBJECTS_test := game/game_data game/game_logic game/game gui/gui_utils gui/game_drawer gui/game_gui utils/geometry

EXECUTABLES := test

# rules
OBJECTS = $(OBJECTS_test)

OBJECTS := $(addprefix build/,$(addsuffix .o,$(OBJECTS)))
EXECUTABLES := $(addprefix build/,$(addsuffix .exe,$(EXECUTABLES)))

all: $(EXECUTABLES)

clear:
	$(DEL) $(OBJECTS)

clear_all: clear
	$(DEL) $(EXECUTABLES)

.SECONDEXPANSION:
$(EXECUTABLES): build/%.exe: $$(addprefix build/,$$(addsuffix .o,$$(OBJECTS_$$*)))
	$(CC) $(CMP_FLAGS) $^ -o $@ $(LNK_FLAGS)
	
$(OBJECTS): build/%.o: src/%.cpp $$(addprefix src/,$$(addsuffix .h,$$(HEADS_$$*)))
	$(CC) $(CMP_FLAGS) -c $< -o $@
