import {app, BrowserWindow, Menu, Tray, ipcMain, globalShortcut, crashReporter, screen, Notification} from 'electron'
import {isDev} from "@/common/utils/AlfwCommon";
import {AlConst} from "@/common/utils/AlfwConst";

var usbDetect = require('usb-detection');
var fs = require("fs")
var winURL;
var homeWinHandle = null;
var settingPageHandle = null;
var dialogPageHandle = null;
var feedBackPageHandle = null;
var trayPageHandle = null;
var tray = null;
var windowBounds = null;
var exitAllStatus = true;
const MaxBrowserWindowLimits = 50;

if (isDev()) {
    winURL = `http://localhost:9080`;
} else {
    winURL = `file://${__dirname}/index.html`;
    global.__static = require('path').join(__dirname, '/static').replace(/\\/g, '\\\\')
}

export function doChangeLangEvent(arg) {
    console.warn("ChangeLangEvent", arg);
    if (homeWinHandle) {
        homeWinHandle.send("ChangeLangEvent", arg);
    }

    if (trayPageHandle) {
        trayPageHandle.send("ChangeLangEvent", arg);
    }

    if (dialogPageHandle) {
        dialogPageHandle.send("ChangeLangEvent", arg);
    }

    if (feedBackPageHandle) {
        feedBackPageHandle.send("ChangeLangEvent", arg);
    }
}

export function doUpdateViewEvent(event, args) {
    if (homeWinHandle) {
        homeWinHandle.send(AlConst.GlobalViewUpdate);
    }

    if (trayPageHandle) {
        trayPageHandle.send(AlConst.GlobalViewUpdate);
    }
}

export function doCreteFileEvent(arg) {
    if (homeWinHandle) {
        homeWinHandle.send("CreteFileEvent", arg);
    }

    if (trayPageHandle) {
        trayPageHandle.send("CreteFileEvent", arg);
    }
}

export function doUsbDeleteFileEvent(arg) {
    if (homeWinHandle) {
        homeWinHandle.send("UsbDeleteFileEvent", arg);
    }
}

export function doUsbAddFileEvent(arg) {
    if (homeWinHandle) {
        homeWinHandle.send("UsbAddFileEvent", arg);
    }
}


export function doSudoPwdEvent(arg) {
    // console.warn("SudoPwdEvent", arg);
    // if (dialogPageHandle) {
    //     dialogPageHandle.send("SudoPwdEvent", arg);
    // }
}


export function doDesktopAppEvent(args) {
    console.warn("DesktopAppEvent", arg);
    if (homeWinHandle) {
        homeWinHandle.send("DesktopAppEvent", arg);
    }
    if (trayPageHandle) {
        trayPageHandle.send("DesktopAppEvent", arg);
    }
}

export function openPages() {
    //shortcut to toggle debug window
    globalShortcut.register('Command+Shift+J', () => {
        let focusWin = BrowserWindow.getFocusedWindow()
        focusWin && focusWin.toggleDevTools()
    });

    openHomePage();

    openTrayPage();

    monitorUsb();

    setTimeout(function () {
        openDialogPage("hide");
        openSettingPage("hide");
        openFeedBackPage("hide");
    }, 3000)
}

export function openPageByName(name) {
    if (name == "openSettingPage") {
        openSettingPage();
    } else if (name == "openAboutPage") {
        openDialogPage();
        dialogPageHandle.send("ShowDialogEvent", "showAbout");
    } else if (name == "openSudoPage") {
        openDialogPage();
        dialogPageHandle.send("ShowDialogEvent", "showSudo");
    } else if (name == "openInstallFusePage") {
        openDialogPage();
        dialogPageHandle.send("ShowDialogEvent", "showInstallFuse");
    } else if (name == "openFeedBackPage") {
        openFeedBackPage();
    } else if (name == "openHomePage") {
        openHomePage();
    } else {
        console.error(name, "openPageByName fail");
    }
}

export function exitAll() {
    try {
        usbDetect.stopMonitoring();
        
        //only exec once
        if (!exitAllStatus) {
            return;
        }

        exitAllStatus = false;

        if (homeWinHandle) {
            homeWinHandle.destroy();
        }
        if (tray) {
            tray.destroy();
        }
        if (settingPageHandle) {
            settingPageHandle.destroy();
        }
        if (dialogPageHandle) {
            dialogPageHandle.destroy();
        }
        if (feedBackPageHandle) {
            feedBackPageHandle.destroy();
        }

        app.quit(0);
    } catch (e) {
        console.error(e,"exitAll");
    }
}

export function toggleTrayMenu() {
    if (tray !== null) {
        tray.destroy();
        tray = null;
        event.returnValue = 'destroy';
    } else {
        openTrayMenu();
        event.returnValue = 'newopen';
    }
}

