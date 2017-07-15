const ffi = require('ffi')

// Call C library
const DataTranslationLib = ffi.Library(__dirname + '/dataTranslation', {
  'setUp': [ 'void', [ 'string', ] ],
  'setUp2': [ 'void', [ 'string', ] ],
  'begin': [ 'bool', [ 'string', ] ],
})

module.exports = {
    readData: DataTranslationLib.readData,
}

