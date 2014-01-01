SUBDIRS:= `ls | egrep -v '^(CVS)$$'`

DATESTRING	:=	$(shell date +%Y)$(shell date +%m)$(shell date +%d)

all:
	@$(MAKE) -C ../../Wire/builds/gcc_wii
	@$(MAKE) -C ../../Tools
	@$(MAKE) -C ../../Samples

clean:
	@$(MAKE) -C ../../Wire/builds/gcc_wii clean
	@$(MAKE) -C ../../Tools clean
	@$(MAKE) -C ../../Samples clean
