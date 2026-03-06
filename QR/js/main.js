// css
require('@materializecss/materialize/dist/css/materialize.min.css');
require('material-design-icons/iconfont/material-icons.css');
require('../css/main.css');

// js
const M = require('@materializecss/materialize');
window.$ = require('jquery');
const qrcode = require('qrcode');
const utils = require('./utils');

const bodyMinWidth = 250;
const pageMaxWidth = 1000;
const pageNonFloatWidth = 600;
const qrImageWidth = 240;

class QrEncPage {
    constructor() {
        // DOM refs (initialized in initContent)
        this.divPageContainer = null;
        this.divQrTextContainer = null;
        this.divQrImgContainer = null;
        this.taQrText = null;
        this.iQrTextClear = null;
        this.canvasQrImgCanvas = null;
    }

    init() {
        M.AutoInit();
        this.initContent();
    }

    initContent() {
        this.divPageContainer = $('div#pageContainer');
        this.divQrTextContainer = $('div#qrTextContainer');
        this.divQrImgContainer = $('div#qrImgContainer');
        this.taQrText = $('textarea#qrText');
        this.iQrTextClear = $('i#qrTextClear');
        this.canvasQrImgCanvas = $('canvas#qrImgCanvas');

        this.initLayout();
        this.initFunc();

        // initial render
        this.generateQrCanvas(this.taQrText.val());
    }

    initLayout() {
        $('body').css({
            'min-width': bodyMinWidth + 'px'
        });

        // run once and on resize (bind with correct this)
        this.onWindowResize();
        $(window).on('resize', () => this.onWindowResize());

        this.divPageContainer.show();

        setTimeout(() => {
            this.taQrText.focus();
        }, 250);
    }

    onWindowResize() {
        let windowWidth = $(window).width();
        utils.log('onWindowResize, windowWidth=%d', windowWidth);
        let pageWidth = windowWidth;
        if (pageWidth > pageMaxWidth)
            pageWidth = pageMaxWidth;

        this.divPageContainer.addClass('floatLeftWrapper');

        this.divPageContainer.css({
            'width': pageWidth + 'px',
            'margin-left': 'auto',
            'margin-right': 'auto'
        });
        this.divQrImgContainer.css({
            'height': '320px'
        });
        this.canvasQrImgCanvas.css({
            'margin-top': '20px'
        });

        if (pageWidth > pageNonFloatWidth) {
            this.divQrTextContainer.css({
                'width': pageWidth / 2 + 'px'
            });
            this.divQrImgContainer.css({
                'width': pageWidth / 2 + 'px'
            });
            this.canvasQrImgCanvas.css({
                'margin-left': (pageWidth / 2 - qrImageWidth) / 2 + 'px'
            });

            this.divQrTextContainer.addClass('floatLeft');
            this.divQrImgContainer.addClass('floatLeft');
        } else {
            this.divQrTextContainer.css({
                'width': pageWidth + 'px'
            });
            this.divQrImgContainer.css({
                'width': pageWidth + 'px'
            });
            this.canvasQrImgCanvas.css({
                'margin-left': (pageWidth - qrImageWidth) / 2 + 'px'
            });

            this.divQrTextContainer.removeClass('floatLeft');
            this.divQrImgContainer.removeClass('floatLeft');
        }
    }

    initFunc() {
        this.onQrTextChange();
        this.taQrText.on('input propertychange', () => {
            this.onQrTextChange();
            this.generateQrCanvas(this.taQrText.val());
        });

        this.iQrTextClear.on('click', () => {
            this.taQrText.val('');
            this.taQrText.trigger('propertychange');
            this.taQrText.focus();
            this.taQrText.get(0).select();
        });

        this.canvasQrImgCanvas.on('click', () => {
            let canvasEl = this.canvasQrImgCanvas.get(0);
            let downloadLink = $('<a/>');
            downloadLink.attr('download', 'qrcode.png');
            downloadLink.attr('href', canvasEl.toDataURL());
            downloadLink.attr('target', '_blank');
            downloadLink[0].click();
        });
    }

    onQrTextChange() {
        M.textareaAutoResize(this.taQrText);
        let taVal = this.taQrText.val();
        if (taVal && taVal.length > 0) {
            this.iQrTextClear.show();
        } else {
            this.iQrTextClear.hide();
        }
    }

    generateQrCanvas(text) {
        utils.log('generateQrCanvas, text=[%s]', text);
        if (!text || text === '') {
            text = 'http://sunjw.us/qr';
        }
        qrcode.toCanvas(this.canvasQrImgCanvas.get(0), text, {
            width: qrImageWidth
        });
    }
}

$(function () {
    utils.log('init, QrEncPage start...');
    const page = new QrEncPage();
    page.init();
});
