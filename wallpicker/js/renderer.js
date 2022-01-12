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

function stopBubble(event) {
    event.stopPropagation();
}

class WallpickerPage {

    constructor() {
        // init consts
    }

    init(options) {
        let that = this;

        this.title = 'Wallpicker';
        this.keyImageDir = 'imageDir';
        this.classImageSelected = 'imageSelected';
        this.imageExts = ['jpg', 'jpeg', 'png'];
        this.tipDropScan = 'Drop a directory to scan...';
        this.tipNoDirDrop = 'No directory dropped.';
        this.tipDropOneDir = 'Drop only one directory.';
        this.tipReadDirError = 'Read directory error.';
        this.minScanTime = 2000;
        this.imageBlockWidth = 230; // width + margin
        this.maxImagePreviewWidth = 200;
        this.maxImagePreviewHeight = 160;
        this.maxImagePreviewRatio = this.maxImagePreviewWidth / this.maxImagePreviewHeight;

        this.options = options;
        this.eleConfig = new eleUtils.EleConfig(this.options.userDataPath);
        this.curImageDir = this.getConfig(this.keyImageDir);
        this.curImageList = [];
        this.selectedImageBlock = null;

        this.body = $('body');
        this.divToolbarWrapper = null;
        this.divPathWrapper = null;
        this.divToolsWrapper = null;
        this.divPathDropInfo = null;
        this.divPathContent = null;
        this.buttonSetWallpaper = null;
        this.divContentWrapper = null;
        this.divImageList = null;
        this.divLoadingWrapper = null;
        this.divLoadingBlock = null;

        this.initLayout();
        this.initHandler();

        this.onWindowResize();
        $(window).on('resize', function () {
            that.onWindowResize();
        });

        this.hideLoading();

        if (this.curImageDir) {
            // we have one, go!
            this.loadImageDir();
        }
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
        // on window resize
        this.fitContentHeight();
        this.fitImageListWidth();
    }

    initLayout() {
        let that = this;
        // init layout

        // init toolbar
        this.divToolbarWrapper = $('<div/>')
            .attr('id', 'divToolbarWrapper')
            .addClass('d-flex');

        // init path
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

        // init tools
        this.divToolsWrapper = $('<div/>')
            .attr('id', 'divToolsWrapper')
            .addClass('d-flex');

        this.buttonSetWallpaper = this.generateToolbarButton('buttonSetWallpaper', 'bi-image', 'Set Wallpaper');
        this.setButtonDisabled(this.buttonSetWallpaper, true);
        this.autoBlurButtonClick(this.buttonSetWallpaper, function () {
            utils.log('buttonSetWallpaper.click');
            that.clearSelection();
        });

        this.divToolsWrapper.append(this.buttonSetWallpaper);
        this.divToolbarWrapper.append(this.divToolsWrapper);

        this.body.append(this.divToolbarWrapper);

        // init content
        this.divContentWrapper = $('<div/>').attr('id', 'divContentWrapper');

        // image list
        this.divImageList = $('<div/>')
            .attr('id', 'divImageList')
            .addClass('ms-auto me-auto d-flex align-content-start flex-wrap')
            .on('click', function () {
                utils.log('divImageList click');
                that.clearSelection();
            });
        this.divContentWrapper.append(this.divImageList);

        // loading
        this.divLoadingWrapper = $('<div/>').attr('id', 'divLoadingWrapper');
        this.divLoadingBlock = $('<div/>')
            .attr('id', 'divLoadingBlock')
            .addClass('d-flex justify-content-center align-items-center');
        let divLoadingSpinner = $('<div/>')
            .attr({
                'role': 'status',
                'aria-hidden': 'true'
            })
            .addClass('spinner-border');
        let divLoadingText = $('<div/>')
            .attr('id', 'divLoadingText')
            .text('Scanning...');
        this.divLoadingBlock.append(divLoadingSpinner);
        this.divLoadingBlock.append(divLoadingText);
        this.divLoadingWrapper.append(this.divLoadingBlock);
        this.divContentWrapper.append(this.divLoadingWrapper);

        this.body.append(this.divContentWrapper);
    }

