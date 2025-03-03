###########################################################
# VARIABLES
###########################################################
TARGET = gameengine

CC = g++

LANG_STD = -std=c++17

COMPILER_FLAGS = -Wall -Wfatal-errors

INCLUDE_PATHS = -I"./libs" \
				-I/opt/homebrew/include

SOURCE = src/*.cpp \
		 src/Game/*.cpp \
		 src/Logger/*.cpp \
		 src/ECS/*.cpp \
		 src/AssetStore/*.cpp

LINKER_FLAGS = -lSDL2 \
			   -lSDL2_image \
			   -lSDL2_mixer \
			   -lSDL2_ttf \
			   -llua

LINKER_PATHS = -L/opt/homebrew/lib
###########################################################
# RULES
###########################################################
build:
	${CC} ${COMPILER_FLAGS} ${LANG_STD} ${INCLUDE_PATHS} ${SOURCE} ${LINKER_PATHS} ${LINKER_FLAGS} -o ${TARGET};

run:
	./${TARGET};

clean:
	rm ${TARGET};