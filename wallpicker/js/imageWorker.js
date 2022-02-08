const fs = require('fs');
const path = require('path');

const sharp = require('sharp');

const utils = require('./utils');

onmessage = function (e) {
    utils.log('imageWorker.onmessage, [%s]', e.data);
    postMessage('Worker!');
};
