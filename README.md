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

## Invalid Index Strings

`isValidCell` matches `h3-js` and returns `false` for malformed strings, including whitespace-padded values, rather than throwing. Other index-consuming functions intentionally throw on malformed strings instead of returning `h3-js` sentinel values such as `-1`, empty arrays, or coordinates derived from an unparsed string.

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

Linux glibc prebuilds are verified before publishing to require no newer than `GLIBC_2.34`, so they load on Amazon Linux 2023. The Linux x64 glibc prebuild is built in a Debian bullseye container to keep the required symbol versions low.

## Why another H3 for Node?

`h3-js` is an emscripten transpilation of the H3 code into Javascript with a Javascript wrapper file similar to a binding in spirit to handle calling these quasi-C functions for you.

Being 100% javascript it works across the entire Javascript ecosystem (especially when paired with browserify or babel to handle the Node-isms for non-Node JS environments), but that portability comes at a cost in performance. Preliminary benchmarks show a significant speedup with the N-API approach:

```
✔ isValidCell_array (1.074318ms)
✔ isValidCell_uint32array (0.096604ms)
✔ isValidCell_invalidStrings (0.237721ms)
✔ invalidCellStringsThrowOutsideIsValidCell (0.193807ms)
✔ cellToLatLng_array (0.441017ms)
✔ cellToLatLng_uint32array (0.130668ms)
✔ cellToBoundary_geoJson (1.393072ms)
✔ cellsToMultiPolygon_geoJson (4.531255ms)
✔ directedEdgeToBoundary_geoJson (0.200069ms)
✔ cellsToMultiPolygon_roundTrip (0.424124ms)
✔ latLngToCell (0.894387ms)
✔ cellToLatLng (0.232201ms)
✔ cellToBoundary (0.281243ms)
✔ getResolution (0.139054ms)
✔ getBaseCellNumber (0.104218ms)
✔ isValidCell (0.090471ms)
✔ isResClassIII (0.098387ms)
✔ isPentagon (0.093256ms)
✔ getIcosahedronFaces (0.309597ms)
✔ gridDisk (3.44296ms)
✔ gridDiskDistances (1.050924ms)
✔ gridRingUnsafe (0.797272ms)
✔ gridDistance (0.577536ms)
✔ cellToLocalIj (0.786111ms)
✔ localIjToCell (0.708543ms)
✔ cellToParent (0.16862ms)
✔ cellToChildren (0.958378ms)
✔ cellToCenterChild (0.147489ms)
✔ compactCells (6.349704ms)
✔ uncompactCells (1.264459ms)
✔ polygonToCells (6.851927ms)
✔ polygonToCellsWithHoles (5.440248ms)
✔ cellsToMultiPolygon (13.254301ms)
✔ cellsToMultiPolygonGeoJsonMode (7.671161ms)
✔ cellsToMultiPolygonTrueMultiPolygon (12.973247ms)
✔ areNeighborCells (0.301432ms)
✔ cellsToDirectedEdge (0.156086ms)
✔ isValidDirectedEdge (0.123735ms)
✔ getDirectedEdgeOrigin (0.121831ms)
✔ getDirectedEdgeDestination (0.132341ms)
✔ directedEdgeToCells (0.159763ms)
✔ originToDirectedEdges (0.129285ms)
✔ directedEdgeToBoundary (0.128794ms)
✔ degsToRads (0.046528ms)
✔ radsToDegs (0.049363ms)
✔ getNumCells (0.087296ms)
✔ getHexagonEdgeLengthAvg (0.101993ms)
✔ edgeLength (0.261006ms)
✔ getHexagonAreaAvg (0.099188ms)
✔ cellArea (0.18982ms)
✔ greatCircleDistance (0.153511ms)
✔ getRes0Cells (0.366556ms)
✔ getPentagons (0.161166ms)
✔ cellToChildPos (0.31129ms)
✔ childPosToCell (0.333463ms)
✔ cellToChildrenSize (0.098216ms)
✔ cellToVertex (0.277597ms)
✔ cellToVertexes (0.252709ms)
✔ vertexToLatLng (0.176134ms)
✔ isValidVertex (0.130437ms)
✔ isValidIndex (0.14249ms)
✔ getIndexDigit (0.114507ms)
✔ gridRing (0.217903ms)
✔ reverseDirectedEdge (0.172817ms)
✔ polygonToCellsExperimental (8.057633ms)
✔ polygonToCellsExperimentalFull (5.297408ms)
✔ polygonToCellsExperimentalOverlapping (4.645071ms)
✔ constructCell (0.225288ms)
✔ isValidVertex_invalid (0.068179ms)
✔ isValidIndex_invalid (0.061827ms)
✔ h3IndexToSplitLong (0.150495ms)
✔ splitLongToH3Index (0.118575ms)
✔ h3IndexToSplitLong_invalid (0.055646ms)
✔ latLngToCellBenchmark (5.289784ms)
✔ cellToLatLngBenchmark (2.474023ms)
✔ cellToBoundaryBenchmark (5.72543ms)
✔ getResolutionBenchmark (1.398233ms)
✔ getBaseCellNumberBenchmark (1.370189ms)
✔ isValidCellBenchmark (0.852617ms)
✔ isResClassIIIBenchmark (1.420945ms)
✔ isPentagonBenchmark (1.121276ms)
✔ getIcosahedronFacesBenchmark (3.202915ms)
✔ gridDiskBenchmark (69.399163ms)
✔ gridDiskDistancesBenchmark (60.784073ms)
✔ gridRingUnsafeBenchmark (8.10874ms)
✔ gridDistanceBenchmark (10.743839ms)
✔ cellToLocalIjBenchmark (11.003631ms)
✔ localIjToCellBenchmark (6.136099ms)
✔ gridPathCellsBenchmark (441.591147ms)
✔ cellToParentBenchmark (1.37598ms)
✔ cellToChildrenBenchmark (400.767956ms)
✔ cellToCenterChildBenchmark (1.442356ms)
✔ compactCellsBenchmark (88.195325ms)
✔ uncompactCellsBenchmark (47.833498ms)
✔ polygonToCellsBenchmark (29.72985ms)
✔ polygonToCellsWithHolesBenchmark (39.09875ms)
✔ cellsToMultiPolygonBenchmark (534.070087ms)
✔ cellsToMultiPolygonGeoJsonModeBenchmark (531.733715ms)
✔ cellsToMultiPolygonTrueMultiPolygonBenchmark (718.799321ms)
✔ areNeighborCellsBenchmark (3.532771ms)
✔ cellsToDirectedEdgeBenchmark (2.275526ms)
✔ isValidDirectedEdgeBenchmark (1.481861ms)
✔ getDirectedEdgeOriginBenchmark (2.165848ms)
✔ getDirectedEdgeDestinationBenchmark (2.283351ms)
✔ directedEdgeToCellsBenchmark (2.663362ms)
✔ originToDirectedEdgesBenchmark (2.893789ms)
✔ directedEdgeToBoundaryBenchmark (3.572376ms)
✔ degsToRadsBenchmark (0.195411ms)
✔ radsToDegsBenchmark (0.203907ms)
✔ getNumCellsBenchmark (0.405659ms)
✔ getHexagonEdgeLengthAvgBenchmark (0.380592ms)
✔ edgeLengthBenchmark (3.611981ms)
✔ getHexagonAreaAvgBenchmark (0.373468ms)
✔ cellAreaBenchmark (3.08978ms)
✔ greatCircleDistanceBenchmark (0.920756ms)
✔ getRes0CellsBenchmark (13.062506ms)
✔ getPentagonsBenchmark (2.772669ms)
✔ cellToChildPosBenchmark (2.329809ms)
✔ childPosToCellBenchmark (2.168393ms)
✔ cellToChildrenSizeBenchmark (1.446614ms)
✔ cellToVertexBenchmark (2.405713ms)
✔ cellToVertexesBenchmark (4.799734ms)
✔ vertexToLatLngBenchmark (2.32474ms)
✔ isValidVertexBenchmark (1.354088ms)
✔ constructCellBenchmark (2.739156ms)
✔ getIndexDigitBenchmark (1.074418ms)
✔ gridRingBenchmark (10.951603ms)
✔ isValidIndexBenchmark (0.716679ms)
✔ polygonToCellsExperimentalBenchmark (148.120637ms)
✔ reverseDirectedEdgeBenchmark (2.61981ms)
✔ allocationGuards (0.320318ms)
✔ ignoredCErrorReturns (0.264872ms)
✔ getResolution_invalidCells (0.054383ms)
✔ unitExactMatching (0.183037ms)
✔ resIntegerValidation (0.25839ms)
✔ cellToChildren_invalidCell (0.106592ms)
✔ corruptCellComputedValuesMatchH3Js (0.091113ms)
✔ test/types-coverage.js (19.641906ms)

latLngToCell Benchmark:
H3-js time in ns:    4292582
H3-node time in ns:  514375

cellToLatLng Benchmark:
H3-js time in ns:    905778
H3-node time in ns:  634995

cellToBoundary Benchmark:
H3-js time in ns:    2084885
H3-node time in ns:  2551350

getResolution Benchmark:
H3-js time in ns:    382726
H3-node time in ns:  130618

getBaseCellNumber Benchmark:
H3-js time in ns:    288638
H3-node time in ns:  131870

isValidCell Benchmark:
H3-js time in ns:    220779
H3-node time in ns:  78739

isResClassIII Benchmark:
H3-js time in ns:    297585
H3-node time in ns:  114096

isPentagon Benchmark:
H3-js time in ns:    176064
H3-node time in ns:  119797

getIcosahedronFaces Benchmark:
H3-js time in ns:    1825342
H3-node time in ns:  689106

gridDisk Benchmark:
H3-js time in ns:    60352523
H3-node time in ns:  8157172

gridDiskDistances Benchmark:
H3-js time in ns:    51355238
H3-node time in ns:  8557461

gridRingUnsafe Benchmark:
H3-js time in ns:    5105143
H3-node time in ns:  2002077

gridDistance Benchmark:
H3-js time in ns:    2118589
H3-node time in ns:  999436

cellToLocalIj Benchmark:
H3-js time in ns:    994256
H3-node time in ns:  569891

localIjToCell Benchmark:
H3-js time in ns:    4331386
H3-node time in ns:  972846

gridPathCells Benchmark:
H3-js time in ns:    44086932
H3-node time in ns:  7785337

cellToParent Benchmark:
H3-js time in ns:    413695
H3-node time in ns:  174802

cellToChildren Benchmark:
H3-js time in ns:    317567285
H3-node time in ns:  82389022

cellToCenterChild Benchmark:
H3-js time in ns:    448631
H3-node time in ns:  172037

compactCells Benchmark:
H3-js time in ns:    67761656
H3-node time in ns:  15632080

uncompactCells Benchmark:
H3-js time in ns:    19058489
H3-node time in ns:  8992236

polygonToCells Benchmark:
H3-js time in ns:    20758854
H3-node time in ns:  8666318

polygonToCellsWithHoles Benchmark:
H3-js time in ns:    25204236
H3-node time in ns:  13543168

cellsToMultiPolygon Benchmark:
H3-js time in ns:    448033417
H3-node time in ns:  81368045

cellsToMultiPolygonGeoJsonMode Benchmark:
H3-js time in ns:    445986113
H3-node time in ns:  81012791

cellsToMultiPolygonTrueMultiPolygon Benchmark:
H3-js time in ns:    588228972
H3-node time in ns:  114303705

areNeighborCells Benchmark:
H3-js time in ns:    941085
H3-node time in ns:  303636

cellsToDirectedEdge Benchmark:
H3-js time in ns:    572065
H3-node time in ns:  297695

isValidDirectedEdge Benchmark:
H3-js time in ns:    205750
H3-node time in ns:  127091

getDirectedEdgeOrigin Benchmark:
H3-js time in ns:    352178
H3-node time in ns:  162899

getDirectedEdgeDestination Benchmark:
H3-js time in ns:    438612
H3-node time in ns:  175833

directedEdgeToCells Benchmark:
H3-js time in ns:    609766
H3-node time in ns:  383657

originToDirectedEdges Benchmark:
H3-js time in ns:    685149
H3-node time in ns:  1578514

directedEdgeToBoundary Benchmark:
H3-js time in ns:    915146
H3-node time in ns:  981993

degsToRads Benchmark:
H3-js time in ns:    18865
H3-node time in ns:  37922

radsToDegs Benchmark:
H3-js time in ns:    19306
H3-node time in ns:  39355

getNumCells Benchmark:
H3-js time in ns:    215659
H3-node time in ns:  52910

getHexagonEdgeLengthAvg Benchmark:
H3-js time in ns:    141849
H3-node time in ns:  58982

edgeLength Benchmark:
H3-js time in ns:    1204345
H3-node time in ns:  584238

getHexagonAreaAvg Benchmark:
H3-js time in ns:    113715
H3-node time in ns:  82216

cellArea Benchmark:
H3-js time in ns:    1445472
H3-node time in ns:  902432

greatCircleDistance Benchmark:
H3-js time in ns:    494317
H3-node time in ns:  243752

getRes0Cells Benchmark:
H3-js time in ns:    10858546
H3-node time in ns:  1996988

getPentagons Benchmark:
H3-js time in ns:    1927756
H3-node time in ns:  633923

cellToChildPos Benchmark:
H3-js time in ns:    1457024
H3-node time in ns:  150866

childPosToCell Benchmark:
H3-js time in ns:    946355
H3-node time in ns:  320978

cellToChildrenSize Benchmark:
H3-js time in ns:    336328
H3-node time in ns:  125778

cellToVertex Benchmark:
H3-js time in ns:    1139592
H3-node time in ns:  392194

cellToVertexes Benchmark:
H3-js time in ns:    2705753
H3-node time in ns:  1467784

vertexToLatLng Benchmark:
H3-js time in ns:    660854
H3-node time in ns:  553590

isValidVertex Benchmark:
H3-js time in ns:    303426
H3-node time in ns:  149553

constructCell Benchmark:
H3-js time in ns:    703414
H3-node time in ns:  360965

getIndexDigit Benchmark:
H3-js time in ns:    222873
H3-node time in ns:  127111

gridRing Benchmark:
H3-js time in ns:    8627194
H3-node time in ns:  1658506

isValidIndex Benchmark:
H3-js time in ns:    213766
H3-node time in ns:  93978

polygonToCellsExperimental Benchmark:
H3-js time in ns:    90878784
H3-node time in ns:  56961291

reverseDirectedEdge Benchmark:
H3-js time in ns:    356757
H3-node time in ns:  189099

ℹ tests 138
ℹ suites 0
ℹ pass 138
ℹ fail 0
ℹ cancelled 0
ℹ skipped 0
ℹ todo 0
ℹ duration_ms 3464.06562
```

`h3-node` is a [Node N-API binding](https://nodejs.org/api/n-api.html) of the [original C H3 code](https://github.com/uber/h3) to provide a higher-performance option in backend Node.js applications.

That makes `h3-node` a "nice to have" but not as required as `h3-js`. (In a similar vein, I intend to write an `h3-wasm` for higher-performance H3 in modern browsers that can be a drop-in replacement for `h3-js` when WebAssembly is present.)
