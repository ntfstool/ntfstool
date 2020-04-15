import {app, BrowserWindow, Menu, Tray, ipcMain,globalShortcut,crashReporter,screen,Notification} from 'electron'
import {isDev} from "@/common/utils/AlfwCommon";
import {AlConst} from "@/common/utils/AlfwConst";
// var usbDetect = require('usb-detection');
var winURL;
var homeWinHandle =null;
var settingPageHandle = null;
var dialogPageHandle=null;
var feedBackPageHandle=null;
var trayPageHandle = null;
var tray = null;
var windowBounds=null;
var exitAllStatus = true;
const MaxBrowserWindowLimits = 50;

if(isDev()){
    winURL =  `http://localhost:9080`;
}else{
    winURL =  `file://${__dirname}/index.html`;
    global.__static = require('path').join(__dirname, '/static').replace(/\\/g, '\\\\')
}

export function doChangeLangEvent(arg) {
    console.warn("ChangeLangEvent", arg);
    if (homeWinHandle) {
        homeWinHandle.send("ChangeLangEvent", arg);
    }
}

export function doUpdateViewEvent(event,args) {
    if (homeWinHandle) {
        homeWinHandle.send(AlConst.GlobalViewUpdate);
    }

    if (trayPageHandle) {
        trayPageHandle.send(AlConst.GlobalViewUpdate);
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

export function openPages(){
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
    }, 5000)
}


export function openPageByName(name){
    if (name == "openSettingPage") {
        openSettingPage();
    } else if (name == "openAboutPage") {
        openDialogPage();
        dialogPageHandle.send("ShowDialogEvent", "showAbout");
    }else if (name == "openSudoPage") {
        openDialogPage();
        dialogPageHandle.send("ShowDialogEvent", "showSudo");
    }else if (name == "openInstallFusePage") {
        openDialogPage();
        dialogPageHandle.send("ShowDialogEvent", "showInstallFuse");
    } else if (name == "openFeedBackPage") {
        openFeedBackPage();
    } else if (name == "openHomePage") {
        openHomePage();
    }else{
        console.error(name,"openPageByName fail");
    }
}

export function exitAll(){
    //only exec once
    if(!exitAllStatus){
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

    // console.warn("usbDetect.stopMonitoring")
    // usbDetect.stopMonitoring();

    app.quit(0);
}

export function toggleTrayMenu(){
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
            homeWinHandle.show();
            if(isDev()){
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

    const iconUrl = process.env.NODE_ENV === 'development' ?  path.join(__dirname, '../../../static/menu/AINTFS18.png') :
        path.join(__dirname, 'static/menu/AINTFS18.png')

    tray = new Tray(iconUrl);

    tray.setPressedImage(iconUrl);

    tray.setIgnoreDoubleClickEvents(true);//Very important to increase click speed

    tray.on('click', (event,trayBounds) => {
        if(trayPageHandle){
            if(trayPageHandle.isVisible()){
                trayPageHandle.hide();
            }else{
                trayPageHandle.setPosition(
                    Math.round(trayBounds.x + (trayBounds.width / 2) - (windowBounds.width / 2)),
                    Math.round(trayBounds.y + trayBounds.height + 4), false)
                trayPageHandle.show()
            }
        }else{
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
            alwaysOnTop:true,
            webPreferences: {
                nodeIntegration: true
            }
        })

        dialogPageHandle.loadURL(winURL + "#dialog")

        dialogPageHandle.setMaxListeners(MaxBrowserWindowLimits)

        dialogPageHandle.once('ready-to-show', () => {
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
                    click: async () => {
                        const {shell} = require('electron')
                        await shell.openExternal("mailto:?cc=service@ntfstool.com&subject=I recommend using this NTFSTool to operate the extended disk&body=Hi!%0d%0a I\'m already using NtfsTool and I\'m really happy with it.%0d%0aFind more info here if you\'re interested:%0d%0ahttps://ntfstool.com/?tellfriends")
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
                        if(homeWinHandle){
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
                { role: 'undo' },
                { role: 'redo' },
                { type: 'separator' },
                { role: 'cut' },
                { role: 'copy' },
                { role: 'paste' },
                { role: 'pasteandmatchstyle' },
                { role: 'delete' },
                { role: 'selectall' }
            ]
        },
        {
            label: 'View',
            submenu: [
                { type: 'separator' },
                { role: 'togglefullscreen' }
            ]
        },
        {
            role: 'window',
            submenu: [
                { role: 'minimize' },
                { role: 'close' }
            ]
        },
        {
            role: 'help',
            submenu: [
                {
                    label: 'Learn More',
                    click() { require('electron').shell.openExternal('https://ntfstool.com') }
                }
            ]
        }
    ];

    Menu.setApplicationMenu(Menu.buildFromTemplate(template));
}


const  monitorUsb  =  function(){
    return;
    usbDetect.startMonitoring();

    usbDetect.on('add', function(device) {
        // add {
        //     locationId: 340787200,
        //         vendorId: 9129,
        //         productId: 61208,
        //         deviceName: 'CoolFlash USB3.1',
        //         manufacturer: 'Teclast',
        //         serialNumber: 'HJ18070000000284',
        //         deviceAddress: 17
        // }



        new Notification({
            title: `device is connected`,
            body: `${device.deviceName}`,
        }).show();


        console.log('add', device);
    });

    usbDetect.on('remove', function(device) {
        new Notification({
            title: `${device.deviceName}`,
            body: `device is disconnected`

        }).show();

        console.log('remove', device);
    });

    // usbDetect.on('change', function(device) {
    //     console.log('change', device);
    // });
    //
    usbDetect.find(function(err, devices) {
        console.log('find', devices, err);
    });
}