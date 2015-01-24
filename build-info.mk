# Desktop
_DESKTOP_LIBS = -lSDL2 -lGLEW -lGL
_DESKTOP_CFLAGS =
_DESKTOP_CXXFLAGS = 
_DESKTOP_SOURCES = \
	source/media/desktop.c

# Android
_ANDROID_LIBS = -llog -landroid  -lEGL -lGLESv2
_ANDROID_CFLAGS =
_ANDROID_CXXFLAGS = 
_ANDROID_SOURCES = \
	source/media/android.c

# Common
_CFLAGS = 
_CXXFLAGS = -std=c++11
_HEADERS = \
	source/media/media.h \
	source/graphics/graphics.h \
	source/vec.hpp \
	source/circle.hpp \
	source/pool.hpp

_SOURCES = \
	source/graphics/graphics.c \
	source/pool.cpp \
	source/circle.cpp \
	source/main.cpp
