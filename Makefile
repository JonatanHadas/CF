CC = g++
DEL = rm -f

SYS = $(shell uname)

DBG_FLAGS = -g

ifeq ($(SYS), Linux)
	CMP_FLAGS = -I"/usr/include/SDL2" -I"enet/include" $(DBG_FLAGS)
	LNK_FLAGS = -L"enet" -lSDL2main -lSDL2 -lSDL2_ttf -lSDL2_image -lSDL2_mixer -lenet
	EXEC_EXT = 
else
ifeq ($(findstring MINGW32, $(SYS)), MINGW32)
	CMP_FLAGS = -I"C:\MinGW\include\SDL2" -I"enet\include" $(DBG_FLAGS)
	LNK_FLAGS = -L"C:\MinGW\lib" -L"enet" -lmingw32 -lSDL2main -lSDL2 -lSDL2_ttf -lSDL2_image -lSDL2_mixer -lenet64 -lws2_32 -lwinmm
	EXEC_EXT = .exe
else
	$(info Unsupported system $(SYS))
endif
endif

# Util objects

HEADS_utils/geometry := utils/geometry
HEADS_utils/serialization := utils/serialization
HEADS_utils/utf8 := utils/utf8
HEADS_utils/text_completer := utils/text_completer

# Game objects

GAME_DATA := game/game_data
GAME_INTERFACES := game/game_advancer game/game_observer game/game_event_listener game/game_view game/player_interface $(GAME_DATA)
GAME_SETTINGS := game/game_settings $(GAME_DATA)
GAME_SETTINGS_INTERFACES := game/game_settings_view game/game_settings_observer game/game_settings_manipulator $(GAME_SETTINGS)

HEADS_game/game_data := $(GAME_DATA) utils/serialization
HEADS_game/game_event_listener_accumulator := game/game_event_listener_accumulator game/game_event_listener $(GAME_DATA)
HEADS_game/powerups := game/powerups $(GAME_DATA)
HEADS_game/game_logic := game/game_logic $(GAME_DATA)
HEADS_game/game_geometry := game/game_geometry game/game_logic game/collision_grid $(GAME_DATA)
HEADS_game/cheese_maker := game/cheese_maker
HEADS_game/game := game/game game/powerups game/game_geometry game/game_logic game/cheese_maker game/collision_grid game/game_event_listener_accumulator $(GAME_INTERFACES)
HEADS_game/game_settings := $(GAME_SETTINGS) utils/serialization
HEADS_game/default_settings := game/default_settings $(GAME_SETTINGS)
HEADS_game/game_settings_observer_accumulator := game/game_settings_observer_accumulator game/game_settings_observer $(GAME_SETTINGS)
HEADS_game/game_settings_manager := game/game_settings_manager game/game_settings_observer_accumulator utils/utils $(GAME_SETTINGS_INTERFACES)
HEADS_game/local_game_creator := game/local_game_creator game/game_event_listener_accumulator game/default_settings game/game_creator game/game_settings_manager game/game_settings_observer_accumulator game/game game/game_logic game/collision_grid game/powerups game/cheese_maker utils/geometry $(GAME_SETTINGS_INTERFACES) $(GAME_INTERFACES)
HEADS_game/observed_settings_view := game/observed_settings_view game/game_settings_observer_accumulator game/default_settings utils/utils $(GAME_SETTINGS_INTERFACES)
HEADS_game/composite_player_interface := game/composite_player_interface game/player_interface
HEADS_game/game_extrapolator := game/game_extrapolator game/game_logic game/game_event_listener_accumulator $(GAME_INTERFACES)

# GUI objects

