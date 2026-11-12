# H3-Node

![Unit Testing](https://github.com/dfellis/h3-node/workflows/Unit%20Testing/badge.svg) [![License](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE)

H3 Bindings to Node using N-API

## Install

```sh
npm i h3-node
```

Prebuilt binaries are bundled for the platforms listed below, so no build tools are needed on those platforms. If no prebuild matches your platform, the package builds itself from source at install time; for that you need `git`, `make`, `cmake`, and a C compiler (`gcc` or `clang` on Unix, MSVC on Windows).

## Versions

The `1.X.Y` versions released before have varying stages of completeness of the API. From version `3.2.0` and on the major and minor versions will match the upstream [C Implementation](https://github.com/uber/h3)'s major and minor versions, while the patch version will increment independently, including fixes for the bindings themselves as well as fixes from upstream.

## Usage

`h3-node` is a drop-in replacement for [H3-js](https://github.com/uber/h3-js). You can [use the H3-js API reference](https://github.com/uber/h3-js#api-reference) for the available methods and the [official H3 documentation](https://uber.github.io/h3/) to fully understand the purposes of the methods. Below is an abbreviated usage of every available method.

```js
const h3 = require('h3-node')

const h3index = h3.latLngToCell(37.77, -122.43, 9)
const h3center = h3.cellToLatLng(h3index)
const h3outline = h3.cellToBoundary(h3index)
const h3outlineGeoJson = h3.cellToBoundary(h3index, true)
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
const bayAreaHexGeoJson = h3.cellsToMultiPolygon(bayAreaHexes, true)
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

// Added in H3 4.5.0 (h3-js 4.5.0)
const [lower, upper] = h3.h3IndexToSplitLong(h3index)
const h3indexAgain = h3.splitLongToH3Index(lower, upper)
const childPos = h3.cellToChildPos(h3index, h3.cellToParent(h3index, 8))
const parentAgain = h3.childPosToCell(h3.cellToParent(h3index, 8), childPos, 9)
const numChildren = h3.cellToChildrenSize(h3index, 10)
const vertexes = h3.cellToVertexes(h3index)
const vertices = h3.cellToVertex(h3index)
const [lat, lng] = h3.vertexToLatLng(vertexes[0])
const isValidVertex = h3.isValidVertex(vertexes[0])
const constructed = h3.constructCell(h3.getBaseCellNumber(h3index), '2'.repeat(9), 9)
const digit = h3.getIndexDigit(h3index, 2)
const ring = h3.gridRing(h3index, 1)
const isValidH3Index = h3.isValidIndex(h3index)
const reversed = h3.reverseDirectedEdge(edgeIndex)
const experimental = h3.polygonToCellsExperimental([
    [37.77, -122.43],
    [37.55, -122.43],
    [37.55, -122.23],
    [37.77, -122.23],
    [37.77, -122.43],
  ], 9, 'containmentFull')
```

## TypeScript

Type definitions are bundled in `index.d.ts`.

The optional GeoJSON arguments mirror `h3-js`: `cellToBoundary(index, true)` and `cellsToMultiPolygon(cells, true)` return closed rings in `[lng, lat]` order, while `directedEdgeToBoundary(edge, true)` returns an open edge in `[lng, lat]` order. The default output remains `[lat, lng]` in degrees.

## Prebuilt Binaries

Prebuilt binaries (N-API, so they work across all supported Node.js versions: 22, 24, and 26) are bundled for:

| Platform | Architectures | Libc |
|:---------|:--------------|:-----|
| Linux    | x64, arm64    | glibc, musl |
| macOS    | arm64         | — |
| Windows  | x64           | — |

If your platform isn't covered, the package falls back to building from source at install time (see [Install](#install)).

Linux glibc prebuilds are verified during release builds to require no newer than `GLIBC_2.34`, so they load on Amazon Linux 2023. The Linux x64 glibc prebuild is built in a Debian bullseye container to keep the required symbol versions low.

## Why another H3 for Node?

`h3-js` is an emscripten transpilation of the H3 code into Javascript with a Javascript wrapper file similar to a binding in spirit to handle calling these quasi-C functions for you.

Being 100% javascript it works across the entire Javascript ecosystem (especially when paired with browserify or babel to handle the Node-isms for non-Node JS environments), but that portability comes at a cost in performance. Preliminary benchmarks show a significant speedup with the N-API approach:

```
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
✔ cellToChildPos
✔ childPosToCell
✔ cellToChildrenSize
✔ cellToVertex
✔ cellToVertexes
✔ vertexToLatLng
✔ isValidVertex
✔ isValidIndex
✔ getIndexDigit
✔ gridRing
✔ reverseDirectedEdge
✔ polygonToCellsExperimental
✔ polygonToCellsExperimentalFull
✔ polygonToCellsExperimentalOverlapping
✔ constructCell
✔ isValidVertex_invalid
✔ isValidIndex_invalid
✔ h3IndexToSplitLong
✔ splitLongToH3Index
✔ h3IndexToSplitLong_invalid

latLngToCell Benchmark:
H3-js time in ns:    4619103
H3-node time in ns:  488187
✔ latLngToCellBenchmark

cellToLatLng Benchmark:
H3-js time in ns:    857377
H3-node time in ns:  563219
✔ cellToLatLngBenchmark

cellToBoundary Benchmark:
H3-js time in ns:    1665811
H3-node time in ns:  1945230
✔ cellToBoundaryBenchmark

getResolution Benchmark:
H3-js time in ns:    271315
H3-node time in ns:  133543
✔ getResolutionBenchmark

getBaseCellNumber Benchmark:
H3-js time in ns:    197505
H3-node time in ns:  131780
✔ getBaseCellNumberBenchmark

isValidCell Benchmark:
H3-js time in ns:    198206
H3-node time in ns:  102865
✔ isValidCellBenchmark

isResClassIII Benchmark:
H3-js time in ns:    190221
H3-node time in ns:  122021
✔ isResClassIIIBenchmark

isPentagon Benchmark:
H3-js time in ns:    202995
H3-node time in ns:  123595
✔ isPentagonBenchmark

getIcosahedronFaces Benchmark:
H3-js time in ns:    1144622
H3-node time in ns:  372416
✔ getIcosahedronFacesBenchmark

gridDisk Benchmark:
H3-js time in ns:    63013568
H3-node time in ns:  16583512
✔ gridDiskBenchmark

gridDiskDistances Benchmark:
H3-js time in ns:    51902568
H3-node time in ns:  20152803
✔ gridDiskDistancesBenchmark

gridRingUnsafe Benchmark:
H3-js time in ns:    5116657
H3-node time in ns:  3881695
✔ gridRingUnsafeBenchmark

gridDistance Benchmark:
H3-js time in ns:    1699194
H3-node time in ns:  852538
✔ gridDistanceBenchmark

cellToLocalIj Benchmark:
H3-js time in ns:    811320
H3-node time in ns:  604086
✔ cellToLocalIjBenchmark

localIjToCell Benchmark:
H3-js time in ns:    4243581
H3-node time in ns:  1036647
✔ localIjToCellBenchmark

gridPathCells Benchmark:
H3-js time in ns:    43459691
H3-node time in ns:  10648602
✔ gridPathCellsBenchmark

cellToParent Benchmark:
H3-js time in ns:    447589
H3-node time in ns:  172287
✔ cellToParentBenchmark

cellToChildren Benchmark:
H3-js time in ns:    300050046
H3-node time in ns:  287434062
✔ cellToChildrenBenchmark

cellToCenterChild Benchmark:
H3-js time in ns:    712712
H3-node time in ns:  260535
✔ cellToCenterChildBenchmark

compactCells Benchmark:
H3-js time in ns:    72515845
H3-node time in ns:  17961426
✔ compactCellsBenchmark

uncompactCells Benchmark:
H3-js time in ns:    20682317
H3-node time in ns:  16906043
✔ uncompactCellsBenchmark

polygonToCells Benchmark:
H3-js time in ns:    25933029
H3-node time in ns:  9079646
✔ polygonToCellsBenchmark

polygonToCellsWithHoles Benchmark:
H3-js time in ns:    25985639
H3-node time in ns:  14340948
✔ polygonToCellsWithHolesBenchmark

cellsToMultiPolygon Benchmark:
H3-js time in ns:    461377552
H3-node time in ns:  84929496
✔ cellsToMultiPolygonBenchmark

cellsToMultiPolygonGeoJsonMode Benchmark:
H3-js time in ns:    461839378
H3-node time in ns:  87037115
✔ cellsToMultiPolygonGeoJsonModeBenchmark

cellsToMultiPolygonTrueMultiPolygon Benchmark:
H3-js time in ns:    610045029
H3-node time in ns:  120697333
✔ cellsToMultiPolygonTrueMultiPolygonBenchmark

areNeighborCells Benchmark:
H3-js time in ns:    994547
H3-node time in ns:  306441
✔ areNeighborCellsBenchmark

cellsToDirectedEdge Benchmark:
H3-js time in ns:    629765
H3-node time in ns:  300991
✔ cellsToDirectedEdgeBenchmark

isValidDirectedEdge Benchmark:
H3-js time in ns:    253210
H3-node time in ns:  131360
✔ isValidDirectedEdgeBenchmark

getDirectedEdgeOrigin Benchmark:
H3-js time in ns:    331408
H3-node time in ns:  170904
✔ getDirectedEdgeOriginBenchmark

getDirectedEdgeDestination Benchmark:
H3-js time in ns:    416540
H3-node time in ns:  181915
✔ getDirectedEdgeDestinationBenchmark

directedEdgeToCells Benchmark:
H3-js time in ns:    507963
H3-node time in ns:  384920
✔ directedEdgeToCellsBenchmark

originToDirectedEdges Benchmark:
H3-js time in ns:    737148
H3-node time in ns:  708925
✔ originToDirectedEdgesBenchmark

directedEdgeToBoundary Benchmark:
H3-js time in ns:    1011980
H3-node time in ns:  1092833
✔ directedEdgeToBoundaryBenchmark

degsToRads Benchmark:
H3-js time in ns:    19587
H3-node time in ns:  40807
✔ degsToRadsBenchmark

radsToDegs Benchmark:
H3-js time in ns:    36149
H3-node time in ns:  38083
✔ radsToDegsBenchmark

getNumCells Benchmark:
H3-js time in ns:    232130
H3-node time in ns:  64342
✔ getNumCellsBenchmark

getHexagonEdgeLengthAvg Benchmark:
H3-js time in ns:    215739
H3-node time in ns:  88438
✔ getHexagonEdgeLengthAvgBenchmark

edgeLength Benchmark:
H3-js time in ns:    1321156
H3-node time in ns:  604257
✔ edgeLengthBenchmark

getHexagonAreaAvg Benchmark:
H3-js time in ns:    161977
H3-node time in ns:  86474
✔ getHexagonAreaAvgBenchmark

cellArea Benchmark:
H3-js time in ns:    1551003
H3-node time in ns:  955222
✔ cellAreaBenchmark

greatCircleDistance Benchmark:
H3-js time in ns:    462517
H3-node time in ns:  259022
✔ greatCircleDistanceBenchmark

getRes0Cells Benchmark:
H3-js time in ns:    11481543
H3-node time in ns:  10656327
✔ getRes0CellsBenchmark

getPentagons Benchmark:
H3-js time in ns:    1209715
H3-node time in ns:  1286511
✔ getPentagonsBenchmark

cellToChildPos Benchmark:
H3-js time in ns:    1071734
H3-node time in ns:  161506
✔ cellToChildPosBenchmark

childPosToCell Benchmark:
H3-js time in ns:    1162004
H3-node time in ns:  321581
✔ childPosToCellBenchmark

cellToChildrenSize Benchmark:
H3-js time in ns:    485771
H3-node time in ns:  221781
✔ cellToChildrenSizeBenchmark

cellToVertex Benchmark:
H3-js time in ns:    1316878
H3-node time in ns:  425497
✔ cellToVertexBenchmark

cellToVertexes Benchmark:
H3-js time in ns:    3041841
H3-node time in ns:  1803081
✔ cellToVertexesBenchmark

vertexToLatLng Benchmark:
H3-js time in ns:    690139
H3-node time in ns:  571565
✔ vertexToLatLngBenchmark

isValidVertex Benchmark:
H3-js time in ns:    332211
H3-node time in ns:  163350
✔ isValidVertexBenchmark

constructCell Benchmark:
H3-js time in ns:    768479
H3-node time in ns:  376584
✔ constructCellBenchmark

getIndexDigit Benchmark:
H3-js time in ns:    243542
H3-node time in ns:  132492
✔ getIndexDigitBenchmark

gridRing Benchmark:
H3-js time in ns:    7962526
H3-node time in ns:  2710041
✔ gridRingBenchmark

isValidIndex Benchmark:
H3-js time in ns:    217833
H3-node time in ns:  97465
✔ isValidIndexBenchmark

polygonToCellsExperimental Benchmark:
H3-js time in ns:    97051925
H3-node time in ns:  60139124
✔ polygonToCellsExperimentalBenchmark

reverseDirectedEdge Benchmark:
H3-js time in ns:    381964
H3-node time in ns:  214928
✔ reverseDirectedEdgeBenchmark

OK: 624 assertions (4427ms)
```

`h3-node` is a [Node N-API binding](https://nodejs.org/api/n-api.html) of the [original C H3 code](https://github.com/uber/h3) to provide a higher-performance option in backend Node.js applications.

That makes `h3-node` a "nice to have" but not as required as `h3-js`. (In a similar vein, I intend to write an `h3-wasm` for higher-performance H3 in modern browsers that can be a drop-in replacement for `h3-js` when WebAssembly is present.)
