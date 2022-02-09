const fs = require('fs');
const path = require('path');

const sharp = require('sharp');
const utils = require('./utils');

function generateImageThumbnail(options) {
    // utils.log('generateImageThumbnail, options=\n%s', utils.objToJsonBeautify(options));

    let imageSrcPath = options.imageSrcPath;
    let imageThumbPath = options.imageThumbPath;
    let imageThumbWidth = options.imageThumbWidth;
    let imageThumbHeight = options.imageThumbHeight;
    let imageThumbFormat = options.imageThumbFormat;

    let sharpImage = sharp(imageSrcPath).resize({
        width: imageThumbWidth,
        height: imageThumbHeight,
        fit: 'fill'
    });
    switch (imageThumbFormat) {
    case 'jpg':
        sharpImage.jpeg();
        break;
    case 'png':
        sharpImage.png();
        break;
    }
    sharpImage.toFile(imageThumbPath, function (err, info) {
        if (err) {
            // utils.log('generateImageThumbnail, error=[%s]', err);
            postMessage({
                'imageSrcPath': imageSrcPath,
                'err': err
            });
            return;
        }

        // utils.log('generateImageThumbnail, thumbnail generated, imageThumbPath=[%s]', imageThumbPath);
        postMessage({
            'imageSrcPath': imageSrcPath,
            'imageThumbPath': imageThumbPath
        });
    });
}

onmessage = function (e) {
    // utils.log('imageWorker.onmessage, [%s]', e.data);
    // postMessage('Worker!');
    generateImageThumbnail(e.data);
};
