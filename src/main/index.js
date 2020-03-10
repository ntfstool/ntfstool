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

if (process.env.NODE_ENV !== 'development') {
    global.__static = require('path').join(__dirname, '/static').replace(/\\/g, '\\\\')
}
const Store = require('electron-store');

const store = new Store();

app.disableHardwareAcceleration();//disable gpu

process.env.ELECTRON_DISABLE_SECURITY_WARNINGS = '1';

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

var settingWin = null;
var trayWin = null;
var tray = null
let mainWindow

const winURL = process.env.NODE_ENV === 'development'
    ? `http://localhost:9080`
    : `file://${__dirname}/index.html`

const initWindow = () => {
    mainWindow = new BrowserWindow({
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

    console.log(winURL, "winURL");
    mainWindow.loadURL(winURL);

    mainWindow.once('ready-to-show', () => {
        mainWindow.show()
    })

    mainWindow.on('closed', () => {
        mainWindow = null
    })


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


    ]
    var menu = Menu.buildFromTemplate(template)
    Menu.setApplicationMenu(menu);

}

//右上角菜单栏
const openTrayMenu = () => {
    // const path = require('path');
    // tray = new Tray(path.join(__dirname, "../renderer/assets/menu/AINTFS18.png"));
    // tray.setPressedImage(path.join(__dirname, "../renderer/assets/menu/AINTFS_active18.png"));
   // tray.on('click', () => toggleTrayWindow())
}

const toggleTrayWindow = () => {
    trayWin.isFocused() ? trayWin.hide() : showWindow()
}

const showWindow = () => {
    const position = getWindowPosition()
    trayWin.setPosition(position.x, position.y, false)
    trayWin.show()
    trayWin.focus()
}

const getWindowPosition = () => {
    const windowBounds = trayWin.getBounds()
    const trayBounds = tray.getBounds()

    // Center window horizontally below the tray icon
    const x = Math.round(trayBounds.x + (trayBounds.width / 2) - (windowBounds.width / 2))
    // Position window 4 pixels vertically below the tray icon
    const y = Math.round(trayBounds.y + trayBounds.height + 4)
    return {x, y}
}

//默认菜单栏
const initTray = () => {
    if (trayWin == null) {
        //已下为插入内容
        const path = require('path')
        trayWin = new BrowserWindow({
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

        trayWin.loadURL(winURL + "#tray")
        // settingWin.webContents.closeDevTools()

        trayWin.on('closed', () => {
            trayWin = null
        })

        trayWin.on('blur', () => {
            trayWin.hide();
        })

    } else {
        trayWin.show()
    }
}
//设置进程调用
const openSysSeeting = () => {
    if (settingWin == null) {
        //已下为插入内容
        const path = require('path')
        settingWin = new BrowserWindow({
            title: "设置",
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

        settingWin.loadURL(winURL + "#setting")
        // settingWin.webContents.closeDevTools();//关闭dev工具

        settingWin.once('ready-to-show', () => {
            settingWin.show()
        })

        settingWin.on('closed', () => {
            settingWin = null
        })
    } else {
        settingWin.show()
    }
}

//默认设置全局配置
const initStore = () => {
    store.set(default_store);
    console.warn("initStore run");
}

//
//
// //监听Mount事件
// const listenMount = () => {
//     //监测USB插入事件
//     var path = '/Volumes/';
//     fs.watch(path, function (event, filename) {
//             if(mainWindow){
//                 mainWindow.send("MountEvent",filename);
//             }
//     });
// }

app.on('ready', () => {
    initWindow()
    initTray()
    if (!store.get("name") || store.get("name") == "undefined") {
        initStore();
    }
    if (store.get("show_menu") != false) {
        openTrayMenu();
    }

    // listenMount();


    globalShortcut.register('Command+Shift+J', () => {
        let focusWin = BrowserWindow.getFocusedWindow()
        focusWin && focusWin.toggleDevTools()
    });
})

app.on('activate', () => {
    if (mainWindow === null) {
        initWindow()
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
    }else if(arg == "resetConf"){
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
    if(mainWindow){
        mainWindow.send("ChangeLangEvent",arg);
    }
})


