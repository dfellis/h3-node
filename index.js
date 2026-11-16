const BASE_16 = 16
const INVALID_HEXIDECIMAL_CHAR = /[^0-9a-fA-F]/

/**
 * Zero-pad a string to a given length
 * @private
 * @param {number} fullLen Target length
 * @param {string} numStr  String to zero-pad
 * @returns {string}       Zero-padded string
 */
function zeroPad(fullLen, numStr) {
  const numZeroes = fullLen - numStr.length
  let outStr = ''
  for (let i = 0; i < numZeroes; i++) {
    outStr += '0'
  }
  outStr = outStr + numStr
  return outStr
}

/**
 * Convert a 32-bit int to a hexadecimal string
 * @private
 * @param {number} num Integer to convert
 * @returns {string}   Hexadecimal string
 */
function hexFrom32Bit(num) {
  if (num >= 0) {
    return num.toString(BASE_16)
  }

  // Handle negative numbers
  num = num & 0x7fffffff
  let tempStr = zeroPad(8, num.toString(BASE_16))
  const topNum = (parseInt(tempStr[0], BASE_16) + 8).toString(BASE_16)
  tempStr = topNum + tempStr.substring(1)
  return tempStr
}

/**
 * Convert an H3 index (64-bit hexadecimal string) into a "split long" - a pair of 32-bit ints
 * @param {string|number[]} h3Index H3 index to convert
 * @returns {number[]} A two-element array with the lower 32 bits and the upper 32 bits
 */
function h3IndexToSplitLong(h3Index) {
  if (
    Array.isArray(h3Index) &&
    h3Index.length === 2 &&
    Number.isInteger(h3Index[0]) &&
    Number.isInteger(h3Index[1])
  ) {
    return h3Index
  }
  if (typeof h3Index !== 'string' || INVALID_HEXIDECIMAL_CHAR.test(h3Index)) {
    return [0, 0]
  }
  const upper = parseInt(h3Index.substring(0, h3Index.length - 8), BASE_16)
  const lower = parseInt(h3Index.substring(h3Index.length - 8), BASE_16)
  return [lower, upper]
}

/**
 * Get an H3 index string from a split long (pair of 32-bit ints)
 * @param {number} lower Lower 32 bits
 * @param {number} upper Upper 32 bits
 * @returns {string} H3 index
 */
function splitLongToH3Index(lower, upper) {
  return hexFrom32Bit(upper) + zeroPad(8, hexFrom32Bit(lower))
}

const native = require('node-gyp-build')(__dirname)

// Index-array bindings return an ArrayBuffer of 17-byte hex slots (see
// h3WriteIndexSlots in h3napi.c) whenever there are 3 or more entries:
// byte 0 of each slot is the hex string length, bytes 1..16 are lowercase
// hex (upper 32 bits unpadded, lower 32 bits zero-padded to 8 — exactly
// splitLongToH3Index's formatting). Decoding the whole block with one
// TextDecoder.decode plus one slice per entry replaces ~90 ns of
// napi_create_string_utf8 + napi_set_element work per entry with a few ns.
const indexDecoder = new TextDecoder()

/**
 * Decode an index-array binding result: an ArrayBuffer of 17-byte hex slots
 * becomes a string[]; plain string[] results (outputs with fewer than 3
 * entries) are returned as-is.
 * @private
 * @param {ArrayBuffer|string[]} result Binding result
 * @returns {string[]}
 */
function readIndexArray(result) {
  if (!(result instanceof ArrayBuffer)) return result
  const u8 = new Uint8Array(result)
  const s = indexDecoder.decode(result)
  const n = u8.length / 17
  const out = new Array(n)
  for (let i = 0; i < n; i++) {
    const b = 17 * i
    out[i] = s.slice(b + 1, b + 1 + u8[b])
  }
  return out
}

/**
 * Decode a gridDiskDistances result: an array of rings, or the fast path's
 * { slots, counts } pair (one slot block for the whole disk plus per-ring
 * slot counts), reassembled here into an array of rings.
 * @private
 * @param {ArrayBuffer[]|{slots: ArrayBuffer, counts: Int32Array}} result
 * @returns {string[][]}
 */
function readIndexRings(result) {
  if (Array.isArray(result)) return result
  const u8 = new Uint8Array(result.slots)
  const s = indexDecoder.decode(result.slots)
  const out = new Array(result.counts.length)
  let start = 0
  for (let i = 0; i < result.counts.length; i++) {
    const count = result.counts[i]
    const ring = new Array(count)
    for (let j = 0; j < count; j++) {
      const b = start + 17 * j
      ring[j] = s.slice(b + 1, b + 1 + u8[b])
    }
    out[i] = ring
    start += 17 * count
  }
  return out
}

module.exports = {
  ...native,
  cellToVertexes: (cell) => readIndexArray(native.cellToVertexes(cell)),
  compactCells: (cells) => readIndexArray(native.compactCells(cells)),
  getPentagons: (res) => readIndexArray(native.getPentagons(res)),
  getRes0Cells: () => readIndexArray(native.getRes0Cells()),
  gridDisk: (cell, k) => readIndexArray(native.gridDisk(cell, k)),
  gridDiskDistances: (cell, k) => readIndexRings(native.gridDiskDistances(cell, k)),
  gridPathCells: (origin, destination) => readIndexArray(native.gridPathCells(origin, destination)),
  gridRing: (cell, k) => readIndexArray(native.gridRing(cell, k)),
  gridRingUnsafe: (cell, k) => readIndexArray(native.gridRingUnsafe(cell, k)),
  originToDirectedEdges: (cell) => readIndexArray(native.originToDirectedEdges(cell)),
  polygonToCells: (polygon, res, geoJsonMode) => readIndexArray(native.polygonToCells(polygon, res, geoJsonMode)),
  polygonToCellsExperimental: (polygon, res, flags) => readIndexArray(native.polygonToCellsExperimental(polygon, res, flags)),
  uncompactCells: (cells, res) => readIndexArray(native.uncompactCells(cells, res)),
  cellToChildren: (cell, res) => readIndexArray(native.cellToChildren(cell, res)),
  h3IndexToSplitLong,
  splitLongToH3Index,
  UNITS: {
    m: 'm',
    km: 'km',
    rads: 'rads',
    m2: 'm2',
    km2: 'km2',
    rads2: 'rads2',
  },
}
