import * as h3 from '..'

const cell: h3.H3Index = h3.latLngToCell(37.55, -122.43, 9)
const input: h3.H3IndexInput = h3.h3IndexToSplitLong(cell)
const typedArrayInput: h3.H3IndexInput = new Uint32Array([0x3fffffff, 0x8528347])

const center: h3.CoordPair = h3.cellToLatLng(cell)
const boundary: h3.CoordPair[] = h3.cellToBoundary(cell, true)
const cells: h3.H3Index[] = h3.gridDisk(cell, 1)
const multiPolygon: h3.CoordPair[][][] = h3.cellsToMultiPolygon(cells, true)
const child: h3.H3Index = h3.cellToChildren(cell, 10)[0]
const childPosition: number = h3.cellToChildPos(child, 9)
const reconstructed: h3.H3Index = h3.childPosToCell(cell, childPosition, 10)
const edge: h3.H3Index = h3.cellsToDirectedEdge(cell, h3.gridDisk(cell, 1)[1])
const edgeCells: [h3.H3Index, h3.H3Index] = h3.directedEdgeToCells(edge)
const edgeBoundary: h3.CoordPair[] = h3.directedEdgeToBoundary(edge, true)
const ij: h3.CoordIJ = h3.cellToLocalIj(cell, edgeCells[0])
const resolution: number = h3.getResolution(cell)
const valid: boolean = h3.isValidCell(cell)
const validIndex: boolean = h3.isValidIndex(cell)
const validVertex: boolean = h3.isValidVertex(h3.cellToVertex(cell, 0))
const areas: number[] = [
  h3.cellArea(cell, h3.UNITS.m2),
  h3.getHexagonAreaAvg(resolution, h3.UNITS.km2)
]
const lengths: number[] = [
  h3.edgeLength(edge, h3.UNITS.m),
  h3.getHexagonEdgeLengthAvg(resolution, h3.UNITS.km)
]
const distance: number = h3.greatCircleDistance(center, boundary[0], h3.UNITS.rads)
const res0Cells: h3.H3Index[] = h3.getRes0Cells()
const pentagons: h3.H3Index[] = h3.getPentagons(resolution)

h3.constructCell(h3.getBaseCellNumber(cell), [1, 2], 2)
h3.constructCell(h3.getBaseCellNumber(cell), [1, 2])
h3.getIndexDigit(cell, 1)
h3.isPentagon(cell)
h3.isResClassIII(cell)
h3.isValidCell(input)
h3.isValidCell(typedArrayInput)
h3.areNeighborCells(cell, edgeCells[1])
h3.reverseDirectedEdge(edge)
h3.gridRing(cell, 1)
h3.gridRingUnsafe(cell, 1)
h3.uncompactCells(h3.compactCells(cells), 9)
h3.vertexToLatLng(h3.cellToVertex(cell, 0))
h3.cellToVertexes(cell)
h3.degsToRads(180)
h3.radsToDegs(Math.PI)
h3.getNumCells(resolution)
h3.cellToCenterChild(cell, 10)
h3.cellToChildrenSize(cell, 10)
h3.cellToParent(cell, 8)
h3.gridDiskDistances(cell, 1)
h3.gridDistance(cell, edgeCells[0])
h3.gridPathCells(cell, edgeCells[0])
h3.getIcosahedronFaces(cell)
h3.isValidDirectedEdge(edge)
h3.latLngToCell(center[0], center[1], resolution)
h3.localIjToCell(cell, ij)
h3.originToDirectedEdges(cell)
h3.polygonToCells(boundary, resolution)
h3.polygonToCells([boundary], resolution, true)
h3.polygonToCellsExperimental([boundary], resolution, 'containmentFull', true)
h3.splitLongToH3Index(...h3.h3IndexToSplitLong(cell))

void center
void cells
void multiPolygon
void reconstructed
void edgeBoundary
void ij
void valid
void validIndex
void validVertex
void areas
void lengths
void distance
void res0Cells
void pentagons
