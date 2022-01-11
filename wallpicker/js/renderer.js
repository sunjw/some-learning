const {
    ipcRenderer
} = require('electron');
const remote = require('@electron/remote');

const fs = require('fs');
const path = require('path');

window.$ = window.jQuery = require('jquery');
const bootstrap = require('bootstrap');
const fixPath = require('fix-path')();
const probeImageSize = require('probe-image-size');
const lazyload = require('lazyload');

const utils = require('./utils');
const eleUtils = require('./eleUtils');

class WallpickerPage {

    constructor() {
        // init consts.
    }

    init(options) {
        let that = this;

        this.title = 'Wallpicker';
        this.keyImageDir = 'imageDir';
        this.imageExts = ['jpg', 'jpeg', 'png'];
        this.tipDropScan = 'Drop a directory to scan...';
        this.tipNoDirDrop = 'No directory dropped.';
        this.tipDropOneDir = 'Drop only one directory.';
        this.tipReadDirError = 'Read directory error.';
        this.maxImagePreviewWidth = 200;
        this.maxImagePreviewHeight = 160;
        this.maxImagePreviewRatio = this.maxImagePreviewWidth / this.maxImagePreviewHeight;

        this.options = options;
        this.eleConfig = new eleUtils.EleConfig(this.options.userDataPath);
        this.curImageDir = this.getConfig(this.keyImageDir);
        this.curImageList = [];

        this.body = $('body');
        this.divToolbarWrapper = null;
        this.divPathWrapper = null;
        this.divPathDropInfo = null;
        this.divPathContent = null;
        this.divContentWrapper = null;
        this.divImageList = null;

        this.initLayout();
        this.initHandler();

        this.onWindowResize();
        $(window).on('resize', function () {
            that.onWindowResize();
        });
    }

    getConfig(key) {
        let value = this.eleConfig.getConfig(key);
        utils.log('getConfig, [%s]=[%s]', key, value);
        return value;
    }

    setConfig(key, value) {
        utils.log('setConfig, [%s]=[%s]', key, value);
        this.eleConfig.setConfig(key, value);
    }

    onWindowResize() {
        // on window resize.
        this.fitContentHeight();
        this.fitImageListWidth();
    }

    initLayout() {
        // init layout.

        // init toolbar.
        this.divToolbarWrapper = $('<div/>')
            .attr('id', 'divToolbarWrapper')
            .addClass('d-flex');

        // init path.
        this.divPathWrapper = $('<div/>')
            .attr('id', 'divPathWrapper')
            .addClass('flex-grow-1');
        this.divPathDropInfo = $('<div/>')
            .attr('id', 'divPathDropInfo')
            .text(this.tipDropScan);
        this.divPathContent = $('<div/>').attr('id', 'divPathContent');
        if (!this.curImageDir) {
            this.divPathContent.text(this.tipDropScan);
        }
        this.divPathWrapper.append(this.divPathDropInfo);
        this.divPathWrapper.append(this.divPathContent);

        this.divToolbarWrapper.append(this.divPathWrapper);
        this.body.append(this.divToolbarWrapper);

        // init content.
        this.divContentWrapper = $('<div/>').attr('id', 'divContentWrapper');
        this.divImageList = $('<div/>')
            .attr('id', 'divImageList')
            .addClass('ms-auto me-auto d-flex align-content-start flex-wrap');
        this.divContentWrapper.append(this.divImageList);
        this.body.append(this.divContentWrapper);
    }

    initHandler() {
        let that = this;

        let bodyDom = this.body.get(0);
        bodyDom.ondragover = function (e) {
            e.stopPropagation();
            // utils.log('ondragover, body.ondragover');
            that.divToolbarWrapper.addClass('dropInfo');
            return false;
        };
        bodyDom.ondragleave = function () {
            // utils.log('ondragleave, body.ondragleave');
            that.divToolbarWrapper.removeClass('dropInfo');
            return false;
        };
        bodyDom.ondragend = function () {
            // utils.log('ondragend, body.ondragend');
            that.divToolbarWrapper.removeClass('dropInfo');
            return false;
        };
        bodyDom.ondrop = function (e) {
            e.preventDefault();
            that.divToolbarWrapper.removeClass('dropInfo');
            that.onDropFiles(e.dataTransfer.files);
            return false;
        };
    }

    fitContentHeight() {
        let windowHeight = $(window).height();
        let divContentWrapperTop = this.divContentWrapper.offset().top;
        let divContentWrapperHeight = windowHeight - divContentWrapperTop - 3;
        this.divContentWrapper.css('height', divContentWrapperHeight + 'px');
    }

    fitImageListWidth() {
        let contentWrapperWidth = this.divContentWrapper.width();
        let imagePerLine = Math.floor(contentWrapperWidth / this.maxImagePreviewWidth);
        let divImageListWidth = imagePerLine * this.maxImagePreviewWidth + 2;
        this.divImageList.css('width', divImageListWidth + 'px');
    }

