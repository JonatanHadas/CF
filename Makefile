CC = g++
DEL = rm -f

CMP_FLAGS = 
LNK_FLAGS = -L"C:\MinGW\lib" -lmingw32

# executables

EXECUTABLES := 

# rules
OBJECTS = 

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
