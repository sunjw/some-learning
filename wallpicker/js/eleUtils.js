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
            this.configJson = JSON.parse(configData);
        }
    }

    getConfig(key) {
        this.loadConfig();
        return this.configJson[key];
    }

    setConfig(key, value) {
        this.loadConfig();
        this.configJson[key] = value;
        fs.writeFileSync(this.configPath, JSON.stringify(this.configJson));
    }
}

exports.EleConfig = EleConfig;
