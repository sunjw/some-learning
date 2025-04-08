// css
require('bootstrap/dist/css/bootstrap.css');
require('bootstrap-icons/font/bootstrap-icons.css');
require('../css/main.css');

// js
window.$ = window.jQuery = require('jquery'); // bootstrap 5 must see window.jQuery
require('bootstrap');
const clipboardCopy = require('clipboard-copy');
const utils = require('./utils');
const npmUtils = require('./npmUtils');
const jqueryUtils = require('./jqueryUtils');
const bsDialog = require('./bsDialog');

function copyText(someText) {
    utils.log('copyText, copied\n%s', someText);
    // navigator.clipboard.writeText(someText);
    clipboardCopy(someText);
}

class QrDecPage {
    constructor() {
        // vars

        // layout
        this.body = $('body');
        this.divContentWrapper = null;
        this.divUploadWrapper = null;
    }

    init() {
        utils.log('QrDecPage.init');
        this.initLayout();
        this.initFunc();
    }

    initLayout() {
        // content
        this.divContentWrapper = $('<div/>')
            .attr('id', 'divContentWrapper')
            .addClass('d-flex justify-content-center mx-auto pt-4');

        this.divUploadWrapper = $('<div/>')
            .attr('id', 'divUploadWrapper')
            .addClass('d-flex flex-column p-2');
        let divUploadFormGroup = $('<div/>')
            .attr('id', 'divUploadFormGroup')
            .addClass('form-group');
        let labelImageUpload = $('<label/>')
            .attr({
                'id': 'labelImageUpload',
                'for': 'inputImageUpload'
            })
            .addClass('form-label')
            .text('Select or drag a image file');
        let inputImageUpload = $('<input/>')
            .attr({
                'id': 'inputImageUpload',
                'type': 'file',
                'accept': 'image/png, image/jpeg'
            })
            .addClass('form-control');
        inputImageUpload.hide();
        divUploadFormGroup.append(labelImageUpload);
        divUploadFormGroup.append(inputImageUpload);
        this.divUploadWrapper.append(divUploadFormGroup);

        this.divContentWrapper.append(this.divUploadWrapper);

        this.body.append(this.divContentWrapper);
    }

    initFunc() {}

}

$(function () {
    utils.log('init, QR decoder start...');

    let page = new QrDecPage();
    page.init();
});
