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
        
    }

    init() {
        utils.log('QrDecPage.init');
        this.initLayout();
        this.initFunc();
    }

    initLayout() {

    }

    initFunc() {

    }

    
}

$(function () {
    utils.log('init, QR decoder start...');

    let page = new QrDecPage();
    page.init();
});
