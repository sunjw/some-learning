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

let divPageContainer = 0;
let divQrTextContainer = 0;
let divQrImgContainer = 0;
let taQrText = 0;
let iQrTextClear = 0;
let canvasQrImgCanvas = 0;

function initContent() {
  M.AutoInit();

  divPageContainer = $('div#pageContainer');
  divQrTextContainer = $('div#qrTextContainer');
  divQrImgContainer = $('div#qrImgContainer');
  taQrText = $('textarea#qrText');
  iQrTextClear = $('i#qrTextClear');
  canvasQrImgCanvas = $('canvas#qrImgCanvas');

  initLayout();
  initFunc();

  // init call
  generateQrCanvas(taQrText.val());
}

function initLayout() {
  $('body').css({
    'min-width': bodyMinWidth + 'px'
  });

  onWindowResize();
  $(window).on('resize', function () {
    onWindowResize();
  });

  divPageContainer.show();

  setTimeout(function () {
    taQrText.focus();
  }, 250);
}

function onWindowResize() {
  let windowWidth = $(window).width();
  utils.log('onWindowResize, windowWidth=%d', windowWidth);
  let pageWidth = windowWidth;
  if (pageWidth > pageMaxWidth) {
    pageWidth = pageMaxWidth;
  }

  divPageContainer.addClass('floatLeftWrapper');

  divPageContainer.css({
    'width': pageWidth + 'px',
    'margin-left': 'auto',
    'margin-right': 'auto'
  });
  divQrImgContainer.css({
    'height': '320px'
  });
  canvasQrImgCanvas.css({
    'margin-top': '20px'
  });

  if (pageWidth > pageNonFloatWidth) {
    divQrTextContainer.css({
      'width': pageWidth / 2 + 'px'
    });

    divQrImgContainer.css({
      'width': pageWidth / 2 + 'px'
    });
    canvasQrImgCanvas.css({
      'margin-left': (pageWidth / 2 - qrImageWidth) / 2 + 'px'
    });

    divQrTextContainer.addClass('floatLeft');
    divQrImgContainer.addClass('floatLeft');
  } else {
    divQrTextContainer.css({
      'width': pageWidth + 'px'
    });

    divQrImgContainer.css({
      'width': pageWidth + 'px'
    });
    canvasQrImgCanvas.css({
      'margin-left': (pageWidth - qrImageWidth) / 2 + 'px'
    });

    divQrTextContainer.removeClass('floatLeft');
    divQrImgContainer.removeClass('floatLeft');
  }
}

function initFunc() {
  onQrTextChange();
  taQrText.bind('input propertychange', function () {
    onQrTextChange();
    generateQrCanvas(taQrText.val());
  });

  iQrTextClear.on('click', function () {
    taQrText.val('');
    taQrText.trigger('propertychange');
    taQrText.focus();
    taQrText.get(0).select();
  });

  canvasQrImgCanvas.on('click', function () {
    //utils.log('canvasQrImgCanvas.click');
    let downloadLink = $('<a/>');
    downloadLink.attr('download', 'qrcode.png');
    downloadLink.attr('href', this.toDataURL());
    downloadLink.attr('target', '_blank');
    downloadLink[0].click();
  });
}

function onQrTextChange() {
  M.textareaAutoResize(taQrText);
  let taVal = taQrText.val();
  if (taVal.length > 0) {
    iQrTextClear.show();
  } else {
    iQrTextClear.hide();
  }
}

function generateQrCanvas(text) {
  utils.log('generateQrCanvas, text=[%s]', text);
  if (text == '') {
    text = 'http://sunjw.us/qr';
  }
  qrcode.toCanvas(canvasQrImgCanvas[0], text, {
    width: qrImageWidth
  });
}

$(function () {
  utils.log('qr start...');
  initContent();
});
