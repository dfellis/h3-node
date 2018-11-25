# H3-Node

[![Build Status](https://travis-ci.org/dfellis/h3-node.svg?branch=master)](https://travis-ci.org/dfellis/h3-node) [![License](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE)

H3 Bindings to Node using N-API

## Install

To install this package, you need several development tools available: `git`, `gyp`, `make`, `cmake`, and a C compiler (`gcc` or `clang`). Once those are available, simply

```sh
yarn add h3-node

-- or --

npm i h3-node
```

## Usage

When fully implemented, it will be a drop-in replacement for [H3-js](https://github.com/uber/h3-js). For now, only the following functions have been implemented:

* `geoToH3`
* `h3ToGeo`
* `h3ToGeoBoundary`
* `h3GetResolution`
* `h3GetBaseCell`
* `h3IsValid`
* `h3IsResClassIII`
* `h3IsPentagon`
* `kRing`
* `kRingDistances`
* `hexRing`
* `h3Distance`
* `experimentalH3ToLocalIj`
* `experimentalLocalIjToH3`
* `h3ToParent`
* `h3ToChildren`
* `compact`
* `uncompact`
* `polyfill`

```js
const h3 = require('h3-node')

const h3index = h3.geoToH3(37.77, -122.43, 9)
const h3center = h3.h3ToGeo(h3index)
const h3outline = h3.h3ToGeoBoundary(h3index)

const res = h3.h3GetResolution(h3index)
const baseCell = h3.h3GetBaseCell(h3index)
const valid = h3.h3IsValid(h3index)
const resIsClassIII = h3.h3IsResClassIII(h3index)
const isPentagon = h3.h3IsPentagon(h3index)
const neighbors = h3.kRing(h3index, 1)
const threeRings = h3.kRingDistances(h3index, 3)
const ring2Away = h3.hexRing(h3index, 2)
const index2 = h3.geoToH3(38.88, -122.34, 9)
const hexDistance = h3.h3Distance(h3index, index2)
const coords = h3.experimentalH3ToLocalIj(h3index, index2)
const index2again = h3.experimentalLocalIjToH3(h3index, coords)
const parent = h3.h3ToParent(h3index, 8)
const immediateChildren = h3.h3ToParent(h3index, 10)
const packedRings = h3.compact(threeRings);
const unpackedRings = h3.uncompact(packedRings, 9);
const bayAreaHexes = h3.polyfill([
    [37.77, -122.43],
    [37.55, -122.43],
    [37.55, -122.23],
    [37.77, -122.23],
    [37.77, -122.43],
  ], 9)
```

## Why another H3 for Node?

`h3-js` is an emscripten transpilation of the H3 code into Javascript with a Javascript wrapper file similar to a binding in spirit to handle calling these quasi-C functions for you.

Being 100% javascript it works across the entire Javascript ecosystem (especially when paired with browserify or babel to handle the Node-isms for non-Node JS environments), but that portability comes at a cost in performance. Preliminary benchmarks show a significant speedup with the N-API approach:

```
damocles@Talyn:~/oss/h3-node(master)$ yarn test
yarn run v1.12.3
$ nodeunit test

index
(node:14600) Warning: N-API is an experimental feature and could change at any time.
✔ geoToH3
✔ h3ToGeo
✔ h3ToGeoBoundary
✔ h3GetResolution
✔ h3GetBaseCell
✔ h3IsValid
✔ h3IsResClassIII
✔ h3IsPentagon
✔ kRing
✔ kRingDistances
✔ hexRing
✔ h3Distance
✔ experimentalH3ToLocalIj
✔ experimentalLocalIjToH3
✔ h3ToParent
✔ h3ToChildren
✔ compact
✔ uncompact
✔ polyfill
✔ polyfillWithHoles

geoToH3 Benchmark:
H3-js time in ns:    28465523
H3-node time in ns:  3908871
✔ geoToH3Benchmark

h3ToGeo Benchmark:
H3-js time in ns:    6195781
H3-node time in ns:  2668031
✔ h3ToGeoBenchmark

h3ToGeoBoundary Benchmark:
H3-js time in ns:    22513949
H3-node time in ns:  8610824
✔ h3ToGeoBoundaryBenchmark

h3GetResolution Benchmark:
H3-js time in ns:    279625
H3-node time in ns:  483233
✔ h3GetResolutionBenchmark

h3GetBaseCell Benchmark:
H3-js time in ns:    520916
H3-node time in ns:  492696
✔ h3GetBaseCellBenchmark

h3IsValid Benchmark:
H3-js time in ns:    2518655
H3-node time in ns:  813764
✔ h3IsValidBenchmark

h3IsResClassIII Benchmark:
H3-js time in ns:    466524
H3-node time in ns:  410371
✔ h3IsResClassIIIBenchmark

h3IsPentagon Benchmark:
H3-js time in ns:    556491
H3-node time in ns:  454306
✔ h3IsPentagonBenchmark

kRing Benchmark:
H3-js time in ns:    227662086
H3-node time in ns:  87472963
✔ kRingBenchmark

kRingDistances Benchmark:
H3-js time in ns:    249748496
H3-node time in ns:  89572581
✔ kRingDistancesBenchmark

hexRing Benchmark:
H3-js time in ns:    23593027
H3-node time in ns:  19919137
✔ hexRingBenchmark

h3Distance Benchmark:
H3-js time in ns:    5713512
H3-node time in ns:  2080585
✔ h3DistanceBenchmark

experimentalH3ToLocalIj Benchmark:
H3-js time in ns:    2777201
H3-node time in ns:  2317927
✔ experimentalH3ToLocalIjBenchmark

experimentalLocalIjToH3 Benchmark:
H3-js time in ns:    15613695
H3-node time in ns:  2932680
✔ experimentalLocalIjToH3Benchmark

h3ToParent Benchmark:
H3-js time in ns:    2295625
H3-node time in ns:  889461
✔ h3ToParentBenchmark

h3ToChildren Benchmark:
H3-js time in ns:    1609965396
H3-node time in ns:  2373352665
✔ h3ToChildrenBenchmark

compact Benchmark:
H3-js time in ns:    306431846
H3-node time in ns:  62116932
✔ compactBenchmark

uncompact Benchmark:
H3-js time in ns:    74380767
H3-node time in ns:  82409300
✔ uncompactBenchmark

polyfill Benchmark:
H3-js time in ns:    39023550
H3-node time in ns:  18329586
✔ polyfillBenchmark

polyfill Benchmark:
H3-js time in ns:    29646987
H3-node time in ns:  19729632
✔ polyfillWithHolesBenchmark

OK: 200 assertions (6190ms)
Done in 6.43s.
```

`h3-node` is a [Node N-API binding](https://nodejs.org/api/n-api.html) of the [original C H3 code](https://github.com/uber/h3) to provide a higher-performance option in backend Node.js applications.

That makes `h3-node` a "nice to have" but not as required as `h3-js`. (In a similar vein, I intend to write an `h3-wasm` for higher-performance H3 in modern browsers that can be a drop-in replacement for `h3-js` when WebAssembly is present.)
