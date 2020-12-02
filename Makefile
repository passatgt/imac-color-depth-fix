imaccolordepthfix: imaccolordepthfix.c header.h
	gcc -o imaccolordepthfix imaccolordepthfix.c -framework IOKit -framework ApplicationServices -Wno-deprecated-declarations
