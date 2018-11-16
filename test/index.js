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
