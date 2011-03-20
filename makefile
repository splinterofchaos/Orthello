
# NOTE:
# LDFLAGS are flags sent to the linker.
# CFLAGS are sent to the C compiler.

ifeq "$(OS)" "Windows_NT"
	# Openmp seems to do more bad than good, so don't include it.
	EXTRA =
	LDFLAGS = -lmingw32 -lSDLmain -lSDL -lopengl32 #-lSDL_mixer -lgdi32 -lSDL_ttf
	OUT     = run.exe
else
	# Linux
	EXTRA =
	LDFLAGS = -lGL -lX11 -lSDL -lSDL_mixer -lSDL_ttf
	OUT     = run
endif

 
CC = g++ 

CFLAGS  += -Wall -Wextra

compile = ${CC} ${CFLAGS} ${EXTRA} -std=gnu++0x -c 
link    = ${CC} ${CFLAGS} ${EXTRA} -std=gnu++0x -o ${OUT}

OBJ = .Timer.o .Keyboard.o .Collision.o .Random.o .Player.o .Platform.o .Screen.o .Texture.o .World.o

${OUT} : ${OBJ} main.cpp Draw.* makefile 
	${link} main.cpp -std=c++0x ${OBJ} ${LDFLAGS}

.Screen.o : Screen.*
	${compile} Screen.cpp    -o .Screen.o

.World.o : World.*
	${compile} World.cpp     -o .World.o

.Texture.o : Texture.*
	${compile} Texture.cpp   -o .Texture.o

.Player.o : Player.* Texture.o
	${compile} Player.cpp    -o .Player.o
.Platform.o : Platform.*
	${compile} Platform.cpp  -o .Platform.o

.Random.o : Random.* Texture.o
	${compile} Random.cpp    -o .Random.o

.Collision.o : Collision.*
	${compile} Collision.cpp -o .Collision.o

.Keyboard.o : Keyboard.*
	${compile} Keyboard.cpp  -o .Keyboard.o

.Timer.o : Timer.*
	${compile} Timer.cpp     -o .Timer.o

clean:
	rm *.o



