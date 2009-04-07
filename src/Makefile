WXINCLUDES = $(shell wx-config --cppflags)
GTKINCLUDES = -I/usr/include/gtk-2.0 -I/usr/lib/gtk-2.0/include -I/usr/include/cairo -I/usr/include/pango-1.0 -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include -I/usr/include/atk-1.0
LIBINCLUDES = -I../external/out/include -I../external/out/usr/include -I/usr/include/out/include/curl -I../external/tinyxml -I../external/pcre -I../external/libtomcrypt -I../external/libtommath

INCLUDES = $(WXINCLUDES) $(GTKINCLUDES) $(LIBINCLUDES) -I../ecore -I.

CMPOPTS = -Wno-write-strings -DHAVE_CONFIG_H
EXCEPT = CygwinDlg.cpp ExceptionHandler.cpp ExceptionAttacher.cpp GetWinVer.cpp IEHtmlWin.cpp MiniVersion.cpp MozillaHtmlWnd.cpp ShellContextMenu.cpp wxactivex.cpp
SRCS = $(filter-out $(EXCEPT), $(wildcard *.cpp)) 
OBJS = $(SRCS:.cpp=.o)

CPP = g++
EXE = e

all: $(EXE)

$(EXE): $(OBJS)
	$(CPP) -o $(EXE) $(OBJS)

%.o: %.cpp
	$(CPP) $(INCLUDES) $(OPTS) $(CMPOPTS) -c -o $@ $<

clean:
	rm -f $(OBJS) $(EXE)

