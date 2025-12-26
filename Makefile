# # linuxa
CC=gcc
EXECUTABLE=zim
CFLAGS = -g -Wall -lm -DLINUX_COMPILE -DLINUX_INSTALL -m64

LIBS = -lncurses  $(shell pkg-config --libs x11)

LDLIBS = -lm -static-libgcc $(LIBS)


SOURCES=main.c \
text_editor.c log.c file_browser.c config.c x11.c



CC_MINGW= x86_64-w64-mingw32-gcc

EXECUTABLE_WINDOWS=zim.exe
CFLAGS_WINDOWS = -g -Wall -lm -DWINDOWS -DWINDOWS_INSTALL  -DWINDOWS_COMPILE -I/usr/x86_64-w64-mingw32/usr/include

LIBS_WINDOWS = -lmingw32 -lkernel32 -luser32 -lgdi32 -m64  -mwindows

LDLIBS_WINDOWS = -lm -static-libgcc $(LIBS_WINDOWS)
# add -pg for gdb


SOURCES_WINDOWS=main_windows.c text_editor.c log.c file_browser.c config.c 

OBJECTS=$(SOURCES:.c=.o)
OBJECTS_MINGW=$(SOURCES_WINDOWS:.c=.o)


# windows: $(SOURCES_WINDOWS) $(EXECUTABLE_WINDOWS)

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(OBJECTS) $(LDLIBS) -o $@

$(EXECUTABLE_WINDOWS): $(OBJECTS_MINGW) 
	$(CC_MINGW) $(OBJECTS_MINGW) $(LDLIBS_WINDOWS) -o $@

# wihndows unused right now
# all: createResourcesO $(SOURCES) $(EXECUTABLE)

# $(EXECUTABLE): $(OBJECTS) icon.o
# 	$(CC) $(OBJECTS) icon.o $(LDLIBS) -o $@

# createResourcesO: icon.rc
# 	windres icon.rc -o icon.o

.c.o:
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm *.o
