const {
    ipcRenderer,
    shell
} = require('electron');
const remote = require('@electron/remote');

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

        this.options = options;
        this.eleConfig = new eleUtils.EleConfig(this.options.userDataPath);

        this.body = $('body');
        this.divToolbarWrapper = null;
        this.divContentWrapper = null;

        this.initLayout();
        this.initHandler();

        this.onWindowResize();
        $(window).on('resize', function () {
            that.onWindowResize();
        });
    }

    onWindowResize() {
        // on window resize.
        this.fitContentHeight();
    }

    initLayout() {
        // init layout.
        this.divToolbarWrapper = $('<div/>').attr('id', 'divToolbarWrapper');

        this.body.append(this.divToolbarWrapper);

        this.divContentWrapper = $('<div/>').attr('id', 'divContentWrapper');

        this.body.append(this.divContentWrapper);
    }

    initHandler() {
        let bodyDom = this.body.get(0);
        bodyDom.ondragover = function (e) {
            e.stopPropagation();
            utils.log('body.ondragover');
            return false;
        };
        bodyDom.ondragleave = function () {
            utils.log('body.ondragleave');
            return false;
        };
        bodyDom.ondragend = function () {
            utils.log('body.ondragend');
            return false;
        };
        bodyDom.ondrop = function (e) {
            e.preventDefault();
            utils.log('body.ondrop, [%s]', e.dataTransfer.files[0].path);
            // for (let f of e.dataTransfer.files) {
            //     pushFile(f.path);
            // }
            return false;
        };
    }

    fitContentHeight() {
        let windowHeight = $(window).height();
        let divContentWrapperTop = this.divContentWrapper.offset().top;
        let divContentWrapperHeight = windowHeight - divContentWrapperTop - 3;
        this.divContentWrapper.css('height', divContentWrapperHeight + 'px');
    }
}

let wallpickerPage = new WallpickerPage();

// ipcRenderer.on('on-find', (e, args) => {
//     dirListFilter.openFilterBox();
// });

ipcRenderer.on('set-userData-path', (event, arg) => {
    utils.log('set-userData-path=[' + arg + ']');

    // let's go.
    wallpickerPage.init({
        userDataPath: arg
    });
});

$(function () {
    utils.log('finish loading.');
});
