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

When fully implemented, it will be a drop-in replacement for [H3-js](https://github.com/uber/h3-js). Most functions have been implemented. The following are still missing versus h3-js:

* h3SetToMultiPolygon
* getH3UnidirectionalEdgeBoundary
* hexArea
* edgeLength
* numHexagons
* degsToRads
* radsToDegs
* UNITS **(not a function, an object used by the `hexArea` and `edgeLength` functions)**

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
const areNeighbors = h3.h3IndexesAreNeighbors(neighbors[0], neighbors[1])
const edgeIndex = h3.getH3UnidirectionalEdge(neighbors[0], neighbors[1])
const isValidEdge = h3.h3UnidirectionalEdgeIsValid(edgeIndex)
const origin = h3.getOriginH3IndexFromUnidirectionalEdge(edgeIndex)
const destination = h3.getDestinationH3IndexFromUnidirectionalEdge(edgeIndex)
const [start, end] = h3.getH3IndexesFromUnidirectionalEdge(edgeIndex)
const edges = h3.getH3UnidirectionalEdgesFromHexagon(h3index)
```

## Why another H3 for Node?

`h3-js` is an emscripten transpilation of the H3 code into Javascript with a Javascript wrapper file similar to a binding in spirit to handle calling these quasi-C functions for you.

Being 100% javascript it works across the entire Javascript ecosystem (especially when paired with browserify or babel to handle the Node-isms for non-Node JS environments), but that portability comes at a cost in performance. Preliminary benchmarks show a significant speedup with the N-API approach:

```
damocles@Talyn:~/oss/h3-node(master)$ yarn test
yarn run v1.12.3
$ nodeunit test

index
(node:2582) Warning: N-API is an experimental feature and could change at any time.
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
✔ h3IndexesAreNeighbors
✔ getH3UnidirectionalEdge
✔ h3UnidirectionalEdgeIsValid
✔ getOriginH3IndexFromUnidirectionalEdge
✔ getDestinationH3IndexFromUnidirectionalEdge
✔ getH3IndexesFromUnidirectionalEdge
✔ getH3UnidirectionalEdgesFromHexagon

geoToH3 Benchmark:
H3-js time in ns:    34252315
H3-node time in ns:  3604428
✔ geoToH3Benchmark

h3ToGeo Benchmark:
H3-js time in ns:    6550867
H3-node time in ns:  2672525
✔ h3ToGeoBenchmark

h3ToGeoBoundary Benchmark:
H3-js time in ns:    22155396
H3-node time in ns:  8790674
✔ h3ToGeoBoundaryBenchmark

h3GetResolution Benchmark:
H3-js time in ns:    281351
H3-node time in ns:  498438
✔ h3GetResolutionBenchmark

h3GetBaseCell Benchmark:
H3-js time in ns:    489901
H3-node time in ns:  461707
✔ h3GetBaseCellBenchmark

h3IsValid Benchmark:
H3-js time in ns:    1824147
H3-node time in ns:  405127
✔ h3IsValidBenchmark

h3IsResClassIII Benchmark:
H3-js time in ns:    523613
H3-node time in ns:  466991
✔ h3IsResClassIIIBenchmark

h3IsPentagon Benchmark:
H3-js time in ns:    625011
H3-node time in ns:  476292
✔ h3IsPentagonBenchmark

kRing Benchmark:
H3-js time in ns:    278281468
H3-node time in ns:  88260294
✔ kRingBenchmark

kRingDistances Benchmark:
H3-js time in ns:    250110658
H3-node time in ns:  103444021
✔ kRingDistancesBenchmark

hexRing Benchmark:
H3-js time in ns:    30894390
H3-node time in ns:  21171726
✔ hexRingBenchmark

h3Distance Benchmark:
H3-js time in ns:    5960651
H3-node time in ns:  2224718
✔ h3DistanceBenchmark

experimentalH3ToLocalIj Benchmark:
H3-js time in ns:    2752553
H3-node time in ns:  2266698
✔ experimentalH3ToLocalIjBenchmark

experimentalLocalIjToH3 Benchmark:
H3-js time in ns:    18222195
H3-node time in ns:  2949904
✔ experimentalLocalIjToH3Benchmark

h3ToParent Benchmark:
H3-js time in ns:    2202377
H3-node time in ns:  893372
✔ h3ToParentBenchmark

h3ToChildren Benchmark:
H3-js time in ns:    1566973572
H3-node time in ns:  2304402687
✔ h3ToChildrenBenchmark

compact Benchmark:
H3-js time in ns:    291818902
H3-node time in ns:  65041820
✔ compactBenchmark

uncompact Benchmark:
H3-js time in ns:    78864935
H3-node time in ns:  77236837
✔ uncompactBenchmark

polyfill Benchmark:
H3-js time in ns:    40677223
H3-node time in ns:  19191525
✔ polyfillBenchmark

polyfillWithHoles Benchmark:
H3-js time in ns:    30339541
H3-node time in ns:  19860869
✔ polyfillWithHolesBenchmark

h3IndexesAreNeighbors Benchmark:
H3-js time in ns:    3793295
H3-node time in ns:  1224241
✔ h3IndexesAreNeighborsBenchmark

getH3UnidirectionalEdge Benchmark:
H3-js time in ns:    3378189
H3-node time in ns:  1657147
✔ getH3UnidirectionalEdgeBenchmark

h3UnidirectionalEdgeIsValid Benchmark:
H3-js time in ns:    685754
H3-node time in ns:  476734
✔ h3UnidirectionalEdgeIsValidBenchmark

getOriginH3IndexFromUnidirectionalEdge Benchmark:
H3-js time in ns:    1782145
H3-node time in ns:  825752
✔ getOriginH3IndexFromUnidirectionalEdgeBenchmark

getDestinationH3IndexFromUnidirectionalEdge Benchmark:
H3-js time in ns:    1313127
H3-node time in ns:  957949
✔ getDestinationH3IndexFromUnidirectionalEdgeBenchmark

getH3IndexesFromUnidirectionalEdge Benchmark:
H3-js time in ns:    2118913
H3-node time in ns:  1761210
✔ getH3IndexesFromUnidirectionalEdgeBenchmark

getH3UnidirectionalEdgesFromHexagon Benchmark:
H3-js time in ns:    4395943
H3-node time in ns:  4212090
✔ getH3UnidirectionalEdgesFromHexagonBenchmark

OK: 270 assertions (6349ms)
Done in 6.60s.
```

`h3-node` is a [Node N-API binding](https://nodejs.org/api/n-api.html) of the [original C H3 code](https://github.com/uber/h3) to provide a higher-performance option in backend Node.js applications.

That makes `h3-node` a "nice to have" but not as required as `h3-js`. (In a similar vein, I intend to write an `h3-wasm` for higher-performance H3 in modern browsers that can be a drop-in replacement for `h3-js` when WebAssembly is present.)
