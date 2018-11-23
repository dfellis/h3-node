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
    test.ok(/.*[pP]entagon.*/.test(e.message)) // Let pentagon encounters through
  }
}

const benchmarkGen = (methodName, genArgs, useTryCatch = false) => test => {
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
  console.log(`${methodName} Benchmark:`)
  console.log('H3-js time in ns:   ', h3jsTime[0] * 1e9 + h3jsTime[1])
  console.log('H3-node time in ns: ', h3nodeTime[0] * 1e9 + h3nodeTime[1])
  test.done()
}

const randCoords = () => [360 * Math.random() - 180, 180 * Math.random() - 90]

const exportTest = (methodName, genArgs, testFn) =>
  exports[methodName] = testGen(methodName, genArgs, testFn)

const exportBenchmark = (methodName, genArgs, useTryCatch = false) =>
  exports[`${methodName}Benchmark`] = benchmarkGen(methodName, genArgs, useTryCatch)

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
exportTest('h3ToParent', () => [
  h3node.geoToH3(...randCoords(), 9),
  Math.floor(Math.random() * 9),
], simpleTest)

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
exportBenchmark('h3ToParent', () => [
  h3node.geoToH3(...randCoords(), 9),
  Math.floor(Math.random() * 9),
])

