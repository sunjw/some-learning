const crypto = require('crypto');
const fs = require('fs');

const sharp = require('sharp');
const { parse: parseIcc } = require('icc');
const probeImageSize = require('probe-image-size');

function hashMd5(strOrBuffer) {
    let md5 = crypto.createHash('md5');
    md5.update(strOrBuffer);
    return md5.digest('hex');
}

// Read full image metadata using sharp.
// Returns { width, height, iccProfileName } or null on failure.
async function readImageMeta(filePath) {
    try {
        let metadata = await sharp(filePath).metadata();
        if (metadata && metadata.width && metadata.height) {
            let iccProfileName = '';
            if (metadata.icc) {
                let profile = parseIcc(metadata.icc);
                if (profile.description) {
                    iccProfileName = profile.description;
                }
            }
            return {
                width: metadata.width,
                height: metadata.height,
                iccProfileName: iccProfileName
            };
        }
    } catch (err) {
        // swallow
    }
    return null;
}

// Read image size only using probe-image-size.
// Returns { width, height } or null on failure.
function readImageSizeSlow(filePath) {
    try {
        let imgData = fs.readFileSync(filePath);
        let imgDim = probeImageSize.sync(imgData);
        if (imgDim && imgDim.width && imgDim.height) {
            return {
                width: imgDim.width,
                height: imgDim.height
            };
        }
    } catch (err) {
        // swallow
    }
    return null;
}

exports.hashMd5 = hashMd5;
exports.readImageMeta = readImageMeta;
exports.readImageSizeSlow = readImageSizeSlow;
