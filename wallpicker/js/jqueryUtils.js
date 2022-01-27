function focusOnInput(inputElem) {
    inputElem.focus();
    inputElem.get(0).select();
}

function stopBubble(event) {
    // only for browser
    event = event ? event : window.event;
    if (window.event) {
        event.cancelBubble = true;
    } else {
        event.stopPropagation();
    }
}

function formOnSubmit(form, submitCallback) {
    form.on('submit', function (e) {
        e.preventDefault();
        submitCallback(e);
    });
}

function inputOnTextChanged(input, changedCallback) {
    input.on('input propertychange', function (e) {
        changedCallback(e);
    });
}

function inputOnEnter(input, enterCallback) {
    input.on('keydown', function (e) {
        if (e.code == 'Enter' || e.code == 'NumpadEnter') {
            e.preventDefault();
            enterCallback(e);
        }
    });
}

// exports
exports.focusOnInput = focusOnInput;
exports.stopBubble = stopBubble;
exports.formOnSubmit = formOnSubmit;
exports.inputOnTextChanged = inputOnTextChanged;
exports.inputOnEnter = inputOnEnter;
