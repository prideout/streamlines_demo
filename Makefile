CC=clang
DEPS=sokol_app.h sokol_gfx.h
FRAMEWORKS=-framework Foundation -framework Cocoa -framework AppKit -framework OpenGL
LIBRARIES=-lobjc
CFLAGS=-Wall -Werror $(SOURCES)
LDFLAGS=$(LIBRARIES) $(FRAMEWORKS)

glfluid: glfluid.o sokol.o
	 $(CC) $(LDFLAGS) -o glfluid glfluid.o sokol.o

.m.o: 
	$(CC) -fobjc-arc -c -Wall $< -o $@

clean:
	rm *.o glfluid
