const h3js = require('h3-js')
const h3node = require('..')

exports.geoToH3 = test => {
  test.expect(10)
  for (let i = 0; i < 10; i++) {
    const lat = 360 * Math.random() - 180
    const lng = 180 * Math.random() - 90
    test.equal(h3node.geoToH3(lat, lng, 9), h3js.geoToH3(lat, lng, 9))
  }
  test.done()
}

exports.h3ToGeo = test => {
  test.expect(20)
  for (let i = 0; i < 10; i++) {
    const lat = 360 * Math.random() - 180
    const lng = 180 * Math.random() - 90
    const node = h3node.h3ToGeo(h3node.geoToH3(lat, lng, 9))
    const js = h3js.h3ToGeo(h3js.geoToH3(lat, lng, 9))
    test.ok(Math.abs(node[0] - js[0]) < 0.0000001)
    test.ok(Math.abs(node[1] - js[1]) < 0.0000001)
  }
  test.done()
}

exports.h3ToGeoBoundary = test => {
  test.expect(120)
  for (let i = 0; i < 10; i++) {
    const lat = 360 * Math.random() - 180
    const lng = 180 * Math.random() - 90
    const node = h3node.h3ToGeoBoundary(h3node.geoToH3(lat, lng, 9))
    const js = h3js.h3ToGeoBoundary(h3js.geoToH3(lat, lng, 9))
    for (let j = 0; j < node.length; j++) {
      test.ok(Math.abs(node[j][0] - js[j][0]) < 0.0000001)
      test.ok(Math.abs(node[j][1] - js[j][1]) < 0.0000001)
    }
  }
  test.done()
}

exports.h3GetResolution = test => {
  test.expect(10);
  const h3Indices = []
  for (let i = 0; i < 10; i++) {
    const lat = 360 * Math.random() - 180
    const lng = 180 * Math.random() - 90
    const res = Math.floor(Math.random() * 16)
    h3Indices.push(h3node.geoToH3(lat, lng, res))
  }
  for (let i = 0; i < 10; i++) {
    test.equal(h3node.h3GetResolution(h3Indices[i]), h3js.h3GetResolution(h3Indices[i]));
  }
  test.done()
}

exports.h3GetBaseCell = test => {
  test.expect(10);
  const h3Indices = []
  for (let i = 0; i < 10; i++) {
    const lat = 360 * Math.random() - 180
    const lng = 180 * Math.random() - 90
    const res = Math.floor(Math.random() * 16)
    h3Indices.push(h3node.geoToH3(lat, lng, res))
  }
  for (let i = 0; i < 10; i++) {
    test.equal(h3node.h3GetBaseCell(h3Indices[i]), h3js.h3GetBaseCell(h3Indices[i]));
  }
  test.done()
}

exports.h3IsValid = test => {
  test.expect(10);
  const h3Indices = []
  for (let i = 0; i < 10; i++) {
    const lat = 360 * Math.random() - 180
    const lng = 180 * Math.random() - 90
    const res = Math.floor(Math.random() * 16)
    if (Math.random() > 0.5) {
      h3Indices.push(h3node.geoToH3(lat, lng, res))
    } else {
      h3Indices.push('asdfjkl;')
    }
  }
  for (let i = 0; i < 10; i++) {
    test.equal(h3node.h3IsValid(h3Indices[i]), h3js.h3IsValid(h3Indices[i]));
  }
  test.done()
}

exports.h3IsResClassIII = test => {
  test.expect(10);
  const h3Indices = []
  for (let i = 0; i < 10; i++) {
    const lat = 360 * Math.random() - 180
    const lng = 180 * Math.random() - 90
    const res = Math.floor(Math.random() * 16)
    h3Indices.push(h3node.geoToH3(lat, lng, res))
  }
  for (let i = 0; i < 10; i++) {
    test.equal(h3node.h3IsResClassIII(h3Indices[i]), h3js.h3IsResClassIII(h3Indices[i]));
  }
  test.done()
}

exports.h3IsPentagon = test => {
  test.expect(10);
  const h3Indices = []
  for (let i = 0; i < 10; i++) {
    const lat = 360 * Math.random() - 180
    const lng = 180 * Math.random() - 90
    const res = Math.floor(Math.random() * 16)
    h3Indices.push(h3node.geoToH3(lat, lng, res))
  }
  for (let i = 0; i < 10; i++) {
    test.equal(h3node.h3IsPentagon(h3Indices[i]), h3js.h3IsPentagon(h3Indices[i]));
  }
  test.done()
}

