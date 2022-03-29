function log(...logString) {
    console.log(...logString);
}

function clone(obj) {
    let copy;

    // Handle the 3 simple types, and null or undefined
    if (null == obj || 'object' != typeof obj) {
        return obj;
    }

    // Handle Buffer
    if (obj instanceof Buffer) {
        copy = Buffer.from(obj);
        return copy;
    }

    // Handle Date
    if (obj instanceof Date) {
        copy = new Date();
        copy.setTime(obj.getTime());
        return copy;
    }

    // Handle Array
    if (obj instanceof Array) {
        copy = [];
        for (let i = 0, len = obj.length; i < len; i++) {
            copy[i] = clone(obj[i]);
        }
        return copy;
    }

    // Handle Object
    if (obj instanceof Object) {
        copy = {};
        for (let attr in obj) {
            if (obj.hasOwnProperty(attr))
                copy[attr] = clone(obj[attr]);
        }
        return copy;
    }

    throw new Error('Unable to copy obj! Its type is not supported.');
}

function variableExists(variable) {
    return !(variable == undefined || variable == null);
}

function isString(variable) {
    return (typeof variable === 'string' || variable instanceof String);
}

function isObject(variable) {
    return (typeof variable === 'object' && variable !== null);
}

function getRandomInt(max) {
    return Math.floor(Math.random() * Math.floor(max));
}

function capitalizeFirstLetter(string) {
    return string.charAt(0).toUpperCase() + string.slice(1);
}

function stringReplaceAll(string, target, replace) {
    return string.replace(new RegExp(target, 'g'), replace);
}

function escapeShellPath(path) {
    const needEscape = [' ', '&', '*', '$', '?', '|', '"', '\'',
        ';', '<', '>', '#', '(', ')'];
    let pathEscape = path;
    for (let escape of needEscape) {
        let escapeReg = escape;
        if (escapeReg == '*' || escapeReg == '$' ||
            escapeReg == '?' || escapeReg == '|' ||
            escapeReg == '(' || escapeReg == ')') {
            escapeReg = '\\' + escapeReg;
        }
        pathEscape = stringReplaceAll(pathEscape, escapeReg, '\\' + escape);
    }
    return pathEscape;
}

function escapeHtml(string) {
    let htmlEscape = string;
    htmlEscape = stringReplaceAll(htmlEscape, '&', '&amp;');
    htmlEscape = stringReplaceAll(htmlEscape, '<', '&lt;');
    htmlEscape = stringReplaceAll(htmlEscape, '>', '&gt;');
    htmlEscape = stringReplaceAll(htmlEscape, ' ', '&nbsp;');
    return htmlEscape;
}

function byteSizeToShortSize(size) {
    const shortSizeUnit = ['K', 'M', 'G'];
    let shortSize = size;
    for (let i = 0; i < shortSizeUnit.length; ++i) {
        if (size >= 1024) {
            shortSize = (size / 1024.0).toFixed(2) + shortSizeUnit[i];
            size = size / 1024;
        } else {
            break;
        }
    }
    return shortSize;
}

function getParentDir(path) {
    let curPath = path;
    if (curPath.endsWith('/')) {
        curPath = curPath.substr(0, curPath.length - 1);
    }
    let pathDelimIdx = curPath.lastIndexOf('/');
    let parentDirPath = curPath.substr(0, pathDelimIdx);
    parentDirPath = parentDirPath + '/';
    return parentDirPath;
}

function getFileName(fullPath) {
    return fullPath.replace(/^.*[\\\/]/, '');
}

function getUrlQueryVariable(queryStr, key) {
    let vars = queryStr.split('&');
    for (let i = 0; i < vars.length; i++) {
        let pair = vars[i].split('=');
        if (decodeURIComponent(pair[0]) == key) {
            return decodeURIComponent(pair[1]);
        }
    }
    return '';
}

function getQueryVariable(key) {
    let query = window.location.search.substring(1);
    return getUrlQueryVariable(query, key);
}

function getLocation() {
    return window.location.href;
}

function navToLocation(url) {
    window.location.href = url;
}

function navToHash(hash) {
    window.location.hash = hash;
}

function isWindows() {
    return (process.platform == 'win32');
}

function isLinux() {
    return (process.platform == 'linux');
}

function isMacOS() {
    return (process.platform == 'darwin');
}

function fixWindowsPath(path) {
    if (isWindows()) {
        path = stringReplaceAll(path, '\/', '\\');
    }
    return path;
}

function expireKey(obj, key, ms, expiredCallback = 0) {
    setTimeout(() => {
        delete obj[key];
        if (expiredCallback != 0) {
            expiredCallback(obj, key);
        }
    }, ms);
}

function getCurrentMills() {
    return Date.now();
}

function getCurrentSeconds() {
    let timestamp = Math.floor(getCurrentMills() / 1000);
    return timestamp;
}

// exports
exports.log = log;
exports.clone = clone;
exports.variableExists = variableExists;
exports.isString = isString;
exports.isObject = isObject;
exports.getRandomInt = getRandomInt;
exports.capitalizeFirstLetter = capitalizeFirstLetter;
exports.stringReplaceAll = stringReplaceAll;
exports.escapeShellPath = escapeShellPath;
exports.escapeHtml = escapeHtml;
exports.byteSizeToShortSize = byteSizeToShortSize;
exports.getParentDir = getParentDir;
exports.getFileName = getFileName;
exports.getUrlQueryVariable = getUrlQueryVariable;
exports.getQueryVariable = getQueryVariable;
exports.getLocation = getLocation;
exports.navToLocation = navToLocation;
exports.navToHash = navToHash;
exports.isWindows = isWindows;
exports.isLinux = isLinux;
exports.isMacOS = isMacOS;
exports.fixWindowsPath = fixWindowsPath;
exports.expireKey = expireKey;
exports.getCurrentMills = getCurrentMills;
exports.getCurrentSeconds = getCurrentSeconds;
