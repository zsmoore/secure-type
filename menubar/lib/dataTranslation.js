const ffi = require('ffi')

// Call C library
const DataTranslationLib = ffi.Library(__dirname + '/dataTranslation', {
  'readData': [ 'pointer', [ 'string', ] ]
})

module.exports = {
    readData: DataTranslationLib.readData,
}

