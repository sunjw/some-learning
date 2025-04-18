const { ipcRenderer, webUtils } = require('electron');
const remote = require('@electron/remote');

const fs = require('fs');
const path = require('path');

window.$ = window.jQuery = require('jquery');
const bootstrap = require('bootstrap');
const probeImageSize = require('probe-image-size');
const wallpaper = require('@sunjw8888/wallpaper');
const sharp = require('sharp');

const utils = require('./utils');
const jqueryUtils = require('./jqueryUtils');
const nodeUtils = require('./nodeUtils');
const eleUtils = require('./eleUtils');
const sqliteDb = require('./sqliteDb');

const TAG_IMAGE_SRC = 'data-image-src';
const TAG_IMAGE_SRC_WIDTH = 'data-image-src-width';
const TAG_IMAGE_SRC_HEIGHT = 'data-image-src-height';
const TAG_IMAGE_SRC_SIZE = 'data-image-src-size';
const TAG_IMAGE_THUMB_SRC = 'data-image-thumb-src';
const TAG_IMAGE_PLACEHOLDER = 'data-image-placeholder';

function handleImageIntersection(entries) {
    // utils.log('handleImageIntersection');
    let showCount = 0;
    let hideCount = 0;
    for (let entryDom of entries) {
        let entryObj = $(entryDom.target);
        let entryImageSrc = entryObj.attr(TAG_IMAGE_SRC);
        let entryImageThumbSrc = entryObj.attr(TAG_IMAGE_THUMB_SRC);
        let entryImagePlaceholder = entryObj.attr(TAG_IMAGE_PLACEHOLDER);
        if (entryImageThumbSrc) {
            entryImageSrc = entryImageThumbSrc;
        }
        if (entryDom.isIntersecting) {
            entryObj.attr('src', entryImageSrc);
            entryObj.addClass('imageFull');
            showCount++;
        } else {
            entryObj.attr('src', entryImagePlaceholder);
            entryObj.removeClass('imageFull');
            hideCount++;
        }
        // utils.log('handleImageIntersection, showCount=%d, hideCount=%d', showCount, hideCount);
    }
}

const obsImageLazyload = new IntersectionObserver(handleImageIntersection);

class WallpickerPage {

    constructor() {
        // init consts
        this.TAG_IMAGE_PATH = 'data-image-path';
        this.TAG_IMAGE_NAME = 'data-image-name';
        this.TAG_SORT_ID = 'data-sort-id';
        this.THUMBNAIL_FORMAT = 'png';
        this.THUMBNAIL_EXPIRE = 60 * 60 * 24 * 7; // 7 days
        this.SCAN_MIN_TIME = 2000; // ms
        this.KEY_IMAGE_DIR = 'imageDir';
        this.KEY_SORT = 'sort';

        this.TIP_DROP_SCAN = 'Drop a directory to scan...';
        this.TIP_NO_DIR_DROP = 'No directory dropped.';
        this.TIP_DROP_ONE_DIR = 'Drop only one directory.';
        this.TIP_READ_DIR_ERROR = 'Read directory error.';

        this.CLASS_IMAGE_SELECTED = 'imageSelected';
        this.CLASS_FILETER_OUT = 'filterOut';
    }

