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

const almostEqualTest = (test, methodName, args) => {
  let almostEqual = true
  const node = h3node[methodName](...args)
  const js = h3js[methodName](...args)
  for (let j = 0; j < node.length; j++) {
    if (node[j] - js[j] > INFINITESIMAL) almostEqual = false
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

exports['h3IsValid_array'] = test => {
    test.ok(h3node.h3IsValid([0x3fffffff, 0x8528347]), 'Integer H3 index is considered an index');
    test.ok(
        !h3node.h3IsValid([0x73fffffff, 0xff2834]),
        'Integer with incorrect bits is not considered an index'
    );
    // TODO: This differs from h3-js, in these cases h3-js would return false rather than throwing
    test.throws(() => h3node.h3IsValid([]), 'Empty array is not valid');
    test.throws(() => h3node.h3IsValid([1]), 'Array with a single element is not valid');
    test.throws(() =>
        h3node.h3IsValid([0x3fffffff, 0x8528347, 0]),
        'Array with an additional element is not valid'
    );
    test.done();
};

exportTest('geoToH3', () => [...randCoords(), 9], simpleTest)
exportTest('h3ToGeo', () => [h3node.geoToH3(...randCoords(), 9)], almostEqualTest)
exportTest('h3ToGeoBoundary', () => [h3node.geoToH3(...randCoords(), 9)], almostEqualTest)
exportTest('h3GetResolution', () => [
  h3node.geoToH3(...randCoords(), Math.floor(Math.random() * 16))
], simpleTest)
exportTest('h3GetBaseCell', () => [h3node.geoToH3(...randCoords(), 9)], simpleTest)
exportTest('h3IsValid', () => [
  Math.random() > 0.5 ? h3node.geoToH3(...randCoords(), 9) : 'asdfjkl;'
], simpleTest)
exportTest('h3IsResClassIII', () => [
  h3node.geoToH3(...randCoords(), Math.floor(Math.random() * 16))
], simpleTest)
exportTest('h3IsPentagon', () => [
  h3node.geoToH3(...randCoords(), Math.floor(Math.random() * 16))
], simpleTest)
exportTest('kRing', () => [
  h3node.geoToH3(...randCoords(), Math.floor(Math.random() * 16)),
  Math.floor(Math.random() * 10 + 1),
], simpleTest)
exportTest('kRingDistances', () => [
  h3node.geoToH3(...randCoords(), Math.floor(Math.random() * 16)),
  Math.floor(Math.random() * 10 + 1),
], simpleTest)
exportTest('hexRing', () => [
  h3node.geoToH3(...randCoords(), Math.floor(Math.random() * 16)),
  Math.floor(Math.random() * 10 + 1),
], allowPentagonTest)
exportTest('h3Distance', () => h3node
  .kRing(h3node.geoToH3(...randCoords(), Math.floor(Math.random() * 16)), 5)
  .filter(function(h, i, a) {
    if (a.length - i === this.count) {
      this.count--
      return true
    }
    if (this.count === 0) return false
    return Math.random() < 0.5
  }, { count: 2 }), simpleTest)
exportTest('experimentalH3ToLocalIj', () => h3node
  .kRing(h3node.geoToH3(...randCoords(), 9), 5)
  .filter(function(h, i, a) {
    if (a.length - i === this.count) {
      this.count--
      return true
    }
    if (this.count === 0) return false
    return Math.random() < 0.5
  }, { count: 2 }), allowPentagonTest)
exportTest('experimentalLocalIjToH3', () => [
  h3node.geoToH3(...randCoords(), Math.floor(Math.random() * 16)),
  {
    i: Math.floor(Math.random() * 5 + 1),
    j: Math.floor(Math.random() * 5 + 1),
  },
], allowPentagonTest);
// Pending solution to H3 issue #184: https://github.com/uber/h3/issues/184
/* exportTest('h3Line', () => h3node.kRing(
  h3node.geoToH3(...randCoords(), 9), Math.floor(Math.random() * 100)
).filter((e, i, a) => i === 0 || i === a.length - 1), allowPentagonLineTest) */
exportTest('h3ToParent', () => [
  h3node.geoToH3(...randCoords(), 9),
  Math.floor(Math.random() * 9),
], simpleTest)
exportTest('h3ToChildren', () => [
  h3node.geoToH3(...randCoords(), 9),
  Math.floor(15 - Math.random() * 6),
], simpleTest)
exportTest('compact', () => [h3node.kRing(h3node.geoToH3(...randCoords(), 9), 6)], simpleTest)
exportTest('uncompact', () => [
  h3node.compact(h3node.kRing(h3node.geoToH3(...randCoords(), 9), 6)),
  9,
], simpleTest)
exportTest('polyfill', () => [
  [
    [37.77, -122.43],
    [37.55, -122.43],
    [37.55, -122.23],
    [37.77, -122.23],
    [37.77, -122.43],
  ],
  6,
], simpleArrTest)
exportTest('polyfill', () => [
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
exportTest('h3SetToMultiPolygon', () => [
  h3node.kRing(h3node.geoToH3(...randCoords(), 9), 6),
], almostEqualTest)
exportTest('h3SetToMultiPolygon', () => [
  h3node.kRing(h3node.geoToH3(...randCoords(), 9), 6),
  true,
], almostEqualTest, 'GeoJsonMode')
exportTest('h3SetToMultiPolygon', () => [
  [...new Set([
    ...h3node.kRing(h3node.geoToH3(...randCoords(), 9), 6),
    ...h3node.kRing(h3node.geoToH3(...randCoords(), 9), 3),
  ])]
], almostEqualTest, 'TrueMultiPolygon')
exportTest('h3IndexesAreNeighbors', () =>
  randElements(h3node.kRing(h3node.geoToH3(...randCoords(), 9), 2), 2),
  simpleTest)
exportTest('getH3UnidirectionalEdge', () => {
  const randIndex = h3node.geoToH3(...randCoords(), 9)
  return [randIndex, h3node.kRing(randIndex, 1).pop()]
}, simpleTest)
exportTest('h3UnidirectionalEdgeIsValid', () => {
  const randIndex = h3node.geoToH3(...randCoords(), 9)
  if (Math.random() > 0.5) {
    return [h3node.getH3UnidirectionalEdge(randIndex, h3node.kRing(randIndex, 1).pop())]
  } else {
    return [randIndex]
  }
}, simpleTest)
exportTest('getOriginH3IndexFromUnidirectionalEdge', () => {
  const randIndex = h3node.geoToH3(...randCoords(), 9)
  return [h3node.getH3UnidirectionalEdge(randIndex, h3node.kRing(randIndex, 1).pop())]
}, simpleTest)
exportTest('getDestinationH3IndexFromUnidirectionalEdge', () => {
  const randIndex = h3node.geoToH3(...randCoords(), 9)
  return [h3node.getH3UnidirectionalEdge(randIndex, h3node.kRing(randIndex, 1).pop())]
}, simpleTest)
exportTest('getH3IndexesFromUnidirectionalEdge', () => {
  const randIndex = h3node.geoToH3(...randCoords(), 9)
  return [h3node.getH3UnidirectionalEdge(randIndex, h3node.kRing(randIndex, 1).pop())]
}, simpleTest)
exportTest('getH3UnidirectionalEdgesFromHexagon', () =>
  [h3node.geoToH3(...randCoords(), 9)], simpleTest)
exportTest('getH3UnidirectionalEdgeBoundary', () => {
  const randIndex = h3node.geoToH3(...randCoords(), 9)
  return [h3node.getH3UnidirectionalEdge(randIndex, h3node.kRing(randIndex, 1).pop())]
}, almostEqualTest)
exportTest('degsToRads', () => [Math.floor(Math.random() * 360)], almostEqualTest)
exportTest('radsToDegs', () => [Math.floor(Math.random() * 2 * Math.PI)], almostEqualTest)
exportTest('numHexagons', () => [Math.floor(Math.random() * 16)], almostEqualTest)
exportTest('edgeLength', () => [
  Math.floor(Math.random() * 16),
  Math.random() > 0.5 ? h3node.UNITS.m : h3node.UNITS.km,
], almostEqualTest)
exportTest('hexArea', () => [
  Math.floor(Math.random() * 16),
  Math.random() > 0.5 ? h3node.UNITS.m2 : h3node.UNITS.km2,
], almostEqualTest)
exportTest('getRes0Indexes', () => [], simpleTest)

exportBenchmark('geoToH3', () => [...randCoords(), 9])
exportBenchmark('h3ToGeo', () => [h3node.geoToH3(...randCoords(), 9)])
exportBenchmark('h3ToGeoBoundary', () => [h3node.geoToH3(...randCoords(), 9)])
exportBenchmark('h3GetResolution', () => [
  h3node.geoToH3(...randCoords(), Math.floor(Math.random() * 16))
])
exportBenchmark('h3GetBaseCell', () => [
  h3node.geoToH3(...randCoords(), Math.floor(Math.random() * 16))
])
exportBenchmark('h3IsValid', () => [
  Math.random() > 0.5 ? h3node.geoToH3(...randCoords(), 9) : 'asdfjkl;'
])
exportBenchmark('h3IsResClassIII', () => [
  h3node.geoToH3(...randCoords(), Math.floor(Math.random() * 16))
])
exportBenchmark('h3IsPentagon', () => [
  h3node.geoToH3(...randCoords(), Math.floor(Math.random() * 16))
])
exportBenchmark('kRing', () => [
  h3node.geoToH3(...randCoords(), Math.floor(Math.random() * 16)),
  Math.floor(Math.random() * 10 + 1),
])
exportBenchmark('kRingDistances', () => [
  h3node.geoToH3(...randCoords(), Math.floor(Math.random() * 16)),
  Math.floor(Math.random() * 10 + 1),
])
exportBenchmark('hexRing', () => [
  h3node.geoToH3(...randCoords(), Math.floor(Math.random() * 16)),
  Math.floor(Math.random() * 10 + 1),
], true)
exportBenchmark('h3Distance', () => h3node
  .kRing(h3node.geoToH3(...randCoords(), Math.floor(Math.random() * 16)), 5)
  .filter(function(h, i, a) {
    if (a.length - i === this.count) {
      this.count--
      return true
    }
    if (this.count === 0) return false
    return Math.random() < 0.5
  }, { count: 2, }))
exportBenchmark('experimentalH3ToLocalIj', () => h3node
  .kRing(h3node.geoToH3(...randCoords(), 9), 5)
  .filter(function(h, i, a) {
    if (a.length - i === this.count) {
      this.count--
      return true
    }
    if (this.count === 0) return false
    return Math.random() < 0.5
  }, { count: 2, }))
exportBenchmark('experimentalLocalIjToH3', () => [
  h3node.geoToH3(...randCoords(), Math.floor(Math.random() * 16)),
  {
    i: Math.floor(Math.random() * 5 + 1),
    j: Math.floor(Math.random() * 5 + 1),
  },
], true);
exportBenchmark('h3Line', () => h3node.kRing(
  h3node.geoToH3(...randCoords(), 9), Math.floor(Math.random() * 100)
).filter((e, i, a) => i === 0 || i === a.length - 1), true)
exportBenchmark('h3ToParent', () => [
  h3node.geoToH3(...randCoords(), 9),
  Math.floor(Math.random() * 9),
])
exportBenchmark('h3ToChildren', () => [
  h3node.geoToH3(...randCoords(), 9),
  Math.floor(15 - Math.random() * 6),
])
exportBenchmark('compact', () => [h3node.kRing(h3node.geoToH3(...randCoords(), 9), 6)])
exportBenchmark('uncompact', () => [
  h3node.compact(h3node.kRing(h3node.geoToH3(...randCoords(), 9), 6)),
  9,
])
exportBenchmark('polyfill', () => [
  [
    [37.77, -122.43],
    [37.55, -122.43],
    [37.55, -122.23],
    [37.77, -122.23],
    [37.77, -122.43],
  ],
  4,
])
exportBenchmark('polyfill', () => [
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
exportBenchmark('h3SetToMultiPolygon', () => [
  h3node.kRing(h3node.geoToH3(...randCoords(), 9), 6),
])
exportBenchmark('h3SetToMultiPolygon', () => [
  h3node.kRing(h3node.geoToH3(...randCoords(), 9), 6),
  true,
], false, 'GeoJsonMode')
exportBenchmark('h3SetToMultiPolygon', () => [
  [...new Set([
    ...h3node.kRing(h3node.geoToH3(...randCoords(), 9), 6),
    ...h3node.kRing(h3node.geoToH3(...randCoords(), 9), 3),
  ])]
], false, 'TrueMultiPolygon')
exportBenchmark('h3IndexesAreNeighbors', () =>
  randElements(h3node.kRing(h3node.geoToH3(...randCoords(), 9), 2), 2))
exportBenchmark('getH3UnidirectionalEdge', () => {
  const randIndex = h3node.geoToH3(...randCoords(), 9)
  return [randIndex, h3node.kRing(randIndex, 1).pop()]
})
exportBenchmark('h3UnidirectionalEdgeIsValid', () => {
  const randIndex = h3node.geoToH3(...randCoords(), 9)
  if (Math.random() > 0.5) {
    return [h3node.getH3UnidirectionalEdge(randIndex, h3node.kRing(randIndex, 1).pop())]
  } else {
    return [randIndex]
  }
})
exportBenchmark('getOriginH3IndexFromUnidirectionalEdge', () => {
  const randIndex = h3node.geoToH3(...randCoords(), 9)
  return [h3node.getH3UnidirectionalEdge(randIndex, h3node.kRing(randIndex, 1).pop())]
})
exportBenchmark('getDestinationH3IndexFromUnidirectionalEdge', () => {
  const randIndex = h3node.geoToH3(...randCoords(), 9)
  return [h3node.getH3UnidirectionalEdge(randIndex, h3node.kRing(randIndex, 1).pop())]
})
exportBenchmark('getH3IndexesFromUnidirectionalEdge', () => {
  const randIndex = h3node.geoToH3(...randCoords(), 9)
  return [h3node.getH3UnidirectionalEdge(randIndex, h3node.kRing(randIndex, 1).pop())]
})
exportBenchmark('getH3UnidirectionalEdgesFromHexagon', () =>
  [h3node.geoToH3(...randCoords(), 9)], simpleTest)
exportBenchmark('getH3UnidirectionalEdgeBoundary', () => {
  const randIndex = h3node.geoToH3(...randCoords(), 9)
  return [h3node.getH3UnidirectionalEdge(randIndex, h3node.kRing(randIndex, 1).pop())]
})
exportBenchmark('degsToRads', () => [Math.floor(Math.random() * 360)])
exportBenchmark('radsToDegs', () => [Math.floor(Math.random() * 2 * Math.PI)])
exportBenchmark('numHexagons', () => [Math.floor(Math.random() * 16)])
exportBenchmark('edgeLength', () => [
  Math.floor(Math.random() * 16),
  Math.random() > 0.5 ? h3node.UNITS.m : h3node.UNITS.km,
])
exportBenchmark('hexArea', () => [
  Math.floor(Math.random() * 16),
  Math.random() > 0.5 ? h3node.UNITS.m2 : h3node.UNITS.km2,
])
exportBenchmark('getRes0Indexes', () => [])

/* console.log(h3node.polyfill(
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
).map(h => h3node.h3ToGeoBoundary(h))
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
