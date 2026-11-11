const h3js = require('h3-js')
const h3node = require('..')

const INFINITESIMAL = 0.0000001

const testGen = (methodName, genArgs, testFn) => test => {
  test.expect(10)
  const runs = []
  for (let i = 0; i < 10; i++) {
    runs.push(genArgs())
  }
  for (let i = 0; i < 10; i++) {
    testFn(test, methodName, runs[i])
  }
  test.done()
}

const simpleTest = (test, methodName, args) =>
  test.deepEqual(h3node[methodName](...args), h3js[methodName](...args))

const simpleArrTest = (test, methodName, args) =>
  test.deepEqual(h3node[methodName](...args).sort(), h3js[methodName](...args).sort())

const simpleTryCatchTest = (test, methodName, args) => {
  let node, js, nodeErr, jsErr;
  try {
    js = h3js[methodName](...args);
  } catch (e) {
    jsErr = e;
  }
  try {
    node = h3node[methodName](...args);
  } catch (e) {
    nodeErr = e;
  }
  if (node && js && !nodeErr && !jsErr) {
    test.deepEqual(node, js);
  } else if (nodeErr && jsErr) {
    test.equal(nodeErr.code, jsErr.code);
  } else {
    console.log({ node, js, nodeErr, jsErr });
    throw new Error('wut');
  }
}

const almostEqualTest = (test, methodName, args) => {
  let almostEqual = true
  const node = h3node[methodName](...args)
  const js = h3js[methodName](...args)
  if (typeof node === 'number' && typeof js === 'number') {
    // Tolerance scaled to the magnitude of the value
    almostEqual = Math.abs(node - js) <= INFINITESIMAL * Math.max(1, Math.abs(js))
  } else {
    for (let j = 0; j < node.length; j++) {
      if (Math.abs(node[j] - js[j]) > INFINITESIMAL * Math.max(1, Math.abs(js[j]))) almostEqual = false
    }
  }
  test.ok(almostEqual)
}

const allowPentagonTest = (test, methodName, args) => {
  let node, js
  try {
    node = h3node[methodName](...args)
    js = h3js[methodName](...args)
    test.deepEqual(node, js)
  } catch (e) {
    console.log(node)
    console.log(js)
    test.ok(/.*[pP]entagon.*/.test(e.message)) // Let pentagon encounters through
  }
}

const allowPentagonLineTest = (test, methodName, args) => {
  let node, js
  try {
    node = h3node[methodName](...args)
    js = h3js[methodName](...args)
    test.deepEqual(node, js)
  } catch (e) {
    test.ok(/.*Line.*/.test(e.message)) // Let pentagon encounters through
  }
}

const benchmarkGen = (methodName, genArgs, useTryCatch = false, extraName = '') => test => {
  const runs = []
  for (let i = 0; i < 1000; i++) {
    runs.push(genArgs())
  }
  let start, h3jsTime, middle, h3nodeTime
  if (useTryCatch) {
    start = process.hrtime()
    for (let i = 0; i < 1000; i++) {
      try {
        h3js[methodName](...runs[i])
      } catch(e) {}
    }
    h3jsTime = process.hrtime(start)
    middle = process.hrtime()
    for (let i = 0; i < 1000; i++) {
      try {
        h3node[methodName](...runs[i])
      } catch(e) {}
    }
    h3nodeTime = process.hrtime(middle)
  } else {
    start = process.hrtime()
    for (let i = 0; i < 1000; i++) {
      h3js[methodName](...runs[i])
    }
    h3jsTime = process.hrtime(start)
    middle = process.hrtime()
    for (let i = 0; i < 1000; i++) {
      h3node[methodName](...runs[i])
    }
    h3nodeTime = process.hrtime(middle)
  }

  console.log('')
  console.log(`${methodName}${extraName} Benchmark:`)
  console.log('H3-js time in ns:   ', h3jsTime[0] * 1e9 + h3jsTime[1])
  console.log('H3-node time in ns: ', h3nodeTime[0] * 1e9 + h3nodeTime[1])
  test.done()
}

const randCoords = () => [360 * Math.random() - 180, 180 * Math.random() - 90]

const randElements = (array, numItems) => Array.from(new Array(numItems), function() {
  let v
  do {
    v = Math.floor(Math.random() * array.length)
  } while (this.usedIndices.includes(v))
  this.usedIndices.push(v)
  return v
}, { usedIndices: [] }).map(i => array[i])

const exportTest = (methodName, genArgs, testFn, extraName = '') =>
  exports[`${methodName}${extraName}`] = testGen(methodName, genArgs, testFn)

