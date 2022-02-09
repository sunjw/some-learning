const {
    ipcRenderer
} = require('electron');

const crypto = require('crypto');
const fs = require('fs');
const path = require('path');

class EleConfig {

    constructor(userDataPath) {
        this.configPath = path.join(userDataPath, 'ele-config.json');
        this.configJson = {};
    }

    loadConfig() {
        if (fs.existsSync(this.configPath)) {
            let configData = fs.readFileSync(this.configPath);
            try {
                this.configJson = JSON.parse(configData);
            } catch (e) {}
        }
    }

    getConfig(key) {
        this.loadConfig();
        return this.configJson[key];
    }

    setConfig(key, value) {
        this.loadConfig();
        this.configJson[key] = value;
        fs.writeFileSync(this.configPath, JSON.stringify(this.configJson, null, 2));
    }
}

function sendToMain(message) {
    ipcRenderer.send('dialog', message);
}

function stopBubble(event) {
    // only for electron
    event.stopPropagation();
}

function stringMd5(str) {
    let md5 = crypto.createHash('md5');
    md5.update(str);
    return md5.digest('hex');
}

exports.EleConfig = EleConfig;
exports.sendToMain = sendToMain;
exports.stopBubble = stopBubble;
exports.stringMd5 = stringMd5;
