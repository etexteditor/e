VERBOSE=0
DEBUG=0

ifeq ($(VERBOSE),0)
	ECHO=echo
	SILENT=@
else
	ECHO=true
	SILENT=
endif

ifeq ($(DEBUG),0)
	ECORE    = ecore
	CXXFLAGS = -O2
	LDFLAGS  =
	EXT_DIR  = ../external/out.release
	OUTDIR   = .objs.release
	EXE      = e
else
	ECORE    = ecored
	CXXFLAGS = -O0 -g
	LDFLAGS  = -g
	EXT_DIR  = ../external/out.debug
	OUTDIR   = .objs.debug
	EXE      = e.debug
endif

WXINCLUDES = $(shell $(EXT_DIR)/bin/wx-config --cppflags)

GTKINCLUDES = $(shell pkg-config --cflags gtk+-2.0 cairo pango atk glib-2.0)

OURINCLUDES = -I$(EXT_DIR)/include

INCLUDES = $(WXINCLUDES) $(OURINCLUDES) $(GTKINCLUDES) -I../ecore -I.

OURLIBPATHS = -L$(EXT_DIR)/lib
WXLIBS = $(shell $(EXT_DIR)/bin/wx-config --libs)
WEBKIT_LIBS = -lwxwebkit -lwebcore-wx -ljscore

LIBS = $(OURLIBPATHS) -L../ecore -l$(ECORE) -lcurl -ltomcrypt -ltommath -lmk4 -lpcre -ltinyxml $(WEBKIT_LIBS) $(WXLIBS)

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
OBJS = $(SRCS:%.cpp=$(OUTDIR)/%.o)
DEPS = $(SRCS:%.cpp=$(OUTDIR)/%.d)

EXTOBJS = $(shell $(EXT_DIR)/bin/wx-config --linkdeps) $(shell pkg-config --libs libxml-2.0 sqlite3 libxslt) $(shell icu-config --ldflags)

CXXFLAGS += -Wall -fno-strict-aliasing -DHAVE_CONFIG_H -DFEAT_BROWSER -MD

.PHONY: all clean

all: $(EXE)

$(EXE): $(OBJS)
	@$(ECHO) "[LD] $@"
	$(SILENT)$(CXX) $(LDFLAGS) -o $(EXE) $(OBJS) $(LIBS) $(EXTOBJS)

$(OUTDIR)/%.o: %.cpp
	@-mkdir -p $(OUTDIR)
	@$(ECHO) "[CC] $<"
	$(SILENT)$(CXX) $(INCLUDES) $(CXXFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJS) $(EXE) $(DEPS)

-include $(DEPS)

