// 2d array of 2 characters
// stores [char1, char2, travel time, delay first, delay second]

let store = {}
let storeEntries = 0
let suspicionScore = 0

const STALE_COUNT = 30

function addEntry(char1, char2, travel, delay1, delay2) {
    let storage = store[char1 + ':' + char2]
    
    if (storage && storage.length > STALE_COUNT) storage.pop()
    if (!storage) storage = []

    storage.push([travel, delay1, delay2])
    // console.log([travel, delay1, delay2])
    console.log(char1 + ' ' +  char2 + ' ' + storage)
}

function resetData() {
    console.log('Reset data.')

    store = {}
    storeEntries = 0
    suspicionScore = 0
}

module.exports = {
    store: store,
    storeEntries: storeEntries,
    addEntry: addEntry,
    resetData: resetData,
    suspicionScore: suspicionScore,
}