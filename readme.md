(NOTE) 
For some reason, cairomm often stores (on linux) cairomm.h within /usr/include/cairomm-1.0/cairomm/ rather
than in the directory /usr/include/cairomm-1.0/, which pkg-config expects. When running pkg-config, it will
actually assume to include /usr/include/cairomm-1.0/. A quick fix for this can be copying cairomm.h from the 
full path into the subdirectory -- or updating your pkg-config includes for cairomm.h 

// How to run this program 
Begin by running "make main.cpp", it should use pkg-config to get everything set up as long as you have the dependencies. Namely cairomm-1.0, gtkmm-3.0, gtk-3.0, etc. All you need to do after that is execute main.

// Where are the comments?
main.cpp has comments. LinAlg.cpp doesn't because it (technically) wasnt required. The key thing to understand is that LinAlg.cpp implements overloaded operators for matrix vector / vector vector operations, which I like. 

// How are cycles determined?
I manually chose for a rotation to occur every 24 frames. Meaning a cycle is 24 frames and every 4 cycles is 96. 

// How are scales determined?
A linear interpolant is used to go between 1 -> 4 -> 1/4 -> 1 every 4 cycles. 

// Is it optimized?
Only one transformation is used to calculate the series of transformations, but the program is not compiled with things like -O2. I tried my best to minimize needless copying, but the way that I wrote my LinAlg library meant that I ended up doing a lot of initialization and copying. In the future I hope to fix that. 


// Fun things I learned

1) Coercion of types is the devil. 
- I ran into a bug where when using 1/4 etc and returning a double in the scale interpolant, I'd end up getting insane floating point error and ergo overflows in my floor() call. It was bad.
2) 1D arrays are supreme. (Alternatively, 'stick with what you got')
- Trying to do a bunch of math that keeps everything in a 2D array seems like 
it would preserve human sanity. However, making functions that transfer between
native 1d arrays (pixel buffers) and a 2d array you've stored is a really miserable experience.
3) I like fiddling with pixel data.
4) gdb is a life saver for determining where segfaults happen.
5) Almost all GUIs are a pain to work with, especially when you opt to do something low level like manually set pixels. 


