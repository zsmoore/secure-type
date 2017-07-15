const menubar = require('menubar')
const path = require('path')
const proc = require('child_process')

// Initializae menubar app.
const app = menubar()

// Set initial render
const dir = app.getOption('dir')
app.setOption('index', 'file://' + dir + '/index.html')

app.app.proc = proc

app.on('ready', () => {
    console.log('Loading Motionpass')
    app.showWindow()
    // app.window.openDevTools();
})

