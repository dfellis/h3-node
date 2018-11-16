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
