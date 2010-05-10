builddir=.
apdir=/usr/local/apache2


all: mod_gradation.so


clean:
	rm -f mod_gradation.so 
	rm -f test

mod_gradation.so: mod_gradation.cc gradation.cc gradation.h bmp.h common.h
	g++ -O0 -g -pthread -fPIC -shared -o $@ mod_gradation.cc gradation.cc -I$(apdir)/include -L $(apdir)/lib -I/usr/include/ImageMagick/ -lMagick++

install: mod_gradation.so
	$(apdir)/bin/apachectl stop || true
	sleep 2
	cp $< $(apdir)/modules
	$(apdir)/bin/apachectl start

test: gradation.cc gradation.h bmp.h common.h
	g++ -O0 -g -pthread -o $@ $< -I $(apdir)/include -L$(apdir)/lib -I/usr/include/ImageMagick/ -lMagick++ -lfreetype -I/usr/include/freetype2
	./test
