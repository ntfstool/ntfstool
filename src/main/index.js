/**
 * @author   service@ntfstool.com
 * Copyright (c) 2020 ntfstool.com
 * Copyright (c) 2020 alfw.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the MIT General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * MIT General Public License for more details.
 *
 * You should have received a copy of the MIT General Public License
 * along with this program (in the main directory of the NTFS Tool
 * distribution in the file COPYING); if not, write to the service@ntfstool.com
 */

import {app, BrowserWindow, Menu, Tray, ipcMain,globalShortcut} from 'electron'

const winURL = process.env.NODE_ENV === 'development'
    ? `http://localhost:9080`
    : `file://${__dirname}/index.html`

if (process.env.NODE_ENV !== 'development') {
    global.__static = require('path').join(__dirname, '/static').replace(/\\/g, '\\\\')
}
const Store = require('electron-store');
const store = new Store();
app.disableHardwareAcceleration();//disable gpu
process.env.ELECTRON_DISABLE_SECURITY_WARNINGS = '1';
var settingPageHandle = null,aboutPageHandle=null,feedBackPageHandle=null,trayHandle = null,tray = null,homeWinHandle;

const default_store = {
    name: "Alntfs",
    auto_run:true,
    theme: "",
    lang: "en",
    show_menu: true,
    common: {
        website_url: "",
        install_bug_type: "auto_solve",
        how_restart_window: "change_to_bacground",
    },
    message: {
        mount_show_msg: "",
        update_show_msg: "",
        error_disk_msg: "",
    },
    disk_list: {
        history_list: [],
        ignore_list: [],
    },
    privacy_url: 'https://github.com',
    update: {
        auto_check: "",
        auto_beta_update: "",
        update_url: "",
        update_beta_url: "",
    },
    sudoPwd:false,
};


app.on('ready', () => {
    if (!store.get("name") || store.get("name") == "undefined") {
        initStore();
    }
    
    homeWindow();
    
    trayBox();

    globalShortcut.register('Command+Shift+J', () => {
        let focusWin = BrowserWindow.getFocusedWindow()
        focusWin && focusWin.toggleDevTools()
    });
})

app.on('activate', () => {
    if (homeWinHandle === null) {
        homeWindow()
    }
})

app.on('window-all-closed', () => {
    if (process.platform !== 'darwin') {
        app.quit()
    }
})

//主进程监听消息
ipcMain.on('MainMsgFromRender', function (event, arg) {
    console.warn(arg, "主进程监听到消息")
    if (arg == "openSysSeeting") {
        openSysSeeting();
        event.sender.send('RenderMsgFromMain', arg + " succ#############")
    }else if(arg == "openAboutPage"){
        openAboutPage();
    } else if(arg == "openFeedBackPage"){
        openFeedBackPage();
    } else if(arg == "resetConf"){
        store.set(default_store);
        event.returnValue = 'succ';
    } else {
        event.sender.send('RenderMsgFromMain', arg + " fail##############")
    }
})


//监听关闭开启菜单图标
ipcMain.on('toggleTrayMenu', function (event, arg) {
    if (tray !== null) {
        tray.destroy();//立刻删除 Tray 图标
        tray = null;
        event.returnValue = 'destroy';
    } else {
        openTrayMenu();
        event.returnValue = 'newopen';
    }
})



ipcMain.on('ChangeLangEvent', function (event, arg) {
    console.warn("ChangeLangEvent",arg);
    if(homeWinHandle){
        homeWinHandle.send("ChangeLangEvent",arg);
    }
})


const homeWindow = () => {
    homeWinHandle = new BrowserWindow({
        show: false,
        fullscreen: false,
        height: 600,
        minHeight: 600,
        minWidth: 800,
        width: 900,
        maxWidth: 900,
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
        console.error("homeWinHandle close start.........")
        // homeWinHandle.hide();
        // homeWinHandle.setSkipTaskbar(true);
        // app.dock.hide()
        // event.preventDefault();

        // setTimeout(function () {
        //     app.dock.show()
        // },10000)
    });
}

