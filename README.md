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
const packedRings = h3.compact([].concat(...threeRings))
const unpackedRings = h3.uncompact(packedRings, 9)
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
const edgeBoundaries = edges.map(h3.getH3UnidirectionalEdgeBoundary)
const sfCoordsRads = [37.77, -122.43].map(h3.degsToRads)
const sfCoordsDegs = sfCoordsRads.map(h3.radsToDegs)
const resStats = Array.from(new Array(16), (x, res) => ({
  res,
  numHexagons: h3.numHexagons(res),
  edgeLength: {
    m: h3.edgeLength(res, h3.UNITS.m),
    km: h3.edgeLength(res, h3.UNITS.km),
  },
  hexArea: {
    m2: h3.hexArea(res, h3.UNITS.m2),
    km2: h3.hexArea(res, h3.UNITS.km2),
  },
}))
	
```

## Why another H3 for Node?

`h3-js` is an emscripten transpilation of the H3 code into Javascript with a Javascript wrapper file similar to a binding in spirit to handle calling these quasi-C functions for you.

Being 100% javascript it works across the entire Javascript ecosystem (especially when paired with browserify or babel to handle the Node-isms for non-Node JS environments), but that portability comes at a cost in performance. Preliminary benchmarks show a significant speedup with the N-API approach:

```
damocles@Talyn:~/oss/h3-node(master)$ yarn test
yarn run v1.12.3
$ nodeunit test

index
(node:16656) Warning: N-API is an experimental feature and could change at any time.
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
✔ getH3UnidirectionalEdgeBoundary
✔ degsToRads
✔ radsToDegs
✔ numHexagons
✔ edgeLength
✔ hexArea

geoToH3 Benchmark:
H3-js time in ns:    29896113
H3-node time in ns:  3558073
✔ geoToH3Benchmark

h3ToGeo Benchmark:
H3-js time in ns:    9235892
H3-node time in ns:  2483098
✔ h3ToGeoBenchmark

h3ToGeoBoundary Benchmark:
H3-js time in ns:    22809714
H3-node time in ns:  8222332
✔ h3ToGeoBoundaryBenchmark

h3GetResolution Benchmark:
H3-js time in ns:    275810
H3-node time in ns:  479139
✔ h3GetResolutionBenchmark

h3GetBaseCell Benchmark:
H3-js time in ns:    518446
H3-node time in ns:  456102
✔ h3GetBaseCellBenchmark

h3IsValid Benchmark:
H3-js time in ns:    1681082
H3-node time in ns:  395163
✔ h3IsValidBenchmark

h3IsResClassIII Benchmark:
H3-js time in ns:    557353
H3-node time in ns:  459256
✔ h3IsResClassIIIBenchmark

h3IsPentagon Benchmark:
H3-js time in ns:    555483
H3-node time in ns:  442347
✔ h3IsPentagonBenchmark

kRing Benchmark:
H3-js time in ns:    265200627
H3-node time in ns:  105037019
✔ kRingBenchmark

kRingDistances Benchmark:
H3-js time in ns:    291531851
H3-node time in ns:  97446367
✔ kRingDistancesBenchmark

hexRing Benchmark:
H3-js time in ns:    26698473
H3-node time in ns:  19413448
✔ hexRingBenchmark

h3Distance Benchmark:
H3-js time in ns:    4517422
H3-node time in ns:  2298966
✔ h3DistanceBenchmark

experimentalH3ToLocalIj Benchmark:
H3-js time in ns:    5232713
H3-node time in ns:  2356066
✔ experimentalH3ToLocalIjBenchmark

experimentalLocalIjToH3 Benchmark:
H3-js time in ns:    14010546
H3-node time in ns:  4639844
✔ experimentalLocalIjToH3Benchmark

h3ToParent Benchmark:
H3-js time in ns:    2090450
H3-node time in ns:  853517
✔ h3ToParentBenchmark

h3ToChildren Benchmark:
H3-js time in ns:    1681751487
H3-node time in ns:  2558350108
✔ h3ToChildrenBenchmark

compact Benchmark:
H3-js time in ns:    323333586
H3-node time in ns:  67224151
✔ compactBenchmark

uncompact Benchmark:
H3-js time in ns:    81524740
H3-node time in ns:  82001671
✔ uncompactBenchmark

polyfill Benchmark:
H3-js time in ns:    42153009
H3-node time in ns:  22233390
✔ polyfillBenchmark

polyfillWithHoles Benchmark:
H3-js time in ns:    29027964
H3-node time in ns:  23606136
✔ polyfillWithHolesBenchmark

h3IndexesAreNeighbors Benchmark:
H3-js time in ns:    4990645
H3-node time in ns:  2144329
✔ h3IndexesAreNeighborsBenchmark

getH3UnidirectionalEdge Benchmark:
H3-js time in ns:    2998564
H3-node time in ns:  2464475
✔ getH3UnidirectionalEdgeBenchmark

h3UnidirectionalEdgeIsValid Benchmark:
H3-js time in ns:    653944
H3-node time in ns:  484543
✔ h3UnidirectionalEdgeIsValidBenchmark

getOriginH3IndexFromUnidirectionalEdge Benchmark:
H3-js time in ns:    1757166
H3-node time in ns:  851987
✔ getOriginH3IndexFromUnidirectionalEdgeBenchmark

getDestinationH3IndexFromUnidirectionalEdge Benchmark:
H3-js time in ns:    1274645
H3-node time in ns:  901020
✔ getDestinationH3IndexFromUnidirectionalEdgeBenchmark

getH3IndexesFromUnidirectionalEdge Benchmark:
H3-js time in ns:    2095066
H3-node time in ns:  1772039
✔ getH3IndexesFromUnidirectionalEdgeBenchmark

getH3UnidirectionalEdgesFromHexagon Benchmark:
H3-js time in ns:    4397788
H3-node time in ns:  4082355
✔ getH3UnidirectionalEdgesFromHexagonBenchmark

getH3UnidirectionalEdgeBoundary Benchmark:
H3-js time in ns:    15735788
H3-node time in ns:  10335979
✔ getH3UnidirectionalEdgeBoundaryBenchmark

degsToRads Benchmark:
H3-js time in ns:    91449
H3-node time in ns:  207321
✔ degsToRadsBenchmark

radsToDegs Benchmark:
H3-js time in ns:    90997
H3-node time in ns:  206666
✔ radsToDegsBenchmark

numHexagons Benchmark:
H3-js time in ns:    420927
H3-node time in ns:  197514
✔ numHexagonsBenchmark

edgeLength Benchmark:
H3-js time in ns:    284680
H3-node time in ns:  247907
✔ edgeLengthBenchmark

hexArea Benchmark:
H3-js time in ns:    279448
H3-node time in ns:  271120
✔ hexAreaBenchmark

OK: 330 assertions (6870ms)
Done in 7.13s.
```

`h3-node` is a [Node N-API binding](https://nodejs.org/api/n-api.html) of the [original C H3 code](https://github.com/uber/h3) to provide a higher-performance option in backend Node.js applications.

That makes `h3-node` a "nice to have" but not as required as `h3-js`. (In a similar vein, I intend to write an `h3-wasm` for higher-performance H3 in modern browsers that can be a drop-in replacement for `h3-js` when WebAssembly is present.)
