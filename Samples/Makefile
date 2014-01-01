SUBDIRS:= `ls | egrep -v '^(CVS)$$'`

DATESTRING	:=	$(shell date +%Y)$(shell date +%m)$(shell date +%d)

all:
	@for i in $(SUBDIRS); do if test -e $$i/builds/gcc_wii/Makefile ; then $(MAKE) -C $$i/builds/gcc_wii || { exit 1;} fi; done;

clean:
	@for i in $(SUBDIRS); do if test -e $$i/builds/gcc_wii/Makefile ; then $(MAKE) -C $$i/builds/gcc_wii clean || { exit 1;} fi; done;
