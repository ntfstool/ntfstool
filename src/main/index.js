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

import {app, BrowserWindow, Menu, Tray, ipcMain,globalShortcut,crashReporter,screen} from 'electron'
const Store = require('electron-store');
const devMod = process.env.NODE_ENV === 'development' ? true : false;
const store = new Store();
app.disableHardwareAcceleration();//disable gpu
process.env.ELECTRON_DISABLE_SECURITY_WARNINGS = '1';
var settingPageHandle = null,aboutPageHandle=null,feedBackPageHandle=null,trayPageHandle = null,tray = null,windowBounds=null,homeWinHandle,before_quit_status = true;//only one time to exit all

const winURL = devMod ? `http://localhost:9080` : `file://${__dirname}/index.html`;

if(!devMod){
    global.__static = require('path').join(__dirname, '/static').replace(/\\/g, '\\\\')
}

try {
    const default_store = {
        name: "Alntfs",
        auto_run: true,
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
        sudoPwd: false,
    };

    app.on('ready', () => {
        if (!store.get("name") || store.get("name") == "undefined") {
            initStore();
        }

        openHomePage();

        openTrayPage();

        setTimeout(function () {
            openAboutPage("hide");
            openSettingPage("hide");
            openFeedBackPage("hide");
        }, 5000)

        globalShortcut.register('Command+Shift+J', () => {
            let focusWin = BrowserWindow.getFocusedWindow()
            focusWin && focusWin.toggleDevTools()
        });
    })

    app.on('before-quit', () => {
        if(before_quit_status){
            before_quit_status = false;
            exitAll();
        }
    })

    //Main process listen message
    ipcMain.on('MainMsgFromRender', function (event, arg) {
        console.warn(arg, "Main process listened the message")
        if (arg == "openSettingPage") {
            openSettingPage();
        } else if (arg == "openAboutPage") {
            openAboutPage();
        } else if (arg == "openFeedBackPage") {
            openFeedBackPage();
        } else if (arg == "openHomePage") {
            openHomePage();
        } else if (arg == "exitAll") {
            exitAll();
        } else if (arg == "resetConf") {
            store.set(default_store);
            event.returnValue = 'succ';
        } else {
            event.sender.send('RenderMsgFromMain', arg + " fail")
        }
    })


    //listen the TrayMenu
    ipcMain.on('toggleTrayMenu', function (event, arg) {
        if (tray !== null) {
            tray.destroy();
            tray = null;
            event.returnValue = 'destroy';
        } else {
            openTrayMenu();
            event.returnValue = 'newopen';
        }
    })

    //listen the lang change
    ipcMain.on('ChangeLangEvent', function (event, arg) {
        console.warn("ChangeLangEvent", arg);
        if (homeWinHandle) {
            homeWinHandle.send("ChangeLangEvent", arg);
        }
    })

    const initStore = () => {
        store.set(default_store);
        console.warn("initStore run");
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
                devMod && homeWinHandle.webContents.openDevTools();
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
                            openSettingPage();
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

    //default tray menu
    const openTrayPage = () => {
        if (trayPageHandle == null) {
            trayPageHandle = new BrowserWindow({
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

            trayPageHandle.loadURL(winURL + "#tray")

            trayPageHandle.once('ready-to-show', () => {
                windowBounds = trayPageHandle.getBounds();

                if (store.get("show_menu") != false) {
                    openTrayMenu();
                }

                devMod && trayPageHandle.webContents.openDevTools();
            })

            trayPageHandle.on('closed', () => {
                trayPageHandle = null
            })
        } else {
            trayPageHandle.show()
        }
    }

    //right tray menu
    const openTrayMenu = () => {
        const path = require('path');
        tray = new Tray(path.join(__dirname, "../renderer/assets/menu/AINTFS18.png"));
        
        tray.setIgnoreDoubleClickEvents(true);

        tray.on('click', (event,trayBounds) => {
            console.warn("openTrayMenu click ++++++++++++++++++++++++++++" + Math.random());
           if(trayPageHandle){
               if(trayPageHandle.isVisible()){
                   trayPageHandle.hide();
               }else{
                   trayPageHandle.setPosition(
                       Math.round(trayBounds.x + (trayBounds.width / 2) - (windowBounds.width / 2)),
                       Math.round(trayBounds.y + trayBounds.height + 4), false)
                   trayPageHandle.show()
                   trayPageHandle.focus()
               }
           }else{
               //Todo log error
               exitAll();
           }
        })
    }

    const showWindow = () => {
        const position = getWindowPosition()
        trayPageHandle.setPosition(position.x, position.y, false)
        trayPageHandle.show()
        trayPageHandle.focus()
    }

    // const getWindowPosition = () => {
    //     const windowBounds = trayPageHandle.getBounds();
    //     const trayBounds = tray.getBounds();
    //
    //     var x = Math.round(trayBounds.x + (trayBounds.width / 2) - (windowBounds.width / 2))
    //
    //     var scr = screen.getDisplayMatching(trayBounds)
    //     if(typeof scr.bounds!= "undefined" && scr.bounds.y != "undefined" && scr.bounds.y != 0){
    //         var y = Math.round(trayBounds.y + trayBounds.height + scr.bounds.y)
    //     }else{
    //         var y = Math.round(trayBounds.y + trayBounds.height + 4)
    //     }
    //
    //     return {x, y}
    // }

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

                devMod && settingPageHandle.webContents.openDevTools();
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
                height: 210,
                width: 350,
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

                devMod && aboutPageHandle.webContents.openDevTools();
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

                devMod && feedBackPageHandle.webContents.openDevTools();
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

    const exitAll = () => {
        if (tray) {
            tray.destroy();
        }
        if (settingPageHandle) {
            settingPageHandle.destroy();
        }
        if (aboutPageHandle) {
            aboutPageHandle.destroy();
        }
        if (feedBackPageHandle) {
            feedBackPageHandle.destroy();
        }
        if (trayPageHandle) {
            trayPageHandle.destroy();
        }
        if (homeWinHandle) {
            homeWinHandle.destroy();
        }
        app.quit();
    }

}catch (e) {
    exitAll();
}