const openHomePage = () => {
    if (homeWinHandle == null) {
        console.warn("create new openHomePage")
        homeWinHandle = new BrowserWindow({
            show: false,
            fullscreen: false,
            height: 600,
            minHeight: 600,
            minWidth: 800,
            width: 900,
            maxWidth: 1200,
            useContentSize: true,
            // center: true,
            frame: false,
            titleBarStyle: 'hidden',
            webPreferences: {
                experimentalFeatures: true,
                nodeIntegration: true
            },
            // transparent: true
        })

        homeWinHandle.loadURL(winURL);

        homeWinHandle.setMaxListeners(MaxBrowserWindowLimits)

        homeWinHandle.once('ready-to-show', () => {
            _homeWinMenu();

            var loginItemSettings = app.getLoginItemSettings();
            if (loginItemSettings && typeof loginItemSettings.wasOpenedAtLogin != "undefined" && loginItemSettings.wasOpenedAtLogin == true) {
                homeWinHandle.hide();
            } else {
                homeWinHandle.show();
            }

            if (isDev()) {
                homeWinHandle.webContents.openDevTools()
            }
        })

        homeWinHandle.on('close', (event) => {
            console.error("homeWinHandle close start")
            homeWinHandle.hide();
            homeWinHandle.setSkipTaskbar(true);
            app.dock.hide()
            event.preventDefault();
        });


        homeWinHandle.on('closed', (event) => {
            console.error("homeWinHandle closed")
        });
    } else {
        homeWinHandle.show();
        homeWinHandle.setSkipTaskbar(false);
        app.dock.show()
    }
}


//default tray menu
const openTrayPage = () => {
    if (trayPageHandle == null) {
        console.warn("create new trayPageHandle")
        trayPageHandle = new BrowserWindow({
            height: 100,
            width: 360,
            frame: false,
            resizable: false,
            show: false,
            transparent: true,
            webPreferences: {
                nodeIntegration: true,
                backgroundThrottling: false
            }
        })

        trayPageHandle.loadURL(winURL + "#tray")

        trayPageHandle.setMaxListeners(MaxBrowserWindowLimits)

        trayPageHandle.once('ready-to-show', () => {
            windowBounds = trayPageHandle.getBounds();
            openTrayMenu();
        })

        trayPageHandle.on('closed', () => {
            trayPageHandle = null
        })

        trayPageHandle.on('blur', () => {
            trayPageHandle.hide();
        })
    } else {
        trayPageHandle.show()
    }
}

//right tray menu
const openTrayMenu = () => {
    const path = require('path');

    const iconUrl = process.env.NODE_ENV === 'development' ? path.join(__dirname, '../../../static/menu/AINTFS18.png') :
        path.join(__dirname, 'static/menu/AINTFS18.png')

    tray = new Tray(iconUrl);

    tray.setPressedImage(iconUrl);

    tray.setIgnoreDoubleClickEvents(true);//Very important to increase click speed

    tray.on('click', (event, trayBounds) => {
        if (trayPageHandle) {
            if (trayPageHandle.isVisible()) {
                trayPageHandle.hide();
            } else {
                trayPageHandle.setPosition(
                    Math.round(trayBounds.x + (trayBounds.width / 2) - (windowBounds.width / 2)),
                    Math.round(trayBounds.y + trayBounds.height + 4), false)
                trayPageHandle.show()
            }
        } else {
            //Todo log error
            exitAll();
        }
    })
}

const openSettingPage = (show_force) => {
    if (settingPageHandle == null) {
        console.warn("create new settingPageHandle")
        settingPageHandle = new BrowserWindow({
            fullscreen: false,
            height: 500,
            width: 750,
            useContentSize: true,
            center: true,
            frame: false,
            titleBarStyle: 'hidden',
            show: false,
            resizable: false,
            minimizable: false,
            maximizable: false,
            skipTaskbar: true,
            webPreferences: {
                nodeIntegration: true
            }
        })

        settingPageHandle.loadURL(winURL + "#setting")

        settingPageHandle.setMaxListeners(MaxBrowserWindowLimits)

        settingPageHandle.once('ready-to-show', () => {
            if (show_force !== "hide") {
                settingPageHandle.show()
            }
        })

        settingPageHandle.on('close', (event) => {
            settingPageHandle.hide();
            event.preventDefault();
        });

        settingPageHandle.on('closed', () => {
            settingPageHandle = null
        })
    } else {
        settingPageHandle.show()
    }
}

