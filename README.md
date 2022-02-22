# H3-Node

![Unit Testing](https://github.com/dfellis/h3-node/workflows/Unit%20Testing/badge.svg) [![License](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE)

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
const h3area = h3.cellArea(h3index)

const res = h3.h3GetResolution(h3index)
const baseCell = h3.h3GetBaseCell(h3index)
const valid = h3.h3IsValid(h3index)
const resIsClassIII = h3.h3IsResClassIII(h3index)
const isPentagon = h3.h3IsPentagon(h3index)
const faces = h3.h3GetFaces(h3Index)
const neighbors = h3.kRing(h3index, 1)
const threeRings = h3.kRingDistances(h3index, 3)
const ring2Away = h3.hexRing(h3index, 2)
const index2 = h3.geoToH3(38.88, -122.34, 9)
const hexLine = h3.h3Line(h3index, index2)
const hexDistance = h3.h3Distance(h3index, index2)
const coords = h3.experimentalH3ToLocalIj(h3index, index2)
const index2again = h3.experimentalLocalIjToH3(h3index, coords)
const parent = h3.h3ToParent(h3index, 8)
const immediateChildren = h3.h3ToChildren(h3index, 10)
const centerChild = h3.h3ToCenterChild(h3index, 10)
const packedRings = h3.compact([].concat(...threeRings))
const unpackedRings = h3.uncompact(packedRings, 9)
const bayAreaHexes = h3.polyfill([
    [37.77, -122.43],
    [37.55, -122.43],
    [37.55, -122.23],
    [37.77, -122.23],
    [37.77, -122.43],
  ], 9)
const diagonalLen = h3.pointDist([37.77, -122.43], [37.55, -122.23], hm.UNITS.km)
const bayAreaHexGeo = h3.h3ToMultiPolygon(bayAreaHexes)
const areNeighbors = h3.h3IndexesAreNeighbors(neighbors[0], neighbors[1])
const edgeIndex = h3.getH3UnidirectionalEdge(neighbors[0], neighbors[1])
const isValidEdge = h3.h3UnidirectionalEdgeIsValid(edgeIndex)
const edgeLenM = h3.exactEdgeLength(edgeIndex, h3.UNITS.m)
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
  pentagons: h3.getPentagonIndexes(res),
}))
const res0Indexes = h3.getRes0Indexes()
```

## Why another H3 for Node?

`h3-js` is an emscripten transpilation of the H3 code into Javascript with a Javascript wrapper file similar to a binding in spirit to handle calling these quasi-C functions for you.

Being 100% javascript it works across the entire Javascript ecosystem (especially when paired with browserify or babel to handle the Node-isms for non-Node JS environments), but that portability comes at a cost in performance. Preliminary benchmarks show a significant speedup with the N-API approach:

```
damocles@zelbinion:~/oss/h3-node(add-missing-functions)$ yarn test
yarn run v1.22.17
$ nodeunit test

index
✔ h3IsValid_array
✔ h3IsValid_uint32array
✔ h3ToGeo_array
✔ h3ToGeo_uint32array
✔ geoToH3
✔ h3ToGeo
✔ h3ToGeoBoundary
✔ h3GetResolution
✔ h3GetBaseCell
✔ h3IsValid
✔ h3IsResClassIII
✔ h3IsPentagon
✔ h3GetFaces
✔ kRing
✔ kRingDistances
✔ hexRing
✔ h3Distance
✔ experimentalH3ToLocalIj
✔ experimentalLocalIjToH3
✔ h3ToParent
✔ h3ToChildren
✔ h3ToCenterChild
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
✔ exactEdgeLength
✔ hexArea
✔ cellArea
✔ pointDist
✔ getRes0Indexes
✔ getPentagonIndexes

geoToH3 Benchmark:
H3-js time in ns:    18767309
H3-node time in ns:  1945301
✔ geoToH3Benchmark

h3ToGeo Benchmark:
H3-js time in ns:    2948717
H3-node time in ns:  2840813
✔ h3ToGeoBenchmark

h3ToGeoBoundary Benchmark:
H3-js time in ns:    6879898
H3-node time in ns:  6483617
✔ h3ToGeoBoundaryBenchmark

h3GetResolution Benchmark:
H3-js time in ns:    3210624
H3-node time in ns:  339291
✔ h3GetResolutionBenchmark

h3GetBaseCell Benchmark:
H3-js time in ns:    439374
H3-node time in ns:  335589
✔ h3GetBaseCellBenchmark

h3IsValid Benchmark:
H3-js time in ns:    503768
H3-node time in ns:  296408
✔ h3IsValidBenchmark

h3IsResClassIII Benchmark:
H3-js time in ns:    443146
H3-node time in ns:  367297
✔ h3IsResClassIIIBenchmark

h3IsPentagon Benchmark:
H3-js time in ns:    470733
H3-node time in ns:  641706
✔ h3IsPentagonBenchmark

h3GetFaces Benchmark:
H3-js time in ns:    5752932
H3-node time in ns:  1352904
✔ h3GetFacesBenchmark

kRing Benchmark:
H3-js time in ns:    111333014
H3-node time in ns:  43828405
✔ kRingBenchmark

kRingDistances Benchmark:
H3-js time in ns:    101881334
H3-node time in ns:  59046078
✔ kRingDistancesBenchmark

hexRing Benchmark:
H3-js time in ns:    19888827
H3-node time in ns:  10109520
✔ hexRingBenchmark

h3Distance Benchmark:
H3-js time in ns:    6211304
H3-node time in ns:  1193595
✔ h3DistanceBenchmark

