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
HEADS_game/default_settings := game/default_settings $(GAME_SETTINGS)
HEADS_game/game_settings_observer_accumulator := game/game_settings_observer_accumulator game/game_settings_observer
HEADS_game/game_settings_manager := game/game_settings_manager game/game_settings_observer_accumulator utils/utils $(GAME_SETTINGS_INTERFACES)
HEADS_game/local_game_creator := game/local_game_creator game/default_settings game/game_creator game/game_settings_manager game/game_settings_observer_accumulator game/game game/game_logic game/collision_grid game/powerups game/cheese_maker utils/geometry $(GAME_SETTINGS_INTERFACES) $(GAME_INTERFACES)
HEADS_game/observed_settings_view := game/observed_settings_view game/game_settings_observer_accumulator game/default_settings utils/utils $(GAME_SETTINGS_INTERFACES)
HEADS_game/composite_player_interface := game/composite_player_interface game/player_interface
HEADS_game/game_extrapolator := game/game_extrapolator game/game_logic $(GAME_INTERFACES)

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
HEADS_gui/game_startup_menu := game/local_game_creator gui/game_startup_menu gui/subview_manager gui/subview gui/button gui/textbox gui/texts game/game_creator game/game_settings_observer_accumulator game/game game/game_logic game/collision_grid game/powerups game/cheese_maker utils/geometry network/client network/network game_network/protocol game_network/remote_game_creator game/game_extrapolator game/composite_player_interface game_network/game_client game_network/game_settings_client game/observed_settings_view $(GAME_SETTINGS_INTERFACES) $(GAME_INTERFACES)
HEADS_gui/game_menu := gui/game_gui gui/game_drawer gui/game_startup_menu game/game_creator game/game_settings_observer_accumulator gui/number_button gui/options_menu gui/player_settings gui/game_settings_subview gui/tab_view gui/game_menu gui/players_subview gui/subview_manager gui/button gui/subview gui/textbox gui/key_choice_button gui/keyset network/client network/network $(GAME_SETTINGS_INTERFACES) $(GAME_INTERFACES)
HEADS_gui/colors := gui/colors
HEADS_gui/texts := gui/texts
HEADS_gui/images := gui/images
HEADS_gui/keyset := gui/keyset utils/serialization utils/utils
HEADS_gui/gui_utils := gui/gui_utils
HEADS_gui/game_drawer := gui/game_drawer gui/images gui/texts gui/gui_utils gui/colors gui/keyset game/game_logic game/game_settings $(GAME_INTERFACES) utils/geometry
HEADS_gui/game_gui := gui/gui gui/game_gui gui/game_drawer gui/images gui/texts gui/gui_utils gui/colors gui/keyset game/game_logic game/game_settings $(GAME_INTERFACES)

# Network objects

HEADS_network/network := network/network
HEADS_network/client := network/client network/network
HEADS_network/server := network/server network/network

# Game networking objects

GAME_PROTOCOL := game_network/protocol utils/serialization

HEADS_game_network/game_server := game_network/game_server game/game game/game_logic game/cheese_maker game/powerups game/collision_grid network/server network/network $(GAME_INTERFACES) $(GAME_PROTOCOL)
HEADS_game_network/game_settings_server := game_network/game_server game/game game/cheese_maker game/powerups game/collision_grid game_network/game_settings_server network/server network/network game/game_settings_manager $(GAME_SETTINGS_INTERFACES) $(GAME_INTERFACES) $(GAME_PROTOCOL)
HEADS_game_network/game_client := game_network/game_client network/client network/network $(GAME_INTERFACES) $(GAME_PROTOCOL)
HEADS_game_network/game_settings_client := game_network/game_settings_client network/client network/network game/game_settings_observer_accumulator $(GAME_SETTINGS_INTERFACES) $(GAME_PROTOCOL)
HEADS_game_network/remote_game_creator := game_network/remote_game_creator game/game_logic game/game_extrapolator game/composite_player_interface game_network/game_client game_network/game_settings_client network/client network/network game/observed_settings_view game/game_creator game/game_settings_observer_accumulator utils/utils $(GAME_SETTINGS_INTERFACES) $(GAME_INTERFACES)

# executables

HEADS_test_main := gui/game_startup_menu game/game_creator game/game_settings_observer_accumulator gui/number_button gui/options_menu gui/game_settings_subview gui/tab_view gui/key_choice_button gui/player_settings gui/textbox gui/game_menu gui/players_subview gui/subview_manager gui/button gui/subview gui/gui gui/game_gui gui/game_drawer gui/texts gui/gui_utils gui/colors gui/keyset game/game game/powerups game/game_logic game/cheese_maker game/game_geometry game/collision_grid gui/images $(GAME_INTERFACES)
HEADS_server_main := game_network/game_settings_server network/server network/network game/default_settings $(GAME_SETTINGS_INTERFACES)

CLIENT_OBJECTS := game/game_extrapolator game/composite_player_interface game_network/game_client network/client game_network/remote_game_creator game_network/game_settings_client game/observed_settings_view game/local_game_creator gui/game_startup_menu gui/powerup_images gui/number_button gui/options_menu gui/game_settings_subview gui/tab_view gui/key_choice_button gui/keyset gui/player_settings utils/utf8 gui/textbox gui/colors gui/game_menu gui/players_subview gui/subview_manager gui/button gui/subview gui/gui gui/clock gui/gui_utils gui/texts gui/game_drawer gui/game_gui gui/images test_main
SERVER_OBJECTS := game_network/game_server server_main game_network/game_settings_server network/server
COMMON_OBJECTS := network/network game/default_settings game/game_data game/game_settings game/game_settings_observer_accumulator utils/serialization game/game_settings_manager game/game_logic game/game game/powerups utils/geometry game/cheese_maker game/game_geometry

OBJECTS_test := $(COMMON_OBJECTS) $(CLIENT_OBJECTS)

OBJECTS_server := $(COMMON_OBJECTS) $(SERVER_OBJECTS)

EXECUTABLES := test server

# rules
OBJECTS = $(COMMON_OBJECTS) $(SERVER_OBJECTS) $(CLIENT_OBJECTS)

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
