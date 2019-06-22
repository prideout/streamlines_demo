MKDIR_P ?= mkdir -p
BUILD_DIR ?= ./build

CC=clang
FRAMEWORKS=-framework Foundation -framework Cocoa -framework AppKit -framework OpenGL
LIBRARIES=-lobjc
CFLAGS=-Iextern -I. -Wall -std=c11 -O3 $(SOURCES)
LDFLAGS=$(LIBRARIES) $(FRAMEWORKS)

DEPS = \
	extern/sokol_app.h \
	extern/sokol_gfx.h \
	par/par_streamlines.h \
	extern/sokol.m

streamlines: $(BUILD_DIR)/streamlines.o $(BUILD_DIR)/sokol.o
	 $(CC) $(LDFLAGS) -o streamlines $^

glfluid: $(BUILD_DIR)/glfluid.o $(BUILD_DIR)/sokol.o
	 $(CC) $(LDFLAGS) -o glfluid $^

$(BUILD_DIR)/glfluid.o: glfluid.c
	$(MKDIR_P) $(dir $@)
	$(CC) $(CFLAGS) -c $^ -o $@

$(BUILD_DIR)/streamlines.o: streamlines.c
	$(MKDIR_P) $(dir $@)
	$(CC) $(CFLAGS) -c $^ -o $@

$(BUILD_DIR)/sokol.o:
	$(MKDIR_P) $(dir $@)
	$(CC) -fobjc-arc -c -Wall -O3 extern/sokol.m -o $@

clean:
	rm -rf $(BUILD_DIR) glfluid streamlines