const exportBenchmark = (methodName, genArgs, useTryCatch = false, extraName = '') =>
  exports[`${methodName}${extraName}Benchmark`] = benchmarkGen(methodName, genArgs, useTryCatch, extraName)

// isValidCell has unique input parsing logic to return false rather than throw on invalid input
exports['isValidCell_array'] = test => {
    test.equal(h3node.isValidCell([0x3fffffff, 0x8528347]), true, 'Integer H3 index is considered an index');
    test.equal(
        h3node.isValidCell([0x73fffffff, 0xff2834]), false,
        'Integer with incorrect bits is not considered an index'
    );
    test.equal(h3node.isValidCell([]), false, 'Empty array is not valid');
    test.equal(h3node.isValidCell([1]), false, 'Array with a single element is not valid');
    test.equal(h3node.isValidCell([1, 'a']), false, 'Array with invalid elements is not valid');
    test.equal(h3node.isValidCell([0x3fffffff, 0x8528347, 0]), false,
        'Array with an additional element is not valid'
    );
    test.done();
};

exports['isValidCell_uint32array'] = test => {
    test.equal(h3node.isValidCell(new Uint32Array([0x3fffffff, 0x8528347])), true, 'Integer H3 index is considered an index');
    test.equal(
        h3node.isValidCell(new Uint32Array([0x73fffffff, 0xff2834])), false,
        'Integer with incorrect bits is not considered an index'
    );
    test.equal(h3node.isValidCell(new Uint32Array([])), false, 'Empty array is not valid');
    test.equal(h3node.isValidCell(new Uint32Array([1])), false, 'Array with a single element is not valid');
    test.equal(h3node.isValidCell(new Uint32Array([0x3fffffff, 0x8528347, 1])), false,
        'Array with too many elements is not valid'
    );
    test.done();
};

// TODO: Write tests for isValidDirectedEdge and isValidVertex

// Exercise the macro used to reading H3 index input from Node
exports['cellToLatLng_array'] = test => {
    test.deepEqual(h3node.cellToLatLng([0x3fffffff, 0x8528347]), h3node.cellToLatLng('85283473fffffff'), 'Integer H3 index is considered an index');
    test.deepEqual(h3node.cellToLatLng([0x73fffffff, 0xff2834]), h3node.cellToLatLng('ff28343fffffff'),
        'Integer with incorrect bits handled consistently'
    );
    test.throws(() => h3node.cellToLatLng([]), 'Empty array is not valid');
    test.throws(() => h3node.cellToLatLng([1]), 'Array with a single element is not valid');
    test.throws(() => h3node.cellToLatLng([1, 'a']), 'Array with invalid elements is not valid');
    test.throws(() => h3node.cellToLatLng([0x3fffffff, 0x8528347, 0]),
        'Array with an additional element is not valid'
    );
    test.done();
};

exports['cellToLatLng_uint32array'] = test => {
    test.deepEqual(h3node.cellToLatLng(new Uint32Array([0x3fffffff, 0x8528347])), h3node.cellToLatLng('85283473fffffff'), 'Integer H3 index is considered an index');
    test.deepEqual(h3node.cellToLatLng(new Uint32Array([0x73fffffff, 0xff2834])), h3node.cellToLatLng('ff28343fffffff'),
        'Integer with incorrect bits handled consistently'
    );
    test.throws(() => h3node.cellToLatLng(new Uint32Array([])), 'Empty array is not valid');
    test.throws(() => h3node.cellToLatLng(new Uint32Array([1])), 'Array with a single element is not valid');
    test.throws(() => h3node.cellToLatLng(new Uint32Array([0x3fffffff, 0x8528347, 1])),
        'Array with too many elements is not valid'
    );
    test.done();
};

