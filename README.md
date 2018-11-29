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
(node:30038) Warning: N-API is an experimental feature and could change at any time.
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
✔ degsToRads
✔ radsToDegs
✔ numHexagons
✔ edgeLength
✔ hexArea

geoToH3 Benchmark:
H3-js time in ns:    30254593
H3-node time in ns:  3297305
✔ geoToH3Benchmark

h3ToGeo Benchmark:
H3-js time in ns:    7575081
H3-node time in ns:  2505491
✔ h3ToGeoBenchmark

h3ToGeoBoundary Benchmark:
H3-js time in ns:    21190074
H3-node time in ns:  7242111
✔ h3ToGeoBoundaryBenchmark

h3GetResolution Benchmark:
H3-js time in ns:    317448
H3-node time in ns:  708908
✔ h3GetResolutionBenchmark

h3GetBaseCell Benchmark:
H3-js time in ns:    490822
H3-node time in ns:  449668
✔ h3GetBaseCellBenchmark

h3IsValid Benchmark:
H3-js time in ns:    1583442
H3-node time in ns:  361977
✔ h3IsValidBenchmark

h3IsResClassIII Benchmark:
H3-js time in ns:    860318
H3-node time in ns:  826284
✔ h3IsResClassIIIBenchmark

h3IsPentagon Benchmark:
H3-js time in ns:    882529
H3-node time in ns:  765352
✔ h3IsPentagonBenchmark

kRing Benchmark:
H3-js time in ns:    246557497
H3-node time in ns:  77940037
✔ kRingBenchmark

kRingDistances Benchmark:
H3-js time in ns:    283595130
H3-node time in ns:  102009722
✔ kRingDistancesBenchmark

hexRing Benchmark:
H3-js time in ns:    28911668
H3-node time in ns:  17965756
✔ hexRingBenchmark

h3Distance Benchmark:
H3-js time in ns:    5604259
H3-node time in ns:  2030520
✔ h3DistanceBenchmark

experimentalH3ToLocalIj Benchmark:
H3-js time in ns:    2692060
H3-node time in ns:  2269589
✔ experimentalH3ToLocalIjBenchmark

experimentalLocalIjToH3 Benchmark:
H3-js time in ns:    14768686
H3-node time in ns:  2823700
✔ experimentalLocalIjToH3Benchmark

h3ToParent Benchmark:
H3-js time in ns:    2070185
H3-node time in ns:  942322
✔ h3ToParentBenchmark

h3ToChildren Benchmark:
H3-js time in ns:    1457137565
H3-node time in ns:  2190247857
✔ h3ToChildrenBenchmark

compact Benchmark:
H3-js time in ns:    303380271
H3-node time in ns:  64309813
✔ compactBenchmark

uncompact Benchmark:
H3-js time in ns:    72907606
H3-node time in ns:  76311483
✔ uncompactBenchmark

polyfill Benchmark:
H3-js time in ns:    42350450
H3-node time in ns:  17122851
✔ polyfillBenchmark

polyfillWithHoles Benchmark:
H3-js time in ns:    29402619
H3-node time in ns:  18610753
✔ polyfillWithHolesBenchmark

h3IndexesAreNeighbors Benchmark:
H3-js time in ns:    3620651
H3-node time in ns:  1100161
✔ h3IndexesAreNeighborsBenchmark

getH3UnidirectionalEdge Benchmark:
H3-js time in ns:    2863220
H3-node time in ns:  2215857
✔ getH3UnidirectionalEdgeBenchmark

h3UnidirectionalEdgeIsValid Benchmark:
H3-js time in ns:    663119
H3-node time in ns:  503890
✔ h3UnidirectionalEdgeIsValidBenchmark

getOriginH3IndexFromUnidirectionalEdge Benchmark:
H3-js time in ns:    1749366
H3-node time in ns:  822361
✔ getOriginH3IndexFromUnidirectionalEdgeBenchmark

getDestinationH3IndexFromUnidirectionalEdge Benchmark:
H3-js time in ns:    1191757
H3-node time in ns:  816657
✔ getDestinationH3IndexFromUnidirectionalEdgeBenchmark

getH3IndexesFromUnidirectionalEdge Benchmark:
H3-js time in ns:    1986001
H3-node time in ns:  1555855
✔ getH3IndexesFromUnidirectionalEdgeBenchmark

getH3UnidirectionalEdgesFromHexagon Benchmark:
H3-js time in ns:    4229138
H3-node time in ns:  3648153
✔ getH3UnidirectionalEdgesFromHexagonBenchmark

degsToRads Benchmark:
H3-js time in ns:    88356
H3-node time in ns:  175680
✔ degsToRadsBenchmark

radsToDegs Benchmark:
H3-js time in ns:    122587
H3-node time in ns:  175559
✔ radsToDegsBenchmark

numHexagons Benchmark:
H3-js time in ns:    418839
H3-node time in ns:  161338
✔ numHexagonsBenchmark

edgeLength Benchmark:
H3-js time in ns:    273082
H3-node time in ns:  220436
✔ edgeLengthBenchmark

hexArea Benchmark:
H3-js time in ns:    300781
H3-node time in ns:  225494
✔ hexAreaBenchmark

OK: 320 assertions (6020ms)
Done in 6.26s.
```

`h3-node` is a [Node N-API binding](https://nodejs.org/api/n-api.html) of the [original C H3 code](https://github.com/uber/h3) to provide a higher-performance option in backend Node.js applications.

That makes `h3-node` a "nice to have" but not as required as `h3-js`. (In a similar vein, I intend to write an `h3-wasm` for higher-performance H3 in modern browsers that can be a drop-in replacement for `h3-js` when WebAssembly is present.)
