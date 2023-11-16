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

const h3index = h3.latLngToCell(37.77, -122.43, 9)
const h3center = h3.cellToLatLng(h3index)
const h3outline = h3.cellToBoundary(h3index)
const h3area = h3.cellArea(h3index, h3.UNITS.m2)

const res = h3.getResolution(h3index)
const baseCell = h3.getBaseCellNumber(h3index)
const valid = h3.isValidCell(h3index)
const resIsClassIII = h3.isResClassIII(h3index)
const isPentagon = h3.isPentagon(h3index)
const faces = h3.getIcosahedronFaces(h3Index)
const neighbors = h3.gridDisk(h3index, 1)
const threeRings = h3.gridDiskDistances(h3index, 3)
const ring2Away = h3.gridRingUnsafe(h3index, 2)
const index2 = h3.latLngToCell(38.88, -122.34, 9)
const hexLine = h3.gridPathCells(h3index, index2)
const hexDistance = h3.gridDistance(h3index, index2)
const coords = h3.cellToLocalIj(h3index, index2)
const index2again = h3.localIjToCell(h3index, coords)
const parent = h3.cellToParent(h3index, 8)
const immediateChildren = h3.cellToChildren(h3index, 10)
const centerChild = h3.cellToCenterChild(h3index, 10)
const packedRings = h3.compactCells([].concat(...threeRings))
const unpackedRings = h3.uncompactCells(packedRings, 9)
const bayAreaHexes = h3.polygonToCells([
    [37.77, -122.43],
    [37.55, -122.43],
    [37.55, -122.23],
    [37.77, -122.23],
    [37.77, -122.43],
  ], 9)
const bayAreaHexGeo = h3.cellsToMultiPolygon(bayAreaHexes)
const areNeighbors = h3.areNeighborCells(neighbors[0], neighbors[1])
const edgeIndex = h3.cellsToDirectedEdge(neighbors[0], neighbors[1])
const isValidEdge = h3.isValidDirectedEdge(edgeIndex)
const edgeLenM = h3.edgeLength(edgeIndex, h3.UNITS.m)
const origin = h3.getDirectedEdgeOrigin(edgeIndex)
const destination = h3.getDirectedEdgeDestination(edgeIndex)
const [start, end] = h3.directedEdgeToCells(edgeIndex)
const edges = h3.originToDirectedEdges(h3index)
const edgeBoundaries = edges.map(h3.directedEdgeToBoundary)
const sfCoordsRads = [37.77, -122.43].map(h3.degsToRads)
const sfCoordsDegs = sfCoordsRads.map(h3.radsToDegs)
const resStats = Array.from(new Array(16), (x, res) => ({
  res,
  numHexagons: h3.numHexagons(res),
  edgeLength: {
    m: h3.getHexagonEdgeLengthAvg(res, h3.UNITS.m),
    km: h3.getHexagonEdgeLengthAvg(res, h3.UNITS.km),
  },
  hexArea: {
    m2: h3.getHexagonAreaAvg(res, h3.UNITS.m2),
    km2: h3.getHexagonAreaAvg(res, h3.UNITS.km2),
  },
  pentagons: h3.getPentagons(res),
}))
const res0Indexes = h3.getRes0Cells()
```

## Why another H3 for Node?

`h3-js` is an emscripten transpilation of the H3 code into Javascript with a Javascript wrapper file similar to a binding in spirit to handle calling these quasi-C functions for you.

Being 100% javascript it works across the entire Javascript ecosystem (especially when paired with browserify or babel to handle the Node-isms for non-Node JS environments), but that portability comes at a cost in performance. Preliminary benchmarks show a significant speedup with the N-API approach:

```
damocles@elack:~/oss/h3-node(upgrade-to-v4)$ yarn test
yarn run v1.22.19
$ nodeunit test

