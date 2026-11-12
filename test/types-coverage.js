const fs = require('fs')
const path = require('path')
const pkg = require('../package.json')
const h3node = require('..')

const typeDefinitionPath = path.join(__dirname, '..', pkg.types)
const typeDefinitions = fs.readFileSync(typeDefinitionPath, 'utf8')

const declaredExports = [...typeDefinitions.matchAll(/export declare (?:const|function) (\w+)/g)]
  .map(match => match[1])
  .sort()

exports['typeDefinitionsArePublished'] = test => {
  test.equal(pkg.types, 'index.d.ts')
  test.ok(pkg.files.includes(pkg.types), 'index.d.ts should be included in published files')
  test.done()
}

exports['typeDefinitionsCoverRuntimeExports'] = test => {
  test.deepEqual(declaredExports, Object.keys(h3node).sort())
  test.done()
}
