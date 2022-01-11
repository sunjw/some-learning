const {
    ipcRenderer
} = require('electron');
const remote = require('@electron/remote');

const fs = require('fs');
const path = require('path');

window.$ = window.jQuery = require('jquery');
const bootstrap = require('bootstrap');
const fixPath = require('fix-path')();

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
        this.tipDropScan = 'Drop a directory to scan...';
        this.tipNoDirDrop = 'No directory dropped.';
        this.tipDropOneDir = 'Drop only one directory.';
        this.tipReadDirError = 'Read directory error.';

        this.options = options;
        this.eleConfig = new eleUtils.EleConfig(this.options.userDataPath);
        this.curImageDir = this.getConfig(this.keyImageDir);

        this.body = $('body');
        this.divToolbarWrapper = null;
        this.divPathWrapper = null;
        this.divPathDropInfo = null;
        this.divPathContent = null;
        this.divContentWrapper = null;

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

        this.divPathContent.text(this.curImageDir);

        this.scanDir(this.curImageDir, 0);
    }

    scanDir(dirPath, deep) {
        utils.log('scanDir, dirPath=[%s], deep=%d', dirPath, deep);
        let that = this;

        fs.readdir(dirPath, (err, files) => {
            if (err) {
                eleUtils.sendToMain({
                    type: 'warning',
                    title: that.title,
                    message: that.tipReadDirError
                });
                return false;
            }

            for (let fileName of files) {
                let filePath = path.join(dirPath, fileName);
                let stat = fs.statSync(filePath);
                if (stat.isDirectory()) {
                    that.scanDir(filePath, deep + 1);
                } else {
                    utils.log('scanDir, found filePath=[%s]', filePath);
                }
            }
        });
    }

    readdirEx() {}
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