experimentalH3ToLocalIj Benchmark:
H3-js time in ns:    2532111
H3-node time in ns:  2190027
✔ experimentalH3ToLocalIjBenchmark

experimentalLocalIjToH3 Benchmark:
H3-js time in ns:    9239362
H3-node time in ns:  2014305
✔ experimentalLocalIjToH3Benchmark

h3Line Benchmark:
H3-js time in ns:    67614960
H3-node time in ns:  47620392
✔ h3LineBenchmark

h3ToParent Benchmark:
H3-js time in ns:    1715383
H3-node time in ns:  682982
✔ h3ToParentBenchmark

h3ToChildren Benchmark:
H3-js time in ns:    939315510
H3-node time in ns:  915475456
✔ h3ToChildrenBenchmark

h3ToCenterChild Benchmark:
H3-js time in ns:    1638208
H3-node time in ns:  507539
✔ h3ToCenterChildBenchmark

compact Benchmark:
H3-js time in ns:    155996866
H3-node time in ns:  50508975
✔ compactBenchmark

uncompact Benchmark:
H3-js time in ns:    49517782
H3-node time in ns:  48204199
✔ uncompactBenchmark

polyfill Benchmark:
H3-js time in ns:    60105856
H3-node time in ns:  26022048
✔ polyfillBenchmark

polyfillWithHoles Benchmark:
H3-js time in ns:    55994135
H3-node time in ns:  42645147
✔ polyfillWithHolesBenchmark

h3SetToMultiPolygon Benchmark:
H3-js time in ns:    464315666
H3-node time in ns:  400713183
✔ h3SetToMultiPolygonBenchmark

h3SetToMultiPolygonGeoJsonMode Benchmark:
H3-js time in ns:    451669984
H3-node time in ns:  403379251
✔ h3SetToMultiPolygonGeoJsonModeBenchmark

h3SetToMultiPolygonTrueMultiPolygon Benchmark:
H3-js time in ns:    588551030
H3-node time in ns:  535388768
✔ h3SetToMultiPolygonTrueMultiPolygonBenchmark

h3IndexesAreNeighbors Benchmark:
H3-js time in ns:    3111099
H3-node time in ns:  832094
✔ h3IndexesAreNeighborsBenchmark

getH3UnidirectionalEdge Benchmark:
H3-js time in ns:    3015695
H3-node time in ns:  1296961
✔ getH3UnidirectionalEdgeBenchmark

h3UnidirectionalEdgeIsValid Benchmark:
H3-js time in ns:    737598
H3-node time in ns:  405152
✔ h3UnidirectionalEdgeIsValidBenchmark

getOriginH3IndexFromUnidirectionalEdge Benchmark:
H3-js time in ns:    778455
H3-node time in ns:  435393
✔ getOriginH3IndexFromUnidirectionalEdgeBenchmark

getDestinationH3IndexFromUnidirectionalEdge Benchmark:
H3-js time in ns:    1009423
H3-node time in ns:  507051
✔ getDestinationH3IndexFromUnidirectionalEdgeBenchmark

getH3IndexesFromUnidirectionalEdge Benchmark:
H3-js time in ns:    1387476
H3-node time in ns:  1084572
✔ getH3IndexesFromUnidirectionalEdgeBenchmark

getH3UnidirectionalEdgesFromHexagon Benchmark:
H3-js time in ns:    2529737
H3-node time in ns:  2227881
✔ getH3UnidirectionalEdgesFromHexagonBenchmark

getH3UnidirectionalEdgeBoundary Benchmark:
H3-js time in ns:    4458486
H3-node time in ns:  2960521
✔ getH3UnidirectionalEdgeBoundaryBenchmark

degsToRads Benchmark:
H3-js time in ns:    31499
H3-node time in ns:  96801
✔ degsToRadsBenchmark

radsToDegs Benchmark:
H3-js time in ns:    55315
H3-node time in ns:  96102
✔ radsToDegsBenchmark

numHexagons Benchmark:
H3-js time in ns:    289215
H3-node time in ns:  79061
✔ numHexagonsBenchmark

edgeLength Benchmark:
H3-js time in ns:    293336
H3-node time in ns:  133048
✔ edgeLengthBenchmark

exactEdgeLength Benchmark:
H3-js time in ns:    5667935
H3-node time in ns:  1829364
✔ exactEdgeLengthBenchmark

hexArea Benchmark:
H3-js time in ns:    207010
H3-node time in ns:  134585
✔ hexAreaBenchmark

cellArea Benchmark:
H3-js time in ns:    6439128
H3-node time in ns:  4002629
✔ cellAreaBenchmark

pointDist Benchmark:
H3-js time in ns:    1123195
H3-node time in ns:  651134
✔ pointDistBenchmark

getRes0Indexes Benchmark:
H3-js time in ns:    36434751
H3-node time in ns:  31639278
✔ getRes0IndexesBenchmark

getPentagonIndexes Benchmark:
H3-js time in ns:    6350848
H3-node time in ns:  4006261
✔ getPentagonIndexesBenchmark

OK: 452 assertions (9508ms)
Done in 9.70s.
```

`h3-node` is a [Node N-API binding](https://nodejs.org/api/n-api.html) of the [original C H3 code](https://github.com/uber/h3) to provide a higher-performance option in backend Node.js applications.

That makes `h3-node` a "nice to have" but not as required as `h3-js`. (In a similar vein, I intend to write an `h3-wasm` for higher-performance H3 in modern browsers that can be a drop-in replacement for `h3-js` when WebAssembly is present.)
