# To build for the web, do:
# source ${EMSDK}/emsdk_env.sh && make -j web

BUILD_DIR = build
SRC_DIR = src
CC = clang
CFLAGS = -Iextern -Wall -O3 -std=c11
CPPFLAGS = -Iextern -Wall -O3 -std=c++14

MAC_LIBRARIES = \
	-framework Foundation \
	-framework Cocoa \
	-framework AppKit \
	-framework OpenGL \
	-lobjc

EXTRA_DEPS = \
	extern/par/par_streamlines.h \
	$(SRC_DIR)/demo.h

OBJECTS = \
	$(BUILD_DIR)/demo_closed.o \
	$(BUILD_DIR)/demo_curves.o \
	$(BUILD_DIR)/demo_endcap.o \
	$(BUILD_DIR)/demo_gradient.o \
	$(BUILD_DIR)/demo_noisy.o \
	$(BUILD_DIR)/demo_streamlines.o \
	$(BUILD_DIR)/demo_wireframe.o \
	$(BUILD_DIR)/main_common.o \
	$(BUILD_DIR)/main_desktop.o \
	$(BUILD_DIR)/shaders.o \
	$(BUILD_DIR)/sokol_mac.o

JSOBJECTS = \
	$(BUILD_DIR)/demo_closed.js.o \
	$(BUILD_DIR)/demo_curves.js.o \
	$(BUILD_DIR)/demo_endcap.js.o \
	$(BUILD_DIR)/demo_gradient.js.o \
	$(BUILD_DIR)/demo_noisy.js.o \
	$(BUILD_DIR)/demo_streamlines.js.o \
	$(BUILD_DIR)/demo_wireframe.js.o \
	$(BUILD_DIR)/main_common.js.o \
	$(BUILD_DIR)/main_web.js.o \
	$(BUILD_DIR)/shaders.js.o

EM_LINKARGS = \
	-s MODULARIZE=1 \
	-s 'EXPORT_NAME="Streamlines"' \
	-s ERROR_ON_UNDEFINED_SYMBOLS=0 \
	-s EXPORTED_FUNCTIONS='["_draw", "_main", "_start"]'

# Desktop target.
streamlines: $(OBJECTS)
	 $(CC) $(MAC_LIBRARIES) -o streamlines $^

# Emscripten target.
web: streamlines.js

streamlines.js: $(JSOBJECTS)
	emcc -o streamlines.js ${EM_LINKARGS} ${JSOBJECTS} $(CFLAGS)

clean:
	rm -rf $(BUILD_DIR) streamlines streamlines.js streamlines.wasm

$(BUILD_DIR):
	mkdir -p $@

$(BUILD_DIR)/%.js.o: $(SRC_DIR)/%.c $(EXTRA_DEPS) | $(BUILD_DIR)
	emcc -c -o $@ $< $(CFLAGS) $(EM_CARGS)

$(BUILD_DIR)/%.js.o: $(SRC_DIR)/%.cpp $(EXTRA_DEPS) | $(BUILD_DIR)
	emcc -c -o $@ $< $(CPPFLAGS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c $(EXTRA_DEPS) | $(BUILD_DIR)
	$(CC) -c -o $@ $< $(CFLAGS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp $(EXTRA_DEPS) | $(BUILD_DIR)
	$(CC) -c -o $@ $< $(CPPFLAGS)

$(BUILD_DIR)/sokol_mac.o: $(BUILD_DIR)
	$(CC) -fobjc-arc -c -Wall -O3 extern/sokol_mac.m -o $@
