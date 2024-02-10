CROc = `pkg-config cairomm-1.0 --cflags`
CROlibs = `pkg-config cairomm-1.0 --libs`
GTKMMc = `pkg-config gtkmm-3.0 --cflags`
GTKMMlibs = `pkg-config gtkmm-3.0 --libs`
GDKMMc = `pkg-config gdkmm-3.0 --cflags`
GDKmmlibs = `pkg-config gdkmm-3.0 --libs`


CFLAGS = $(GTKMMc) $(CROc) $(GDKMMc)
LIBS = $(GTKMMlibs) $(CROlibs) $(GDKMMlibs)

debugMain:main.cpp 
	g++ -g $(CFLAGS) main.cpp  $(LIBS) 
main:main.cpp
	g++ $(CFLAGS) main.cpp  $(LIBS) -o main

clean:
	rm -f main
	rm -f a.out 