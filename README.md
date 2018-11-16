# H3-Node

H3 Bindings to Node using N-API

## Install (when published)

```sh
yarn add h3-node
```

## Usage

When fully implemented, it will be a drop-in replacemenr for [H3-js](https://github.com/uber/h3-js). For now, only the core `h3ToGeo` and `geoToH3` functions are implemented.

```js
const h3 = require('h3-node')

const h3index = h3.geoToH3(37.77, -122.43, 9)
const h3center = h3.h3ToGeo(h3index)
```

## Why another H3 for Node?

H3-js is an emscripten transpilation of the H3 code into Javascript with a Javascript wrapper file similar to a binding in spirit to handle calling these quasi-C functions for you.

Being 100% javascript it works across the entire Javascript ecosystem (especially when paired with browserify or babel to handle the Node-isms for non-Node JS environments), but that portability comes at a cost in performance. (TODO: write benchmarks to compare h3-js to h3-node)

`h3-node` is a [Node N-API binding](https://nodejs.org/api/n-api.html) of the [original C H3 code](https://github.com/uber/h3) to provide a higher-performance option in backend Node.js applications.

That makes `h3-node` a "nice to have" but not as required as `h3-js`. (In a similar vein, I intend to write an `h3-wasm` for higher-perfoance H3 in modern browsers that can be a drop-in replacement for `h3-js` when WebAssembly is present.)
