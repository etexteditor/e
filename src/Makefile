VERBOSE=0
DEBUG=0

VERSION=2.0
RELEASE=1
ROOTDIR=..

ifeq ($(VERBOSE),0)
	ECHO=echo
	SILENT=@
	RPM_EXTRA=--quiet
else
	ECHO=true
	SILENT=
	RPM_EXTRA=
endif

ifeq ($(DEBUG),0)
	ECORE    = ecore
	CXXFLAGS = -O2
	LDFLAGS  =
	EXT_DIR  = $(ROOTDIR)/external/out.release
	OUTDIR   = .objs.release
	EXE      = e
	PKGSUFF  =
else
	ECORE    = ecore
	CXXFLAGS = -O0 -g
	LDFLAGS  = -g
	EXT_DIR  = $(ROOTDIR)/external/out.debug
	OUTDIR   = .objs.debug
	EXE      = e.debug
	PKGSUFF  = -debug
endif

# Use 64bit version of libecore on x86_64
LBITS := $(shell getconf LONG_BIT)
ifeq ($(LBITS),64)
	ECORE    := $(ECORE)64
endif

E_DEPLOY_DIR = $(ROOTDIR)/.deploy
E_STUFF_DIR  = $(ROOTDIR)/external/stuff

WXINCLUDES = $(shell $(EXT_DIR)/bin/wx-config --cppflags)

GTKINCLUDES = $(shell pkg-config --cflags gtk+-2.0 cairo pango atk glib-2.0)

OURINCLUDES = -I$(EXT_DIR)/include

INCLUDES = $(WXINCLUDES) $(OURINCLUDES) $(GTKINCLUDES) -I$(ROOTDIR)/ecore -I.

OURLIBPATHS = -L$(EXT_DIR)/lib
WXLIBS = $(shell $(EXT_DIR)/bin/wx-config --libs)
WEBKIT_LIBS = -lwxwebkit -ljscore
BOOST_LIBS = -lboost_system

LIBS = $(OURLIBPATHS) -L$(ROOTDIR)/ecore -l$(ECORE) -lcurl -ltomcrypt -ltommath -lmk4 -lpcre -ltinyxml $(WEBKIT_LIBS) $(WXLIBS) $(BOOST_LIBS)

# Exceptions which do not need to be compiled on linux
EXCEPT = CygwinDlg.cpp \
         ExceptionHandler.cpp \
         ExceptionAttacher.cpp \
         GetWinVer.cpp \
         IEHtmlWin.cpp \
         MiniVersion.cpp \
         ShellContextMenu.cpp \
         wxactivex.cpp

SRCS = $(filter-out $(EXCEPT), $(wildcard *.cpp hessian_ipc/*.cpp webconnect/*.cpp))
OBJS = $(SRCS:%.cpp=$(OUTDIR)/%.o)
DEPS = $(SRCS:%.cpp=$(OUTDIR)/%.d)

EXTOBJS = $(shell $(EXT_DIR)/bin/wx-config --linkdeps) $(shell pkg-config --libs libxml-2.0 sqlite3 libxslt) $(shell icu-config --ldflags)

CXXFLAGS += -Wall -fno-strict-aliasing -DHAVE_CONFIG_H -DFEAT_BROWSER -MD

.PHONY: all clean prep-tree tar rpm deb .test-stuff .FORCE

all: $(EXE)

$(EXE): $(OBJS)
	@$(ECHO) "[LD] $@"
	$(SILENT)$(CXX) $(LDFLAGS) -o $(EXE) $(OBJS) $(LIBS) $(EXTOBJS)

$(OUTDIR)/%.o: %.cpp
	@-mkdir -p $(OUTDIR) $(OUTDIR)/hessian_ipc $(OUTDIR)/webconnect
	@$(ECHO) "[CC] $<"
	$(SILENT)$(CXX) $(INCLUDES) $(CXXFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJS) $(EXE) $(DEPS)

.test-stuff: .FORCE
	@$(ECHO) "[TEST] $(E_STUFF_DIR) contents"
	$(SILENT)test -d $(E_STUFF_DIR) -a -d $(E_STUFF_DIR)/Themes -a -d $(E_STUFF_DIR)/Support
	$(SILENT)test -d $(E_STUFF_DIR)/Bundles -a -d $(E_STUFF_DIR)/Bundles/Text.tmbundle

prep-tree: $(EXE) .test-stuff
	$(SILENT)rm -rf $(E_DEPLOY_DIR)
	@$(ECHO) "[MKDIR] $(E_DEPLOY_DIR)/opt/e"
	$(SILENT)mkdir -p $(E_DEPLOY_DIR)/opt/e
	@$(ECHO) "[CP] $(EXE)"
	$(SILENT)cp -f $(EXE) $(E_DEPLOY_DIR)/opt/e/e
ifeq ($(DEBUG),0)
	@$(ECHO) "[STRIP] $(EXE)"
	$(SILENT)strip $(E_DEPLOY_DIR)/opt/e/e
endif
	@$(ECHO) "[CP] Bundles, Support and Themes from $(E_STUFF_DIR)"
	$(SILENT)cp -Rf $(E_STUFF_DIR)/Bundles $(E_STUFF_DIR)/Support $(E_STUFF_DIR)/Themes $(E_DEPLOY_DIR)/opt/e

tar: prep-tree
	@$(ECHO) "[TAR] e-$(VERSION).$(RELEASE).tar.gz"
	$(SILENT)tar -czf ../e-$(VERSION).$(RELEASE)$(PKGSUFF).tar.gz -C $(E_DEPLOY_DIR) opt/e

rpm: prep-tree
	$(SILENT)sed "s/%VERSION%/$(VERSION)/; s/%RELEASE%;$(RELEASE)/; s/%DEPLOY%/$(E_DEPLOY_DIR)/" \
		$(E_STUFF_DIR)/e-rpm.spec.in > $(OUTDIR)/e-rpm.spec
	@$(ECHO) "[RPMBUILD] $(OUTDIR)/e-rpm.spec"
	$(SILENT)rpmbuild -bb $(RPM_EXTRA) $(OUTDIR)/e-rpm.spec
	$(SILENT)cp /usr/src/packages/RPMS/*/e*rpm ../

deb: prep-tree
	$(SILENT)mkdir -p $(E_DEPLOY_DIR)/DEBIAN
	$(SILENT)sed "s/%VERSION%/$(VERSION)/; s/%RELEASE%/$(RELEASE)/" $(E_STUFF_DIR)/control.in > $(E_DEPLOY_DIR)/DEBIAN/control
	@$(ECHO) "[DPKG] $(E_DEPLOY_DIR)"
	$(SILENT)dpkg -b $(E_DEPLOY_DIR) ../e-$(VERSION)-$(RELEASE)$(PKGSUFF).deb

-include $(DEPS)