const openDialogPage = (show_force) => {
    if (dialogPageHandle == null) {
        console.warn("create new dialogPageHandle")
        dialogPageHandle = new BrowserWindow({
            fullscreen: false,
            height: 300,
            width: 300,
            show: false,
            backgroundColor: 'rgb(243, 243, 243)',
            resizable: false,
            minimizable: false,
            maximizable: false,
            alwaysOnTop: true,
            webPreferences: {
                nodeIntegration: true
            }
        })

        dialogPageHandle.loadURL(winURL + "#dialog")

        dialogPageHandle.setMaxListeners(MaxBrowserWindowLimits)

        dialogPageHandle.once('ready-to-show', () => {
            if (!fs.existsSync("/Library/Frameworks/OSXFUSE.framework")) {
                dialogPageHandle.send("ShowDialogEvent", "showInstallFuse");
                dialogPageHandle.show()
            }

            if (show_force !== "hide") {
                dialogPageHandle.show()
            }
        })

        dialogPageHandle.on('close', (event) => {
            dialogPageHandle.hide();
            event.preventDefault();
        });

        dialogPageHandle.on('closed', () => {
            dialogPageHandle = null
        });

        dialogPageHandle.on('close', (event) => {
            dialogPageHandle.hide();
            event.preventDefault();
        });
    } else {
        dialogPageHandle.show()
    }
}

const openFeedBackPage = (show_force) => {
    if (feedBackPageHandle == null) {
        console.warn("create new feedBackPageHandle")
        feedBackPageHandle = new BrowserWindow({
            fullscreen: false,
            height: 560,
            width: 700,
            useContentSize: true,
            center: true,
            frame: false,
            titleBarStyle: 'hidden',
            show: false,
            resizable: false,
            minimizable: false,
            maximizable: false,
            skipTaskbar: true,
            webPreferences: {
                nodeIntegration: true
            }
        })

        feedBackPageHandle.loadURL(winURL + "#feedBack")

        feedBackPageHandle.setMaxListeners(MaxBrowserWindowLimits)

        feedBackPageHandle.once('ready-to-show', () => {
            if (show_force !== "hide") {
                feedBackPageHandle.show()
            }
        })

        feedBackPageHandle.on('close', (event) => {
            feedBackPageHandle.hide();
            event.preventDefault();
        });

        feedBackPageHandle.on('closed', () => {
            feedBackPageHandle = null
        })
    } else {
        feedBackPageHandle.show()
    }
}


const _homeWinMenu = () => {
    var template = [
        {
            label: 'Close',
            click: function () {
                win.close();
            },
            submenu: [
                {
                    label: 'About',
                    click: async () => {
                        openPageByName("openAboutPage");
                    }
                },
                {
                    label: 'Share',
                    click: () => {
                        trayPageHandle.send("OpenShare");
                    }
                },
                {type: 'separator'},
                {
                    label: 'preferences',
                    click: async () => {
                        openSettingPage();
                    }
                },
                {
                    label: 'Check update',
                    click: async () => {
                        console.warn("Checkforupdates");
                    }
                },
                {role: 'services'},
                {
                    label: 'Hide Desktop',
                    click: async () => {
                        if (homeWinHandle) {
                            homeWinHandle.hide();
                            homeWinHandle.setSkipTaskbar(true);
                            app.dock.hide()
                        }
                    }
                },
                {
                    label: 'Submit feedback',
                    click: async () => {
                        openFeedBackPage();
                    }
                },
                {type: 'separator'},
                {
                    label: 'Quit',
                    accelerator: 'CmdOrCtrl+Q',
                    role: 'quit'
                },
            ],
        },
        {
            label: 'Edit',
            submenu: [
                {role: 'undo'},
                {role: 'redo'},
                {type: 'separator'},
                {role: 'cut'},
                {role: 'copy'},
                {role: 'paste'},
                {role: 'pasteandmatchstyle'},
                {role: 'delete'},
                {role: 'selectall'}
            ]
        },
        {
            label: 'View',
            submenu: [
                {type: 'separator'},
                {role: 'togglefullscreen'}
            ]
        },
        {
            role: 'window',
            submenu: [
                {role: 'minimize'},
                {role: 'close'}
            ]
        },
        {
            role: 'help',
            submenu: [
                {
                    label: 'Learn More',
                    click() {
                        require('electron').shell.openExternal('https://ntfstool.com')
                    }
                }
            ]
        }
    ];

    Menu.setApplicationMenu(Menu.buildFromTemplate(template));
}


const monitorUsb = function () {
    try {
        usbDetect.startMonitoring();

        usbDetect.on('add', function (device) {
            console.warn('usbDeviceMonitorAdd', device);
            doUsbAddFileEvent(device.deviceName);
        });
        usbDetect.on('remove', function (device) {
            console.warn('usbDeviceMonitorRemove', device.deviceName);
            doUsbDeleteFileEvent(device.deviceName);
        });
    } catch (e) {
        console.error(e, "usbDeviceMonitor Error")
    }
}