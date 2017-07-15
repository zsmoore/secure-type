const userData = require('../models/user_data')
const proc = require('child_process')

const SUSPICION_EXPONENT_CAP = 8
const SUSPICION_CAP = 3
const SUSPICION_THRESHOLD = 75
const SUSPICION_EXP_BASE = 3
const SUSPICION_TURN_DEPRECATE = 0.95
const ENTRY_SENSITIVITY = 2
const MIN_STORE_ENTRIES_IN_SLOT = 20

function isUserSuspicious() {
    console.log('SUS SCORE: ' + userData.suspicionScore)
    return userData.suspicionScore > SUSPICION_THRESHOLD
}

function isEntrySuspicious(lastEntry) {
    const char1 = lastEntry[0]
    const char2 = lastEntry[1]
    const newest_travel = lastEntry[2]
    const newest_delay1 = lastEntry[3]
    const newest_delay2 = lastEntry[4]

    let entries = userData.store[char1 + ':' + char2]
    if (!entries) {
        userData.store[char1 + ':' + char2] = []
        entries = userData.store[char1 + ':' + char2]
    }

    let avg_travel = 0, avg_delay1 = 0, avg_delay2 = 0
    
    const size = entries.length
    if (size < 2) return false

    // Calculate average
    for (let i = 0; i < size; i++) {
        let entry = entries[i]
        let travel = entry[0]
        let delay1 = entry[1]
        let delay2 = entry[2]

        avg_travel = avg_travel + travel
        avg_delay1 = avg_delay1 + delay1
        avg_delay2 = avg_delay2 + delay2
    }

    avg_travel = avg_travel / size
    avg_delay1 = avg_delay1 / size
    avg_delay2 = avg_delay2 / size

    // Standard deviation
    let sd_travel = 0, sd_delay1 = 0, sd_delay2 = 0

    for (let i = 0; i < size; i++) {
        let entry = entries[i]
        let travel = entry[0]
        let delay1 = entry[1]
        let delay2 = entry[2]

        sd_travel = sd_travel + Math.pow(avg_travel - entry[0], 2)
        sd_delay1 = sd_delay1 + Math.pow(avg_delay1 - entry[1], 2)
        sd_delay2 = sd_delay2 + Math.pow(avg_delay2 - entry[2], 2)
    }
    
    sd_travel = Math.sqrt(sd_travel / size)
    sd_delay1 = Math.sqrt(sd_delay1 / size)
    sd_delay2 = Math.sqrt(sd_delay2 / size)

    console.log('SD: ' + sd_travel + ' ' + sd_delay1 + ' ' + sd_delay2)

    // Calculate suspiciousness
    let sus_travel = 0, sus_delay1 = 0, sus_delay2 = 0

    sus_travel = Math.abs(newest_travel - avg_travel) / sd_travel
    sus_delay1 = Math.abs(newest_delay1 - avg_delay1) / sd_delay1
    sus_delay2 = Math.abs(newest_delay2 - avg_delay2) / sd_delay2

    let suspicion = 0
    if (sus_travel > SUSPICION_CAP) {
        if (sus_travel > SUSPICION_EXPONENT_CAP){
            sus_travel = SUSPICION_EXPONENT_CAP
        }
        suspicion++
    }

    if (sus_delay1 > SUSPICION_CAP) {
        if (sus_delay1 > SUSPICION_EXPONENT_CAP){
            sus_delay1 = SUSPICION_EXPONENT_CAP
        }
        suspicion = suspicion + 2
    }

    if (sus_delay2 > SUSPICION_CAP) {
        if (sus_delay2 > SUSPICION_EXPONENT_CAP){
            sus_delay2 = SUSPICION_EXPONENT_CAP
        }
        suspicion = suspicion + 2
    }
    // console.log('Sus in service' + userData.suspicionScore)
    // console.log(sus_delay2)
    // console.log(sus_delay1)
    userData.suspicionScore = userData.suspicionScore * SUSPICION_TURN_DEPRECATE + 
                            //   sus_travel + sus_delay1 +
                            //   sus_travel + sus_delay1 + sus_delay2
                              Math.pow(SUSPICION_EXP_BASE, sus_travel) +
                              Math.pow(SUSPICION_EXP_BASE, sus_delay1) + 
                              Math.pow(SUSPICION_EXP_BASE, sus_delay2)

    console.log('Suspicion ' + suspicion)

    // if (size < MIN_STORE_ENTRIES_IN_SLOT) return false

    return suspicion > ENTRY_SENSITIVITY
}

function logOut() {
    if (process.platform !== 'darwin') {
        console.log('Platform unsupported.')
        return
    }
    
    console.log('Logged out.')
    // userData.resetData()
    proc.exec('"/System/Library/CoreServices/Menu Extras/User.menu/Contents/Resources/CGSession\" -suspend')
}

module.exports = {
    isUserSuspicious: isUserSuspicious,
    isEntrySuspicious: isEntrySuspicious,
    logOut: logOut,
}