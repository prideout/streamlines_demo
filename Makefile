# To build for the web, do:
# source ${EMSDK}/emsdk_env.sh && make -j web

# Uncomment the following lines to enable address sanitizer.
# Also replace -O3 with -g for a meaningful call stack.
# ASAN_CFLAGS = -fsanitize=undefined -fsanitize=address -fstack-protector
# ASAN_LINKFLAGS = $(ASAN_CFLAGS) -lstdc++

BUILD_DIR = build
SRC_DIR = src
HOST_CC = /usr/bin/clang
WEB_CC = emcc
CFLAGS = -Iextern -Ibuild -Wall -O3 -std=c11 $(ASAN_CFLAGS)
CPPFLAGS = -Iextern -Ibuild -Wall -O3 -std=c++14 $(ASAN_CFLAGS)

MAC_LIBRARIES = \
	-framework Foundation \
	-framework Cocoa \
	-framework AppKit \
	-framework OpenGL \
	-lobjc \
	$(ASAN_LINKFLAGS)

EXTRA_DEPS = \
	extern/par/par_streamlines.h \
	$(BUILD_DIR)/shaders.h \
	$(SRC_DIR)/demo.h \
	$(SRC_DIR)/vmath.h

OBJECTS = \
	$(BUILD_DIR)/demo_closed.o \
	$(BUILD_DIR)/demo_curves.o \
	$(BUILD_DIR)/demo_endcap.o \
	$(BUILD_DIR)/demo_gradient.o \
	$(BUILD_DIR)/demo_noisy.o \
	$(BUILD_DIR)/demo_streamlines.o \
	$(BUILD_DIR)/demo_wireframe.o \
	$(BUILD_DIR)/common.o \
	$(BUILD_DIR)/main_macos.o

JSOBJECTS = \
	$(BUILD_DIR)/demo_closed.js.o \
	$(BUILD_DIR)/demo_curves.js.o \
	$(BUILD_DIR)/demo_endcap.js.o \
	$(BUILD_DIR)/demo_gradient.js.o \
	$(BUILD_DIR)/demo_noisy.js.o \
	$(BUILD_DIR)/demo_streamlines.js.o \
	$(BUILD_DIR)/demo_wireframe.js.o \
	$(BUILD_DIR)/common.js.o \
	$(BUILD_DIR)/main_web.js.o

EM_LINKARGS = \
	-s MODULARIZE=1 \
	-s 'EXPORT_NAME="Streamlines"' \
	-s ERROR_ON_UNDEFINED_SYMBOLS=0 \
	-s EXPORTED_FUNCTIONS='["_draw", "_main", "_start"]'

# Desktop target.
streamlines: $(OBJECTS)
	$(HOST_CC) $(MAC_LIBRARIES) -o streamlines $^

# Emscripten target.
web: streamlines.js
streamlines.js: $(JSOBJECTS)
	$(WEB_CC) -o streamlines.js ${EM_LINKARGS} ${JSOBJECTS} $(CFLAGS)

# Shader tool.
shader_tool: src/shader_tool.c extern/par/par_shaders.h
	$(HOST_CC) $(MAC_LIBRARIES) -o shader_tool src/shader_tool.c $(CFLAGS)

clean:
	rm -rf $(BUILD_DIR) shader_tool streamlines streamlines.js streamlines.wasm

$(BUILD_DIR):
	mkdir -p $@

$(BUILD_DIR)/%.js.o: $(SRC_DIR)/%.c $(EXTRA_DEPS) | $(BUILD_DIR)
	$(WEB_CC) -c -o $@ $< $(CFLAGS) $(EM_CARGS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c $(EXTRA_DEPS) | $(BUILD_DIR)
	$(HOST_CC) -c -o $@ $< $(CFLAGS)

$(BUILD_DIR)/%.h: $(SRC_DIR)/%.glsl shader_tool | $(BUILD_DIR)
	./shader_tool $< $@ SHADERS

$(BUILD_DIR)/main_macos.o: $(BUILD_DIR)
	$(HOST_CC) -fobjc-arc -c $(CFLAGS) src/main_macos.m -o $@
