#---------------------------------------------------------------------------------
# Clear the implicit built in rules
#---------------------------------------------------------------------------------
.SUFFIXES:
#---------------------------------------------------------------------------------
ifeq ($(strip $(DEVKITPPC)),)
$(error "Please set DEVKITPPC in your environment. export DEVKITPPC=<path to>devkitPPC")
endif

include $(DEVKITPPC)/wii_rules

#---------------------------------------------------------------------------------
# TARGET is the name of the output
# BUILD is the directory where object files & intermediate files will be placed
# SOURCES is a list of directories containing source code
# INCLUDES is a list of directories containing extra header files
#---------------------------------------------------------------------------------
WIREDIR			:=  ../../../../../Wire
FREETYPEDIR		:=	../../../../../Tools/freetype
BULLETDIR		:=  ../../../../../Tools/bullet-2.77
TARGET			:=	Importer
SOURCES			:=	../../src
DATA			:=
INCLUDES		:=
ifeq ($(WIRE_DEBUG),y)
BUILD		:=	Debug
LIBIMPORTER		:=	libImporterd.a
BUILDPOSTFIX	:=	d
else
BUILD		:=	Release
LIBIMPORTER		:=	libImporter.a
BUILDPOSTFIX	:=
endif
LIBWIRE_LIB 	:=	$(WIREDIR)/lib/gcc_wii/$(BUILD)

#---------------------------------------------------------------------------------
# options for code generation
#---------------------------------------------------------------------------------

CFLAGS	= -save-temps -g -O2 -Wall $(MACHDEP) $(INCLUDE) -DWIRE_WII
ifeq ($(WIRE_DEBUG),y)
CFLAGS += -DWIRE_DEBUG
endif
CXXFLAGS	=	$(CFLAGS)

LDFLAGS	=	-g $(MACHDEP) -Wl,-Map,$(notdir $@).map

#---------------------------------------------------------------------------------
# any extra libraries we wish to link with the project
#---------------------------------------------------------------------------------
LIBS	:= -lBulletDynamics -lBulletCollision -lLinearMath -lWire$(BUILDPOSTFIX) -lfreetype -lwiiuse -lbte -lfat -logc -lm

#---------------------------------------------------------------------------------
# list of directories containing libraries, this must be the top level containing
# include and lib
#---------------------------------------------------------------------------------
LIBDIRS	:=

#---------------------------------------------------------------------------------
# no real need to edit anything past this point unless you need to add additional
# rules for different file extensions
#---------------------------------------------------------------------------------
ifneq ($(BUILD),$(notdir $(CURDIR)))
#---------------------------------------------------------------------------------

export OUTPUT	:=	$(CURDIR)/$(TARGET)$(BUILDPOSTFIX)

export VPATH	:=	$(foreach dir,$(SOURCES),$(CURDIR)/$(dir)) \
					$(foreach dir,$(DATA),$(CURDIR)/$(dir))

export DEPSDIR	:=	$(CURDIR)/$(BUILD)

#---------------------------------------------------------------------------------
# automatically build a list of object files for our project
#---------------------------------------------------------------------------------
CFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.c)))
CPPFILES	:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.cpp)))
sFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.s)))
SFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.S)))
BINFILES	:=	$(foreach dir,$(DATA),$(notdir $(wildcard $(dir)/*.*)))

#---------------------------------------------------------------------------------
# use CXX for linking C++ projects, CC for standard C
#---------------------------------------------------------------------------------
ifeq ($(strip $(CPPFILES)),)
	export LD	:=	$(CC)
else
	export LD	:=	$(CXX)
endif

export OFILES	:=	$(addsuffix .o,$(BINFILES)) \
					$(CPPFILES:.cpp=.o) $(CFILES:.c=.o) \
					$(sFILES:.s=.o) $(SFILES:.S=.o)

#---------------------------------------------------------------------------------
# build a list of include paths
#---------------------------------------------------------------------------------
export INCLUDE	:=	$(foreach dir,$(INCLUDES),-I$(CURDIR)/$(dir)) \
					$(foreach dir,$(LIBDIRS),-I$(dir)/include) \
					-I$(CURDIR)/$(BUILD) \
					-I$(LIBOGC_INC) \
					-I$(WIREDIR)/Foundation \
					-I$(WIREDIR)/Foundation/Wii \
					-I$(WIREDIR)/Engine \
					-I$(FREETYPEDIR)/include \
					-I$(BULLETDIR)/src

#---------------------------------------------------------------------------------
# build a list of library paths
#---------------------------------------------------------------------------------
export LIBPATHS	:=	$(foreach dir,$(LIBDIRS),-L$(dir)/lib) \
					-L$(LIBOGC_LIB) -L$(LIBWIRE_LIB) -L$(FREETYPEDIR) -L$(BULLETDIR)/lib/wii

export OUTPUT	:=	$(CURDIR)/$(TARGET)
.PHONY: $(BUILD) clean

#---------------------------------------------------------------------------------
$(BUILD):
	@[ -d $@ ] || mkdir -p $@
	@$(MAKE) --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile
	@mkdir -p ../../lib/gcc_wii/$(BUILD)
	@cp -p $(BUILD)/$(LIBIMPORTER) ../../lib/gcc_wii/$(BUILD)/$(LIBIMPORTER)

#---------------------------------------------------------------------------------
clean:
	@echo clean ...
	@rm -f $(BUILD)/*.*
	@rm -f ../../lib/gcc_wii/$(BUILD)/$(LIBIMPORTER)
#---------------------------------------------------------------------------------
lib: $(LIBIMPORTER)

#---------------------------------------------------------------------------------
else

DEPENDS	:=	$(OFILES:.o=.d)

#---------------------------------------------------------------------------------
# main targets
#---------------------------------------------------------------------------------
$(LIBIMPORTER): $(OFILES)

-include $(DEPENDS)

#---------------------------------------------------------------------------------
endif
#---------------------------------------------------------------------------------
