const { app, BrowserWindow } = require('electron');
const path = require('path');
const { ipcMain } = require('electron');

let isWin = (process.platform === "win32") || (process.platform === "win64");
let isMacOs = process.platform === "darwin";
let isLinux = process.platform === "linux";

console.log(isWin);
console.log(isMacOs);
console.log(isLinux);

function createWindow () {
  const win = new BrowserWindow({
    width: 1600,
    height: 600,
    webPreferences: {
      preload: path.join(__dirname, 'preload.js'),
      nodeIntegration: true,
      contextIsolation: false
    }
  });

  win.loadFile('index.html');
  win.webContents.openDevTools();
}

app.whenReady().then(() => {
  createWindow();

  app.on('activate', () => {
    if (BrowserWindow.getAllWindows().length === 0) {
      createWindow();
    }
  });
});

app.on('window-all-closed', () => {
  if (process.platform !== 'darwin') {
    app.quit();
  }
});

app.on('resized', () => {
  console.log('window has been resized');
});

ipcMain.on('synchronous-message', (event, arg) => {
  if (arg == 'getPlatform') {
    if (isWin) event.returnValue = 'win';
    if (isMacOs) event.returnValue = "macos";
    if (isLinux) event.returnValue = "linux";
  } else {
    console.log(arg) // prints "ping"
    event.returnValue = 'pongus'
  }
});
