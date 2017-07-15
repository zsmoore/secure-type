const key_util = require('./util/key')
const gkm = require('gkm')
const menubar = require('menubar')
const musec = require('microseconds')
const path = require('path')
const proc = require('child_process')

// Initialize menubar app.
const app = menubar()

// Set initial render
const dir = app.getOption('dir')
app.setOption('index', 'file://' + dir + '/index.html')

app.app.proc = proc
app.app.console = console

app.on('ready', () => {
    console.log('Loading Motionpass')
    app.showWindow()
    // app.window.openDevTools();
})

let polling = []
let lastMicrosec = 0
let prevKey = null
// let entries = 0
// let total = 0

function sendPolling() {
    // Send data to pipeline.
    console.log(polling)
    console.log('AVERAGE: ' + (total / entries))
    // Clear polling once data is sent to pipeline.
    polling = []
    prevKey = null
    lastMicrosec = 0
    // entries = 0
    // total = 0
}

// Send polling every 1 second.
setInterval(sendPolling, 1000 * 1)

gkm.events.on('key.pressed', function(data) {
    data = key_util.getKeyCode(data)
    if (prevKey == null) return
    if (data != null) {
        const timelapse = (musec.since(lastMicrosec) / 1000).toString().split('.', 1)[0]
        polling.push(prevKey + ',' + data + ',' + timelapse)
        // total = total + parseInt(timelapse)
        // entries = entries + 1
    }

    prevKey = null
    lastMicrosec = 0
})

gkm.events.on('key.released', function(data) {
    // Convert label to key code
    data = key_util.getKeyCode(data)
    
    // If the key is a tracked key, store it until the next interaction is added.
    if (data != null) {
        prevKey = data
        lastMicrosec = musec.now()
    }
})

