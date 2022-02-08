require('@electron/remote/main').initialize()

const fs = require('fs')
const path = require('path')

const electronLocalshortcut = require('electron-localshortcut')
const windowStateKeeper = require('electron-window-state')

const utils = require('./js/utils')

// Modules to control application life and create native browser window
const {app, BrowserWindow, dialog, ipcMain} = require('electron')

// Keep a global reference of the window object, if you don't, the window will
// be closed automatically when the JavaScript object is garbage collected.
let mainWindow

let minWindowWidth = 800
let minWindowHeight = 600

function createWindow () {
  // Load the previous state with fallback to defaults
  let mainWindowState = windowStateKeeper({
    defaultWidth: minWindowWidth,
    defaultHeight: minWindowHeight
  })

  // Create the browser window.
  mainWindow = new BrowserWindow({
    x: mainWindowState.x,
    y: mainWindowState.y,
    width: mainWindowState.width,
    height: mainWindowState.height,
    minWidth: minWindowWidth,
    minHeight: minWindowHeight,
    webPreferences: {
      nodeIntegration: true,
      nodeIntegrationInWorker: true,
      contextIsolation: false,
      enableRemoteModule: true
    }
  })

  if (mainWindowState.isMaximized) {
    mainWindow.maximize()
  }
  if (mainWindowState.isFullScreen) {
    mainWindow.setFullScreen(true)
  }

  mainWindow.setMenu(null)

  // and load the index.html of the app.
  mainWindow.loadFile('index.html')

  if (!fs.existsSync(path.join(__dirname, 'assets/RELEASED'))) {
    // Open the DevTools.
    mainWindow.webContents.openDevTools()
  }

  // Emitted when the window is closed.
  mainWindow.on('closed', function () {
    // Dereference the window object, usually you would store windows
    // in an array if your app supports multi windows, this is the time
    // when you should delete the corresponding element.
    mainWindow = null
  })

  // Send Message
  mainWindow.webContents.on('did-finish-load', () => {
    mainWindow.webContents.send('set-userData-path', app.getPath('userData'))
  })

  let accelerator = 'Ctrl+F'
  if (utils.isMacOS()) {
    accelerator = 'Command+F'
  }
  electronLocalshortcut.register(mainWindow, accelerator, () => {
    mainWindow.webContents.send('on-find')
  })

  // Let us register listeners on the window, so we can update the state
  // automatically (the listeners will be removed when the window is closed)
  // and restore the maximized or full screen state
  mainWindowState.manage(mainWindow)
}

// Fix for electron-9.
app.allowRendererProcessReuse = false

// This method will be called when Electron has finished
// initialization and is ready to create browser windows.
// Some APIs can only be used after this event occurs.
app.on('ready', createWindow)

// Quit when all windows are closed.
app.on('window-all-closed', function () {
  // On macOS it is common for applications and their menu bar
  // to stay active until the user quits explicitly with Cmd + Q
  app.quit()
})

app.on('activate', function () {
  // On macOS it's common to re-create a window in the app when the
  // dock icon is clicked and there are no other windows open.
  if (mainWindow === null) createWindow()
})

ipcMain.on('dialog', (event, arg) => {
  dialog.showMessageBox(
    mainWindow, {
    type: arg.type,
    buttons: ['OK'],
    title: arg.title,
    message: arg.message
  })
})

// In this file you can include the rest of your app's specific main process
// code. You can also put them in separate files and require them here.