    initHandler() {
        let that = this;

        let bodyDom = this.body.get(0);
        bodyDom.ondragover = function (e) {
            stopBubble(e);
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
        let contentHeight = windowHeight - divContentWrapperTop - 2;
        this.divContentWrapper.css('height', contentHeight + 'px');
        this.divLoadingBlock.css('height', contentHeight + 'px');
    }

    fitImageListWidth() {
        let contentWrapperWidth = this.divContentWrapper.width();
        let imagePerLine = Math.floor(contentWrapperWidth / this.imageBlockWidth);
        let divImageListWidth = imagePerLine * this.imageBlockWidth + 2;
        this.divImageList.css('width', divImageListWidth + 'px');
    }

    generateToolbarButton(buttonId, iconName, title) {
        let button = $('<button/>').attr({
            'id': buttonId,
            'type': 'button'
        }).addClass('btn btn-light align-self-center toolbarBtn');
        if (title) {
            button.attr('title', title);
        }
        let buttonIcon = $('<i/>').addClass('bi').addClass(iconName);
        button.append(buttonIcon);
        return button;
    }

    autoBlurButtonClick(button, handler) {
        let that = this;
        button.on('click', function () {
            handler();
            that.resetButtonBlur(button);
        });
    }

    resetButtonBlur(button) {
        setTimeout(function () {
            button.blur();
        }, 250);
    }

    setButtonDisabled(button, disabled) {
        if (disabled) {
            button.attr('disabled', 'disabled');
        } else {
            button.removeAttr('disabled');
        }
    }

    showLoading() {
        utils.log('showLoading');
        this.divLoadingWrapper.show();
    }

    hideLoading() {
        utils.log('hideLoading');
        this.divLoadingWrapper.hide();
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
        fs.lstat(dropPath, (err, stats) => {
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
        this.renderPath();
        this.clearImageList();
        this.showLoading();
        setTimeout(() => {
            that.scanDir(this.curImageDir, 0);
        }, 100);
    }

    scanDir(dirPath, deep) {
        let that = this;

        let scanStart = new Date().getTime();

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
                let stat = fs.lstatSync(filePath);
                if (stat.isDirectory() && !stat.isSymbolicLink()) {
                    // dir
                    that.scanDir(filePath, deep + 1);
                } else if (stat.isFile()) {
                    // file
                    // utils.log('scanDir, found filePath=[%s]', filePath);
                    that.processFile(filePath, stat);
                } else {
                    utils.log('scanDir, ignore filePath=[%s]', filePath);
                }
            }

            if (deep > 0) {
                return;
            }

            // top level
            utils.log('scanDir, finished, found %d images.', that.curImageList.length);
            let scanEnd = new Date().getTime();
            let scanDuration = scanEnd - scanStart;
            if (scanDuration < that.minScanTime) {
                let waitSome = that.minScanTime - scanDuration;
                utils.log('scanDir, waitSome=%dms', waitSome);
                setTimeout(() => {
                    that.renderImageList();
                }, waitSome);
            } else {
                that.renderImageList();
            }
        });
    }

