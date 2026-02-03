# wasmnut

A spinning ASCII donut rendered via WebAssembly, using signed distance functions.

## Demo

Open `index.html` in a browser (needs to be served, not opened as a file).

```bash
python -m http.server 8000
# then open http://localhost:8000
```

## How it works

The rendering is done in C (`sdf-donut-wasm.c`), compiled to WebAssembly. The donut is rendered using signed distance functions (SDFs) - a technique where shapes are defined by distance to their surface rather than explicit geometry.

JavaScript (`donut.js`) handles:
- Loading the WASM module
- Reading the frame buffer from WASM memory
- Rendering to the page

## Building

```bash
make
```

NOTE: you'll need Emscripten

## Files

- `sdf-donut-wasm.c` - The C source (SDF donut rendering)
- `donut.wasm` - Compiled WebAssembly binary
- `donut.js` - JavaScript glue code
- `index.html` - Demo page
- `favicon.svg` - A donut, naturally

## License

MIT