exportTest('latLngToCell', () => [...randCoords(), 9], simpleTest)
exportTest('cellToLatLng', () => [h3node.latLngToCell(...randCoords(), 9)], almostEqualTest)
exportTest('cellToBoundary', () => [h3node.latLngToCell(...randCoords(), 9)], almostEqualTest)
exportTest('getResolution', () => [
  h3node.latLngToCell(...randCoords(), Math.floor(Math.random() * 16))
], simpleTest)
exportTest('getBaseCellNumber', () => [h3node.latLngToCell(...randCoords(), 9)], simpleTest)
exportTest('isValidCell', () => [
  Math.random() > 0.5 ? h3node.latLngToCell(...randCoords(), 9) : 'asdfjkl;'
], simpleTest)
// TODO: Write tests for isValidDirectedEdge and isValidVertex
exportTest('isResClassIII', () => [
  h3node.latLngToCell(...randCoords(), Math.floor(Math.random() * 16))
], simpleTest)
exportTest('isPentagon', () => [
  h3node.latLngToCell(...randCoords(), Math.floor(Math.random() * 16))
], simpleTest)
exportTest('getIcosahedronFaces', () => [
  h3node.latLngToCell(...randCoords(), 2)
], simpleTest)
exportTest('gridDisk', () => [
  h3node.latLngToCell(...randCoords(), Math.floor(Math.random() * 16)),
  Math.floor(Math.random() * 10 + 1),
], simpleTest)
exportTest('gridDiskDistances', () => [
  h3node.latLngToCell(...randCoords(), Math.floor(Math.random() * 16)),
  Math.floor(Math.random() * 10 + 1),
], simpleTest)
exportTest('gridRingUnsafe', () => [
  h3node.latLngToCell(...randCoords(), Math.floor(Math.random() * 16)),
  Math.floor(Math.random() * 10 + 1),
], allowPentagonTest)
exportTest('gridDistance', () => h3node
  .gridDisk(h3node.latLngToCell(...randCoords(), Math.floor(Math.random() * 16)), 5)
  .filter(function(h, i, a) {
    if (a.length - i === this.count) {
      this.count--
      return true
    }
    if (this.count === 0) return false
    return Math.random() < 0.5
  }, { count: 2 }), simpleTryCatchTest)
exportTest('cellToLocalIj', () => h3node
  .gridDisk(h3node.latLngToCell(...randCoords(), 9), 5)
  .filter(function(h, i, a) {
    if (a.length - i === this.count) {
      this.count--
      return true
    }
    if (this.count === 0) return false
    return Math.random() < 0.5
  }, { count: 2 }), allowPentagonTest)
exportTest('localIjToCell', () => [
  h3node.latLngToCell(...randCoords(), Math.floor(Math.random() * 16)),
  {
    i: Math.floor(Math.random() * 5 + 1),
    j: Math.floor(Math.random() * 5 + 1),
  },
], allowPentagonTest);
// Pending validation of solution to H3 issue #184: https://github.com/uber/h3/issues/184
/* exportTest('gridPathCells', () => h3node.gridDisk(
  h3node.latLngToCell(...randCoords(), 9), Math.floor(Math.random() * 100)
).filter((e, i, a) => i === 0 || i === a.length - 1), allowPentagonLineTest) */
exportTest('cellToParent', () => [
  h3node.latLngToCell(...randCoords(), 9),
  Math.floor(Math.random() * 9),
], simpleTest)
exportTest('cellToChildren', () => [
  h3node.latLngToCell(...randCoords(), 9),
  Math.floor(15 - Math.random() * 6),
], simpleTest)
exportTest('cellToCenterChild', () => [
  h3node.latLngToCell(...randCoords(), 9),
  Math.floor(15 - Math.random() * 6),
], simpleTest)
exportTest('compactCells', () => [h3node.gridDisk(h3node.latLngToCell(...randCoords(), 9), 6)], simpleTest)
exportTest('uncompactCells', () => [
  h3node.compactCells(h3node.gridDisk(h3node.latLngToCell(...randCoords(), 9), 6)),
  9,
], simpleTest)
exportTest('polygonToCells', () => [
  [
    [37.77, -122.43],
    [37.55, -122.43],
    [37.55, -122.23],
    [37.77, -122.23],
    [37.77, -122.43],
  ],
  6,
], simpleArrTest)
exportTest('polygonToCells', () => [
  [
    [
      [37.77, -122.43],
      [37.55, -122.43],
      [37.55, -122.23],
      [37.77, -122.23],
      [37.77, -122.43],
    ], [
      [37.67, -122.43],
      [37.55, -122.43],
      [37.55, -122.33],
      [37.67, -122.33],
      [37.67, -122.43],
    ],
  ],
  6,
], simpleArrTest, 'WithHoles')
exportTest('cellsToMultiPolygon', () => [
  h3node.gridDisk(h3node.latLngToCell(...randCoords(), 9), 6),
], almostEqualTest)
exportTest('cellsToMultiPolygon', () => [
  h3node.gridDisk(h3node.latLngToCell(...randCoords(), 9), 6),
  true,
], almostEqualTest, 'GeoJsonMode')
exportTest('cellsToMultiPolygon', () => [
  [...new Set([
    ...h3node.gridDisk(h3node.latLngToCell(...randCoords(), 9), 6),
    ...h3node.gridDisk(h3node.latLngToCell(...randCoords(), 9), 3),
  ])]
], almostEqualTest, 'TrueMultiPolygon')
exportTest('areNeighborCells', () =>
  randElements(h3node.gridDisk(h3node.latLngToCell(...randCoords(), 9), 2), 2),
  simpleTest)