    onDropFiles(dropFiles) {
        let that = this;
        if (dropFiles.length <= 0) {
            eleUtils.sendToMain({
                type: 'warning',
                title: this.title,
                message: this.tipNoDirDrop
            });
            return;
        }
        if (dropFiles.length > 1) {
            eleUtils.sendToMain({
                type: 'warning',
                title: this.title,
                message: this.tipDropOneDir
            });
            return;
        }
        let dropPath = dropFiles[0].path;
        utils.log('onDropFiles, dropPath=[%s]', dropPath);
        fs.stat(dropPath, (err, stats) => {
            if (err || !stats.isDirectory()) {
                eleUtils.sendToMain({
                    type: 'warning',
                    title: that.title,
                    message: that.tipDropOneDir
                });
                return;
            }

            // a foler, go!
            that.curImageDir = dropPath;
            that.setConfig(that.keyImageDir, that.curImageDir);
            that.loadImageDir();
        });
    }

    loadImageDir() {
        let that = this;
        this.curImageList = [];
        this.divPathContent.text(this.curImageDir);
        setTimeout(() => {
            that.scanDir(this.curImageDir, 0);
        }, 100);
    }

    scanDir(dirPath, deep) {
        let that = this;

        utils.log('scanDir, dirPath=[%s], deep=%d', dirPath, deep);
        this.readdirEx(dirPath, (deep != 0), (err, files) => {
            if (err) {
                eleUtils.sendToMain({
                    type: 'warning',
                    title: that.title,
                    message: that.tipReadDirError
                });
                return;
            }

            for (let fileName of files) {
                let filePath = path.join(dirPath, fileName);
                let stat = fs.statSync(filePath);
                if (stat.isDirectory()) {
                    // dir
                    that.scanDir(filePath, deep + 1);
                } else {
                    // file
                    // utils.log('scanDir, found filePath=[%s]', filePath);
                    that.processFile(filePath, stat);
                }
            }

            if (deep > 0) {
                return;
            }

            // top level.
            utils.log('scanDir, finished, found %d images.', that.curImageList.length);
            that.renderImageList();
        });
    }

    readdirEx(dirPath, isSync, callback) {
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
    }

    processFile(filePath, stat) {
        let basename = path.basename(filePath);
        let extname = path.extname(filePath);
        if (extname.startsWith('.')) {
            extname = extname.substring(1);
        }
        let extnameLower = extname.toLowerCase();
        if (!this.imageExts.includes(extnameLower)) {
            // not image.
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
        let imgDim = probeImageSize.sync(imgData);
        fileObj.width = imgDim.width;
        fileObj.height = imgDim.height;

        // utils.log('processFile, image, fileObj=\n%s', JSON.stringify(fileObj, null, 2));
        this.curImageList.push(fileObj);
    }

    renderImageList() {
        // clear
        this.divImageList.empty();

        // sort
        this.curImageList.sort((fo1, fo2) => {
            return (fo2.ctime - fo1.ctime); // create time reverse
        });

        // render
        for (let fileObj of this.curImageList) {
            let divImageBlock = $('<div/>').addClass('imageBlock');
            // let fileObjJson = JSON.stringify(fileObj, null, 2);
            // fileObjJson = utils.escapeHtml(fileObjJson);
            // fileObjJson = utils.stringReplaceAll(fileObjJson, '\n', '<br/>');
            // divImageBlock.html(fileObjJson);

            let divImageWrapper = $('<div/>').addClass('d-flex justify-content-center imageWrapper');
            let divInfoWrapper = $('<div/>').addClass('d-flex justify-content-center infoWrapper');

            let imgContent = $('<img/>')
                .attr({
                    'src': 'assets/placeholder.png',
                    'data-src': fileObj.path
                })
                .addClass('align-self-end lazyload imageContent');
            let imagePreviewWidth = fileObj.width;
            let imagePreviewHeight = fileObj.height;
            let imageRatio = fileObj.width / fileObj.height;
            if (imageRatio > this.maxImagePreviewRatio) {
                imagePreviewWidth = this.maxImagePreviewWidth;
                imagePreviewHeight = imagePreviewWidth / imageRatio;
            } else {
                imagePreviewHeight = this.maxImagePreviewHeight;
                imagePreviewWidth = imagePreviewHeight * imageRatio;
            }
            imgContent.css({
                'width': imagePreviewWidth + 'px',
                'height': imagePreviewHeight + 'px'
            });
            imgContent.lazyload(); // lazy
            divImageWrapper.append(imgContent);

            let divInfo = $('<div/>').addClass('align-self-start imageInfo');
            divInfo.html(utils.escapeHtml(fileObj.basename));
            divInfoWrapper.append(divInfo);

            divImageBlock.append(divImageWrapper);
            divImageBlock.append(divInfoWrapper);

            this.divImageList.append(divImageBlock);
        }
    }
}

let wallpickerPage = new WallpickerPage();

// ipcRenderer.on('on-find', (e, args) => {
//     dirListFilter.openFilterBox();
// });

ipcRenderer.on('set-userData-path', (event, arg) => {
    utils.log('set-userData-path, arg=[%s]', arg);

    // let's go.
    wallpickerPage.init({
        userDataPath: arg
    });
});

$(function () {
    utils.log('finish loading.');
});
