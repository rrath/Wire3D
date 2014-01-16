SAMPLESSUBDIRS:= `ls ../../Samples | egrep -v '^(CVS)$$'`
TOOLSSUBDIRS:= `ls ../../Tools | egrep -v '^(CVS)$$'`

DATESTRING	:=	$(shell date +%Y)$(shell date +%m)$(shell date +%d)

all:
	@$(MAKE) -C ../../Wire/builds/gcc_wii
	@$(MAKE) -C ../../Tools/freetype
	@for i in $(TOOLSSUBDIRS); do if test -e ../../Tools/$$i/builds/gcc_wii/Makefile ; then $(MAKE) -C ../../Tools/$$i/builds/gcc_wii || { exit 1;} fi; done;
	@for i in $(SAMPLESSUBDIRS); do if test -e ../../Samples/$$i/builds/gcc_wii/Makefile ; then $(MAKE) -C ../../Samples/$$i/builds/gcc_wii || { exit 1;} fi; done;

clean:
	@$(MAKE) -C ../../Wire/builds/gcc_wii clean
	@$(MAKE) -C ../../Tools/freetype clean
	@for i in $(TOOLSSUBDIRS); do if test -e ../../Tools/$$i/builds/gcc_wii/Makefile ; then $(MAKE) -C ../../Tools/$$i/builds/gcc_wii clean || { exit 1;} fi; done;
	@for i in $(SAMPLESSUBDIRS); do if test -e ../../Samples/$$i/builds/gcc_wii/Makefile ; then $(MAKE) -C ../../Samples/$$i/builds/gcc_wii clean || { exit 1;} fi; done;
