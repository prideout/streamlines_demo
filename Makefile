BUILD_DIR = build
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
	emcc -o streamlines.js ${EMLINKARGS} ${JSOBJECTS} $(CFLAGS)

# This doesn't actually work because it doesn't import the environment.
# I'll leave it here as a hint to humans.
activate_emscripten:
	. ${EMSDK}/emsdk_env.sh

$(BUILD_DIR):
	mkdir -p $@

clean:
	rm -rf $(BUILD_DIR) streamlines streamlines.js streamlines.wasm

$(BUILD_DIR)/%.js.o: %.c $(EXTRA_DEPS) activate_emscripten | $(BUILD_DIR)
	emcc -c -o $@ $< $(CFLAGS)

$(BUILD_DIR)/%.o: %.c $(EXTRA_DEPS) | $(BUILD_DIR)
	$(CC) -c -o $@ $< $(CFLAGS)

$(BUILD_DIR)/sokol_mac.o: $(BUILD_DIR)
	$(CC) -fobjc-arc -c -Wall -O3 extern/sokol_mac.m -o $@
