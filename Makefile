
include $(ROOT)/Makefile.define

# 需要建立的目录
PATHS		= bin lib include build \
			  bin/log bin/config

# 项目中需要用第三方库
THIRDLIBS 	= evlite leveldb \
			   tcmalloc_minimal
ROOT = $(HOME)/tinydb

# 定义工程中的所有项目
DATAD		= $(ROOT)/src/tinydb

# 定义工程
SOLUTION	= $(DATAD)

.PHONY: all install uninstall release clean test $(SOLUTION)

PATHS	  := $(addprefix $(ROOT)/, $(PATHS))
THIRDLIBS := $(addprefix lib, $(THIRDLIBS))
THIRDLIBS := $(addsuffix .so, $(THIRDLIBS))

all : $(SOLUTION)
test : $(SOLUTION)
clean : $(SOLUTION)

install : $(PATHS) $(THIRDLIBS)
	$(COPY) -a $(ROOT)/config/* $(ROOT)/bin/config/
	$(RM) $(RMFLAGS) $(ROOT)/bin/lib; $(LINK) -s $(ROOT)/lib $(ROOT)/bin/lib

uninstall :
	$(RM) $(RMFLAGS) $(ROOT)/bin/scripts
	$(RM) $(RMFLAGS) $(PATHS)

$(PATHS) :
	$(MKDIR) -p $@

$(SOLUTION):
	$(MAKE) -C $@ $(MAKECMDGOALS)

$(THIRDLIBS) : lib% :
	$(COPY) $(shell /sbin/ldconfig -p | grep $@ | cut -d '>' -f 2 | head -1)  $(ROOT)/lib/

# 工程中项目依赖
$(DATAD) :
