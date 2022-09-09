CC = g++
DEL = rm -f

SYS = $(shell uname)

DBG_FLAGS = -g

ifeq ($(SYS), Linux)
	CMP_FLAGS = -I"/usr/include/SDL2" $(DBG_FLAGS)
	LNK_FLAGS = -lSDL2main -lSDL2 -lSDL2_ttf -lSDL2_image
	EXEC_EXT = 
else
ifeq ($(findstring MINGW32, $(SYS)), MINGW32)
	CMP_FLAGS = -I"C:\MinGW\include\SDL2" $(DBG_FLAGS)
	LNK_FLAGS = -L"C:\MinGW\lib" -lmingw32 -lSDL2main -lSDL2 -lSDL2_ttf -lSDL2_image
	EXEC_EXT = .exe
endif
endif

# Util objects

HEADS_utils/geometry := utils/geometry
HEADS_utils/serialization := utils/serialization
HEADS_utils/utf8 := utils/utf8

# Game objects

GAME_DATA := game/game_data
GAME_INTERFACES := game/game_advancer game/game_observer game/game_view game/player_interface $(GAME_DATA)
GAME_SETTINGS := game/game_settings $(GAME_DATA)
GAME_SETTINGS_INTERFACES := game/game_settings_view game/game_settings_observer game/game_settings_manipulator $(GAME_SETTINGS)

HEADS_game/game_data := $(GAME_DATA) utils/serialization
HEADS_game/powerups := game/powerups $(GAME_DATA)
HEADS_game/game_logic := game/game_logic $(GAME_DATA)
HEADS_game/game_geometry := game/game_geometry game/game_logic game/collision_grid $(GAME_DATA)
HEADS_game/cheese_maker := game/cheese_maker
HEADS_game/game := game/game game/powerups game/game_geometry game/game_logic game/cheese_maker game/collision_grid $(GAME_INTERFACES)
HEADS_game/game_settings := $(GAME_SETTINGS) utils/serialization
HEADS_game/game_settings_observer_accumulator := game/game_settings_observer_accumulator game/game_settings_observer
HEADS_game/game_settings_manager := game/game_settings_manager game/game_settings_observer_accumulator utils/utils $(GAME_SETTINGS_INTERFACES)

# GUI objects

HEADS_gui/powerup_images := gui/powerup_images gui/images $(GAME_DATA)
HEADS_gui/clock := gui/clock
HEADS_gui/gui := gui/gui gui/clock
HEADS_gui/subview := gui/subview
HEADS_gui/button := gui/button gui/subview
HEADS_gui/textbox := gui/textbox gui/subview gui/texts utils/utf8
HEADS_gui/key_choice_button := gui/key_choice_button gui/subview gui/texts gui/keyset
HEADS_gui/options_menu := gui/options_menu gui/button gui/subview gui/subview_manager gui/texts gui/gui_utils gui/colors
HEADS_gui/number_button := gui/number_button gui/button gui/subview gui/subview_manager gui/texts gui/colors
HEADS_gui/subview_manager := gui/subview_manager gui/subview
HEADS_gui/tab_view := gui/tab_view gui/subview gui/subview_manager gui/button gui/texts
HEADS_gui/player_settings := gui/player_settings $(GAME_SETTINGS_INTERFACES)
HEADS_gui/players_subview := game/powerups gui/player_settings gui/players_subview gui/subview_manager gui/button gui/subview gui/colors gui/textbox gui/key_choice_button gui/keyset $(GAME_SETTINGS_INTERFACES)
HEADS_gui/game_settings_subview := game/game_settings_observer_accumulator utils/utils gui/powerup_images gui/images gui/number_button gui/options_menu gui/game_settings_subview gui/tab_view gui/subview gui/subview_manager gui/button gui/texts gui/colors $(GAME_SETTINGS_INTERFACES)
HEADS_gui/game_menu := game/game_settings_observer_accumulator gui/number_button gui/options_menu gui/player_settings gui/game_settings_subview gui/tab_view gui/game_menu gui/players_subview gui/subview_manager gui/button gui/subview gui/textbox gui/key_choice_button gui/keyset $(GAME_SETTINGS_INTERFACES)
HEADS_gui/colors := gui/colors
HEADS_gui/texts := gui/texts
HEADS_gui/images := gui/images
HEADS_gui/keyset := gui/keyset utils/serialization utils/utils
HEADS_gui/gui_utils := gui/gui_utils
HEADS_gui/game_drawer := gui/game_drawer gui/images gui/texts gui/gui_utils gui/colors gui/keyset game/game_logic $(GAME_INTERFACES) utils/geometry
HEADS_gui/game_gui := gui/gui gui/game_gui gui/game_drawer gui/images gui/texts gui/gui_utils gui/colors gui/keyset game/game_logic $(GAME_INTERFACES)

# executables

HEADS_test_main := game/game_settings_observer_accumulator gui/number_button gui/options_menu gui/game_settings_subview gui/tab_view gui/key_choice_button gui/player_settings gui/textbox gui/game_menu gui/players_subview gui/subview_manager gui/button gui/subview gui/gui gui/game_gui gui/game_drawer gui/texts gui/gui_utils gui/colors gui/keyset game/game game/powerups game/game_logic game/cheese_maker game/game_geometry game/collision_grid gui/images $(GAME_INTERFACES)

OBJECTS_test := game/game_settings_observer_accumulator gui/powerup_images gui/number_button gui/options_menu gui/game_settings_subview gui/tab_view gui/key_choice_button gui/keyset gui/player_settings utils/utf8 gui/textbox gui/colors gui/game_menu gui/players_subview gui/subview_manager gui/button gui/subview gui/gui gui/clock utils/serialization game/game_settings game/game_settings_manager game/game_data game/game_logic game/game game/powerups gui/gui_utils gui/texts gui/game_drawer gui/game_gui utils/geometry game/cheese_maker game/game_geometry gui/images test_main

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
