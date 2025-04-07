window.$ = window.jQuery = require('jquery'); // bootstrap 5 must see window.jQuery
require('bootstrap');
const jqueryUtils = require('./jqueryUtils');

class BsDialog {

    constructor() {
        this.okText = 'OK';
        this.closeText = 'Close';
        this.loadingText = 'Working...';

        this.divModal = null;
        this.h5ModalTitle = null;
        this.divModalBody = null;
        this.divModalFooter = null;

        this.spanTips = null;
        this.buttonOk = null;
        this.buttonOkHandler = null;
        this.spanOkText = null;
        this.spanOkLoadingSpinner = null;
        this.buttonClose = null;

        this.isShown = false;
        this.pendingClose = false;

        this.dataHandler = null;
        this.showHandler = null;
        this.closeHandler = null;
    }

    init(modalId, isStatic = false, needOkButton = false, okButtonText = null) {
        let that = this;
        this.divModal = $('<div/>').attr({
            'id': modalId,
            'tabindex': -1
        }).addClass('modal fade');
        if (isStatic) {
            this.divModal.attr('data-bs-backdrop', 'static');
        }
        let divModalDialog = $('<div/>').addClass('modal-dialog modal-dialog-centered modal-dialog-scrollable');
        let divModalContent = $('<div/>').addClass('modal-content');

        let divModalHeader = $('<div/>').addClass('modal-header pb-0 border-bottom-0');
        this.h5ModalTitle = $('<h5/>').addClass('modal-title');
        divModalHeader.append(this.h5ModalTitle);
        divModalContent.append(divModalHeader);

        this.divModalBody = $('<div/>').addClass('modal-body pt-2 pb-2');
        divModalContent.append(this.divModalBody);

        this.divModalFooter = $('<div/>').addClass('modal-footer pt-0 border-top-0');
        this.spanTips = $('<span/>').addClass('flex-grow-1 text-secondary dialogTips');
        this.divModalFooter.append(this.spanTips);
        if (needOkButton) {
            if (okButtonText) {
                this.okText = okButtonText;
            }
            this.buttonOk = $('<button/>').attr({
                'type': 'button'
            }).addClass('btn btn-primary')
            this.spanOkText = $('<span/>').text(this.okText);
            this.spanOkLoadingSpinner = $('<span/>')
                .attr('role', 'status')
                .addClass('spinner-border spinner-border-sm');
            this.spanOkLoadingSpinner.hide();
            this.buttonOk.append(this.spanOkLoadingSpinner);
            this.buttonOk.append(this.spanOkText);
            this.buttonOk.on('click', function () {
                if (that.buttonOkHandler) {
                    that.buttonOkHandler();
                }
            });
            this.divModalFooter.append(this.buttonOk);
        }
        this.buttonClose = $('<button/>').attr({
            'type': 'button',
            'data-bs-dismiss': 'modal'
        }).addClass('btn btn-outline-secondary').text(this.closeText);
        this.divModalFooter.append(this.buttonClose);
        divModalContent.append(this.divModalFooter);

        divModalDialog.append(divModalContent);
        this.divModal.append(divModalDialog);

        this.divModal.on('click', function (e) {
            jqueryUtils.stopBubble(e);
        });
        this.divModal.on('shown.bs.modal', function () {
            that.isShown = true;
            if (that.showHandler) {
                that.showHandler();
            }
            if (that.pendingClose) {
                that.close();
            }
        });
        this.divModal.on('hidden.bs.modal', function () {
            that.isShown = false;
            that.onclose();
        });
    }

    onclose() {
        this.hideOkButtonLoading();
        if (this.closeHandler) {
            this.closeHandler();
        }
    }

    show() {
        this.divModal.modal('show');
    }

    close() {
        if (this.isShown) {
            this.divModal.modal('hide');
            this.pendingClose = false;
        } else {
            this.pendingClose = true;
        }
    }

    addClass(className) {
        this.divModal.addClass(className);
    }

    removeClass(className) {
        this.divModal.removeClass(className);
    }

    setTitle(titleText) {
        this.h5ModalTitle.text(titleText);
    }

    appendBody(childElement) {
        this.divModalBody.append(childElement);
    }

    setDataHandler(dataHandler) {
        this.dataHandler = dataHandler;
    }

    setData(data) {
        if (this.dataHandler) {
            this.dataHandler(data);
        }
    }

    hasShown() {
        return this.isShown;
    }

    handleUpdate() {
        this.divModal.modal('handleUpdate');
    }

    setShowHandler(showHandler) {
        this.showHandler = showHandler;
    }

    setCloseHandler(closeHandler) {
        this.closeHandler = closeHandler;
    }

    setTipsText(tipsText) {
        this.spanTips.text(tipsText);
    }

    setOkButtonHandler(okHandler) {
        if (this.buttonOk == null) {
            return;
        }
        this.buttonOkHandler = okHandler;
    }

    showOkButtonLoading() {
        if (this.buttonOk == null) {
            return;
        }
        this.buttonOk.attr('disabled', 'disabled');
        this.buttonClose.attr('disabled', 'disabled');
        this.spanOkText.text(this.loadingText);
        this.spanOkText.addClass('ms-2 loadingText');
        this.spanOkLoadingSpinner.show();
    }

    hideOkButtonLoading() {
        if (this.buttonOk == null) {
            return;
        }
        this.spanOkText.text(this.okText);
        this.spanOkText.removeClass('ms-2 loadingText');
        this.spanOkLoadingSpinner.hide();
        this.buttonOk.removeAttr('disabled');
        this.buttonClose.removeAttr('disabled');
    }

    clickOkButton() {
        if (this.buttonOk == null) {
            return;
        }
        this.buttonOk.get(0).click();
    }

    setCloseButtonText(closeText) {
        this.closeText = closeText;
        this.buttonClose.text(this.closeText);
    }
}

module.exports = BsDialog;
