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
const {
    threadId
} = require('worker_threads');

// ipcRenderer.on('on-find', (e, args) => {
//     dirListFilter.openFilterBox();
// });

class WallpickerPage {

    constructor() {
        // init consts.
    }

    init() {
        let that = this;

        this.body = $('body');
        this.divToolbarWrapper = null;
        this.divContentWrapper = null;

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
        this.divToolbarWrapper = $('<div/>').attr('id', 'divToolbarWrapper');

        this.body.append(this.divToolbarWrapper);

        this.divContentWrapper = $('<div/>').attr('id', 'divContentWrapper');

        this.body.append(this.divContentWrapper);
    }
}

$(function () {
    let wallpickerPage = new WallpickerPage();
    wallpickerPage.init();
});
