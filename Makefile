MKDIR_P ?= mkdir -p
BUILD_DIR ?= ./build

CC=clang
FRAMEWORKS=-framework Foundation -framework Cocoa -framework AppKit -framework OpenGL
LIBRARIES=-lobjc
CFLAGS=-Iextern -I. -Wall -std=c11 -O3 $(SOURCES)
LDFLAGS=$(LIBRARIES) $(FRAMEWORKS)
EXTRA_DEPS = \
	par/par_streamlines.h \
	streamlines.h

OBJECTS = \
	$(BUILD_DIR)/streamlines.o \
	$(BUILD_DIR)/demo_simple.o \
	$(BUILD_DIR)/demo_wireframe.o \
	$(BUILD_DIR)/demo_gradient.o \
	$(BUILD_DIR)/sokol.o

streamlines: $(OBJECTS)
	 $(CC) $(LDFLAGS) -o streamlines $^

$(BUILD_DIR)/%.o: %.c $(EXTRA_DEPS)
	$(MKDIR_P) $(dir $@)
	$(CC) -c -o $@ $< $(CFLAGS)

$(BUILD_DIR)/sokol.o:
	$(MKDIR_P) $(dir $@)
	$(CC) -fobjc-arc -c -Wall -O3 extern/sokol.m -o $@

clean:
	rm -rf $(BUILD_DIR) streamlines
