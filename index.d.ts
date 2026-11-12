// Type definitions for h3-node

/**
 * Hexadecimal string representation of an H3 index.
 */
export type H3Index = string;

/**
 * 64-bit H3 index represented as two 32-bit integers, in little-endian order.
 */
export type SplitLong = [number, number];

/**
 * H3 indexes can be represented by a hexadecimal string, a [lower, upper]
 * split-long pair, or a Uint32Array containing the lower and upper 32 bits.
 */
export type H3IndexInput = H3Index | readonly number[] | Uint32Array;

/** Coordinate pair in [lat, lng] order, unless GeoJSON output is requested. */
export type CoordPair = [number, number];

/** Coordinates as an `{i, j}` pair. */
export interface CoordIJ {
  i: number;
  j: number;
}

export type DistanceUnit = 'm' | 'km' | 'rads';
export type AreaUnit = 'm2' | 'km2' | 'rads2';

export type PolygonToCellsFlag =
  | 'containmentCenter'
  | 'containmentFull'
  | 'containmentOverlapping'
  | 'containmentOverlappingBbox';

export declare const UNITS: {
  readonly m: 'm';
  readonly km: 'km';
  readonly rads: 'rads';
  readonly m2: 'm2';
  readonly km2: 'km2';
  readonly rads2: 'rads2';
};

/** Convert an H3 index into a [lower, upper] split long. */
export declare function h3IndexToSplitLong(h3Index: H3Index | readonly number[]): SplitLong;

/** Convert a [lower, upper] split long into an H3 index. */
export declare function splitLongToH3Index(lower: number, upper: number): H3Index;

/** Get the H3 cell containing the given coordinate. */
export declare function latLngToCell(lat: number, lng: number, resolution: number): H3Index;

/** Get the center coordinates of a cell as [lat, lng] in degrees. */
export declare function cellToLatLng(h3Index: H3IndexInput): CoordPair;

/**
 * Get the vertices of a cell.
 *
 * By default coordinates are returned as an open ring of [lat, lng] pairs. When
 * `formatAsGeoJson` is true, coordinates use [lng, lat] order and the ring is
 * closed.
 */
export declare function cellToBoundary(h3Index: H3IndexInput, formatAsGeoJson?: boolean): CoordPair[];

/** Get one vertex of a cell. */
export declare function cellToVertex(h3Index: H3IndexInput, vertexNum: number): H3Index;

/** Get all vertexes of a cell. */
export declare function cellToVertexes(h3Index: H3IndexInput): H3Index[];

/** Get the coordinates of a vertex as [lat, lng] in degrees. */
export declare function vertexToLatLng(h3Index: H3IndexInput): CoordPair;

/** Get the resolution of an H3 index. Returns -1 for invalid indexes. */
export declare function getResolution(h3Index: H3IndexInput): number;

/** Get the base cell number of an H3 index. */
export declare function getBaseCellNumber(h3Index: H3IndexInput): number;

/** Get the indexing digit for an H3 index at the given resolution. */
export declare function getIndexDigit(h3Index: H3IndexInput, resolution: number): number;

/** Construct a cell index from its components. */
export declare function constructCell(
  baseCellNumber: number,
  digits: readonly number[],
  resolution?: number
): H3Index;

/** Whether an index is a valid H3 cell. */
export declare function isValidCell(h3Index: H3IndexInput): boolean;

/** Whether an index is any valid H3 index (cell, edge, or vertex). */
export declare function isValidIndex(h3Index: H3IndexInput): boolean;

/** Whether an index is a valid vertex index. */
export declare function isValidVertex(h3Index: H3IndexInput): boolean;

/** Whether the cell is a pentagon. */
export declare function isPentagon(h3Index: H3IndexInput): boolean;

/** Whether the cell has a Class III resolution. */
export declare function isResClassIII(h3Index: H3IndexInput): boolean;

/** Get the icosahedron faces intersected by a cell. */
export declare function getIcosahedronFaces(h3Index: H3IndexInput): number[];

/** Get all cells within `ringSize` steps of an origin. */
export declare function gridDisk(origin: H3IndexInput, ringSize: number): H3Index[];

/** Get cells grouped by grid distance from an origin. */
export declare function gridDiskDistances(origin: H3IndexInput, ringSize: number): H3Index[][];

/** Get cells in a hollow ring. Throws if a pentagon is encountered. */
export declare function gridRingUnsafe(origin: H3IndexInput, ringSize: number): H3Index[];

/** Get cells in a hollow ring, traversing across pentagons when needed. */
export declare function gridRing(origin: H3IndexInput, ringSize: number): H3Index[];

/** Get the grid distance between two cells. */
export declare function gridDistance(origin: H3IndexInput, destination: H3IndexInput): number;

/** Get local IJ coordinates for `destination` relative to `origin`. */
export declare function cellToLocalIj(origin: H3IndexInput, destination: H3IndexInput): CoordIJ;

/** Convert local IJ coordinates back to a cell index. */
export declare function localIjToCell(origin: H3IndexInput, coordinates: CoordIJ): H3Index;