    init(options) {
        let that = this;

        this.title = 'Wallpicker';

        this.imageExts = ['jpg', 'jpeg', 'png'];
        this.imagePlaceholder = 'assets/placeholder.png';
        this.imageBlockWidth = 230; // width + margin
        this.maxImagePreviewWidth = 200;
        this.maxImagePreviewHeight = 160;
        this.maxImagePreviewRatio = this.maxImagePreviewWidth / this.maxImagePreviewHeight;

        this.webWorkerImage = new Worker('js/imageWorker.js');

        this.options = options;
        this.eleConfig = new eleUtils.EleConfig(this.options.userDataPath);
        this.curImageDir = this.getConfig(this.KEY_IMAGE_DIR);
        this.curImageList = [];
        this.scanStart = 0;
        this.curFilter = '';
        this.sortOptions = {};
        this.curSortId = '';

        this.selectedImageBlock = null;
        this.imageThumbDir = path.join(this.options.userDataPath, 'imageThumb');
        this.imageThumbDbPath = path.join(this.options.userDataPath, 'thumbnail.sqlite');
        this.curGenThumbIndex = 0;
        this.genThumbStart = 0;
        this.genThumbCount = 0;
        this.curImageThumbMap = new Map();

        this.body = $('body');
        this.divToolbarWrapper = null;
        this.divPathWrapper = null;
        this.divToolsWrapper = null;
        this.divPathDropInfo = null;
        this.divPathContent = null;
        this.divFilterBlock = null;
        this.inputFilter = null;
        this.divFilterClear = null;
        this.btnToolbarRefresh = null;
        this.btnToolbarRandom = null;
        this.btnToolbarSetWallpaper = null;
        this.btnToolbarShowFile = null;
        this.btnToolbarShowSelected = null;
        this.btnToolbarSort = null;
        this.divContentWrapper = null;
        this.divImageList = null;
        this.divLoadingWrapper = null;
        this.divLoadingBlock = null;
        this.divToastWrapperLeft = null;
        this.divToastWrapperRight = null;

        this.initWorker();
        this.initLayout();
        this.initHandler();

        this.onWindowResize();
        $(window).on('resize', function () {
            that.onWindowResize();
        });
        $(window).on('scroll', function () {
            that.onWindowScroll();
        });

        this.disableAllButtons();
        this.hideLoading();

        this.checkAndLoadImageDir();
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

    initWorker() {
        let that = this;
        if (!fs.existsSync(this.imageThumbDir)) {
            utils.log('initWorker, make thumbnail directory [%s]', this.imageThumbDir);
            fs.mkdirSync(this.imageThumbDir);
        }

        let onInitWorker = function (result) {
            if (result.err) {
                utils.log('initWorker.onInitWorker, init table error, err=\n%s', err.message);
            } else {
                utils.log('initWorker.onInitWorker, create table success.');
            }
        };
        let onScanImageDir = function (result) {
            if (result.err) {
                eleUtils.sendToMain({
                    type: 'warning',
                    title: that.title,
                    message: that.TIP_READ_DIR_ERROR
                });
                return;
            }
            that.curImageList = result.scanImageList;
            let scanEnd = utils.getCurTimestamp();
            let scanDuration = scanEnd - that.scanStart;
            if (scanDuration < that.SCAN_MIN_TIME) {
                let waitSome = that.SCAN_MIN_TIME - scanDuration;
                utils.log('initWorker.onScanImageDir, scanDuration=%dms, waitSome=%dms', scanDuration, waitSome);
                setTimeout(() => {
                    that.renderAll();
                }, waitSome);
            } else {
                utils.log('initWorker.onScanImageDir, scanDuration=%dms', scanDuration);
                that.renderAll();
            }
        };
        let onGenerateImageThumbnail = function (result) {
            that.processImageThumbnailResult(result);
            that.generateImageThumbnailNext();
        };
        let onUpdateImageThumbnailDb = function (result) {
            if (result.err) {
                utils.log('initWorker.onUpdateImageThumbnailDb, error, err=\n%s', err.message);
            } else {
                // utils.log('initWorker.onUpdateImageThumbnailDb, create table success.');
            }
        };
        let onClearImageThumbnail = function (result) {
            if (result.err) {
                utils.log('initWorker.onClearImageThumbnail, error, err=\n%s', err.message);
            } else {
                utils.log('initWorker.onClearImageThumbnail, finished, dbChanges=%d, imageThumbDeleted=%d',
                    result.dbChanges, result.imageThumbDeleted);
            }
        };

        const messageHandlerMap = {
            'initWorker': onInitWorker,
            'scanImageDir': onScanImageDir,
            'generateImageThumbnail': onGenerateImageThumbnail,
            'updateImageThumbnailDb': onUpdateImageThumbnailDb,
            'clearImageThumbnail': onClearImageThumbnail
        };

        this.webWorkerImage.onmessage = function (e) {
            let messageData = e.data;
            let messageHandler = messageHandlerMap[messageData.messageId];
            if (!messageHandler) {
                utils.log('initWorker.webWorkerImage, unknown messageId=[%s]', messageData.messageId);
                return;
            }
            messageHandler(messageData);
        };

        // init worker
        let initWorkerOptions = {
            'messageId': 'initWorker',
            'imageThumbDbPath': this.imageThumbDbPath
        }
        this.webWorkerImage.postMessage(initWorkerOptions);
    }

    onWindowResize() {
        // on window resize
        this.fitContentHeight();
        this.fitImageListWidth();
    }

    onWindowScroll() {
        let windowScrollTop = $(window).scrollTop();
        if (windowScrollTop >= 6) {
            this.divToolbarWrapper.addClass('toolbarScrolled');
        } else {
            this.divToolbarWrapper.removeClass('toolbarScrolled');
        }
    }

    initLayout() {
        let that = this;
        // init layout

        // init toolbar
        this.divToolbarWrapper = $('<div/>')
            .attr('id', 'divToolbarWrapper')
            .addClass('d-flex justify-content-between');

        // init path
        this.divPathWrapper = $('<div/>')
            .attr('id', 'divPathWrapper')
            .addClass('flex-shrink-1 overflow-hidden');
        this.divPathDropInfo = $('<div/>')
            .attr('id', 'divPathDropInfo')
            .text(this.TIP_DROP_SCAN);
        this.divPathContent = $('<div/>')
            .attr('id', 'divPathContent')
            .addClass('text-truncate');
        if (!this.curImageDir) {
            this.divPathContent.text(this.TIP_DROP_SCAN);
        }
        this.divPathContent.on('click', function () {
            that.scrollToTop();
        });
        this.divPathWrapper.append(this.divPathDropInfo);
        this.divPathWrapper.append(this.divPathContent);

        this.divToolbarWrapper.append(this.divPathWrapper);

        // init tools
        this.divToolsWrapper = $('<div/>')
            .attr('id', 'divToolsWrapper')
            .addClass('d-flex ps-2');

        this.divFilterBlock = $('<div/>')
            .attr('id', 'divFilterBlock')
            .addClass('d-flex me-2');
        this.inputFilter = $('<input/>')
            .attr({
                'id': 'inputFilter',
                'typ': 'text',
                'placeholder': 'filter path...',
                'aria-label': '.form-control-sm'
            })
            .addClass('form-control form-control-sm mt-auto mb-auto noBsFocus');
        jqueryUtils.inputOnTextChanged(this.inputFilter, function () {
            that.onFilterTextChanged();
        });
        jqueryUtils.inputOnEnter(this.inputFilter, function () {
            that.onFilter();
        });
        this.divFilterBlock.append(this.inputFilter);
        this.divFilterClear = $('<div/>')
            .attr('id', 'divFilterClear');
        let iClose = $('<i/>').addClass('bi bi-x filterClear');
        iClose.on('click', function () {
            utils.log('iClose.click');
            let thisObj = $(this);
            thisObj.addClass('focus');
            that.inputFilter.val('');
            jqueryUtils.focusOnInput(that.inputFilter);
            that.onFilter();
            setTimeout(() => {
                that.onFilterTextChanged();
                thisObj.removeClass('focus');
            }, 250);
        });
        this.divFilterClear.append(iClose);
        this.divFilterBlock.append(this.divFilterClear);
        this.onFilterTextChanged();
        this.divToolsWrapper.append(this.divFilterBlock);

        this.btnToolbarRefresh = this.generateButton('btnToolbarRefresh', 'bi-arrow-clockwise', 'Random select');
        this.autoBlurButtonClick(this.btnToolbarRefresh, function () {
            utils.log('btnToolbarRefresh.click');
            that.checkAndLoadImageDir();
        });
        this.divToolsWrapper.append(this.btnToolbarRefresh);

        this.btnToolbarRandom = this.generateButton('btnToolbarRandom', 'bi-lightbulb', 'Random select');
        this.autoBlurButtonClick(this.btnToolbarRandom, function () {
            utils.log('btnToolbarRandom.click');
            that.randomSelect();
        });
        this.divToolsWrapper.append(this.btnToolbarRandom);

        this.btnToolbarSetWallpaper = this.generateButton('btnToolbarSetWallpaper', 'bi-image', 'Set wallpaper');
        this.autoBlurButtonClick(this.btnToolbarSetWallpaper, function () {
            utils.log('btnToolbarSetWallpaper.click');
            that.setOsWallpaper();
        });
        this.divToolsWrapper.append(this.btnToolbarSetWallpaper);

        this.btnToolbarShowFile = this.generateButton('btnToolbarShowFile', 'bi-folder2-open', 'Show file');
        this.autoBlurButtonClick(this.btnToolbarShowFile, function () {
            utils.log('btnToolbarShowFile.click');
            that.openImageInDirectory();
        });
        this.divToolsWrapper.append(this.btnToolbarShowFile);

        this.btnToolbarShowSelected = this.generateButton('btnToolbarShowSelected', 'bi-fullscreen-exit', 'Show selected image');
        this.autoBlurButtonClick(this.btnToolbarShowSelected, function () {
            utils.log('btnToolbarShowSelected.click');
            that.scrollToSelected();
        });
        this.divToolsWrapper.append(this.btnToolbarShowSelected);

        this.initSort();

        this.divToolbarWrapper.append(this.divToolsWrapper);

        this.body.append(this.divToolbarWrapper);

        // init content
        this.divContentWrapper = $('<div/>')
            .attr('id', 'divContentWrapper')
            .on('click', function () {
                utils.log('divContentWrapper click');
                that.clearSelection();
            });

        // image list
        this.divImageList = $('<div/>')
            .attr('id', 'divImageList')
            .addClass('ms-auto me-auto d-flex align-content-start flex-wrap');
        this.divContentWrapper.append(this.divImageList);

        // loading
        this.divLoadingWrapper = $('<div/>').attr('id', 'divLoadingWrapper');
        this.divLoadingBlock = $('<div/>')
            .attr('id', 'divLoadingBlock')
            .addClass('d-flex justify-content-center align-items-center');
        let divLoadingSpinner = $('<div/>')
            .attr({
                'role': 'status',
                'aria-hidden': 'true'
            })
            .addClass('spinner-border');
        let divLoadingText = $('<div/>')
            .attr('id', 'divLoadingText')
            .text('Scanning...');
        this.divLoadingBlock.append(divLoadingSpinner);
        this.divLoadingBlock.append(divLoadingText);
        this.divLoadingWrapper.append(this.divLoadingBlock);
        this.divContentWrapper.append(this.divLoadingWrapper);

        this.body.append(this.divContentWrapper);

        // toast
        this.divToastWrapperLeft = $('<div/>').attr('id', 'divToastWrapperLeft');
        this.divToastWrapperRight = $('<div/>').attr('id', 'divToastWrapperRight');
        this.body.append(this.divToastWrapperLeft);
        this.body.append(this.divToastWrapperRight);
    }

    initSort() {
        let that = this;
        let sortByTime = {
            'id': 'sortByTime',
            'icon': 'bi-sort-numeric-down-alt',
            'title': 'Sort by time',
            'sortImageFunc': function (fo1, fo2) {
                return (fo2.mtime - fo1.mtime); // modified time reverse
            },
            'next': 'sortByName'
        };
        let sortByName = {
            'id': 'sortByName',
            'icon': 'bi-sort-alpha-down',
            'title': 'Sort by name',
            'sortImageFunc': function (fo1, fo2) {
                // basename
                let bn1 = fo1.basename;
                let bn2 = fo2.basename;
                if (bn1 < bn2) {
                    return -1;
                }
                if (bn1 > bn2) {
                    return 1;
                }
                return 0;
            },
            'next': 'sortByTime'
        };
        this.sortOptions[sortByTime.id] = sortByTime;
        this.sortOptions[sortByName.id] = sortByName;

        let savedSort = this.getConfig(this.KEY_SORT);
        if (!savedSort || savedSort == '') {
            savedSort = sortByTime.id;
        }

        this.curSortId = savedSort;
        let savedSortOption = this.sortOptions[this.curSortId];
        this.btnToolbarSort = this.generateButton('btnToolbarSort', savedSortOption.icon, savedSortOption.title);
        this.btnToolbarSort.attr(this.TAG_SORT_ID, savedSortOption.id);
        this.autoBlurButtonClick(this.btnToolbarSort, function (button) {
            let sortId = button.attr(that.TAG_SORT_ID);
            let curSortOption = that.sortOptions[sortId];
            let nextSortOption = that.sortOptions[curSortOption.next];
            utils.log('btnToolbarSort.click, sortId=[%s], nextSortId=[%s]', curSortOption.id, nextSortOption.id);
            that.setConfig(that.KEY_SORT, nextSortOption.id);
            // change button
            that.btnToolbarSort.attr(that.TAG_SORT_ID, nextSortOption.id);
            that.btnToolbarSort.find('i').removeClass(curSortOption.icon);
            that.btnToolbarSort.find('i').addClass(nextSortOption.icon);
            that.btnToolbarSort.attr('title', nextSortOption.title);
            // render
            that.curSortId = nextSortOption.id;
            let oldSelectedImageBlock = that.selectedImageBlock;
            that.clearDisplayImageList();
            that.renderImageList();
            if (oldSelectedImageBlock) {
                // restore select
                let imagePath = oldSelectedImageBlock.attr(that.TAG_IMAGE_PATH);
                let imageBlock = that.getImageBlockByImagePath(imagePath);
                that.selectImage(imageBlock);
            }
        });
        this.divToolsWrapper.append(this.btnToolbarSort);
    }

    initHandler() {
        let that = this;

        let lastenter = null;
        let divToolbarWrapperDom = this.divToolbarWrapper.get(0);
        divToolbarWrapperDom.ondragenter = function (e) {
            lastenter = e.target;
        };
        divToolbarWrapperDom.ondragover = function (e) {
            // utils.log('divToolbarWrapperDom.ondragover');
            eleUtils.stopBubble(e);
            that.divToolbarWrapper.addClass('dropInfo');
            return false;
        };
        divToolbarWrapperDom.ondragleave = function (e) {
            // utils.log('divToolbarWrapperDom.ondragleave');
            if (lastenter == e.target) {
                that.divToolbarWrapper.removeClass('dropInfo');
            }
            return false;
        };
        divToolbarWrapperDom.ondragend = function () {
            // utils.log('divToolbarWrapperDom.ondragend');
            that.divToolbarWrapper.removeClass('dropInfo');
            return false;
        };
        divToolbarWrapperDom.ondrop = function (e) {
            e.preventDefault();
            that.divToolbarWrapper.removeClass('dropInfo');
            that.onDropFiles(e.dataTransfer.files);
            return false;
        };
    }

    fitContentHeight() {
        let windowHeight = $(window).height();
        let divContentWrapperTop = this.divContentWrapper.offset().top;
        let contentHeight = windowHeight - divContentWrapperTop - 2;
        // this.divContentWrapper.css('height', contentHeight + 'px');
        this.divLoadingBlock.css('height', contentHeight + 'px');
    }

    fitImageListWidth() {
        let contentWrapperWidth = this.divContentWrapper.width();
        let imagePerLine = Math.floor(contentWrapperWidth / this.imageBlockWidth);
        let divImageListWidth = imagePerLine * this.imageBlockWidth + 2;
        this.divImageList.css('width', divImageListWidth + 'px');
    }

    generateButton(buttonId, iconName, title) {
        let button = $('<button/>').attr({
            'id': buttonId,
            'type': 'button'
        }).addClass('btn btn-light align-self-center noBsFocus toolbarBtn');
        if (title) {
            button.attr('title', title);
        }
        let buttonIcon = $('<i/>').addClass('bi').addClass(iconName);
        button.append(buttonIcon);
        return button;
    }

    autoBlurButtonClick(button, handler) {
        let that = this;
        button.on('click', function () {
            handler($(this));
            that.resetButtonBlur(button);
        });
    }

    resetButtonBlur(button) {
        setTimeout(function () {
            button.blur();
        }, 250);
    }

    setControlDisabled(control, disabled) {
        if (disabled) {
            control.attr('disabled', 'disabled');
        } else {
            control.removeAttr('disabled');
        }
    }

    showLoading() {
        utils.log('showLoading');
        this.divLoadingWrapper.show();
    }

    hideLoading() {
        utils.log('hideLoading');
        this.divLoadingWrapper.hide();
    }

    onFind() {
        jqueryUtils.focusOnInput(this.inputFilter);
    }

    onDropFiles(dropFiles) {
        let that = this;
        if (dropFiles.length <= 0) {
            eleUtils.sendToMain({
                type: 'warning',
                title: this.title,
                message: this.TIP_NO_DIR_DROP
            });
            return;
        }
        if (dropFiles.length > 1) {
            eleUtils.sendToMain({
                type: 'warning',
                title: this.title,
                message: this.TIP_DROP_ONE_DIR
            });
            return;
        }
        let dropPath = webUtils.getPathForFile(dropFiles[0]);
        utils.log('onDropFiles, dropPath=[%s]', dropPath);
        fs.lstat(dropPath, (err, stats) => {
            if (err || !stats.isDirectory()) {
                eleUtils.sendToMain({
                    type: 'warning',
                    title: that.title,
                    message: that.TIP_DROP_ONE_DIR
                });
                return;
            }

            // a foler, go!
            that.curImageDir = dropPath;
            that.setConfig(that.KEY_IMAGE_DIR, that.curImageDir);
            that.loadImageDir();
        });
    }

    checkAndLoadImageDir() {
        if (this.curImageDir) {
            this.loadImageDir();
        }
    }

    loadImageDir() {
        let that = this;
        this.curGenThumbIndex = -100;
        this.renderPath();
        this.clearImageList();
        this.disableAllButtons();
        this.showLoading();
        setTimeout(() => {
            that.scanDirInWorker();
        }, 100);
    }

    scanDirInWorker() {
        this.scanStart = utils.getCurTimestamp();
        let scanImageDirOptions = {
            'messageId': 'scanImageDir',
            'imageDirPath': this.curImageDir,
            'imageExts': this.imageExts,
            'imageThumbDir': this.imageThumbDir,
            'imageThumbFormat': this.THUMBNAIL_FORMAT,
            'imageThumbExpire': this.THUMBNAIL_EXPIRE
        }
        this.webWorkerImage.postMessage(scanImageDirOptions);
    }

    renderPath() {
        let that = this;

        // clear
        this.divPathContent.empty();

        // render
        let pathParts = this.curImageDir.split(path.sep);
        if (pathParts[0] == '') {
            pathParts = pathParts.slice(1);
        }
        utils.log('renderPath, pathPart=[%s]', pathParts.join(','));
        let pathRender = '';
        let pathPartsLen = pathParts.length;
        for (let i = 0; i < pathPartsLen - 1; i++) {
            pathRender += (' ' + path.sep + ' ' + pathParts[i]);
        }
        pathRender += (' ' + path.sep + ' ');
        if (utils.isWindows()) {
            pathRender = pathRender.substring(3);
        }
        pathRender = pathRender.trimStart();
        this.divPathContent.append($('<span/>').html(utils.escapeHtml(pathRender)));
        let spanPathDirname = $('<span/>')
            .addClass('pathDirname')
            .html(utils.escapeHtml(pathParts[pathParts.length - 1]));
        // spanPathDirname.on('click', function () {
        //     that.scrollToTop();
        // });
        this.divPathContent.append(spanPathDirname);
    }

    clearImageList() {
        utils.log('clearImageList');
        this.curImageList = [];
        this.clearDisplayImageList();
        this.inputFilter.val('');
        this.curFilter = '';
        this.onFilterTextChanged();
    }

    clearDisplayImageList() {
        this.divImageList.empty();
        this.clearSelection();
    }

    sortImageList() {
        let curSortOption = this.sortOptions[this.curSortId];
        utils.log('sortImageList, sortId=[%s]', curSortOption.id);
        this.curImageList.sort(curSortOption.sortImageFunc);
    }

    renderAll() {
        this.renderImageList();

        this.hideLoading();
        this.refreshButtonState();

        // toast
        this.showToast('Found <span class="highlight">' + this.curImageList.length + '</span> images.',
            'right', true);

        // start generate thumbnail
        this.generateImageThumbnailStart();
    }

    renderImageList() {
        let that = this;

        // sort
        this.sortImageList();

        // render
        for (let fileObj of this.curImageList) {
            let filePath = fileObj.path;
            let divImageBlock = $('<div/>')
                .attr(this.TAG_IMAGE_PATH, filePath)
                .addClass('imageBlock');

            let fnClickToSelectImage = function () {
                utils.log('renderImageList, fnClickToSelectImage.');
                let imgParentBlock = divImageBlock;
                that.selectImage(imgParentBlock);
            };

            // let fileObjJson = utils.objToJsonBeautify(fileObj);
            // fileObjJson = utils.escapeHtml(fileObjJson);
            // fileObjJson = utils.stringReplaceAll(fileObjJson, '\n', '<br/>');
            // divImageBlock.html(fileObjJson);

            let divImageWrapper = $('<div/>').addClass('d-flex justify-content-center imageWrapper');
            let divInfoWrapper = $('<div/>').addClass('d-flex justify-content-center infoWrapper');

            let imgContent = $('<img/>')
                .attr('src', this.imagePlaceholder)
                .addClass('align-self-end rounded imageContent');
            imgContent.attr(TAG_IMAGE_SRC, filePath);
            // imgContent.attr(TAG_IMAGE_SRC_WIDTH, fileObj.width);
            // imgContent.attr(TAG_IMAGE_SRC_HEIGHT, fileObj.height);
            // imgContent.attr(TAG_IMAGE_SRC_SIZE, utils.convertSizeToShortSize(fileObj.size));
            imgContent.attr(TAG_IMAGE_PLACEHOLDER, this.imagePlaceholder);
            if (fileObj.thumbPath) {
                imgContent.attr(TAG_IMAGE_THUMB_SRC, fileObj.thumbPath);
            }
            let imagePreviewWidth = fileObj.width;
            let imagePreviewHeight = fileObj.height;
            let imageRatio = fileObj.width / fileObj.height;
            if (imageRatio > this.maxImagePreviewRatio) {
                imagePreviewWidth = this.maxImagePreviewWidth;
                imagePreviewHeight = imagePreviewWidth / imageRatio;
            } else {
                imagePreviewHeight = this.maxImagePreviewHeight;
                imagePreviewWidth = imagePreviewHeight * imageRatio;
            }
            fileObj.previewWidth = imagePreviewWidth;
            fileObj.previewHeight = imagePreviewHeight;
            imgContent.css({
                'width': imagePreviewWidth + 'px',
                'height': imagePreviewHeight + 'px'
            });
            // lazy
            obsImageLazyload.observe(imgContent.get(0));
            imgContent.on('click', function (e) {
                eleUtils.stopBubble(e);
                fnClickToSelectImage();
            });
            imgContent.on('dblclick', function (e) {
                eleUtils.stopBubble(e);
                // fnClickToSelectImage();
                that.openImage();
            });
            divImageWrapper.append(imgContent);

            let imageMetaInfo = fileObj.width + 'x' + fileObj.height +
                ' ' + utils.convertSizeToShortSize(fileObj.size);
            let imageMetaIcon = $('<i/>').addClass('imageMeta bi bi-info-circle');
            let imageMetaIconRight = 6;
            if (imagePreviewWidth < this.maxImagePreviewWidth) {
                imageMetaIconRight = imageMetaIconRight + ((this.maxImagePreviewWidth - imagePreviewWidth) / 2);
            }
            imageMetaIcon.css('right', imageMetaIconRight + 'px');
            let imageMetaIconTipPlace = 'left';
            if (imagePreviewWidth < (this.maxImagePreviewWidth / 2)) {
                imageMetaIconTipPlace = 'top';
            }
            imageMetaIcon.attr({
                'data-bs-toggle': 'tooltip',
                'data-bs-placement': imageMetaIconTipPlace,
                'data-bs-delay': '{"show":400,"hide":500}',
                // 'data-bs-trigger': 'click',
                'data-bs-title': imageMetaInfo
            });
            imageMetaIcon.on('click', function (e) {
                eleUtils.stopBubble(e);
                fnClickToSelectImage();
            });
            new bootstrap.Tooltip(imageMetaIcon.get(0));
            divImageWrapper.append(imageMetaIcon);

            let divImageInfo = $('<div/>').addClass('align-self-start rounded imageInfo');
            let imageBasenameFull = fileObj.basename;
            let imageBasename = imageBasenameFull;
            // let imageExtname = fileObj.extname;
            if (imageBasename.length > 45) {
                let imageBasenameShortLen = 36;
                if (utils.isMacOS()) {
                    imageBasenameShortLen = 32;
                }
                let imageBasenameFix = imageBasename.substring(0, imageBasenameShortLen);
                imageBasenameFix += '...';
                imageBasenameFix += imageBasename.substring(imageBasename.length - 8, imageBasename.length);
                imageBasename = imageBasenameFix;
            }
            divImageInfo.attr('title', imageBasenameFull);
            divImageInfo.attr(this.TAG_IMAGE_NAME, imageBasenameFull);
            divImageInfo.html(utils.escapeHtml(imageBasename));
            divImageInfo.on('click', function (e) {
                eleUtils.stopBubble(e);
                fnClickToSelectImage();
            });
            divImageInfo.on('dblclick', function (e) {
                eleUtils.stopBubble(e);
                // fnClickToSelectImage();
                that.openImage();
            });
            divInfoWrapper.append(divImageInfo);

            divImageBlock.append(divImageWrapper);
            divImageBlock.append(divInfoWrapper);

            this.filterImageBlock(divImageBlock);

            this.divImageList.append(divImageBlock);
        }
    }

    refreshButtonState() {
        if (this.selectedImageBlock) {
            // selected
            this.setControlDisabled(this.btnToolbarSetWallpaper, false);
            this.setControlDisabled(this.btnToolbarShowFile, false);
            this.setControlDisabled(this.btnToolbarShowSelected, false);
        } else {
            // no selection
            this.setControlDisabled(this.btnToolbarSetWallpaper, true);
            this.setControlDisabled(this.btnToolbarShowFile, true);
            this.setControlDisabled(this.btnToolbarShowSelected, true);
        }
        this.setControlDisabled(this.inputFilter, false);
        this.setControlDisabled(this.btnToolbarRefresh, false);
        this.setControlDisabled(this.btnToolbarRandom, false);
        this.setControlDisabled(this.btnToolbarSort, false);
    }

    disableAllButtons() {
        this.setControlDisabled(this.inputFilter, true);
        this.setControlDisabled(this.btnToolbarRefresh, true);
        this.setControlDisabled(this.btnToolbarSetWallpaper, true);
        this.setControlDisabled(this.btnToolbarShowFile, true);
        this.setControlDisabled(this.btnToolbarShowSelected, true);
        this.setControlDisabled(this.btnToolbarRandom, true);
        this.setControlDisabled(this.btnToolbarSort, true);
    }

    getFileObjectByImagePath(imagePath) {
        for (let fileObj of this.curImageList) {
            if (fileObj.path == imagePath) {
                return fileObj;
            }
        }
        return null;
    }

    getAllImageBlocks() {
        return this.divImageList.find('.imageBlock');
    }

    getFirstVisibleImageBlock() {
        let imageBlocks = this.getAllImageBlocks();
        for (let imageBlockDom of imageBlocks) {
            let imageBlock = $(imageBlockDom);
            if (!imageBlock.hasClass(this.CLASS_FILETER_OUT)) {
                return imageBlock;
            }
        }
        return null;
    }

    getImageBlockByFileObject(fileObj) {
        return this.getImageBlockByImagePath(fileObj.path);
    }

    getImageBlockByImagePath(targetImagePath) {
        let imageBlocks = this.getAllImageBlocks();
        for (let imageBlockDom of imageBlocks) {
            let imageBlock = $(imageBlockDom);
            let imagePath = imageBlock.attr(this.TAG_IMAGE_PATH);
            if (imagePath == targetImagePath) {
                return imageBlock;
            }
        }
        return null;
    }

    clearSelection(refreshButton = true) {
        utils.log('clearSelection');
        let imageBlocks = this.getAllImageBlocks();
        imageBlocks.removeClass(this.CLASS_IMAGE_SELECTED);
        this.selectedImageBlock = null;
        if (refreshButton) {
            this.refreshButtonState();
        }
    }

    selectImage(imageBlock) {
        this.clearSelection(false);
        utils.log('selectImage, imageBlock=[%s]', imageBlock.attr(this.TAG_IMAGE_PATH));
        imageBlock.addClass(this.CLASS_IMAGE_SELECTED);
        this.selectedImageBlock = imageBlock;
        this.refreshButtonState();
    }

    onFilterTextChanged() {
        let filterVal = this.inputFilter.val();
        utils.log('onFilterTextChanged, filterVal=[%s]', filterVal);
        if (filterVal.length > 0) {
            this.divFilterClear.show();
        } else {
            this.divFilterClear.hide();
        }
    }

    onFilter() {
        this.curFilter = this.inputFilter.val().trim();
        this.curFilter = this.curFilter.toLowerCase();
        utils.log('onFilter, curFilter=[%s]', this.curFilter);
        this.filterAllImageBlocks();
    }

    filterAllImageBlocks() {
        let imageBlocks = this.getAllImageBlocks();
        if (this.curFilter == '') {
            // show all
            imageBlocks.removeClass(this.CLASS_FILETER_OUT);
        } else {
            for (let imageBlockDom of imageBlocks) {
                let imageBlock = $(imageBlockDom);
                this.filterImageBlock(imageBlock);
            }
        }

        if (this.selectedImageBlock &&
            this.selectedImageBlock.hasClass(this.CLASS_FILETER_OUT)) {
            // selection is hidden
            this.clearSelection();
        }
    }

    filterImageBlock(imageBlock) {
        let filterWords = this.curFilter.split(' ');
        let imagePath = imageBlock.attr(this.TAG_IMAGE_PATH);
        // let imageInfo = imageBlock.find('.imageInfo');
        // let imageName = imageInfo.attr(this.TAG_IMAGE_NAME);
        // let filterTarget = imageName.toLowerCase();
        let filterTarget = imagePath.toLowerCase();
        let allMatch = false;
        for (let filterWord of filterWords) {
            if (filterTarget.includes(filterWord)) {
                allMatch = true;
            } else {
                allMatch = false;
                break;
            }
        }
        if (allMatch) {
            // show
            imageBlock.removeClass(this.CLASS_FILETER_OUT);
        } else {
            // hide
            imageBlock.addClass(this.CLASS_FILETER_OUT);
        }
    }

    randomSelect() {
        if (this.curImageList.length == 0) {
            return;
        }
        let randomImagePath = null;
        let imageBlocks = this.getAllImageBlocks();
        let imageBlockRandom = null;
        while (true) {
            let randomImage = this.curImageList[utils.getRandomInt(this.curImageList.length)];
            randomImagePath = randomImage.path;
            for (let imageBlockDom of imageBlocks) {
                let imageBlock = $(imageBlockDom);
                if (imageBlock.hasClass(this.CLASS_FILETER_OUT)) {
                    // filter out, skip
                    continue;
                }
                let imagePath = imageBlock.attr(this.TAG_IMAGE_PATH);
                if (imagePath == randomImagePath) {
                    imageBlockRandom = imageBlock;
                    break;
                }
            }
            if (imageBlockRandom) {
                break;
            }
        }
        utils.log('randomSelect, randomImagePath=[%s]', randomImagePath);
        this.selectImage(imageBlockRandom);
        this.scrollToSelected();
    }

    setOsWallpaper() {
        let imagePath = this.selectedImageBlock.attr(this.TAG_IMAGE_PATH);
        utils.log('setWallpaper, imagePath=[%s]', imagePath);
        setTimeout(() => {
            wallpaper.set(imagePath);
        }, 1000);
        this.showToast('Set wallpaper to "<span class="highlight">' + imagePath + '</span>".',
            'right', true);
    }

    openImageInDirectory() {
        let imagePath = this.selectedImageBlock.attr(this.TAG_IMAGE_PATH);
        utils.log('openImageInDirectory, imagePath=[%s]', imagePath);
        remote.shell.showItemInFolder(imagePath);
    }

    openImage() {
        let imagePath = this.selectedImageBlock.attr(this.TAG_IMAGE_PATH);
        utils.log('openImage, imagePath=[%s]', imagePath);
        remote.shell.openPath(imagePath);
    }

    scrollToImageBlockDom(imageBlockDom) {
        imageBlockDom.scrollIntoView({
            behavior: 'smooth',
            block: 'center'
        });
    }

    scrollToTop() {
        utils.log('scrollToTop');
        let imageBlockFirstVis = this.getFirstVisibleImageBlock();
        if (imageBlockFirstVis) {
            let imageBlockFirstVisDom = imageBlockFirstVis.get(0);
            this.scrollToImageBlockDom(imageBlockFirstVisDom);
        }
    }

    scrollToSelected() {
        utils.log('scrollToSelected');
        let imageBlockDom = this.selectedImageBlock.get(0);
        this.scrollToImageBlockDom(imageBlockDom);
    }

    showToast(message, position, autoHide) {
        utils.log('showToast, position=%s, autoHide=%s', position, autoHide);

        let divToast = $('<div/>').attr({
            'role': 'alert',
            'aria-live': 'assertive',
            'aria-atomic': 'true'
        }).addClass('toast d-flex');

        if (autoHide) {
            divToast.attr({
                'data-bs-autohide': 'true',
                'data-bs-delay': '10000'
            });
        } else {
            divToast.attr('data-bs-autohide', 'false');
        }

        let divToastBody = $('<div/>')
            .addClass('toast-body flex-grow-1')
            .html(message);
        divToast.append(divToastBody);
        let buttonToastClose = $('<button/>').attr({
            'type': 'button',
            'data-bs-dismiss': 'toast',
            'aria-label': 'Close'
        }).addClass('btn-close m-auto me-2 noBsFocus');
        divToast.append(buttonToastClose);

        if (position == 'left') {
            this.divToastWrapperLeft.append(divToast);
        } else if (position == 'right') {
            this.divToastWrapperRight.append(divToast);
        }

        divToast.on('hidden.bs.toast', function () {
            // remove self
            utils.log('showToast, clear.');
            setTimeout(() => {
                divToast.remove();
            }, 2000);
        });

        divToast.toast('show');
    }

    generateImageThumbnailPath(fileObj) {
        return path.join(this.imageThumbDir, fileObj.hash + '.' + this.THUMBNAIL_FORMAT);
    }

    generateImageThumbnailInWorker() {
        if (this.curGenThumbIndex < 0) {
            utils.log('generateImageThumbnailInWorker, stop');
            return;
        }

        let imageListLen = this.curImageList.length;
        if (this.curGenThumbIndex >= imageListLen) {
            let genThumbEnd = utils.getCurTimestamp();
            let genThumbDuration = genThumbEnd - this.genThumbStart;
            utils.log('generateImageThumbnailInWorker, finished, genThumbCount=[%d], curImageThumbMap=[%d], genThumbDuration=%dms',
                this.genThumbCount, this.curImageThumbMap.size, genThumbDuration);
            this.clearImageThumbnailInWorker();
            return;
        }

        if (this.curGenThumbIndex == 0) {
            this.genThumbStart = utils.getCurTimestamp();
            this.genThumbCount = 0;
            utils.log('generateImageThumbnailInWorker, start...');
        }

        // utils.log('generateImageThumbnailInWorker, %d/%d', (this.curGenThumbIndex + 1), imageListLen);
        let fileObj = this.curImageList[this.curGenThumbIndex];
        // utils.log('generateImageThumbnailInWorker, fileObj=\n%s', utils.objToJsonBeautify(fileObj));

        let imagePath = fileObj.path;
        if (fileObj.thumbPath) {
            // found when init
            // utils.log('generateImageThumbnailInWorker, skip found thumbnail.');
            let result = {
                'imageSrcPath': imagePath,
                'imageThumbPath': fileObj.thumbPath
            };
            this.processImageThumbnailResult(result);
            this.generateImageThumbnailNext();
            return;
        }

        let imageThumbWidth = Math.floor(fileObj.previewWidth * 2);
        let imageThumbHeight = Math.floor(fileObj.previewHeight * 2);
        if (fileObj.width <= imageThumbWidth && fileObj.height <= imageThumbHeight) {
            utils.log('generateImageThumbnailInWorker, skip small image, imagePath=[%s]', imagePath);
            this.generateImageThumbnailNext();
            return;
        }

        let imageThumbPath = this.generateImageThumbnailPath(fileObj);
        if (this.checkImageThumbnail(imageThumbPath)) {
            // already generated
            utils.log('generateImageThumbnailInWorker, skip generated thumbnail.');
            let result = {
                'imageSrcPath': imagePath,
                'imageThumbPath': imageThumbPath
            };
            this.processImageThumbnailResult(result);
            this.generateImageThumbnailNext();
            return;
        }

        this.genThumbCount++;
        utils.log('generateImageThumbnailInWorker, [%d]', this.genThumbCount);
        let generateImageThumbnailOptions = {
            'messageId': 'generateImageThumbnail',
            'imageSrcPath': imagePath,
            'imageThumbPath': imageThumbPath,
            'imageThumbWidth': imageThumbWidth,
            'imageThumbHeight': imageThumbHeight,
            'imageThumbFormat': this.THUMBNAIL_FORMAT
        }
        this.webWorkerImage.postMessage(generateImageThumbnailOptions);
    }

    generateImageThumbnailStart() {
        this.curGenThumbIndex = 0;
        this.generateImageThumbnailInWorker();
    }

    generateImageThumbnailNext() {
        this.curGenThumbIndex++;
        this.generateImageThumbnailInWorker();
    }

    processImageThumbnailResult(result) {
        let imageSrcPath = result.imageSrcPath;
        let imageThumbPath = result.imageThumbPath;
        if (result.err) {
            utils.log('initWorker.webWorkerImage, error, imageSrcPath=[%s], err=\n%s', imageSrcPath, result.err);
            if (fs.existsSync(imageThumbPath)) {
                fs.unlink(imageThumbPath, (err) => {
                    if (err) {
                        utils.log('initWorker.webWorkerImage, unlink error, imageThumbPath=[%s], err=\n%s',
                            imageThumbPath, err);
                    } else {
                        utils.log('initWorker.webWorkerImage, unlink imageThumbPath=[%s]',
                            imageThumbPath);
                    }
                });
            }
            return;
        }

        // success
        // utils.log('initWorker.webWorkerImage, result=\n%s', utils.objToJsonBeautify(result));
        if (!this.curImageThumbMap.has(imageThumbPath)) {
            // utils.log('initWorker.webWorkerImage, new imageThumbPath=[%s]', imageThumbPath);
            this.curImageThumbMap.set(imageThumbPath, []);
        }
        let imageThumbItemArray = this.curImageThumbMap.get(imageThumbPath);
        if (!imageThumbItemArray.includes(imageSrcPath)) {
            imageThumbItemArray.push(imageSrcPath);
        }
        if (imageThumbItemArray.length > 1) {
            utils.log('initWorker.webWorkerImage, found the same images\n%s', utils.objToJsonBeautify(imageThumbItemArray));
        }

        this.updateImageThumbnailDb(imageThumbPath);
        this.processImageBlockThumb(imageSrcPath, imageThumbPath);
    }

    updateImageThumbnailDb(imageThumbPath) {
        let updateImageThumbnailDbOptions = {
            'messageId': 'updateImageThumbnailDb',
            'imageThumbPath': imageThumbPath
        }
        this.webWorkerImage.postMessage(updateImageThumbnailDbOptions);
    }

    checkImageThumbnail(imageThumbPath) {
        if (!fs.existsSync(imageThumbPath)) {
            return false;
        }

        let imgData = fs.readFileSync(imageThumbPath);
        let imgDim = probeImageSize.sync(imgData);
        if (!imgDim) {
            return false;
        }

        return true;
    }

    processImageBlockThumb(imageSrcPath, imageThumbPath) {
        let fileObj = this.getFileObjectByImagePath(imageSrcPath);
        if (!fileObj || fileObj.thumbPath) {
            return;
        }
        fileObj.thumbPath = imageThumbPath;

        let imageBlock = this.getImageBlockByFileObject(fileObj);
        if (!imageBlock) {
            return;
        }
        let imageContent = imageBlock.find('.imageContent');
        imageContent.attr(TAG_IMAGE_THUMB_SRC, fileObj.thumbPath);
        if (imageContent.hasClass('imageFull')) {
            imageContent.attr('src', fileObj.thumbPath);
            utils.log('processImageBlockThumb, replace shown image, imagePath=[%s]', fileObj.path);
        }
    }

    clearImageThumbnailInWorker() {
        let clearImageThumbnailOptions = {
            'messageId': 'clearImageThumbnail'
        }
        this.webWorkerImage.postMessage(clearImageThumbnailOptions);
    }
}

let wallpickerPage = new WallpickerPage();

ipcRenderer.on('set-userData-path', (e, arg) => {
    utils.log('set-userData-path, arg=[%s]', arg);

    // let's go.
    wallpickerPage.init({
        userDataPath: arg
    });
});

ipcRenderer.on('on-find', (e, args) => {
    wallpickerPage.onFind();
});

$(function () {
    utils.log('finish loading.');
});
