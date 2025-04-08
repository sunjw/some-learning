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
        this.divImagePreview = null;
        this.labelImageUpload = null;
        this.inputImageUpload = null;
        this.imgPreview = null;
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

        // upload
        this.divUploadWrapper = $('<div/>')
            .attr('id', 'divUploadWrapper')
            .addClass('d-flex flex-column p-2');

        let divUploadFormGroup = $('<div/>')
            .attr('id', 'divUploadFormGroup')
            .addClass('form-group');
        this.labelImageUpload = $('<label/>')
            .attr({
                'id': 'labelImageUpload',
                'for': 'inputImageUpload'
            })
            .addClass('form-label')
            .text('Select or drag a QR image to decode');
        this.inputImageUpload = $('<input/>')
            .attr({
                'id': 'inputImageUpload',
                'type': 'file',
                'accept': 'image/png, image/jpeg'
            })
            .addClass('form-control');
        this.inputImageUpload.hide();
        this.inputImageUpload.on('change', () => {
            this.onImageUploadChange();
        });
        divUploadFormGroup.append(this.labelImageUpload);
        divUploadFormGroup.append(this.inputImageUpload);

        this.divImagePreview = $('<div/>')
            .attr('id', 'divImagePreview')
            .addClass('d-flex justify-content-center');
        this.imgPreview = $('<img/>')
            .attr({
                'id': 'imgPreview',
                'src': ''
            })
            .addClass('mt-2 mb-2 noImage');
        this.divImagePreview.append(this.imgPreview);

        this.divUploadWrapper.append(divUploadFormGroup);
        this.divUploadWrapper.append(this.divImagePreview);

        // result

        this.divContentWrapper.append(this.divUploadWrapper);

        this.body.append(this.divContentWrapper);
    }

    initFunc() {
        this.initImagePreview();
        this.initDragDrop();
    }

    initImagePreview() {
        let divImagePreviewWidth = this.divImagePreview.width();
        this.imgPreview.css({
            'width': divImagePreviewWidth + 'px',
            'height': (divImagePreviewWidth * 0.6) + 'px'
        });
    }

    initDragDrop() {
        let that = this;

        const dragDropClass = 'dragDrop';
        let bodyElem = this.body.get(0);
        bodyElem.ondragover = function (e) {
            e.preventDefault();
            that.labelImageUpload.addClass(dragDropClass);
            return false;
        };
        bodyElem.ondragleave = function (e) {
            e.preventDefault();
            that.labelImageUpload.removeClass(dragDropClass);
            return false;
        };
        bodyElem.ondragend = function (e) {
            e.preventDefault();
            that.labelImageUpload.removeClass(dragDropClass);
            return false;
        };
        bodyElem.ondrop = function (e) {
            utils.log('bodyElem.ondrop');
            e.preventDefault();
            that.labelImageUpload.removeClass(dragDropClass);
            if (e && e.dataTransfer) {
                let dragDropFiles = e.dataTransfer.files;
                if (dragDropFiles.length > 0) {
                    that.inputImageUpload.get(0).files = dragDropFiles;
                    that.onImageUploadChange();
                }
            }
            return false;
        }
    }

    onImageUploadChange() {
        let that = this;
        let inputImageUploadElem = this.inputImageUpload.get(0);
        if (inputImageUploadElem.files.length > 0) {
            let imageFile = inputImageUploadElem.files[0];
            let imageFileReader = new FileReader();
            imageFileReader.onload = function (e) {
                utils.log('onImageUploadChange, [' + imageFile.name + '] loaded');
                let imgObj = new Image();
                imgObj.src = e.target.result;
                imgObj.onload = function () {
                    let divImagePreviewWidth = that.divImagePreview.width();
                    let imgWidth = imgObj.width;
                    let imgHeight = imgObj.height;
                    let imgRatio = imgWidth / imgHeight;
                    if (imgWidth > divImagePreviewWidth) {
                        imgWidth = divImagePreviewWidth;
                        imgHeight = imgWidth / imgRatio;
                    }
                    that.imgPreview.css({
                        'width': imgWidth + 'px',
                        'height': imgHeight + 'px'
                    });

                    that.imgPreview.removeClass('noImage');
                    that.imgPreview.attr('src', imgObj.src);
                };
            };
            imageFileReader.readAsDataURL(imageFile);
        }
    }

}

$(function () {
    utils.log('init, QR decoder start...');

    let page = new QrDecPage();
    page.init();
});
