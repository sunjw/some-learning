const fs = require('fs');
const path = require('path');

const sharp = require('sharp');
const utils = require('./utils');
const sqliteDb = require('./sqliteDb');

const DB_CREATE_THUMBNAIL_TABLE = 'CREATE TABLE IF NOT EXISTS "thumbnail" (' +
    '"id" INTEGER PRIMARY KEY AUTOINCREMENT,' +
    '"path" varchar(1024) NOT NULL,' +
    '"last_used" INTEGER NOT NULL' +
    ');';
const DB_SELECT_THUMBNAIL_BY_PATH = 'SELECT * FROM thumbnail WHERE path=?';
const DB_INSERT_THUMBNAIL = 'INSERT INTO thumbnail (path, last_used) VALUES (?, ?)';
const DB_UPDATE_THUMBNAIL_LAST_USED_BY_ID = 'UPDATE thumbnail SET last_used=? WHERE id=?';

let imageThumbDb = null;

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

let messageHandlerMap = {
    'initWorker': initWorker,
    'generateImageThumbnail': generateImageThumbnail,
    'updateImageThumbnailDb': updateImageThumbnailDb
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