exportTest('cellsToDirectedEdge', () => {
  const randIndex = h3node.latLngToCell(...randCoords(), 9)
  return [randIndex, h3node.gridDisk(randIndex, 1).pop()]
}, simpleTest)
exportTest('isValidDirectedEdge', () => {
  const randIndex = h3node.latLngToCell(...randCoords(), 9)
  if (Math.random() > 0.5) {
    return [h3node.cellsToDirectedEdge(randIndex, h3node.gridDisk(randIndex, 1).pop())]
  } else {
    return [randIndex]
  }
}, simpleTest)
exportTest('getDirectedEdgeOrigin', () => {
  const randIndex = h3node.latLngToCell(...randCoords(), 9)
  return [h3node.cellsToDirectedEdge(randIndex, h3node.gridDisk(randIndex, 1).pop())]
}, simpleTest)
exportTest('getDirectedEdgeDestination', () => {
  const randIndex = h3node.latLngToCell(...randCoords(), 9)
  return [h3node.cellsToDirectedEdge(randIndex, h3node.gridDisk(randIndex, 1).pop())]
}, simpleTest)
exportTest('directedEdgeToCells', () => {
  const randIndex = h3node.latLngToCell(...randCoords(), 9)
  return [h3node.cellsToDirectedEdge(randIndex, h3node.gridDisk(randIndex, 1).pop())]
}, simpleTest)
exportTest('originToDirectedEdges', () =>
  [h3node.latLngToCell(...randCoords(), 9)], simpleTest)
exportTest('directedEdgeToBoundary', () => {
  const randIndex = h3node.latLngToCell(...randCoords(), 9)
  return [h3node.cellsToDirectedEdge(randIndex, h3node.gridDisk(randIndex, 1).pop())]
}, almostEqualTest)
exportTest('degsToRads', () => [Math.floor(Math.random() * 360)], almostEqualTest)
exportTest('radsToDegs', () => [Math.floor(Math.random() * 2 * Math.PI)], almostEqualTest)
exportTest('getNumCells', () => [Math.floor(Math.random() * 16)], almostEqualTest)
exportTest('getHexagonEdgeLengthAvg', () => [
  Math.floor(Math.random() * 16),
  Math.random() > 0.5 ? h3node.UNITS.m : h3node.UNITS.km,
], almostEqualTest)
exportTest('edgeLength', () => {
  const randIndex = h3node.latLngToCell(...randCoords(), 9)
  return [
    h3node.cellsToDirectedEdge(randIndex, h3node.gridDisk(randIndex, 1).pop()),
    Math.random() < 0.34 ? h3node.UNITS.m : Math.random() > 0.5 ? h3node.UNITS.km : h3node.UNITS.rads
  ]
}, almostEqualTest)
exportTest('getHexagonAreaAvg', () => [
  Math.floor(Math.random() * 16),
  Math.random() > 0.5 ? h3node.UNITS.m2 : h3node.UNITS.km2,
], almostEqualTest)
exportTest('cellArea', () => [
  h3node.latLngToCell(...randCoords(), 9),
  Math.random() < 0.34 ? h3node.UNITS.m2 : Math.random() > 0.5 ? h3node.UNITS.km2 : h3node.UNITS.rads2
], almostEqualTest)
exportTest('greatCircleDistance', () => [
  randCoords(),
  randCoords(),
  Math.random() < 0.34 ? h3node.UNITS.m : Math.random() > 0.5 ? h3node.UNITS.km : h3node.UNITS.rads
], almostEqualTest)
exportTest('getRes0Cells', () => [], simpleTest)
exportTest('getPentagons', () => [Math.floor(Math.random() * 16)], simpleArrTest)