/** Get a path of cells between two cells. */
export declare function gridPathCells(origin: H3IndexInput, destination: H3IndexInput): H3Index[];

/** Get the parent of a cell at a coarser resolution. */
export declare function cellToParent(h3Index: H3IndexInput, resolution: number): H3Index;

/** Get the children of a cell at a finer resolution. */
export declare function cellToChildren(h3Index: H3IndexInput, resolution: number): H3Index[];

/** Get the number of children a cell has at a finer resolution. */
export declare function cellToChildrenSize(h3Index: H3IndexInput, resolution: number): number;

/** Get the center child of a cell at a finer resolution. */
export declare function cellToCenterChild(h3Index: H3IndexInput, resolution: number): H3Index;

/** Get the position of a child cell within its parent. */
export declare function cellToChildPos(child: H3IndexInput, parentResolution: number): number;

/** Get the child cell at a given position under a parent. */
export declare function childPosToCell(parent: H3IndexInput, position: number, childResolution: number): H3Index;

/** Compact a set of cells across resolutions. */
export declare function compactCells(cells: readonly H3IndexInput[]): H3Index[];

/** Uncompact a set of cells to a target resolution. */
export declare function uncompactCells(compactedCells: readonly H3IndexInput[], resolution: number): H3Index[];

/**
 * Get cells contained in a polygon. Coordinates use [lat, lng] by default, or
 * [lng, lat] when `isGeoJson` is true. Multiple loops are interpreted as an
 * exterior loop followed by holes.
 */
export declare function polygonToCells(
  coordinates: number[][] | number[][][],
  resolution: number,
  isGeoJson?: boolean
): H3Index[];

/** Get cells intersecting a polygon using an H3 containment mode. */
export declare function polygonToCellsExperimental(
  coordinates: number[][] | number[][][],
  resolution: number,
  flag: PolygonToCellsFlag | number,
  isGeoJson?: boolean
): H3Index[];

/**
 * Get outlines for a set of cells as a MultiPolygon.
 *
 * By default coordinates are degrees in [lat, lng] order with open rings. When
 * `formatAsGeoJson` is true, coordinates use [lng, lat] order and each ring is
 * closed by repeating its first coordinate at the end.
 */
export declare function cellsToMultiPolygon(
  h3Indexes: readonly H3IndexInput[],
  formatAsGeoJson?: boolean
): CoordPair[][][];

/** Whether two cells share an edge. */
export declare function areNeighborCells(origin: H3IndexInput, destination: H3IndexInput): boolean;

/** Create a directed edge between neighboring cells. */
export declare function cellsToDirectedEdge(origin: H3IndexInput, destination: H3IndexInput): H3Index;

/** Whether an index is a valid directed edge. */
export declare function isValidDirectedEdge(edge: H3IndexInput): boolean;

/** Get the origin cell of a directed edge. */
export declare function getDirectedEdgeOrigin(edge: H3IndexInput): H3Index;

/** Get the destination cell of a directed edge. */
export declare function getDirectedEdgeDestination(edge: H3IndexInput): H3Index;

/** Get the origin and destination cells of a directed edge. */
export declare function directedEdgeToCells(edge: H3IndexInput): [H3Index, H3Index];

/** Get all directed edges originating from a cell. */
export declare function originToDirectedEdges(origin: H3IndexInput): H3Index[];

/**
 * Get the boundary of a directed edge.
 *
 * By default coordinates are [lat, lng] in degrees. When `formatAsGeoJson` is
 * true, coordinates use [lng, lat] order. Edge boundaries remain open line
 * strings, matching h3-js.
 */
export declare function directedEdgeToBoundary(edge: H3IndexInput, formatAsGeoJson?: boolean): CoordPair[];

/** Reverse a directed edge. */
export declare function reverseDirectedEdge(edge: H3IndexInput): H3Index;

/** Convert degrees to radians. */
export declare function degsToRads(degrees: number): number;

/** Convert radians to degrees. */
export declare function radsToDegs(radians: number): number;

/** Get the number of cells at a resolution. */
export declare function getNumCells(resolution: number): number;

/** Get the average edge length for a resolution. */
export declare function getHexagonEdgeLengthAvg(resolution: number, unit: DistanceUnit): number;

/** Get the average cell area for a resolution. */
export declare function getHexagonAreaAvg(resolution: number, unit: AreaUnit): number;

/** Get the exact area of a cell. */
export declare function cellArea(h3Index: H3IndexInput, unit: AreaUnit): number;

/** Get the exact edge length of a directed edge. */
export declare function edgeLength(edge: H3IndexInput, unit: DistanceUnit): number;

/** Calculate the great-circle distance between two coordinates. */
export declare function greatCircleDistance(
  latLng1: CoordPair,
  latLng2: CoordPair,
  unit: DistanceUnit
): number;

/** Get all resolution-zero cells. */
export declare function getRes0Cells(): H3Index[];

/** Get all pentagons at a resolution. */
export declare function getPentagons(resolution: number): H3Index[];
