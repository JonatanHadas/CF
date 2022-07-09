CC = g++
DEL = rm -f

CMP_FLAGS = 
LNK_FLAGS = -L"C:\MinGW\lib" -lmingw32


GAME_DATA := game/game_data utils/serialization

HEADS_game/game_data := $(GAME_DATA)
HEADS_game/game_logic := game/game_logic $(GAME_DATA)

COMMON_OBJECTS := game/game_data game/game_logic

# executables

OBJECTS_test := $(COMMON_OBJECTS)

EXECUTABLES := test

# rules
OBJECTS = $(COMMON_OBJECTS)

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