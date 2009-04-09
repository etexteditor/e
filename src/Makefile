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

GTKINCLUDES = -I/usr/include/gtk-2.0 \
              -I/usr/include/cairo \
              -I/usr/include/pango-1.0 \
              -I/usr/include/atk-1.0 \
              -I/usr/include/glib-2.0 \
              -I/usr/lib/glib-2.0/include \
              -I/usr/lib/gtk-2.0/include \

OURINCLUDES = -I../external/out/include \
              -I../external/out/usr/include \
              -I../external/tinyxml \
              -I../external/pcre \
              -I../external/libtomcrypt \
              -I../external/libtommath \
              -I../external/webkit/WebKit

INCLUDES = $(WXINCLUDES) $(OURINCLUDES) $(GTKINCLUDES) -I../ecore -I.

OURLIBPATHS = -L../external/out/lib \
              -L../external/webkit/WebKitBuild/Release

WXLIBS = $(shell ../external/out/bin/wx-config --libs)
WEBKIT_LIBS = -lwxwebkit -lwebcore-wx -ljscore
WEBKIT_LIBS += $(shell pkg-config --libs libxml-2.0 sqlite3 libxslt) $(shell icu-config --ldflags)

LIBS = $(OURLIBPATHS) -L../ecore -l$(ECORE) -lcurl -ltomcrypt -ltommath -lmk4 $(WEBKIT_LIBS) $(WXLIBS)

# Exceptions which do not need to be compiled on linux
EXCEPT = CygwinDlg.cpp \
         ExceptionHandler.cpp \
         ExceptionAttacher.cpp \
         GetWinVer.cpp \
         IEHtmlWin.cpp \
         MiniVersion.cpp \
         ShellContextMenu.cpp \
         wxactivex.cpp

SRCS = $(filter-out $(EXCEPT), $(wildcard *.cpp)) 
OBJS = $(SRCS:.cpp=.o)
DEPS = $(OBJS:.o=.d)

EXTOBJS = $(shell ../external/out/bin/wx-config --linkdeps) \
          ../external/tinyxml/tinystr.o \
          ../external/tinyxml/tinyxmlerror.o \
          ../external/tinyxml/tinyxml.o \
          ../external/tinyxml/tinyxmlparser.o

CXXFLAGS = -Wall -fno-strict-aliasing -DHAVE_CONFIG_H -DFEAT_BROWSER -O2 -MD

EXE = e

.PHONY: all clean

all: $(EXE)

$(EXE): $(OBJS)
	@$(ECHO) "[LD] $@"
	$(SILENT)$(CXX) -o $(EXE) $(OBJS) $(EXTOBJS) $(LIBS)

%.o: %.cpp
	@$(ECHO) "[CC] $<"
	$(SILENT)$(CXX) $(INCLUDES) $(CXXFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJS) $(EXE) $(DEPS)

-include $(DEPS)