// --- H3 v4.5.0 API additions -------------------------------------------------
exportTest('cellToChildPos', () => {
  const childRes = Math.floor(9 + Math.random() * 6)
  const child = h3node.latLngToCell(...randCoords(), childRes)
  const parentRes = Math.floor(Math.random() * childRes)
  return [child, parentRes]
}, simpleTest)
exportTest('childPosToCell', () => {
  const childRes = Math.floor(9 + Math.random() * 6)
  const child = h3node.latLngToCell(...randCoords(), childRes)
  const parentRes = Math.floor(Math.random() * childRes)
  const parent = h3node.cellToParent(child, parentRes)
  const childPos = h3node.cellToChildPos(child, parentRes)
  return [childPos, parent, childRes]
}, simpleTest)
exportTest('cellToChildrenSize', () => {
  const cellRes = Math.floor(9 + Math.random() * 4)
  const childRes = Math.floor(13 + Math.random() * 3)
  return [h3node.latLngToCell(...randCoords(), cellRes), childRes]
}, simpleTest)
exportTest('cellToVertex', () => {
  const cell = h3node.latLngToCell(...randCoords(), 9)
  const numVerts = h3node.isPentagon(cell) ? 5 : 6
  return [cell, Math.floor(Math.random() * numVerts)]
}, simpleTest)
exportTest('cellToVertexes', () => [
  h3node.latLngToCell(...randCoords(), 9),
], simpleArrTest)
exportTest('vertexToLatLng', () => {
  const cell = h3node.latLngToCell(...randCoords(), 9)
  return [h3node.cellToVertex(cell, Math.floor(Math.random() * (h3node.isPentagon(cell) ? 5 : 6)))]
}, almostEqualTest)
exportTest('isValidVertex', () => {
  const cell = h3node.latLngToCell(...randCoords(), 9)
  return [h3node.cellToVertex(cell, 0)]
}, simpleTest)
exportTest('isValidIndex', () => {
  const cell = h3node.latLngToCell(...randCoords(), 9)
  return [Math.random() > 0.5 ? cell : h3node.cellToVertex(cell, 0)]
}, simpleTest)
exportTest('getIndexDigit', () => {
  const res = Math.floor(2 + Math.random() * 14)
  const cell = h3node.latLngToCell(...randCoords(), res)
  return [cell, Math.floor(1 + Math.random() * res)]
}, simpleTest)
exportTest('gridRing', () => [
  h3node.latLngToCell(...randCoords(), Math.floor(Math.random() * 16)),
  Math.floor(Math.random() * 5 + 1),
], allowPentagonTest)
exportTest('reverseDirectedEdge', () => {
  const randIndex = h3node.latLngToCell(...randCoords(), 9)
  const neighbor = h3node.gridDisk(randIndex, 1).filter(c => c !== randIndex)[0]
  return [h3node.cellsToDirectedEdge(randIndex, neighbor)]
}, simpleTest)
exportTest('polygonToCellsExperimental', () => [
  [
    [37.77, -122.43],
    [37.55, -122.43],
    [37.55, -122.23],
    [37.77, -122.23],
    [37.77, -122.43],
  ],
  6,
  'containmentCenter',
], simpleArrTest)
exportTest('polygonToCellsExperimental', () => [
  [
    [37.77, -122.43],
    [37.55, -122.43],
    [37.55, -122.23],
    [37.77, -122.23],
    [37.77, -122.43],
  ],
  6,
  'containmentFull',
], simpleArrTest, 'Full')
exportTest('polygonToCellsExperimental', () => [
  [
    [37.77, -122.43],
    [37.55, -122.43],
    [37.55, -122.23],
    [37.77, -122.23],
    [37.77, -122.43],
  ],
  6,
  'containmentOverlapping',
], simpleArrTest, 'Overlapping')

// constructCell is the inverse of getResolution/getBaseCellNumber/getIndexDigit,
// so exercise it as a round-trip.
exports['constructCell'] = test => {
  const res = Math.floor(2 + Math.random() * 13)
  const cell = h3node.latLngToCell(...randCoords(), res)
  const baseCellNumber = h3node.getBaseCellNumber(cell)
  const digits = []
  for (let i = 1; i <= res; i++) digits.push(h3node.getIndexDigit(cell, i))
  test.equal(h3node.constructCell(baseCellNumber, digits, res), cell, 'constructCell reconstructs the cell')
  test.equal(h3node.constructCell(baseCellNumber, digits, res), h3js.constructCell(baseCellNumber, digits, res), 'constructCell matches h3-js')
  test.equal(h3node.constructCell(baseCellNumber, digits), cell, 'constructCell infers res from digits length')
  test.done()
}

// isValidVertex / isValidIndex should report false (not throw) for unparseable input
exports['isValidVertex_invalid'] = test => {
  test.equal(h3node.isValidVertex('zzzzzzzzzzzzzzzz'), false, 'non-hex string is not a valid vertex')
  test.equal(h3node.isValidVertex([]), false, 'empty array is not a valid vertex')
  test.equal(h3node.isValidVertex(12345), false, 'bare number is not a valid vertex')
  test.done()
}
exports['isValidIndex_invalid'] = test => {
  test.equal(h3node.isValidIndex('zzzzzzzzzzzzzzzz'), false, 'non-hex string is not a valid index')
  test.equal(h3node.isValidIndex([]), false, 'empty array is not a valid index')
  test.equal(h3node.isValidIndex(12345), false, 'bare number is not a valid index')
  test.done()
}