const _homeWinMenu = () => {

    // //左上角菜单栏
    // const contextMenu = Menu.buildFromTemplate([
    //     {
    //         label: '退出', click: function () {
    //             app.quit()
    //         }
    //     },
    //     {
    //         label: '退出2', click: function () {
    //             app.quit()
    //         }
    //     },
    //     {
    //         label: 'Edit',
    //         submenu: [
    //             { role: 'undo' },
    //             { role: 'redo' },
    //             { type: 'separator' },
    //             { role: 'cut' },
    //             { role: 'copy' },
    //             { role: 'paste' },
    //             [
    //                 { role: 'pasteAndMatchStyle' },
    //                 { role: 'delete' },
    //                 { role: 'selectAll' },
    //                 { type: 'separator' },
    //                 {
    //                     label: 'Speech',
    //                     submenu: [
    //                         { role: 'startspeaking' },
    //                         { role: 'stopspeaking' }
    //                     ]
    //                 }
    //             ]
    //         ]
    //     }
    // ])
    // tray.setToolTip('测试气泡提示文字')
    // tray.setContextMenu(contextMenu)


    //左上角菜单栏
    var template = [
        {
            label: '关闭',
            click: function () {
                win.close();
                console.log("关闭")
            },
            submenu: [
                {
                    label: '关于AlNtfs',
                    click: async () => {
                        const {shell} = require('electron')
                        await shell.openExternal('https://ntfstool.com')
                    }
                },
                {type: 'separator'},
                {
                    label: '偏好设置',
                    click: async () => {
                        openSysSeeting();
                    }
                },
                {role: 'services'},
                {type: 'separator'},
                {
                    label: '退出AlNtfs',
                    accelerator: 'CmdOrCtrl+Z',
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
                {role: 'delete'},
                {role: 'selectAll'},
                {type: 'separator'},
            ]
        }


    ];

    Menu.setApplicationMenu(Menu.buildFromTemplate(template));
}

//默认菜单栏
const trayBox = () => {
    if (trayHandle == null) {
        //已下为插入内容
        const path = require('path')
        trayHandle = new BrowserWindow({
            height: 100,
            width: 380,
            frame: false,
            resizable: false,
            show: false,
            transparent: true,
            webPreferences: {
                nodeIntegration: true,
                backgroundThrottling: false
            }
        })

        trayHandle.once('ready-to-show', () => {

            trayHandle.openDevTools()

            if (store.get("show_menu") != false) {
                openTrayMenu();
            }
        })

        trayHandle.loadURL(winURL + "#tray")
        // settingPageHandle.webContents.closeDevTools()

        trayHandle.on('closed', () => {
            trayHandle = null
        })

        trayHandle.on('blur', () => {
            // trayHandle.hide();
        })

    } else {
        trayHandle.show()
    }
}

//右上角菜单栏
const openTrayMenu = () => {
    const path = require('path');
    tray = new Tray(path.join(__dirname, "../renderer/assets/menu/AINTFS18.png"));
    tray.setPressedImage(path.join(__dirname, "../renderer/assets/menu/AINTFS_active18.png"));
   tray.on('click', () => toggletrayHandledow())
}

const toggletrayHandledow = () => {
    trayHandle.isFocused() ? trayHandle.hide() : showWindow()
}

const showWindow = () => {
    const position = getWindowPosition()
    trayHandle.setPosition(position.x, position.y, false)
    trayHandle.show()
    trayHandle.focus()
}

const getWindowPosition = () => {
    const windowBounds = trayHandle.getBounds();
    const trayBounds = tray.getBounds();

    // Center window horizontally below the tray icon
    const x = Math.round(trayBounds.x + (trayBounds.width / 2) - (windowBounds.width / 2))
    // Position window 4 pixels vertically below the tray icon
    const y = Math.round(trayBounds.y + trayBounds.height + 4)
    return {x, y}
}


//设置进程调用
const openSysSeeting = () => {
    if (settingPageHandle == null) {
        //已下为插入内容
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
        // settingPageHandle.webContents.closeDevTools();//关闭dev工具

        settingPageHandle.once('ready-to-show', () => {
            settingPageHandle.show()

            settingPageHandle.webContents.openDevTools();
        })

        settingPageHandle.on('closed', () => {
            settingPageHandle = null
        })
    } else {
        settingPageHandle.show()
    }
}


//关于我们
const openAboutPage = () => {
    if (aboutPageHandle == null) {
        //已下为插入内容
        aboutPageHandle = new BrowserWindow({
            title:"",
            fullscreen: false,
            height: 210,
            width: 350,
            // useContentSize: true,
            // center: true,
            // frame: false,
            // titleBarStyle: 'show',
            show: false,
            backgroundColor:'rgb(243, 243, 243)',
            resizable: false,
            minimizable: false,
            maximizable: false,
            // skipTaskbar: true,
            // webPreferences: {
            //     nodeIntegration: true
            // }
        })

        aboutPageHandle.loadURL(winURL + "#about")

        aboutPageHandle.once('ready-to-show', () => {
            aboutPageHandle.show()
        })

        aboutPageHandle.on('closed', () => {
            aboutPageHandle = null
        })
    } else {
        aboutPageHandle.show()
    }
}

//反馈
const openFeedBackPage = () => {
    if (feedBackPageHandle == null) {
        //已下为插入内容
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
            feedBackPageHandle.show()
            feedBackPageHandle.webContents.openDevTools();
        })

        feedBackPageHandle.on('closed', () => {
            feedBackPageHandle = null
        })
    } else {
        feedBackPageHandle.show()
    }
}

//默认设置全局配置
const initStore = () => {
    store.set(default_store);
    console.warn("initStore run");
}

