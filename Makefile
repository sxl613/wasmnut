##
# wasmnut

.PHONY: build
build:
	../emsdk/emcc donut.c -o donut.js \
	-s EXPORTED_FUNCTIONS='["_get_donut_frame", "_get_sphere_frame", "_get_octa_frame", "_get_sierpinski_frame"]' \
	-s EXPORTED_RUNTIME_METHODS='["ccall","cwrap"]' \
	-s NO_EXIT_RUNTIME=1 -O3 \
	-s ALLOW_MEMORY_GROWTH=1 \
	-s WASM=1 \
	--no-entry
# end