HEADS_gui/powerup_images := gui/powerup_images gui/images $(GAME_DATA)
HEADS_gui/clock := gui/clock
HEADS_gui/gui := gui/gui gui/clock
HEADS_gui/subview := gui/subview
HEADS_gui/button := gui/button gui/subview
HEADS_gui/textbox := gui/textbox gui/subview gui/texts utils/utf8 utils/text_completer
HEADS_gui/key_choice_button := gui/key_choice_button gui/subview gui/texts gui/keyset
HEADS_gui/options_menu := gui/options_menu gui/gui_utils gui/button gui/subview gui/subview_manager gui/texts gui/gui_utils gui/colors
HEADS_gui/number_button := gui/number_button gui/button gui/subview gui/subview_manager gui/texts gui/colors
HEADS_gui/subview_manager := gui/subview_manager gui/subview
HEADS_gui/tab_view := gui/tab_view gui/subview gui/subview_manager gui/button gui/texts
HEADS_gui/player_settings := gui/player_settings $(GAME_SETTINGS_INTERFACES)
HEADS_gui/players_subview := gui/gui_utils game/powerups gui/player_settings gui/players_subview gui/subview_manager gui/button gui/subview gui/colors gui/texts gui/images gui/player_texture gui/sounds gui/textbox utils/text_completer gui/key_choice_button gui/keyset $(GAME_SETTINGS_INTERFACES)
HEADS_gui/game_settings_subview := gui/gui_utils game/game_settings_observer_accumulator utils/utils gui/powerup_images gui/images gui/number_button gui/options_menu gui/game_settings_subview gui/tab_view gui/subview gui/subview_manager gui/button gui/textbox utils/text_completer gui/texts gui/colors gui/images gui/player_texture gui/sounds $(GAME_SETTINGS_INTERFACES)
HEADS_gui/game_startup_menu := gui/gui_utils gui/texts gui/colors gui/sounds game/local_game_creator game/game_event_listener_accumulator gui/game_startup_menu gui/subview_manager gui/subview gui/button gui/textbox utils/text_completer gui/texts game/game_creator game/game_settings_observer_accumulator game/game game/game_logic game/collision_grid game/powerups game/cheese_maker utils/geometry network/client network/network game_network/protocol game_network/remote_game_creator game/game_extrapolator game/composite_player_interface game_network/game_client game_network/game_settings_client game/observed_settings_view $(GAME_SETTINGS_INTERFACES) $(GAME_INTERFACES)
HEADS_gui/game_menu := gui/winner_display game/game_event_listener_accumulator gui/game_gui gui/game_drawer gui/game_startup_menu game/game_creator game/game_settings_observer_accumulator gui/number_button gui/options_menu gui/player_settings gui/game_settings_subview gui/tab_view gui/game_menu gui/players_subview gui/subview_manager gui/button gui/subview gui/textbox utils/text_completer gui/key_choice_button gui/keyset network/client network/network gui/sounds gui/images gui/colors $(GAME_SETTINGS_INTERFACES) $(GAME_INTERFACES)
HEADS_gui/player_texture := gui/player_texture gui/images
HEADS_gui/colors := gui/colors
HEADS_gui/texts := gui/texts
HEADS_gui/images := gui/images
HEADS_gui/sounds := gui/sounds
HEADS_gui/keyset := gui/keyset utils/serialization utils/utils
HEADS_gui/gui_utils := gui/gui_utils
HEADS_gui/game_drawer := gui/game_drawer gui/keyset gui/images gui/player_texture gui/texts gui/gui_utils gui/colors gui/keyset game/game_logic game/game_settings $(GAME_INTERFACES) utils/geometry
HEADS_gui/game_gui := gui/winner_display gui/subview_manager gui/subview gui/button gui/sounds gui/gui gui/game_gui gui/game_drawer gui/images gui/texts gui/gui_utils gui/colors gui/keyset game/game_logic game/game_settings game/game_event_listener_accumulator $(GAME_INTERFACES)
HEADS_gui/winner_display := gui/winner_display gui/gui gui/colors gui/player_texture gui/sounds gui/images gui/texts game/game_view $(GAME_SETTINGS)

# Network objects

HEADS_network/network := network/network
HEADS_network/client := network/client network/network
HEADS_network/server := network/server network/network

# Game networking objects

GAME_PROTOCOL := game_network/protocol utils/serialization

