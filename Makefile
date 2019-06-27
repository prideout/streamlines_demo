# To build for the web, do:
# source ${EMSDK}/emsdk_env.sh && make -j web

BUILD_DIR = build
CC=clang
FRAMEWORKS=-framework Foundation -framework Cocoa -framework AppKit -framework OpenGL
LIBRARIES=-lobjc
CFLAGS=-Iextern -I. -Wall -std=c11 -O3
CPPFLAGS=-Iextern -I. -Wall -O3 -std=c++14
LDFLAGS=$(LIBRARIES) $(FRAMEWORKS)
EXTRA_DEPS = \
	par/par_streamlines.h \
	demo.h

OBJECTS = \
	$(BUILD_DIR)/demo_closed.o \
	$(BUILD_DIR)/demo_endcap.o \
	$(BUILD_DIR)/demo_gradient.o \
	$(BUILD_DIR)/demo_noisy.o \
	$(BUILD_DIR)/demo_simple.o \
	$(BUILD_DIR)/demo_streamlines.o \
	$(BUILD_DIR)/demo_wireframe.o \
	$(BUILD_DIR)/main_common.o \
	$(BUILD_DIR)/main_desktop.o \
	$(BUILD_DIR)/shaders.o \
	$(BUILD_DIR)/sokol_mac.o

JSOBJECTS = \
	$(BUILD_DIR)/demo_closed.js.o \
	$(BUILD_DIR)/demo_endcap.js.o \
	$(BUILD_DIR)/demo_gradient.js.o \
	$(BUILD_DIR)/demo_noisy.js.o \
	$(BUILD_DIR)/demo_simple.js.o \
	$(BUILD_DIR)/demo_streamlines.js.o \
	$(BUILD_DIR)/demo_wireframe.js.o \
	$(BUILD_DIR)/shaders.js.o \
	$(BUILD_DIR)/main_common.js.o \
	$(BUILD_DIR)/main_web.js.o

streamlines: $(OBJECTS)
	 $(CC) $(LDFLAGS) -o streamlines $^

web: streamlines.js

EM_LARGS = \
	-s MODULARIZE=1 \
	-s 'EXPORT_NAME="Streamlines"' \
	-s ERROR_ON_UNDEFINED_SYMBOLS=0 \
	-s EXPORTED_FUNCTIONS='["_draw", "_main", "_start"]'

streamlines.js: $(JSOBJECTS)
	emcc -o streamlines.js ${EM_LARGS} ${JSOBJECTS} $(CFLAGS)

clean:
	rm -rf $(BUILD_DIR) streamlines streamlines.js streamlines.wasm

$(BUILD_DIR):
	mkdir -p $@

$(BUILD_DIR)/%.js.o: %.c $(EXTRA_DEPS) | $(BUILD_DIR)
	emcc -c -o $@ $< $(CFLAGS) $(EM_CARGS)

$(BUILD_DIR)/%.js.o: %.cpp $(EXTRA_DEPS) | $(BUILD_DIR)
	emcc -c -o $@ $< $(CPPFLAGS)

$(BUILD_DIR)/%.o: %.c $(EXTRA_DEPS) | $(BUILD_DIR)
	$(CC) -c -o $@ $< $(CFLAGS)

$(BUILD_DIR)/%.o: %.cpp $(EXTRA_DEPS) | $(BUILD_DIR)
	$(CC) -c -o $@ $< $(CPPFLAGS)

$(BUILD_DIR)/sokol_mac.o: $(BUILD_DIR)
	$(CC) -fobjc-arc -c -Wall -O3 extern/sokol_mac.m -o $@