// h3IndexToSplitLong / splitLongToH3Index are pure-JS helpers (not N-API
// bindings), but they are part of the h3-js API surface we mirror
exportTest('h3IndexToSplitLong', () => [h3node.latLngToCell(...randCoords(), 9)], simpleTest)
exportTest('splitLongToH3Index', () => h3node.h3IndexToSplitLong(h3node.latLngToCell(...randCoords(), 9)), simpleTest)
exports['h3IndexToSplitLong_invalid'] = test => {
  test.deepEqual(h3node.h3IndexToSplitLong('zzzzzzzzzzzzzzzz'), [0, 0], 'non-hex string is [0, 0]')
  test.deepEqual(h3node.h3IndexToSplitLong(12345), [0, 0], 'bare number is [0, 0]')
  test.deepEqual(h3node.h3IndexToSplitLong([1, 2]), [1, 2], 'split longs pass through')
  test.done()
}

exportBenchmark('latLngToCell', () => [...randCoords(), 9])
exportBenchmark('cellToLatLng', () => [h3node.latLngToCell(...randCoords(), 9)])
exportBenchmark('cellToBoundary', () => [h3node.latLngToCell(...randCoords(), 9)])
exportBenchmark('getResolution', () => [
  h3node.latLngToCell(...randCoords(), Math.floor(Math.random() * 16))
])
exportBenchmark('getBaseCellNumber', () => [
  h3node.latLngToCell(...randCoords(), Math.floor(Math.random() * 16))
])
exportBenchmark('isValidCell', () => [
  Math.random() > 0.5 ? h3node.latLngToCell(...randCoords(), 9) : 'asdfjkl;'
])
// TODO: Write test for isValidDirectedEdge and isValidVertex
exportBenchmark('isResClassIII', () => [
  h3node.latLngToCell(...randCoords(), Math.floor(Math.random() * 16))
])
exportBenchmark('isPentagon', () => [
  h3node.latLngToCell(...randCoords(), Math.floor(Math.random() * 16))
])
exportBenchmark('getIcosahedronFaces', () => [
  h3node.latLngToCell(...randCoords(), Math.floor(Math.random() * 16))
])
exportBenchmark('gridDisk', () => [
  h3node.latLngToCell(...randCoords(), Math.floor(Math.random() * 16)),
  Math.floor(Math.random() * 10 + 1),
])
exportBenchmark('gridDiskDistances', () => [
  h3node.latLngToCell(...randCoords(), Math.floor(Math.random() * 16)),
  Math.floor(Math.random() * 10 + 1),
])
exportBenchmark('gridRingUnsafe', () => [
  h3node.latLngToCell(...randCoords(), Math.floor(Math.random() * 16)),
  Math.floor(Math.random() * 10 + 1),
], true)
exportBenchmark('gridDistance', () => h3node
  .gridDisk(h3node.latLngToCell(...randCoords(), Math.floor(Math.random() * 16)), 5)
  .filter(function(h, i, a) {
    if (a.length - i === this.count) {
      this.count--
      return true
    }
    if (this.count === 0) return false
    return Math.random() < 0.5
  }, { count: 2, }), true)
exportBenchmark('cellToLocalIj', () => h3node
  .gridDisk(h3node.latLngToCell(...randCoords(), 9), 5)
  .filter(function(h, i, a) {
    if (a.length - i === this.count) {
      this.count--
      return true
    }
    if (this.count === 0) return false
    return Math.random() < 0.5
  }, { count: 2, }))
