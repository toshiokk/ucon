all : imjcuitest

install :
	mkdir -pv ~/.imj
	cp -auv imj.dic imj.hst ~/.imj

clean :
	rm -f *.o

imjcuitest : imjcuitest.o imjcui.o imj.o myminmax.o mystring.o myutf8.o mydebug.o util.o
	LANG=C g++ -o imjcuitest imjcuitest.o imjcui.o imj.o myminmax.o mystring.o myutf8.o mydebug.o util.o

imjcuitest.o : imjcuitest.cpp imjcui.h imj.h
	LANG=C g++ -c -O2 -o imjcuitest.o imjcuitest.cpp

imjcui.o : imjcui.cpp imjcui.h imj.h
	LANG=C g++ -c -O2 -o imjcui.o imjcui.cpp

imj.o : imj.cpp imj.h
	LANG=C g++ -c -O2 -o imj.o imj.cpp

myminmax.o : myminmax.c myminmax.h
	LANG=C gcc -c -O2 -o myminmax.o myminmax.c

mystring.o : mystring.c mystring.h
	LANG=C gcc -c -O2 -o mystring.o mystring.c

myutf8.o : myutf8.c myutf8.h
	LANG=C gcc -c -O2 -o myutf8.o myutf8.c

mydebug.o : mydebug.c mydebug.h
	LANG=C gcc -c -O2 -o mydebug.o mydebug.c

util.o : util.c util.h
	LANG=C gcc -c -O2 -o util.o util.c