index
✔ isValidCell_array
✔ isValidCell_uint32array
✔ cellToLatLng_array
✔ cellToLatLng_uint32array
✔ latLngToCell
✔ cellToLatLng
✔ cellToBoundary
✔ getResolution
✔ getBaseCellNumber
✔ isValidCell
✔ isResClassIII
✔ isPentagon
✔ getIcosahedronFaces
✔ gridDisk
✔ gridDiskDistances
undefined
undefined
✔ gridRingUnsafe
✔ gridDistance
✔ cellToLocalIj
undefined
undefined
✔ localIjToCell
✔ cellToParent
✔ cellToChildren
✔ cellToCenterChild
✔ compactCells
✔ uncompactCells
✔ polygonToCells
✔ polygonToCellsWithHoles
✔ cellsToMultiPolygon
✔ cellsToMultiPolygonGeoJsonMode
✔ cellsToMultiPolygonTrueMultiPolygon
✔ areNeighborCells
✔ cellsToDirectedEdge
✔ isValidDirectedEdge
✔ getDirectedEdgeOrigin
✔ getDirectedEdgeDestination
✔ directedEdgeToCells
✔ originToDirectedEdges
✔ directedEdgeToBoundary
✔ degsToRads
✔ radsToDegs
✔ getNumCells
✔ getHexagonEdgeLengthAvg
✔ edgeLength
✔ getHexagonAreaAvg
✔ cellArea
✔ greatCircleDistance
✔ getRes0Cells
✔ getPentagons

latLngToCell Benchmark:
H3-js time in ns:    8407061
H3-node time in ns:  1696666
✔ latLngToCellBenchmark

cellToLatLng Benchmark:
H3-js time in ns:    3957443
H3-node time in ns:  1500690
✔ cellToLatLngBenchmark

cellToBoundary Benchmark:
H3-js time in ns:    7288756
H3-node time in ns:  5075260
✔ cellToBoundaryBenchmark

getResolution Benchmark:
H3-js time in ns:    2497192
H3-node time in ns:  299202
✔ getResolutionBenchmark

getBaseCellNumber Benchmark:
H3-js time in ns:    341876
H3-node time in ns:  304160
✔ getBaseCellNumberBenchmark

isValidCell Benchmark:
H3-js time in ns:    259113
H3-node time in ns:  263024
✔ isValidCellBenchmark

isResClassIII Benchmark:
H3-js time in ns:    649109
H3-node time in ns:  470734
✔ isResClassIIIBenchmark

isPentagon Benchmark:
H3-js time in ns:    477089
H3-node time in ns:  310935
✔ isPentagonBenchmark

getIcosahedronFaces Benchmark:
H3-js time in ns:    4100479
H3-node time in ns:  1311698
✔ getIcosahedronFacesBenchmark

gridDisk Benchmark:
H3-js time in ns:    119649189
H3-node time in ns:  41318171
✔ gridDiskBenchmark

gridDiskDistances Benchmark:
H3-js time in ns:    104035995
H3-node time in ns:  53421119
✔ gridDiskDistancesBenchmark

gridRingUnsafe Benchmark:
H3-js time in ns:    13028001
H3-node time in ns:  9361658
✔ gridRingUnsafeBenchmark

gridDistance Benchmark:
H3-js time in ns:    6042499
H3-node time in ns:  1839702
✔ gridDistanceBenchmark

cellToLocalIj Benchmark:
H3-js time in ns:    3213419
H3-node time in ns:  2277539
✔ cellToLocalIjBenchmark

localIjToCell Benchmark:
H3-js time in ns:    10561051
H3-node time in ns:  1960318
✔ localIjToCellBenchmark

gridPathCells Benchmark:
H3-js time in ns:    66085174
H3-node time in ns:  48151768
✔ gridPathCellsBenchmark

cellToParent Benchmark:
H3-js time in ns:    1062642
H3-node time in ns:  411856
✔ cellToParentBenchmark

cellToChildren Benchmark:
H3-js time in ns:    985474186
H3-node time in ns:  895306179
✔ cellToChildrenBenchmark

cellToCenterChild Benchmark:
H3-js time in ns:    1203932
H3-node time in ns:  454949
✔ cellToCenterChildBenchmark

compactCells Benchmark:
H3-js time in ns:    120589679
H3-node time in ns:  49026745
✔ compactCellsBenchmark

uncompactCells Benchmark:
H3-js time in ns:    51719983
H3-node time in ns:  43166044
✔ uncompactCellsBenchmark

