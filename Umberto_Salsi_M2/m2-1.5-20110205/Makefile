export PROGNAME := m2
export VER := 1.5
export WEBDIR := /home/www.icosaedro.it/public_html/m2

DIRS := bin doc src lib test
BUILD := $(shell date +%Y%m%d)

DISTFILE := ${PROGNAME}-${VER}-${BUILD}

all: sub-dirs
	@[ -f include/missing.c ] || echo "ERROR: you must run ./configure first"

# SUBROUTINE:
sub-dirs:
	for d in bin doc include src lib test; do \
		$(MAKE) -C $$d  ${MAKECMDGOALS} ; \
	done

clean: sub-dirs
	rm -f HelloWorld

dist: distclean
	echo ${VER}-${BUILD} > VERSION
	echo "#define VERSION \"${VER}-${BUILD}\"" > src/version.h
	cd doc/reference \
	&& wget --timestamping www.icosaedro.it/m2/reference/index.html \
	&& echo -e ",s@/img/@@g\nw" | (ed -s index.html || true) \
	&& wget --timestamping www.icosaedro.it/m2/reference/memory.gif \
	&& wget --timestamping www.icosaedro.it/img/icosaedro-16x16.png \
	&& wget --timestamping www.icosaedro.it/img/m2.png
	cd .. && \
		ln -sf $(PROGNAME) $(DISTFILE) && \
		tar cvhf - $(DISTFILE) --owner=root --group=root --exclude=CVS \
			| gzip > /tmp/$(DISTFILE).tar.gz && \
		rm $(DISTFILE) && \
		echo "Package available in /tmp/${DISTFILE}.tar.gz"

distclean: sub-dirs
	rm -f lib/endianess.h include/missing.c

web: dist
	@cp CHANGES ${WEBDIR}
	@cd ${WEBDIR} && \
		mv /tmp/${DISTFILE}.tar.gz . && \
		echo ${VER}-${BUILD} > version && \
		touch index.html && \
		touch report.html && \
		touch reference/index.html && \
		touch library.html && \
		touch download.html
	@echo "Now rebuilding the program..."
	$(MAKE) all
	bnf_chk --print-html --print-index doc/bnf.txt > ${WEBDIR}/report/bnf
	echo "Rebuilding..."
	./configure && make

test: sub-dirs

install:
	@echo ""
	@echo "M2 Installation Procedure"
	@echo "========================="
	@echo ""
	@echo "M2 does not require to be installed on any particular place."
	@echo "Just use the script ./bin/m2 to run the compiler. Read the"
	@echo "INSTALL file for some hints."
	@echo ""
	@echo "Read more about the M2 language on:"
	@echo ""
	@echo "     http://www.icosaedro.it/m2/"
	@echo ""

uninstall:
	@echo ""
	@echo "M2 Uninstallation Procedure"
	@echo "==========================="
	@echo ""
	@echo "Since M2 do not require to be installed on any particular place,"
	@echo "you may simply delete this directory."
	@echo ""

upgrade:
	@echo ""
	@echo "M2 Upgrade Procedure"
	@echo "===================="
	@echo ""
	@echo "The version of this package is `cat VERSION`. Check for new releases on:"
	@echo ""
	@echo "    http://www.icosaedro.it/m2/download.html"
	@echo ""
