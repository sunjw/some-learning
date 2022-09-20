const fs = require('fs');
const path = require('path');

const probeImageSize = require('probe-image-size');
const sharp = require('sharp');

const utils = require('./utils');
const nodeUtils = require('./nodeUtils');
const sqliteDb = require('./sqliteDb');

const DB_CREATE_THUMBNAIL_TABLE = 'CREATE TABLE IF NOT EXISTS "thumbnail" (' +
    '"id" INTEGER PRIMARY KEY AUTOINCREMENT,' +
    '"path" varchar(1024) NOT NULL,' +
    '"last_used" INTEGER NOT NULL' +
    ');';
const DB_SELECT_THUMBNAIL_BY_PATH = 'SELECT * FROM thumbnail WHERE path = ?';
const DB_INSERT_THUMBNAIL = 'INSERT INTO thumbnail (path, last_used) VALUES (?, ?)';
const DB_UPDATE_THUMBNAIL_LAST_USED_BY_ID = 'UPDATE thumbnail SET last_used = ? WHERE id = ?';
const DB_DELETE_THUMBNAIL_BY_LAST_USED_BEFORE = 'DELETE FROM thumbnail WHERE last_used < ?';

let imageThumbDb = null;
let scanImageOptions = null;
let scanImageList = [];

function initWorker(options) {
    let imageThumbDbPath = options.imageThumbDbPath;
    imageThumbDb = new sqliteDb(imageThumbDbPath, true);
    imageThumbDb.open();
    imageThumbDb.exec(DB_CREATE_THUMBNAIL_TABLE, [], (err, info) => {
        let result = {
            'messageId': 'initWorker'
        };
        if (err) {
            // utils.log('initWorker, init table error, err=\n%s', err.message);
            result.err = err;
        }
        // utils.log('initWorker, info=\n%s', utils.objToJsonBeautify(info));
        // utils.log('initWorker, create table success.');
        postMessage(result);
    });
}

function scanImageDir(options) {
    scanImageOptions = options;
    scanImageList = [];
    scanDir(scanImageOptions.imageDirPath, 0);
}

function scanDir(dirPath, deep) {
    utils.log('scanDir, dirPath=[%s], deep=%d', dirPath, deep);
    readdirEx(dirPath, (deep != 0), (err, files) => {
        let result = {
            'messageId': 'scanImageDir'
        };
        if (err) {
            result.err = err;
            return;
        }

        for (let fileName of files) {
            let filePath = path.join(dirPath, fileName);
            let stat = fs.lstatSync(filePath);
            if (stat.isDirectory() && !stat.isSymbolicLink()) {
                // dir
                scanDir(filePath, deep + 1);
            } else if (stat.isFile()) {
                // file
                // utils.log('scanDir, found filePath=[%s]', filePath);
                processFile(filePath, stat);
            } else {
                utils.log('scanDir, ignore filePath=[%s]', filePath);
            }
        }

        if (deep > 0) {
            return;
        }

        // top level
        utils.log('scanDir, finished, found %d images.', scanImageList.length);
        result.scanImageList = scanImageList;
        postMessage(result);
    });
}

function readdirEx(dirPath, isSync, callback) {
    try {
        if (!isSync) {
            // async
            fs.readdir(dirPath, (err, files) => {
                callback(err, files);
            });
        } else {
            // sync
            let files = fs.readdirSync(dirPath);
            callback(null, files);
        }
    } catch (e) {
        utils.log('readdirEx, error, dirPath=[%s]', dirPath);
        callback(null, []);
    }
}

function processFile(filePath, stat) {
    let basename = path.basename(filePath);
    let extname = path.extname(filePath);
    if (extname.startsWith('.')) {
        extname = extname.substring(1);
    }
    let extnameLower = extname.toLowerCase();
    if (!scanImageOptions.imageExts.includes(extnameLower)) {
        // not image
        utils.log('processFile, not an image, filePath=[%s]', filePath);
        return;
    }

    let fileObj = {
        path: filePath,
        basename: basename,
        extname: extname,
        size: stat.size,
        atime: stat.atime.getTime(),
        mtime: stat.mtime.getTime(),
        ctime: stat.ctime.getTime()
    }

    // utils.log('processFile, image, filePath=[%s]', filePath);
    let imgData = fs.readFileSync(filePath);
    let imgDataHash = nodeUtils.hashMd5(imgData);

    fileObj.hash = imgDataHash;

    let imgDim = probeImageSize.sync(imgData);
    if (!imgDim) {
        utils.log('processFile, corrupt image, filePath=[%s]', filePath);
        return;
    }
    fileObj.width = imgDim.width;
    fileObj.height = imgDim.height;

    fileObj.thumbPath = null;
    let imageThumbPath = generateImageThumbnailPath(fileObj);
    if (checkImageThumbnail(imageThumbPath)) {
        // found thumbnail
        fileObj.thumbPath = imageThumbPath;
    }

    // utils.log('processFile, image, fileObj=\n%s', utils.objToJsonBeautify(fileObj));
    scanImageList.push(fileObj);
}

function generateImageThumbnailPath(fileObj) {
    return path.join(scanImageOptions.imageThumbDir, fileObj.hash + '.' + scanImageOptions.imageThumbFormat);
}

