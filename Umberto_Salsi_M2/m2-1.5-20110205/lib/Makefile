M2 = ../bin/m2
#M2FLAGS = --no-warn
MODULES = \
m2runtime \
m2 \
str \
unx \
io \
DirList \
encode \
base64 \
tcltk \
time \
serialize \
buffer \
math \
cgi \
date \
digest \
encrypt \
fixed \
gcry_common \
gcry_md \
gcry_cipher \
gcry_mpi \
gcry_ac \
gl \
glut \
iconv \
m3d \
net \
parser \
pg \
win \
gtk \
fmt \
img \

all:
	@for m in $(MODULES); do \
		if [ -e $$m.def ]; then \
			if [ -e $$m.imp ]; then \
				if [ $$m.def -nt $$m.imp ] \
				|| [ ! -e $$m.c ] \
				|| [ $$m.imp -nt $$m.c ]; then \
					[ $$m.def -nt $$m.imp ] && touch $$m.imp ;\
					echo "Compiling $$m.imp..." ;\
					$(M2) $(M2FLAGS) $$m.imp || exit 1;\
				fi ;\
			elif [ ! -e $$m.c ]; then \
				echo "WARNING: missing implementation for module '$$m'" ;\
			fi ;\
		else \
			echo "WARNING: missing module '$$m.def'" ;\
		fi ;\
	done || true

clean:
	@for m in $(MODULES); do \
		[ -e $$m.c -a -e $$m.imp ] && rm -f $$m.c $$m.lnk ;\
	done || true

distclean: clean

dist: clean

test: all

web: clean

install: all

uninstall:

upgrade:
