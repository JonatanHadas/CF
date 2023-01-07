docker run \
	-v "$PWD"/game_data:/usr/src/app/game_data \
	-v /tmp/.X11-unix:/tmp/.X11-unix \
	-v /dev:/dev \
	--env DISPLAY="$DISPLAY" \
	--net=host \
	-it --rm jonatan_h/curve-fever