exportBenchmark('localIjToCell', () => [
  h3node.latLngToCell(...randCoords(), Math.floor(Math.random() * 16)),
  {
    i: Math.floor(Math.random() * 5 + 1),
    j: Math.floor(Math.random() * 5 + 1),
  },
], true);
exportBenchmark('gridPathCells', () => h3node.gridDisk(
  h3node.latLngToCell(...randCoords(), 9), Math.floor(Math.random() * 100)
).filter((e, i, a) => i === 0 || i === a.length - 1), true)
exportBenchmark('cellToParent', () => [
  h3node.latLngToCell(...randCoords(), 9),
  Math.floor(Math.random() * 9),
])
exportBenchmark('cellToChildren', () => [
  h3node.latLngToCell(...randCoords(), 9),
  Math.floor(15 - Math.random() * 6),
])
exportBenchmark('cellToCenterChild', () => [
  h3node.latLngToCell(...randCoords(), 9),
  Math.floor(15 - Math.random() * 6),
])
exportBenchmark('compactCells', () => [h3node.gridDisk(h3node.latLngToCell(...randCoords(), 9), 6)])
exportBenchmark('uncompactCells', () => [
  h3node.compactCells(h3node.gridDisk(h3node.latLngToCell(...randCoords(), 9), 6)),
  9,
])
exportBenchmark('polygonToCells', () => [
  [
    [37.77, -122.43],
    [37.55, -122.43],
    [37.55, -122.23],
    [37.77, -122.23],
    [37.77, -122.43],
  ],
  4,
])
exportBenchmark('polygonToCells', () => [
  [
    [
      [37.77, -122.43],
      [37.55, -122.43],
      [37.55, -122.23],
      [37.77, -122.23],
      [37.77, -122.43],
    ], [
      [37.67, -122.43],
      [37.55, -122.43],
      [37.55, -122.33],
      [37.67, -122.33],
      [37.67, -122.43],
    ],
  ],
  4,
], false, 'WithHoles')
exportBenchmark('cellsToMultiPolygon', () => [
  h3node.gridDisk(h3node.latLngToCell(...randCoords(), 9), 6),
])
exportBenchmark('cellsToMultiPolygon', () => [
  h3node.gridDisk(h3node.latLngToCell(...randCoords(), 9), 6),
  true,
], false, 'GeoJsonMode')
exportBenchmark('cellsToMultiPolygon', () => [
  [...new Set([
    ...h3node.gridDisk(h3node.latLngToCell(...randCoords(), 9), 6),
    ...h3node.gridDisk(h3node.latLngToCell(...randCoords(), 9), 3),
  ])]
], false, 'TrueMultiPolygon')
exportBenchmark('areNeighborCells', () =>
  randElements(h3node.gridDisk(h3node.latLngToCell(...randCoords(), 9), 2), 2))
exportBenchmark('cellsToDirectedEdge', () => {
  const randIndex = h3node.latLngToCell(...randCoords(), 9)
  return [randIndex, h3node.gridDisk(randIndex, 1).pop()]
})
exportBenchmark('isValidDirectedEdge', () => {
  const randIndex = h3node.latLngToCell(...randCoords(), 9)
  if (Math.random() > 0.5) {
    return [h3node.cellsToDirectedEdge(randIndex, h3node.gridDisk(randIndex, 1).pop())]
  } else {
    return [randIndex]
  }
})
exportBenchmark('getDirectedEdgeOrigin', () => {
  const randIndex = h3node.latLngToCell(...randCoords(), 9)
  return [h3node.cellsToDirectedEdge(randIndex, h3node.gridDisk(randIndex, 1).pop())]
})
exportBenchmark('getDirectedEdgeDestination', () => {
  const randIndex = h3node.latLngToCell(...randCoords(), 9)
  return [h3node.cellsToDirectedEdge(randIndex, h3node.gridDisk(randIndex, 1).pop())]
})
exportBenchmark('directedEdgeToCells', () => {
  const randIndex = h3node.latLngToCell(...randCoords(), 9)
  return [h3node.cellsToDirectedEdge(randIndex, h3node.gridDisk(randIndex, 1).pop())]
})
exportBenchmark('originToDirectedEdges', () =>
  [h3node.latLngToCell(...randCoords(), 9)], simpleTest)
exportBenchmark('directedEdgeToBoundary', () => {
  const randIndex = h3node.latLngToCell(...randCoords(), 9)
  return [h3node.cellsToDirectedEdge(randIndex, h3node.gridDisk(randIndex, 1).pop())]
})
exportBenchmark('degsToRads', () => [Math.floor(Math.random() * 360)])
exportBenchmark('radsToDegs', () => [Math.floor(Math.random() * 2 * Math.PI)])
exportBenchmark('getNumCells', () => [Math.floor(Math.random() * 16)])
exportBenchmark('getHexagonEdgeLengthAvg', () => [
  Math.floor(Math.random() * 16),
  Math.random() > 0.5 ? h3node.UNITS.m : h3node.UNITS.km,
])
exportBenchmark('edgeLength', () => {
  const randIndex = h3node.latLngToCell(...randCoords(), 9)
  return [
    h3node.cellsToDirectedEdge(randIndex, h3node.gridDisk(randIndex, 1).pop()),
    Math.random() < 0.34 ? h3node.UNITS.m : Math.random() > 0.5 ? h3node.UNITS.km : h3node.UNITS.rads
  ]
})
exportBenchmark('getHexagonAreaAvg', () => [
  Math.floor(Math.random() * 16),
  Math.random() > 0.5 ? h3node.UNITS.m2 : h3node.UNITS.km2,
])
exportBenchmark('cellArea', () => [
  h3node.latLngToCell(...randCoords(), 9),
  Math.random() < 0.34 ? h3node.UNITS.m2 : Math.random() > 0.5 ? h3node.UNITS.km2 : h3node.UNITS.rads2
])
exportBenchmark('greatCircleDistance', () => [
  randCoords(),
  randCoords(),
  Math.random() < 0.34 ? h3node.UNITS.m : Math.random() > 0.5 ? h3node.UNITS.km : h3node.UNITS.rads
])
exportBenchmark('getRes0Cells', () => [])
exportBenchmark('getPentagons', () => [Math.floor(Math.random() * 16)])

