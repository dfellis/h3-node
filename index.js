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

module.exports = {
  ...require('node-gyp-build')(__dirname),
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
