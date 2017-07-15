const gkm = require('gkm')
const menubar = require('menubar')
const musec = require('microseconds')
const path = require('path')
const proc = require('child_process')

// Import custom libraries
const keyUtil = require('./util/key')
const userData = require('./models/user_data')
const serviceSuspicion = require('./services/suspicion')

// Initialize menubar app.
const app = menubar()

// Set initial render
const dir = app.getOption('dir')
app.setOption('index', 'file://' + dir + '/index.html')
app.app.commandLine.appendSwitch('js-flags', '--max-old-space-size=4096');

app.app.proc = proc
app.app.console = console

app.on('ready', () => {
    console.log('Loading Motionpass')
    app.showWindow()
    // app.window.openDevTools();
})

let lastMicrosec = 0

let lastEntry = null  // Array of data from last entry into the database.

const MIN_STORE_ENTRIES = 10

let char1 = null
let char2 = null
let travel = 0
let delay1 = 0
let delay2 = 0

let state = 0
// State 0: waiting for user input
// State 1: start of first keypress
// State 2: end of first keypress, calculate travel time
// State 3: start of second keypress, end of travel time

gkm.events.on('key.pressed', function (data) {
    data = keyUtil.getKeyCode(data)  // Format key text to key code.
    
    if (state == 0) {
        char2 = null
        delay1 = 0
        delay2 = 0
        travel = 0

        char1 = data
        lastMicrosec = musec.now()
        state = 1
        return

    } else if (state == 2) {
        const timelapse = (musec.since(lastMicrosec) / 1000).toString().split('.', 1)[0]
        travel = timelapse
        char2 = data
        lastMicrosec = musec.now()
        state = 3
        return

    }
})

const MIN_TIME_IN_MS = 1
const MAX_TIME_IN_MS = 2000

gkm.events.on('key.released', function (data) {
    data = keyUtil.getKeyCode(data)  // Format key text to key code.

    if (state == 1) {
        const timelapse = (musec.since(lastMicrosec) / 1000).toString().split('.', 1)[0]
        delay1 = timelapse
        lastMicrosec = musec.now()
        state = 2
        return

    } else {
        if (state == 3) {
            const timelapse = (musec.since(lastMicrosec) / 1000).toString().split('.', 1)[0]
            delay2 = timelapse
            
            // Keep track of last typed key.
            lastEntry = [char1, char2, parseInt(travel), parseInt(delay1), parseInt(delay2)]

            // Add entry to data store.
            if (!serviceSuspicion.isEntrySuspicious(lastEntry)) {
                travel = parseInt(travel)
                delay1 = parseInt(delay1)
                delay2 = parseInt(delay2)

                if (travel < MIN_TIME_IN_MS || travel > MAX_TIME_IN_MS ||
                    delay1 < MIN_TIME_IN_MS || delay1 > MAX_TIME_IN_MS ||
                    delay2 < MIN_TIME_IN_MS || delay2 > MAX_TIME_IN_MS) {
                    console.log('Travel time outside of allowed times.')
                    // Reset data
                    char1 = null
                    char2 = null
                    delay1 = 0
                    delay2 = 0
                    travel = 0
                    lastMicrosec = 0
                    state = 0
                    return
                }

                userData.addEntry(char1, char2, travel, delay1, delay2)
                userData.storeEntries++
            } else {
                console.log('Not adding entry. Suspicious.')
            }

            if (userData.storeEntries > MIN_STORE_ENTRIES && serviceSuspicion.isUserSuspicious()) {
                serviceSuspicion.logOut()
            }            
        }

        // Reset data
        char1 = null
        char2 = null
        delay1 = 0
        delay2 = 0
        travel = 0
        lastMicrosec = 0
        state = 0
        return
    } 
})