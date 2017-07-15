const remote = require('electron').remote
const proc = remote.app.proc

function logOut(event) {
    if (process.platform !== 'darwin') {
        console.log('Platform unsupported.')
        return
    }
    
    console.log('Logged out.')
    proc.exec('"/System/Library/CoreServices/Menu Extras/User.menu/Contents/Resources/CGSession\" -suspend')
}

document.querySelector('#btn-log-out').addEventListener('click', logOut)
