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

// ipcRenderer.on('on-find', (e, args) => {
//     dirListFilter.openFilterBox();
// });

class WallpickerPage {

    constructor() {}

    init() {
        let that = this;

        this.initLayout();
        this.onWindowResize();
        $(window).on('resize', function () {
            that.onWindowResize();
        });
    }

    onWindowResize() {
        // on window resize.
    }

    initLayout() {
        // init layout.
    }
}

$(function () {
    let wallpickerPage = new WallpickerPage();
    wallpickerPage.init();
});
