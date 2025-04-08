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
        this.divResultWrapper = null;
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
            .addClass('d-flex flex-column m-2');

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
            .addClass('mt-2 noImage');
        this.divImagePreview.append(this.imgPreview);

        this.divUploadWrapper.append(divUploadFormGroup);
        this.divUploadWrapper.append(this.divImagePreview);

        // result
        this.divResultWrapper = $('<div/>')
            .attr('id', 'divResultWrapper')
            .addClass('d-flex flex-column m-2');

        // header
        let divResultHeader = $('<div/>')
            .attr('id', 'divResultHeader')
            .addClass('d-flex');

        // left
        let divResultHeaderLeft = $('<div/>')
            .attr('id', 'divResultHeaderLeft')
            .addClass('flex-grow-1 d-flex align-items-center m-2');
        // title
        let divResultTitle = $('<div/>')
            .attr('id', 'divResultTitle')
            .addClass('text-truncate ps-2')
            .html('Result');
        divResultHeaderLeft.append(divResultTitle);
        divResultHeader.append(divResultHeaderLeft);

        // oper
        let divResultOper = $('<div/>')
            .attr('id', 'divResultOper')
            .addClass('d-flex align-items-center m-2');
        let btnCopy = this.generateSwitchButton(
                'btnCopy', 'bi-clipboard', 'bi-clipboard-check', 'Copy');
        this.switchButtonClick(btnCopy, function () {
            utils.log('switchButtonClick.btnCopy');
        });
        divResultOper.append(btnCopy);
        divResultHeader.append(divResultOper);

        this.divResultWrapper.append(divResultHeader);

        this.divContentWrapper.append(this.divUploadWrapper);
        this.divContentWrapper.append(this.divResultWrapper);

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

    generateButton(buttonClass, iconName, title) {
        let button = $('<button/>')
            .attr('type', 'button')
            .addClass('btn toolbarBtn');
        button.addClass(buttonClass);
        if (title) {
            button.attr('title', title);
        }
        let buttonIcon = $('<i/>').addClass('bi').addClass(iconName);
        button.append(buttonIcon);
        return button;
    }

    needFixButtonFocus() {
        return (npmUtils.isSafari());
    }

    buttonClick(button, handler) {
        let that = this;
        button.on('click', function () {
            if (that.needFixButtonFocus()) {
                button.addClass('focus');
            }
            handler();
            if (that.needFixButtonFocus()) {
                setTimeout(function () {
                    button.removeClass('focus');
                }, 2000);
            }
        });
    }

    generateSwitchButton(buttonClass, iconName1, iconName2, title) {
        let button = this.generateButton(buttonClass, iconName1, title);
        let buttonIcon = button.children('.bi');
        buttonIcon.attr({
            'data-icon-1': iconName1,
            'data-icon-2': iconName2
        });
        return button;
    }

    switchButtonClick(btnSwitch, handler) {
        let buttonIcon = btnSwitch.children('.bi');
        let iconName1 = buttonIcon.attr('data-icon-1');
        let iconName2 = buttonIcon.attr('data-icon-2');
        this.buttonClick(btnSwitch, function () {
            handler();
            buttonIcon.removeClass(iconName1);
            buttonIcon.addClass(iconName2);
            setTimeout(function () {
                buttonIcon.removeClass(iconName2);
                buttonIcon.addClass(iconName1);
            }, 2000);
        });
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
