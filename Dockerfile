FROM jonatan_h/sdl2

# Create workspace
WORKDIR /usr/src/app

COPY src src
COPY enet enet
COPY data data
COPY Makefile Makefile

# Install needed packages
RUN apt -y install libenet-dev

# Build curve fever
RUN make

EXPOSE 32123

# Deafult command
CMD ["build/curve_fever"]