exports.geoToH3Benchmark = test => {
  const start = process.hrtime()
  for (let i = 0; i < 1000; i++) {
    const lat = 360 * Math.random() - 180
    const lng = 180 * Math.random() - 90
    h3js.geoToH3(lat, lng, 9)
  }
  const h3jsTime = process.hrtime(start)
  const middle = process.hrtime()
  for (let i = 0; i < 1000; i++) {
    const lat = 360 * Math.random() - 180
    const lng = 180 * Math.random() - 90
    h3node.geoToH3(lat, lng, 9)
  }
  const h3nodeTime = process.hrtime(middle)

  console.log('')
  console.log('geoToH3 Benchmark:')
  console.log('H3-js time in ns:   ', h3jsTime[0] * 1e9 + h3jsTime[1])
  console.log('H3-node time in ns: ', h3nodeTime[0] * 1e9 + h3nodeTime[1])
  test.done()
}

exports.h3ToGeoBenchmark = test => {
  const h3Indices = []
  for (let i = 0; i < 1000; i++) {
    const lat = 360 * Math.random() - 180
    const lng = 180 * Math.random() - 90
    h3Indices.push(h3node.geoToH3(lat, lng, 9))
  }
  const start = process.hrtime()
  for (let i = 0; i < 1000; i++) {
    h3js.h3ToGeo(h3Indices[i])
  }
  const h3jsTime = process.hrtime(start)
  const middle = process.hrtime()
  for (let i = 0; i < 1000; i++) {
    h3node.h3ToGeo(h3Indices[i])
  }
  const h3nodeTime = process.hrtime(middle)

  console.log('')
  console.log('h3ToGeo Benchmark:')
  console.log('H3-js time in ns:   ', h3jsTime[0] * 1e9 + h3jsTime[1])
  console.log('H3-node time in ns: ', h3nodeTime[0] * 1e9 + h3nodeTime[1])
  test.done()
}

exports.h3ToGeoBoundaryBenchmark = test => {
  const h3Indices = []
  for (let i = 0; i < 1000; i++) {
    const lat = 360 * Math.random() - 180
    const lng = 180 * Math.random() - 90
    h3Indices.push(h3node.geoToH3(lat, lng, 9))
  }
  const start = process.hrtime()
  for (let i = 0; i < 1000; i++) {
    h3js.h3ToGeoBoundary(h3Indices[i])
  }
  const h3jsTime = process.hrtime(start)
  const middle = process.hrtime()
  for (let i = 0; i < 1000; i++) {
    h3node.h3ToGeoBoundary(h3Indices[i])
  }
  const h3nodeTime = process.hrtime(middle)

  console.log('')
  console.log('h3ToGeoBoundary Benchmark:')
  console.log('H3-js time in ns:   ', h3jsTime[0] * 1e9 + h3jsTime[1])
  console.log('H3-node time in ns: ', h3nodeTime[0] * 1e9 + h3nodeTime[1])
  test.done()
}

exports.h3GetResolutionBenchmark = test => {
  const h3Indices = []
  for (let i = 0; i < 1000; i++) {
    const lat = 360 * Math.random() - 180
    const lng = 180 * Math.random() - 90
    const res = Math.floor(Math.random() * 16)
    h3Indices.push(h3node.geoToH3(lat, lng, res))
  }
  const start = process.hrtime()
  for (let i = 0; i < 1000; i++) {
    h3js.h3GetResolution(h3Indices[i])
  }
  const h3jsTime = process.hrtime(start)
  const middle = process.hrtime()
  for (let i = 0; i < 1000; i++) {
    h3node.h3GetResolution(h3Indices[i])
  }
  const h3nodeTime = process.hrtime(middle)

  console.log('')
  console.log('h3GetResolution Benchmark:')
  console.log('H3-js time in ns:   ', h3jsTime[0] * 1e9 + h3jsTime[1])
  console.log('H3-node time in ns: ', h3nodeTime[0] * 1e9 + h3nodeTime[1])
  test.done()
}

