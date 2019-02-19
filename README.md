# H3-Node

[![Build Status](https://travis-ci.org/dfellis/h3-node.svg?branch=master)](https://travis-ci.org/dfellis/h3-node) [![Build status](https://ci.appveyor.com/api/projects/status/eli9fi8on6tqiy0r?svg=true)](https://ci.appveyor.com/project/dfellis/h3-node) [![License](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE)

H3 Bindings to Node using N-API

## Install

To install this package, you need several development tools available: `git`, `gyp`, `make`, `cmake`, and a C compiler (`gcc` or `clang`). Once those are available, simply

```sh
yarn add h3-node

-- or --

npm i h3-node
```

## Versions

The `1.X.Y` versions released before have varying stages of completeness of the API. From version `3.2.0` and on the major and minor versions will match the upstream [C Implementation](https://github.com/uber/h3)'s major and minor versions, while the patch version will increment independently, including fixes for the bindings themselves as well as fixes from upstream.

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
const hexLine = h3.h3Line(h3index, index2)
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
const res0Indexes = h3.getRes0Indexes()
	
```

## Why another H3 for Node?

`h3-js` is an emscripten transpilation of the H3 code into Javascript with a Javascript wrapper file similar to a binding in spirit to handle calling these quasi-C functions for you.

Being 100% javascript it works across the entire Javascript ecosystem (especially when paired with browserify or babel to handle the Node-isms for non-Node JS environments), but that portability comes at a cost in performance. Preliminary benchmarks show a significant speedup with the N-API approach:

```
damocles@Talyn:~/oss/h3-node(master)$ yarn test
yarn run v1.12.3
$ nodeunit test

index
(node:21680) Warning: N-API is an experimental feature and could change at any time.
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
✔ h3Line
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
✔ getRes0Indexes

geoToH3 Benchmark:
H3-js time in ns:    37053320
H3-node time in ns:  4948791
✔ geoToH3Benchmark

h3ToGeo Benchmark:
H3-js time in ns:    6991804
H3-node time in ns:  2543928
✔ h3ToGeoBenchmark

h3ToGeoBoundary Benchmark:
H3-js time in ns:    9363250
H3-node time in ns:  8859828
✔ h3ToGeoBoundaryBenchmark

h3GetResolution Benchmark:
H3-js time in ns:    421729
H3-node time in ns:  575353
✔ h3GetResolutionBenchmark

h3GetBaseCell Benchmark:
H3-js time in ns:    1009243
H3-node time in ns:  621067
✔ h3GetBaseCellBenchmark

h3IsValid Benchmark:
H3-js time in ns:    5292364
H3-node time in ns:  426803
✔ h3IsValidBenchmark

h3IsResClassIII Benchmark:
H3-js time in ns:    874960
H3-node time in ns:  479571
✔ h3IsResClassIIIBenchmark

h3IsPentagon Benchmark:
H3-js time in ns:    996449
H3-node time in ns:  466665
✔ h3IsPentagonBenchmark

kRing Benchmark:
H3-js time in ns:    215144033
H3-node time in ns:  112755357
✔ kRingBenchmark

kRingDistances Benchmark:
H3-js time in ns:    237245798
H3-node time in ns:  99332518
✔ kRingDistancesBenchmark

hexRing Benchmark:
H3-js time in ns:    32159864
H3-node time in ns:  29250788
✔ hexRingBenchmark

h3Distance Benchmark:
H3-js time in ns:    10576821
H3-node time in ns:  2263446
✔ h3DistanceBenchmark

experimentalH3ToLocalIj Benchmark:
H3-js time in ns:    3560641
H3-node time in ns:  2767738
✔ experimentalH3ToLocalIjBenchmark

experimentalLocalIjToH3 Benchmark:
H3-js time in ns:    22968173
H3-node time in ns:  3099782
✔ experimentalLocalIjToH3Benchmark

h3Line Benchmark:
H3-js time in ns:    96361818
H3-node time in ns:  84495601
✔ h3LineBenchmark

h3ToParent Benchmark:
H3-js time in ns:    4451580
H3-node time in ns:  1088388
✔ h3ToParentBenchmark

h3ToChildren Benchmark:
H3-js time in ns:    1825357087
H3-node time in ns:  2512486250
✔ h3ToChildrenBenchmark

compact Benchmark:
H3-js time in ns:    202311592
H3-node time in ns:  68058802
✔ compactBenchmark

uncompact Benchmark:
H3-js time in ns:    93321495
H3-node time in ns:  90170303
✔ uncompactBenchmark

polyfill Benchmark:
H3-js time in ns:    52779585
H3-node time in ns:  16534263
✔ polyfillBenchmark

polyfillWithHoles Benchmark:
H3-js time in ns:    23985204
H3-node time in ns:  18861196
✔ polyfillWithHolesBenchmark

h3SetToMultiPolygon Benchmark:
H3-js time in ns:    585107474
H3-node time in ns:  642896833
✔ h3SetToMultiPolygonBenchmark

h3SetToMultiPolygonGeoJsonMode Benchmark:
H3-js time in ns:    501041963
H3-node time in ns:  647480798
✔ h3SetToMultiPolygonGeoJsonModeBenchmark

h3SetToMultiPolygonTrueMultiPolygon Benchmark:
H3-js time in ns:    658786084
H3-node time in ns:  878605470
✔ h3SetToMultiPolygonTrueMultiPolygonBenchmark

h3IndexesAreNeighbors Benchmark:
H3-js time in ns:    6773878
H3-node time in ns:  1234947
✔ h3IndexesAreNeighborsBenchmark

getH3UnidirectionalEdge Benchmark:
H3-js time in ns:    5293199
H3-node time in ns:  1709837
✔ getH3UnidirectionalEdgeBenchmark

h3UnidirectionalEdgeIsValid Benchmark:
H3-js time in ns:    1208102
H3-node time in ns:  487279
✔ h3UnidirectionalEdgeIsValidBenchmark

getOriginH3IndexFromUnidirectionalEdge Benchmark:
H3-js time in ns:    2690489
H3-node time in ns:  838847
✔ getOriginH3IndexFromUnidirectionalEdgeBenchmark

getDestinationH3IndexFromUnidirectionalEdge Benchmark:
H3-js time in ns:    2303120
H3-node time in ns:  899137
✔ getDestinationH3IndexFromUnidirectionalEdgeBenchmark

getH3IndexesFromUnidirectionalEdge Benchmark:
H3-js time in ns:    3650529
H3-node time in ns:  1915532
✔ getH3IndexesFromUnidirectionalEdgeBenchmark

getH3UnidirectionalEdgesFromHexagon Benchmark:
H3-js time in ns:    6054690
H3-node time in ns:  4248423
✔ getH3UnidirectionalEdgesFromHexagonBenchmark

getH3UnidirectionalEdgeBoundary Benchmark:
H3-js time in ns:    18813297
H3-node time in ns:  9267771
✔ getH3UnidirectionalEdgeBoundaryBenchmark

degsToRads Benchmark:
H3-js time in ns:    45939
H3-node time in ns:  185695
✔ degsToRadsBenchmark

radsToDegs Benchmark:
H3-js time in ns:    76681
H3-node time in ns:  186188
✔ radsToDegsBenchmark

numHexagons Benchmark:
H3-js time in ns:    984968
H3-node time in ns:  174963
✔ numHexagonsBenchmark

edgeLength Benchmark:
H3-js time in ns:    526063
H3-node time in ns:  226033
✔ edgeLengthBenchmark

hexArea Benchmark:
H3-js time in ns:    559212
H3-node time in ns:  247401
✔ hexAreaBenchmark

getRes0Indexes Benchmark:
H3-js time in ns:    75643777
H3-node time in ns:  45794319
✔ getRes0IndexesBenchmark

OK: 380 assertions (19780ms)
Done in 20.04s.
```

`h3-node` is a [Node N-API binding](https://nodejs.org/api/n-api.html) of the [original C H3 code](https://github.com/uber/h3) to provide a higher-performance option in backend Node.js applications.

That makes `h3-node` a "nice to have" but not as required as `h3-js`. (In a similar vein, I intend to write an `h3-wasm` for higher-performance H3 in modern browsers that can be a drop-in replacement for `h3-js` when WebAssembly is present.)
