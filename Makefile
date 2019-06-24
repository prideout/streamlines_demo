MKDIR_P ?= mkdir -p
BUILD_DIR ?= ./build

CC=clang
FRAMEWORKS=-framework Foundation -framework Cocoa -framework AppKit -framework OpenGL
LIBRARIES=-lobjc
CFLAGS=-Iextern -I. -Wall -std=c11 -O3 $(SOURCES)
LDFLAGS=$(LIBRARIES) $(FRAMEWORKS)
EXTRA_DEPS = \
	par/par_streamlines.h \
	demo.h

OBJECTS = \
	$(BUILD_DIR)/main_desktop.o \
	$(BUILD_DIR)/demo_simple.o \
	$(BUILD_DIR)/demo_wireframe.o \
	$(BUILD_DIR)/demo_gradient.o \
	$(BUILD_DIR)/demo_closed.o \
	$(BUILD_DIR)/demo_endcap.o \
	$(BUILD_DIR)/demo_noisy.o \
	$(BUILD_DIR)/demo_streamlines.o \
	$(BUILD_DIR)/sokol_mac.o

JSOBJECTS = \
	$(BUILD_DIR)/demo_simple.js.o \
	$(BUILD_DIR)/demo_wireframe.js.o \
	$(BUILD_DIR)/demo_gradient.js.o \
	$(BUILD_DIR)/demo_closed.js.o \
	$(BUILD_DIR)/demo_endcap.js.o \
	$(BUILD_DIR)/demo_noisy.js.o \
	$(BUILD_DIR)/demo_streamlines.js.o \
	$(BUILD_DIR)/main_web.js.o

streamlines: $(OBJECTS)
	 $(CC) $(LDFLAGS) -o streamlines $^

web: $(JSOBJECTS)
	source $(EMSDK)/emsdk_env.sh
	emcc -o streamlines.js ${EMLINKARGS} ${JSOBJECTS}

$(BUILD_DIR)/%.js.o: %.c $(EXTRA_DEPS)
	$(MKDIR_P) $(dir $@)
	emcc -c -o $@ $< $(CFLAGS)

$(BUILD_DIR)/%.o: %.c $(EXTRA_DEPS)
	$(MKDIR_P) $(dir $@)
	$(CC) -c -o $@ $< $(CFLAGS)

$(BUILD_DIR)/sokol_mac.o:
	$(MKDIR_P) $(dir $@)
	$(CC) -fobjc-arc -c -Wall -O3 extern/sokol_mac.m -o $@

clean:
	rm -rf $(BUILD_DIR) streamlines