// Benchmarks for the functions added in H3 4.5.0
exportBenchmark('cellToChildPos', () => {
  return [h3node.latLngToCell(...randCoords(), 9), Math.floor(Math.random() * 9)]
})
exportBenchmark('childPosToCell', () => {
  const parent = h3node.latLngToCell(...randCoords(), 9)
  // h3-js (and the binding) take the child position before the parent
  return [Math.floor(Math.random() * h3node.cellToChildrenSize(parent, 10)), parent, 10]
})
exportBenchmark('cellToChildrenSize', () => [
  h3node.latLngToCell(...randCoords(), 9),
  Math.floor(10 + Math.random() * 6),
])
exportBenchmark('cellToVertex', () => {
  const cell = h3node.latLngToCell(...randCoords(), 9)
  return [cell, Math.floor(Math.random() * (h3node.isPentagon(cell) ? 5 : 6))]
})
exportBenchmark('cellToVertexes', () => [h3node.latLngToCell(...randCoords(), 9)])
exportBenchmark('vertexToLatLng', () => [
  h3node.cellToVertex(h3node.latLngToCell(...randCoords(), 9), 0),
])
exportBenchmark('isValidVertex', () => {
  const cell = h3node.latLngToCell(...randCoords(), 9)
  return [Math.random() > 0.5 ? h3node.cellToVertex(cell, 0) : 'zzzzzzzzzzzzzzzz']
})
exportBenchmark('constructCell', () => {
  const cell = h3node.latLngToCell(...randCoords(), 9)
  const digits = []
  for (let i = 1; i <= 9; i++) digits.push(h3node.getIndexDigit(cell, i))
  return [h3node.getBaseCellNumber(cell), digits]
})
exportBenchmark('getIndexDigit', () => {
  const res = Math.floor(2 + Math.random() * 14)
  const cell = h3node.latLngToCell(...randCoords(), res)
  return [cell, Math.floor(1 + Math.random() * res)]
})
exportBenchmark('gridRing', () => [
  h3node.latLngToCell(...randCoords(), Math.floor(Math.random() * 16)),
  Math.floor(Math.random() * 5 + 1),
], true)
exportBenchmark('isValidIndex', () => [
  Math.random() > 0.5 ? h3node.latLngToCell(...randCoords(), 9) : 'asdfjkl;'
])
exportBenchmark('polygonToCellsExperimental', () => [
  [
    [37.77, -122.43],
    [37.55, -122.43],
    [37.55, -122.23],
    [37.77, -122.23],
    [37.77, -122.43],
  ],
  4,
  'containmentFull',
])
exportBenchmark('reverseDirectedEdge', () => {
  const randIndex = h3node.latLngToCell(...randCoords(), 9)
  return [h3node.cellsToDirectedEdge(randIndex, h3node.gridDisk(randIndex, 1).filter(c => c !== randIndex)[0])]
})

/* console.log(h3node.polygonToCells(
  [
    [
      [37.77, -122.43],
      [37.55, -122.43],
      [37.55, -122.23],
      [37.77, -122.23],
      [37.77, -122.43],
    ], [
      [37.67, -122.43],
      [37.55, -122.43],
      [37.55, -122.33],
      [37.67, -122.33],
      [37.67, -122.43],
    ],
  ],
  6,
).map(h => h3node.cellToBoundary(h))
  .map(c => [...c.map(co => [co[1], co[0]])])
  .map(c2 => [...c2, c2[0]])
  .map(b => ({
    type: 'Feature',
    properties: {},
    geometry: {
      type: 'Polygon',
      coordinates: [b],
    },
  }))
  .map(g => JSON.stringify(g, undefined, '  ')).join(', ')) */
