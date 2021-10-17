
CFLAGS = -O3 -Wall -Wextra -std=gnu89 -pedantic -Wno-long-long
CLIBS = `sdl2-config --libs`

all: simplesok

simplesok: sok.o sok_core.o crc32.o save.o gz.o net.o
	gcc $(CFLAGS) sok.o sok_core.o crc32.o save.o gz.o net.o -o simplesok $(CLIBS)

sok.o: sok.c
	gcc -c $(CFLAGS) sok.c -o sok.o

sok_core.o: sok_core.c
	gcc -c $(CFLAGS) sok_core.c -o sok_core.o

crc32.o: crc32.c
	gcc -c $(CFLAGS) crc32.c -o crc32.o

save.o: save.c
	gcc -c $(CFLAGS) save.c -o save.o

gz.o: gz.c
	gcc -c $(CFLAGS) gz.c -o gz.o

net.o: net.c
	gcc -c $(CFLAGS) net.c -o net.o

clean:
	rm -f *.o simplesok file2c

data: data_img.h data_lev.h data_fnt.h data_skn.h data_ico.h

data_img.h: img/*.png file2c png2bmp zopfli
	echo "/* This file is part of the sok project. */" > data_img.h
	for x in img/*.png ; do ./png2bmp $$x ; done
	for x in img/*.bmp ; do ./zopfli $$x ; done
	rm img/*.bmp
	for x in img/*.bmp.gz ; do ./file2c $$x >> data_img.h ; done
	rm img/*.bmp.gz

data_skn.h: skin/*.png file2c zopfli
	echo "/* This file is part of the sok project. */" > data_skn.h
	for x in skin/*.png ; do ./png2bmp $$x ; done
	for x in skin/*.bmp ; do ./zopfli $$x ; done
	rm skin/*.bmp
	for x in skin/*.bmp.gz ; do ./file2c $$x >> data_skn.h ; done
	rm skin/*.bmp.gz

data_lev.h: levels/*.xsb file2c zopfli
	echo "/* This file is part of the sok project. */" > data_lev.h
	for x in levels/*.xsb ; do ./zopfli $$x ; done
	for x in levels/*.xsb.gz ; do ./file2c $$x >> data_lev.h ; done
	rm levels/*.xsb.gz

data_fnt.h: font/*.png file2c zopfli
	echo "/* This file is part of the sok project. */" > data_fnt.h
	for x in font/*.png ; do ./png2bmp $$x ; done
	for x in font/*.bmp ; do ./zopfli $$x ; done
	rm font/*.bmp
	for x in font/*.bmp.gz ; do ./file2c $$x >> data_fnt.h ; done
	rm font/*.bmp.gz

data_ico.h: simplesok.png zopfli
	echo "/* This file is part of the sok project. */" > data_ico.h
	./png2bmp simplesok.png
	./zopfli simplesok.bmp
	rm simplesok.bmp
	./file2c simplesok.bmp.gz >> data_ico.h
	rm simplesok.bmp.gz

file2c: file2c.c
	gcc $(CFLAGS) file2c.c -o file2c

png2bmp: png2bmp.c
	gcc $(CFLAGS) png2bmp.c -o png2bmp `sdl2-config --libs` -lSDL2_image

zopfli: zopfli-1.0/*.c
	gcc zopfli-1.0/*.c -O2 -W -Wall -Wextra -ansi -pedantic -lm -o zopfli
