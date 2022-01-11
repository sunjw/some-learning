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
        this.keyImageFolder = 'imageFolder';
        this.tipDropScan = 'Drop a folder to scan...';
        this.tipNoFolderDrop = 'No folder dropped!';
        this.tipDropOneFolder = 'Drop only one folder!';

        this.options = options;
        this.eleConfig = new eleUtils.EleConfig(this.options.userDataPath);
        this.curImageFolder = this.getConfig(this.keyImageFolder);

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
        if (!this.curImageFolder) {
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
                message: this.tipNoFolderDrop
            });
            return;
        }
        if (dropFiles.length > 1) {
            eleUtils.sendToMain({
                type: 'warning',
                title: this.title,
                message: this.tipDropOneFolder
            });
            return;
        }
        let dropPath = dropFiles[0].path;
        utils.log('onDropFiles, dropPath=[%s]', dropPath);
        fs.stat(dropPath, (err, stats) => {
            if (!stats.isDirectory()) {
                eleUtils.sendToMain({
                    type: 'warning',
                    title: that.title,
                    message: that.tipDropOneFolder
                });
                return;
            }

            // a foler, go!
            that.curImageFolder = dropPath;
            that.setConfig(that.keyImageFolder, that.curImageFolder);
            that.loadImageFolder();
        });
    }

    loadImageFolder() {
        this.divPathContent.text(this.curImageFolder);
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
