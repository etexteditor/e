VERBOSE=0
ECORE=ecore

ifeq ($(VERBOSE),0)
	ECHO=echo
	SILENT=@
else
	ECHO=true
	SILENT=
endif

WXINCLUDES = $(shell ../external/out/bin/wx-config --cppflags)
WXLIBS = $(shell ../external/out/bin/wx-config --libs)
GTKINCLUDES = -I/usr/include/gtk-2.0 -I/usr/lib/gtk-2.0/include -I/usr/include/cairo -I/usr/include/pango-1.0 -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include -I/usr/include/atk-1.0
LIBINCLUDES = -I../external/out/include -I../external/out/usr/include -I/usr/include/out/include/curl -I../external/tinyxml -I../external/pcre -I../external/libtomcrypt -I../external/libtommath

INCLUDES = $(WXINCLUDES) $(GTKINCLUDES) $(LIBINCLUDES) -I../ecore -I.

CMPOPTS = -Wall  -fno-strict-aliasing -DHAVE_CONFIG_H -O2
EXCEPT = CygwinDlg.cpp ExceptionHandler.cpp ExceptionAttacher.cpp GetWinVer.cpp IEHtmlWin.cpp MiniVersion.cpp MozillaHtmlWnd.cpp ShellContextMenu.cpp wxactivex.cpp
SRCS = $(filter-out $(EXCEPT), $(wildcard *.cpp)) 
OBJS = $(SRCS:.cpp=.o)
DEPS = $(OBJS:.o=.d)

LOPTS = $(WXLIBS) -lpcre -lcurl -ltomcrypt -ltommath -lmk4

CPP = g++
EXE = e

.PHONY: all clean

all: $(EXE)

$(EXE): $(OBJS)
	@$(ECHO) "LD $@"
	$(SILENT)$(CPP) -o $(EXE) $(OBJS) -L../ecore -l$(ECORE) $(LOPTS) $(shell ../external/out/bin/wx-config --linkdeps) ../external/tinyxml/tinystr.o ../external/tinyxml/tinyxmlerror.o ../external/tinyxml/tinyxml.o ../external/tinyxml/tinyxmlparser.o

%.o: %.cpp
	@$(ECHO) "CC $<"
	$(SILENT)$(CPP) $(INCLUDES) $(CMPOPTS) -MD -c -o $@ $<

clean:
	rm -f $(OBJS) $(EXE) $(DEPS)

-include $(DEPS)