    readdirEx(dirPath, isSync, callback) {
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

    processFile(filePath, stat) {
        let basename = path.basename(filePath);
        let extname = path.extname(filePath);
        if (extname.startsWith('.')) {
            extname = extname.substring(1);
        }
        let extnameLower = extname.toLowerCase();
        if (!this.imageExts.includes(extnameLower)) {
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
        let imgDim = probeImageSize.sync(imgData);
        fileObj.width = imgDim.width;
        fileObj.height = imgDim.height;

        // utils.log('processFile, image, fileObj=\n%s', JSON.stringify(fileObj, null, 2));
        this.curImageList.push(fileObj);
    }

    renderPath() {
        // clear
        this.divPathContent.empty();

        // render
        let pathParts = this.curImageDir.split(path.sep);
        if (pathParts[0] == '') {
            pathParts = pathParts.slice(1);
        }
        utils.log('renderPath, pathPart=[%s]', pathParts.join(','));
        let pathRender = '';
        let pathPartsLen = pathParts.length;
        for (let i = 0; i < pathPartsLen - 1; i++) {
            pathRender += (' ' + path.sep + ' ' + pathParts[i]);
        }
        pathRender += (' ' + path.sep + ' ');
        if (utils.isWindows()) {
            pathRender = pathRender.substring(3);
        }
        pathRender = pathRender.trimStart();
        this.divPathContent.append($('<span/>').html(utils.escapeHtml(pathRender)));
        this.divPathContent.append($('<span/>').addClass('pathDirname').html(utils.escapeHtml(pathParts[pathParts.length - 1])));
    }

    clearImageList() {
        this.curImageList = [];
        this.divImageList.empty();
        this.clearSelection();
    }

    renderImageList() {
        let that = this;

        // sort
        this.curImageList.sort((fo1, fo2) => {
            return (fo2.mtime - fo1.mtime); // modified time reverse
        });

        // render
        for (let fileObj of this.curImageList) {
            let filePath = fileObj.path;
            let divImageBlock = $('<div/>')
                .attr('data-ref', filePath)
                .addClass('imageBlock');

            // let fileObjJson = JSON.stringify(fileObj, null, 2);
            // fileObjJson = utils.escapeHtml(fileObjJson);
            // fileObjJson = utils.stringReplaceAll(fileObjJson, '\n', '<br/>');
            // divImageBlock.html(fileObjJson);

            let divImageWrapper = $('<div/>').addClass('d-flex justify-content-center imageWrapper');
            let divInfoWrapper = $('<div/>').addClass('d-flex justify-content-center infoWrapper');

            let imgContent = $('<img/>')
                .attr({
                    'src': 'assets/placeholder.png',
                    'data-src': filePath
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
            imgContent.on('click', function (e) {
                stopBubble(e);
                let imgParentBlock = divImageBlock;
                that.selectImage(imgParentBlock);
            });
            divImageWrapper.append(imgContent);

            let divImageInfo = $('<div/>').addClass('align-self-start imageInfo');
            let imageBasename = fileObj.basename;
            // let imageExtname = fileObj.extname;
            if (imageBasename.length > 45) {
                let imageBasenameShortLen = 36;
                if (utils.isMacOS()) {
                    imageBasenameShortLen = 32;
                }
                let imageBasenameFix = imageBasename.substring(0, imageBasenameShortLen);
                imageBasenameFix += '...';
                imageBasenameFix += imageBasename.substring(imageBasename.length - 8, imageBasename.length);
                imageBasename = imageBasenameFix;
            }
            divImageInfo.html(utils.escapeHtml(imageBasename));
            divImageInfo.on('click', function (e) {
                stopBubble(e);
                let imgParentBlock = divImageBlock;
                that.selectImage(imgParentBlock);
            });
            divInfoWrapper.append(divImageInfo);

            divImageBlock.append(divImageWrapper);
            divImageBlock.append(divInfoWrapper);

            this.divImageList.append(divImageBlock);
        }

        this.hideLoading();
    }

    refreshButtonState() {
        if (this.selectedImageBlock) {
            // selected
            this.setButtonDisabled(this.buttonSetWallpaper, false);
        } else {
            // no selection
            this.setButtonDisabled(this.buttonSetWallpaper, true);
        }
    }

    clearSelection(refreshButton = true) {
        utils.log('clearSelection');
        let imageBlocks = this.divImageList.find('.imageBlock');
        imageBlocks.removeClass(this.classImageSelected);
        this.selectedImageBlock = null;
        if (refreshButton) {
            this.refreshButtonState();
        }
    }

    selectImage(imageBlock) {
        this.clearSelection(false);
        utils.log('selectImage, imageBlock=[%s]', imageBlock.attr('data-ref'));
        this.selectedImageBlock = imageBlock;
        imageBlock.addClass(this.classImageSelected);
        this.refreshButtonState();
    }
}

let wallpickerPage = new WallpickerPage();

ipcRenderer.on('set-userData-path', (e, arg) => {
    utils.log('set-userData-path, arg=[%s]', arg);

    // let's go.
    wallpickerPage.init({
        userDataPath: arg
    });
});

// ipcRenderer.on('on-find', (e, args) => {
//     dirListFilter.openFilterBox();
// });

$(function () {
    utils.log('finish loading.');
});
