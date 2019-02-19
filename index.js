module.exports = {
  ...require('node-gyp-build')(__dirname),
  UNITS: {
    m: 'm',
    km: 'km',
    m2: 'm2',
    km2: 'km2',
  },
}