function checkImageThumbnail(imageThumbPath) {
    if (!fs.existsSync(imageThumbPath)) {
        return false;
    }

    let imgData = fs.readFileSync(imageThumbPath);
    let imgDim = probeImageSize.sync(imgData);
    if (!imgDim) {
        return false;
    }

    return true;
}

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
        let result = {
            'messageId': 'generateImageThumbnail',
            'imageSrcPath': imageSrcPath,
            'imageThumbPath': imageThumbPath
        };
        if (err) {
            // utils.log('generateImageThumbnail, error=[%s]', err);
            result.err = err;
        } else {
            // utils.log('generateImageThumbnail, thumbnail generated, imageThumbPath=[%s]', imageThumbPath);
        }
        postMessage(result);
    });
}

function updateImageThumbnailDb(options) {
    let imageThumbPath = options.imageThumbPath;
    imageThumbDb.query(DB_SELECT_THUMBNAIL_BY_PATH, [imageThumbPath], (err, rows) => {
        let result = {
            'messageId': 'updateImageThumbnailDb'
        };
        if (err) {
            // utils.log('updateImageThumbnailDb, query error, err=\n%s', err.message);
            result.err = err;
            postMessage(result);
            return;
        }

        let curTimestamp = utils.getCurTimestamp();
        if (rows.length == 0) {
            // insert
            imageThumbDb.exec(DB_INSERT_THUMBNAIL, [imageThumbPath, curTimestamp], (err, info) => {
                if (err) {
                    // utils.log('updateImageThumbnailDb, insert error, err=\n%s', err.message);
                    result.err = err;
                } else {
                    // utils.log('updateImageThumbnailDb, insert success, info=\n%s', utils.objToJsonBeautify(info));
                }
                postMessage(result);
            });
        } else {
            // update
            let thumbId = rows[0].id;
            imageThumbDb.exec(DB_UPDATE_THUMBNAIL_LAST_USED_BY_ID, [curTimestamp, thumbId], (err, info) => {
                if (err) {
                    // utils.log('updateImageThumbnailDb, update error, err=\n%s', err.message);
                    result.err = err;
                } else {
                    // utils.log('updateImageThumbnailDb, update success, info=\n%s', utils.objToJsonBeautify(info));
                }
                postMessage(result);
            });
        }
    });
}

function clearImageThumbnail(options) {
    let curTimestamp = utils.getCurTimestamp();
    let deleteThumbnailThreshold = curTimestamp - scanImageOptions.imageThumbExpire * 1000;
    imageThumbDb.exec(DB_DELETE_THUMBNAIL_BY_LAST_USED_BEFORE, [deleteThumbnailThreshold], (err, info) => {
        let result = {
            'messageId': 'clearImageThumbnail'
        };
        if (err) {
            // utils.log('clearImageThumbnail, delete error, err=\n%s', err.message);
            result.err = err;
            postMessage(result);
            return;
        }

        result.dbChanges = info.changes;
        result.imageThumbDeleted = 0;

        let curImageThumbDir = scanImageOptions.imageThumbDir;
        readdirEx(curImageThumbDir, false, (err, files) => {
            if (err) {
                result.err = err;
                postMessage(result);
                return;
            }

            utils.log('clearImageThumbnail, found %d thumbnails.', files.length);
            for (let fileName of files) {
                let imageThumbPath = path.join(curImageThumbDir, fileName);
                let stat = fs.lstatSync(imageThumbPath);
                if (stat.isFile()) {
                    // file
                    imageThumbDb.query(DB_SELECT_THUMBNAIL_BY_PATH, [imageThumbPath], (err, rows) => {
                        if (err) {
                            result.err = err;
                            postMessage(result);
                            return;
                        }
                        if (rows.length == 0) {
                            // not exists in db, to delete
                            fs.unlink(imageThumbPath, (err) => {
                                if (err) {
                                    utils.log('clearImageThumbnail, unlink error, imageThumbPath=[%s], err=\n%s',
                                        imageThumbPath, err);
                                } else {
                                    utils.log('clearImageThumbnail, unlink imageThumbPath=[%s]',
                                        imageThumbPath);
                                }
                            });
                            result.imageThumbDeleted++;
                        }
                    });
                }
            }

            // utils.log('clearImageThumbnail, delete success, dbChanges=%d, imageThumbDeleted=%d',
            //     result.dbChanges, result.imageThumbDeleted);
            postMessage(result);
        });
    });
}

const messageHandlerMap = {
    'initWorker': initWorker,
    'scanImageDir': scanImageDir,
    'generateImageThumbnail': generateImageThumbnail,
    'updateImageThumbnailDb': updateImageThumbnailDb,
    'clearImageThumbnail': clearImageThumbnail
};

onmessage = function (e) {
    // utils.log('imageWorker.onmessage, [%s]', e.data);
    // postMessage('Worker!');
    let messageData = e.data;
    let messageHandler = messageHandlerMap[messageData.messageId];
    if (!messageHandler) {
        utils.log('imageWorker.onmessage, unknown messageId=[%s]', messageData.messageId);
        return;
    }
    messageHandler(messageData);
};
