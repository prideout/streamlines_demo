MKDIR_P ?= mkdir -p
BUILD_DIR ?= ./build

CC=clang
DEPS=sokol_app.h sokol_gfx.h
FRAMEWORKS=-framework Foundation -framework Cocoa -framework AppKit -framework OpenGL
LIBRARIES=-lobjc
CFLAGS=-Wall -Werror $(SOURCES)
LDFLAGS=$(LIBRARIES) $(FRAMEWORKS)

OBJS := \
	$(BUILD_DIR)/glfluid.o \
	$(BUILD_DIR)/sokol.o

glfluid: $(OBJS)
	 $(CC) $(LDFLAGS) -o glfluid $(OBJS)

$(BUILD_DIR)/glfluid.o:
	$(MKDIR_P) $(dir $@)
	$(CC) $(CFLAGS) -c glfluid.c -o $@

$(BUILD_DIR)/sokol.o:
	$(MKDIR_P) $(dir $@)
	$(CC) -fobjc-arc -c -Wall sokol.m -o $@

clean:
	rm -rf $(BUILD_DIR) glfluid