polygonToCells Benchmark:
H3-js time in ns:    51227180
H3-node time in ns:  27323211
✔ polygonToCellsBenchmark

polygonToCellsWithHoles Benchmark:
H3-js time in ns:    57260529
H3-node time in ns:  41699019
✔ polygonToCellsWithHolesBenchmark

cellsToMultiPolygon Benchmark:
H3-js time in ns:    471233062
H3-node time in ns:  393672608
✔ cellsToMultiPolygonBenchmark

cellsToMultiPolygonGeoJsonMode Benchmark:
H3-js time in ns:    443336590
H3-node time in ns:  390950036
✔ cellsToMultiPolygonGeoJsonModeBenchmark

cellsToMultiPolygonTrueMultiPolygon Benchmark:
H3-js time in ns:    578690116
H3-node time in ns:  518527732
✔ cellsToMultiPolygonTrueMultiPolygonBenchmark

areNeighborCells Benchmark:
H3-js time in ns:    2038122
H3-node time in ns:  647922
✔ areNeighborCellsBenchmark

cellsToDirectedEdge Benchmark:
H3-js time in ns:    1948026
H3-node time in ns:  993080
✔ cellsToDirectedEdgeBenchmark

isValidDirectedEdge Benchmark:
H3-js time in ns:    635001
H3-node time in ns:  294313
✔ isValidDirectedEdgeBenchmark

getDirectedEdgeOrigin Benchmark:
H3-js time in ns:    717554
H3-node time in ns:  400194
✔ getDirectedEdgeOriginBenchmark

getDirectedEdgeDestination Benchmark:
H3-js time in ns:    1119284
H3-node time in ns:  978832
✔ getDirectedEdgeDestinationBenchmark

directedEdgeToCells Benchmark:
H3-js time in ns:    1186332
H3-node time in ns:  933715
✔ directedEdgeToCellsBenchmark

originToDirectedEdges Benchmark:
H3-js time in ns:    2235844
H3-node time in ns:  2371965
✔ originToDirectedEdgesBenchmark

directedEdgeToBoundary Benchmark:
H3-js time in ns:    3099228
H3-node time in ns:  2619554
✔ directedEdgeToBoundaryBenchmark

degsToRads Benchmark:
H3-js time in ns:    33804
H3-node time in ns:  98267
✔ degsToRadsBenchmark

radsToDegs Benchmark:
H3-js time in ns:    87302
H3-node time in ns:  90584
✔ radsToDegsBenchmark

getNumCells Benchmark:
H3-js time in ns:    330701
H3-node time in ns:  88140
✔ getNumCellsBenchmark

getHexagonEdgeLengthAvg Benchmark:
H3-js time in ns:    352072
H3-node time in ns:  111467
✔ getHexagonEdgeLengthAvgBenchmark

edgeLength Benchmark:
H3-js time in ns:    4884662
H3-node time in ns:  2593644
✔ edgeLengthBenchmark

getHexagonAreaAvg Benchmark:
H3-js time in ns:    320085
H3-node time in ns:  112515
✔ getHexagonAreaAvgBenchmark

cellArea Benchmark:
H3-js time in ns:    8145085
H3-node time in ns:  3914910
✔ cellAreaBenchmark

greatCircleDistance Benchmark:
H3-js time in ns:    1210916
H3-node time in ns:  730614
✔ greatCircleDistanceBenchmark

getRes0Cells Benchmark:
H3-js time in ns:    35733206
H3-node time in ns:  28694344
✔ getRes0CellsBenchmark

getPentagons Benchmark:
H3-js time in ns:    5991654
H3-node time in ns:  3668299
✔ getPentagonsBenchmark

OK: 452 assertions (9021ms)
Done in 9.18s.
```

`h3-node` is a [Node N-API binding](https://nodejs.org/api/n-api.html) of the [original C H3 code](https://github.com/uber/h3) to provide a higher-performance option in backend Node.js applications.

That makes `h3-node` a "nice to have" but not as required as `h3-js`. (In a similar vein, I intend to write an `h3-wasm` for higher-performance H3 in modern browsers that can be a drop-in replacement for `h3-js` when WebAssembly is present.)
