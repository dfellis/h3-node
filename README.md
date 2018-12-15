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

`h3-node` is a drop-in replacement for [H3-js](https://github.com/uber/h3-js). You can [use the H3-js API reference](https://github.com/uber/h3-js#api-reference) for the available methods and the [official H3 documentation](https://uber.github.io/h3/) to fully understand the purposes of the methods. Below is an abbreviated usage of every available method.

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
const bayAreaHexGeo = h3.h3ToMultiPolygon(bayAreaHexes)
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
(node:26570) Warning: N-API is an experimental feature and could change at any time.
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
✔ h3SetToMultiPolygon
✔ h3SetToMultiPolygonGeoJsonMode
✔ h3SetToMultiPolygonTrueMultiPolygon
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
H3-js time in ns:    26000657
H3-node time in ns:  5148913
✔ geoToH3Benchmark

h3ToGeo Benchmark:
H3-js time in ns:    3284399
H3-node time in ns:  2667621
✔ h3ToGeoBenchmark

h3ToGeoBoundary Benchmark:
H3-js time in ns:    7436655
H3-node time in ns:  7902614
✔ h3ToGeoBoundaryBenchmark

h3GetResolution Benchmark:
H3-js time in ns:    457119
H3-node time in ns:  843462
✔ h3GetResolutionBenchmark

h3GetBaseCell Benchmark:
H3-js time in ns:    797770
H3-node time in ns:  773036
✔ h3GetBaseCellBenchmark

h3IsValid Benchmark:
H3-js time in ns:    2758258
H3-node time in ns:  412931
✔ h3IsValidBenchmark

h3IsResClassIII Benchmark:
H3-js time in ns:    544688
H3-node time in ns:  475376
✔ h3IsResClassIIIBenchmark

h3IsPentagon Benchmark:
H3-js time in ns:    557811
H3-node time in ns:  451157
✔ h3IsPentagonBenchmark

kRing Benchmark:
H3-js time in ns:    275692339
H3-node time in ns:  92995170
✔ kRingBenchmark

kRingDistances Benchmark:
H3-js time in ns:    343642189
H3-node time in ns:  96905862
✔ kRingDistancesBenchmark

hexRing Benchmark:
H3-js time in ns:    21990669
H3-node time in ns:  19162152
✔ hexRingBenchmark

h3Distance Benchmark:
H3-js time in ns:    5288333
H3-node time in ns:  2292057
✔ h3DistanceBenchmark

experimentalH3ToLocalIj Benchmark:
H3-js time in ns:    4458281
H3-node time in ns:  2318876
✔ experimentalH3ToLocalIjBenchmark

experimentalLocalIjToH3 Benchmark:
H3-js time in ns:    12065882
H3-node time in ns:  2827554
✔ experimentalLocalIjToH3Benchmark

h3ToParent Benchmark:
H3-js time in ns:    1896427
H3-node time in ns:  918546
✔ h3ToParentBenchmark

h3ToChildren Benchmark:
H3-js time in ns:    1631880198
H3-node time in ns:  2562248739
✔ h3ToChildrenBenchmark

compact Benchmark:
H3-js time in ns:    304959533
H3-node time in ns:  67996556
✔ compactBenchmark

uncompact Benchmark:
H3-js time in ns:    79207524
H3-node time in ns:  80307022
✔ uncompactBenchmark

polyfill Benchmark:
H3-js time in ns:    37142472
H3-node time in ns:  18458912
✔ polyfillBenchmark

polyfillWithHoles Benchmark:
H3-js time in ns:    21551079
H3-node time in ns:  20838692
✔ polyfillWithHolesBenchmark

h3SetToMultiPolygon Benchmark:
H3-js time in ns:    895969078
H3-node time in ns:  658795919
✔ h3SetToMultiPolygonBenchmark

h3SetToMultiPolygonGeoJsonMode Benchmark:
H3-js time in ns:    885385041
H3-node time in ns:  648712504
✔ h3SetToMultiPolygonGeoJsonModeBenchmark

h3SetToMultiPolygonTrueMultiPolygon Benchmark:
H3-js time in ns:    1114265076
H3-node time in ns:  873380364
✔ h3SetToMultiPolygonTrueMultiPolygonBenchmark

h3IndexesAreNeighbors Benchmark:
H3-js time in ns:    4879684
H3-node time in ns:  2117648
✔ h3IndexesAreNeighborsBenchmark

getH3UnidirectionalEdge Benchmark:
H3-js time in ns:    2859663
H3-node time in ns:  1630762
✔ getH3UnidirectionalEdgeBenchmark

h3UnidirectionalEdgeIsValid Benchmark:
H3-js time in ns:    678197
H3-node time in ns:  507790
✔ h3UnidirectionalEdgeIsValidBenchmark

getOriginH3IndexFromUnidirectionalEdge Benchmark:
H3-js time in ns:    1061877
H3-node time in ns:  818962
✔ getOriginH3IndexFromUnidirectionalEdgeBenchmark

getDestinationH3IndexFromUnidirectionalEdge Benchmark:
H3-js time in ns:    1256017
H3-node time in ns:  932105
✔ getDestinationH3IndexFromUnidirectionalEdgeBenchmark

getH3IndexesFromUnidirectionalEdge Benchmark:
H3-js time in ns:    2077358
H3-node time in ns:  1762693
✔ getH3IndexesFromUnidirectionalEdgeBenchmark

getH3UnidirectionalEdgesFromHexagon Benchmark:
H3-js time in ns:    4372965
H3-node time in ns:  4076243
✔ getH3UnidirectionalEdgesFromHexagonBenchmark

getH3UnidirectionalEdgeBoundary Benchmark:
H3-js time in ns:    12775609
H3-node time in ns:  9806594
✔ getH3UnidirectionalEdgeBoundaryBenchmark

degsToRads Benchmark:
H3-js time in ns:    86747
H3-node time in ns:  211436
✔ degsToRadsBenchmark

radsToDegs Benchmark:
H3-js time in ns:    93456
H3-node time in ns:  227887
✔ radsToDegsBenchmark

numHexagons Benchmark:
H3-js time in ns:    808198
H3-node time in ns:  361962
✔ numHexagonsBenchmark

edgeLength Benchmark:
H3-js time in ns:    533990
H3-node time in ns:  438234
✔ edgeLengthBenchmark

hexArea Benchmark:
H3-js time in ns:    514214
H3-node time in ns:  446460
✔ hexAreaBenchmark

OK: 360 assertions (12301ms)
Done in 12.56s.
```

`h3-node` is a [Node N-API binding](https://nodejs.org/api/n-api.html) of the [original C H3 code](https://github.com/uber/h3) to provide a higher-performance option in backend Node.js applications.

That makes `h3-node` a "nice to have" but not as required as `h3-js`. (In a similar vein, I intend to write an `h3-wasm` for higher-performance H3 in modern browsers that can be a drop-in replacement for `h3-js` when WebAssembly is present.)
