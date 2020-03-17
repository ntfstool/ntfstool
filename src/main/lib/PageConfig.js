import {app, BrowserWindow, Menu, Tray, ipcMain,globalShortcut,crashReporter,screen} from 'electron'
import {isDev} from "@/common/utils/AlfwCommon";
var winURL;
var homeWinHandle =null;
var settingPageHandle = null;
var dialogPageHandle=null;
var feedBackPageHandle=null;
var trayPageHandle = null;
var tray = null;
var windowBounds=null;
var aboutPageHandle = null;
var exitAllStatus = true;

if(isDev()){
    winURL =  `http://localhost:9080`;
}else{
    winURL =  `file://${__dirname}/index.html`;
    global.__static = require('path').join(__dirname, '/static').replace(/\\/g, '\\\\')
}


export function openPages(){
    openHomePage();

    openTrayPage();

    openAboutPage();//测试用

    setTimeout(function () {
        openAboutPage("hide");
        openDialogPage("");
        openSettingPage("hide");
        openFeedBackPage("hide");
    }, 5000)

    //shortcut to toggle debug window
    globalShortcut.register('Command+Shift+J', () => {
        let focusWin = BrowserWindow.getFocusedWindow()
        focusWin && focusWin.toggleDevTools()
    });
}


export function openPageByName(name){
    if (name == "openSettingPage") {
        openSettingPage();
    } else if (name == "openAboutPage") {
        openAboutPage();
    }else if (name == "openDialogPage") {
        openDialogPage();
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
    if (aboutPageHandle) {
        aboutPageHandle.destroy();
    }
    if (dialogPageHandle) {
        dialogPageHandle.destroy();
    }
    if (feedBackPageHandle) {
        feedBackPageHandle.destroy();
    }
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

        homeWinHandle.once('ready-to-show', () => {
            _homeWinMenu();
            homeWinHandle.show();
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

const openAboutPage = (show_force) => {
    if (aboutPageHandle == null) {
        //已下为插入内容
        aboutPageHandle = new BrowserWindow({
            title: "",
            fullscreen: false,
            height: 265,
            width: 400,
            show: false,
            backgroundColor: 'rgb(243, 243, 243)',
            resizable: false,
            minimizable: false,
            maximizable: false,
            // skipTaskbar: true,
            webPreferences: {
                nodeIntegration: true
            }
        })

        aboutPageHandle.loadURL(winURL + "#about")

        aboutPageHandle.once('ready-to-show', () => {
            if (show_force !== "hide") {
                aboutPageHandle.show()
            }
        })

        aboutPageHandle.on('close', (event) => {
            aboutPageHandle.hide();
            event.preventDefault();
        });

        aboutPageHandle.on('closed', () => {
            aboutPageHandle = null
        });

        aboutPageHandle.on('close', (event) => {
            aboutPageHandle.hide();
            event.preventDefault();
        });
    } else {
        aboutPageHandle.show()
    }
}

const openDialogPage = (show_force) => {
    if (dialogPageHandle == null) {
        //已下为插入内容
        dialogPageHandle = new BrowserWindow({
            title: "系统设置",
            fullscreen: false,
            height: 210,
            width: 500,
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

        dialogPageHandle.once('ready-to-show', () => {
            if (show_force !== "hide") {
                dialogPageHandle.show()
            }

            dialogPageHandle.webContents.openDevTools()
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
        feedBackPageHandle = new BrowserWindow({
            fullscreen: false,
            height: 500,
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
            label: '关闭',
            click: function () {
                win.close();
                console.log("关闭")
            },
            submenu: [
                {
                    label: '关于',
                    click: async () => {
                        openAboutPage();
                    }
                },
                {
                    label: '分享给朋友',
                    click: async () => {
                        const {shell} = require('electron')
                        await shell.openExternal("mailto:?cc=service@ntfstool.com&subject=I recommend using this NTFSTool to operate the extended disk&body=Hi!%0d%0a I\'m already using NtfsTool and I\'m really happy with it.%0d%0aFind more info here if you\'re interested:%0d%0ahttps://ntfstool.com/?tellfriends")
                    }
                },
                {type: 'separator'},
                {
                    label: '偏好设置',
                    click: async () => {
                        openSettingPage();
                    }
                },
                {
                    label: '检查更新',
                    click: async () => {
                        console.warn("检查更新");
                    }
                },
                {role: 'services'},
                {
                    label: '隐藏桌面',
                    click: async () => {
                        if(homeWinHandle){
                            homeWinHandle.hide();
                            homeWinHandle.setSkipTaskbar(true);
                            app.dock.hide()
                        }
                    }
                },
                {
                    label: '提交反馈',
                    click: async () => {
                        openFeedBackPage();
                    }
                },
                {type: 'separator'},
                {
                    label: '退出',
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