HEADS_game_network/game_server := game_network/game_server game/game game/game_logic game/cheese_maker game/powerups game/collision_grid network/server network/network game/game_event_listener_accumulator $(GAME_INTERFACES) $(GAME_PROTOCOL)
HEADS_game_network/game_settings_server := game_network/game_server game/game game/cheese_maker game/powerups game/collision_grid game_network/game_settings_server network/server network/network game/game_settings_manager game/game_event_listener_accumulator $(GAME_SETTINGS_INTERFACES) $(GAME_INTERFACES) $(GAME_PROTOCOL)
HEADS_game_network/game_client := game_network/game_client network/client network/network $(GAME_INTERFACES) $(GAME_PROTOCOL)
HEADS_game_network/game_settings_client := game_network/game_settings_client network/client network/network game/game_settings_observer_accumulator $(GAME_SETTINGS_INTERFACES) $(GAME_PROTOCOL)
HEADS_game_network/remote_game_creator := game_network/remote_game_creator game/game_event_listener_accumulator game/game_logic game/game_extrapolator game/composite_player_interface game_network/game_client game_network/game_settings_client network/client network/network game/observed_settings_view game/game_creator game/game_settings_observer_accumulator utils/utils $(GAME_SETTINGS_INTERFACES) $(GAME_INTERFACES)

# executables

HEADS_client_main := gui/winner_display gui/game_startup_menu game/game_creator game/game_settings_observer_accumulator gui/number_button gui/options_menu gui/game_settings_subview gui/tab_view gui/key_choice_button gui/player_settings gui/textbox utils/text_completer gui/game_menu gui/players_subview gui/subview_manager gui/button gui/subview gui/gui gui/game_gui gui/game_drawer gui/texts gui/gui_utils gui/colors gui/keyset game/game game/powerups game/game_logic game/cheese_maker game/game_geometry game/collision_grid gui/images gui/sounds $(GAME_INTERFACES)
HEADS_server_main := game_network/game_settings_server network/server network/network game/default_settings $(GAME_SETTINGS_INTERFACES)

CLIENT_OBJECTS := gui/winner_display game/game_extrapolator game/composite_player_interface game_network/game_client network/client game_network/remote_game_creator game_network/game_settings_client game/observed_settings_view game/local_game_creator gui/game_startup_menu gui/powerup_images gui/number_button gui/options_menu gui/game_settings_subview gui/tab_view gui/key_choice_button gui/keyset gui/player_settings utils/utf8 gui/textbox utils/text_completer gui/colors gui/game_menu gui/players_subview gui/subview_manager gui/button gui/subview gui/gui gui/clock gui/gui_utils gui/texts gui/game_drawer gui/game_gui gui/images gui/player_texture gui/sounds client_main
SERVER_OBJECTS := game_network/game_server server_main game_network/game_settings_server network/server
COMMON_OBJECTS := network/network game/default_settings game/game_data game/game_settings game/game_settings_observer_accumulator utils/serialization game/game_settings_manager game/game_logic game/game game/powerups utils/geometry game/cheese_maker game/game_geometry game/game_event_listener_accumulator

OBJECTS_curve_fever := $(COMMON_OBJECTS) $(CLIENT_OBJECTS)

OBJECTS_server := $(COMMON_OBJECTS) $(SERVER_OBJECTS)

CLIENT_EXEC := curve_fever
SERVER_EXEC := server

# rules
OBJECTS = $(COMMON_OBJECTS) $(SERVER_OBJECTS) $(CLIENT_OBJECTS)

OBJECTS := $(addprefix build/,$(addsuffix .o,$(OBJECTS)))
SERVER_EXEC := $(addprefix build/,$(addsuffix $(EXEC_EXT),$(SERVER_EXEC)))
CLIENT_EXEC := $(addprefix build/,$(addsuffix $(EXEC_EXT),$(CLIENT_EXEC)))
EXECUTABLES := $(CLIENT_EXEC) $(SERVER_EXEC)

all: client server

.PHONY: client server docker

client: $(CLIENT_EXEC)

server: $(SERVER_EXEC)

docker: Dockerfile
	sudo docker build -t jonatan_h/curve-fever .

clear:
	$(DEL) $(OBJECTS)

clear_all: clear
	$(DEL) $(EXECUTABLES)

.SECONDEXPANSION:
$(EXECUTABLES): build/%$(EXEC_EXT): $$(addprefix build/,$$(addsuffix .o,$$(OBJECTS_$$*)))
	mkdir -p $(dir $@)
	$(CC) $(CMP_FLAGS) $^ -o $@ $(LNK_FLAGS)
	
$(OBJECTS): build/%.o: src/%.cpp $$(addprefix src/,$$(addsuffix .h,$$(HEADS_$$*)))
	mkdir -p $(dir $@)
	$(CC) $(CMP_FLAGS) -c $< -o $@