exports.h3GetBaseCellBenchmark = test => {
  const h3Indices = []
  for (let i = 0; i < 1000; i++) {
    const lat = 360 * Math.random() - 180
    const lng = 180 * Math.random() - 90
    const res = Math.floor(Math.random() * 16)
    h3Indices.push(h3node.geoToH3(lat, lng, res))
  }
  const start = process.hrtime()
  for (let i = 0; i < 1000; i++) {
    h3js.h3GetBaseCell(h3Indices[i])
  }
  const h3jsTime = process.hrtime(start)
  const middle = process.hrtime()
  for (let i = 0; i < 1000; i++) {
    h3node.h3GetBaseCell(h3Indices[i])
  }
  const h3nodeTime = process.hrtime(middle)

  console.log('')
  console.log('h3GetBaseCell Benchmark:')
  console.log('H3-js time in ns:   ', h3jsTime[0] * 1e9 + h3jsTime[1])
  console.log('H3-node time in ns: ', h3nodeTime[0] * 1e9 + h3nodeTime[1])
  test.done()
}

exports.h3IsValidBenchmark = test => {
  const h3Indices = []
  for (let i = 0; i < 1000; i++) {
    const lat = 360 * Math.random() - 180
    const lng = 180 * Math.random() - 90
    const res = Math.floor(Math.random() * 16)
    if (Math.random() > 0.5) {
      h3Indices.push(h3node.geoToH3(lat, lng, res))
    } else {
      h3Indices.push('asdfjkl;')
    }
  }
  const start = process.hrtime()
  for (let i = 0; i < 1000; i++) {
    h3js.h3IsValid(h3Indices[i])
  }
  const h3jsTime = process.hrtime(start)
  const middle = process.hrtime()
  for (let i = 0; i < 1000; i++) {
    h3node.h3IsValid(h3Indices[i])
  }
  const h3nodeTime = process.hrtime(middle)

  console.log('')
  console.log('h3IsValid Benchmark:')
  console.log('H3-js time in ns:   ', h3jsTime[0] * 1e9 + h3jsTime[1])
  console.log('H3-node time in ns: ', h3nodeTime[0] * 1e9 + h3nodeTime[1])
  test.done()
}

exports.h3IsResClassIIIBenchmark = test => {
  const h3Indices = []
  for (let i = 0; i < 1000; i++) {
    const lat = 360 * Math.random() - 180
    const lng = 180 * Math.random() - 90
    const res = Math.floor(Math.random() * 16)
    h3Indices.push(h3node.geoToH3(lat, lng, res))
  }
  const start = process.hrtime()
  for (let i = 0; i < 1000; i++) {
    h3js.h3IsResClassIII(h3Indices[i])
  }
  const h3jsTime = process.hrtime(start)
  const middle = process.hrtime()
  for (let i = 0; i < 1000; i++) {
    h3node.h3IsResClassIII(h3Indices[i])
  }
  const h3nodeTime = process.hrtime(middle)

  console.log('')
  console.log('h3IsResClassIII Benchmark:')
  console.log('H3-js time in ns:   ', h3jsTime[0] * 1e9 + h3jsTime[1])
  console.log('H3-node time in ns: ', h3nodeTime[0] * 1e9 + h3nodeTime[1])
  test.done()
}

exports.h3IsPentagonBenchmark = test => {
  const h3Indices = []
  for (let i = 0; i < 1000; i++) {
    const lat = 360 * Math.random() - 180
    const lng = 180 * Math.random() - 90
    const res = Math.floor(Math.random() * 16)
    h3Indices.push(h3node.geoToH3(lat, lng, res))
  }
  const start = process.hrtime()
  for (let i = 0; i < 1000; i++) {
    h3js.h3IsPentagon(h3Indices[i])
  }
  const h3jsTime = process.hrtime(start)
  const middle = process.hrtime()
  for (let i = 0; i < 1000; i++) {
    h3node.h3IsPentagon(h3Indices[i])
  }
  const h3nodeTime = process.hrtime(middle)

  console.log('')
  console.log('h3IsPentagon Benchmark:')
  console.log('H3-js time in ns:   ', h3jsTime[0] * 1e9 + h3jsTime[1])
  console.log('H3-node time in ns: ', h3nodeTime[0] * 1e9 + h3nodeTime[1])
  test.done()
}
