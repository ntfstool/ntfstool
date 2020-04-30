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

// import {app, BrowserWindow, Menu, Tray, ipcMain,globalShortcut,crashReporter,screen} from 'electron'

import {app, ipcMain, ipcRenderer, Notification, dialog, shell} from 'electron'

const saveLog = require('electron-log');


import {checkNeedInitStore, setDefaultStore, InitSystemInfo,getStore} from '../common/utils/AlfwStore.js'
import {
    openPages,
    openPageByName,
    toggleTrayMenu,
    exitAll,
    doChangeLangEvent,
    doDesktopAppEvent,
    doUpdateViewEvent,
    doCreteFileEvent
} from '../main/lib/PageConfig.js'
import {AlConst} from "@/common/utils/AlfwConst";


var get = require('get');
app.disableHardwareAcceleration();//disable gpu

process.env.ELECTRON_DISABLE_SECURITY_WARNINGS = '1';

app.allowRendererProcessReuse = true;

try {
    app.on('ready', () => {
        InitSystemInfo();

        if (checkNeedInitStore()) {
            //first run
            try {
                app.setLoginItemSettings({
                    openAtLogin: true,
                    openAsHidden: true
                })
            } catch (e) {
                console.error(e, "changeAutoRun Error0");
            }
        }
        if(getStore("update.auto_check")){
            checkUpdate();
        }
        openPages();
    })

    app.on('before-quit', () => {
        exitAll();
    })

    //for ctrl + c exit
    process.on("SIGINT", function () {
        console.log('WTF')
        exitAll();
        process.exit(0)

    });

    //Main process listen message
    ipcMain.on('MainMsgFromRender', function (event, arg) {
        console.warn(arg, "Main process listened the message");
        if (arg == "exitAll") {
            exitAll();
        } else if (arg == "resetConf") {
            setDefaultStore();
            setTimeout(function () {
                app.relaunch()
                exitAll();
            },1000);
            event.returnValue = 'succ';
        } else {
            //TODO
            openPageByName(arg);
        }
    })

    //listen the TrayMenu
    ipcMain.on('toggleTrayMenu', function (event, arg) {
        toggleTrayMenu();
    })

    //listen the lang change
    ipcMain.on('ChangeLangEvent', function (event, arg) {
        doChangeLangEvent(arg);
    })

    //listen and send the device event
    ipcMain.on('DesktopAppEvent', function (event, arg) {
        doDesktopAppEvent(arg);
    })

    //listen and send the device event
    ipcMain.on('AutoRunEvent', function (event, status) {
        console.warn(status, "Main AutoRunEvent");
        try {
            app.setLoginItemSettings({
                openAtLogin: status,
                openAsHidden: true
            })
        } catch (e) {
            console.error(e, "changeAutoRun Error");
        }
    })

    //sudo pwd event
    ipcMain.on(AlConst.SudoPwdEvent, function (event, arg) {
        console.warn("Main SudoPwdEvent Start >>>>>>>>>>>>>>>>>>>>")
        openPageByName("openSudoPage");
    })

    ipcMain.on(AlConst.InstallFuseEvent, function (event, arg) {
        console.warn("Main InstallFuseEvent Start >>>>>>>>>>>>>>>>>>>>")
        openPageByName("openInstallFusePage");
    })


    ipcMain.on(AlConst.GlobalViewUpdate, function (event, arg) {
        console.warn("Main GlobalViewUpdate Start >>>>>>>>>>>>>>>>>>>>")
        doUpdateViewEvent();
    })

    ipcMain.on("CreteFileEvent", function (event, arg) {
        console.warn({event, arg},"Main CreteFileEvent Start >>>>>>>>>>>>>>>>>>>>")
        doCreteFileEvent(arg);
    })


    //sudo pwd event
    ipcMain.on('NoticeEvent', function (event, arg) {
        console.warn(arg, "MainProcessNotice")
        new Notification(arg).show();
    })
} catch (e) {
    saveLog.error(e, "mainError exitAll");
    exitAll();
}

function versionStringCompare(preVersion='', lastVersion=''){
    var sources = preVersion.split('.');
    var dests = lastVersion.split('.');
    var maxL = Math.max(sources.length, dests.length);
    var result = 0;
    for (let i = 0; i < maxL; i++) {
        let preValue = sources.length>i ? sources[i]:0;
        let preNum = isNaN(Number(preValue)) ? preValue.charCodeAt() : Number(preValue);
        let lastValue = dests.length>i ? dests[i]:0;
        let lastNum =  isNaN(Number(lastValue)) ? lastValue.charCodeAt() : Number(lastValue);
        if (preNum < lastNum) {
            result = -1;
            break;
        } else if (preNum > lastNum) {
            result = 1;
            break;
        }
    }
    return result;
}

function checkUpdate() {
    var cur_version = process.env.NODE_ENV === 'development' ? process.env.npm_package_version : app.getVersion()
    // console.warn(this.$http,"this.$http")

    try {
        get('https://ntfstool.com/version.json').asString(function (err, ret) {
            if (err) throw err;
            var data = {
                "version": "",
                "url": "https://ntfstool.com/",
                "title": "New Version Found",
                "detail": "update"
            };

            try {


                var getData = JSON.parse(ret);
                if (!getData || typeof getData.version == "undefined" || !getData.version) {
                    saveLog.error("not found version!")
                    return;
                }
                if (typeof getData.version != "undefined") {
                    data.version = getData.version;
                }
                if (typeof getData.url != "undefined") {
                    data.url = getData.url;
                }
                if (typeof getData.title != "undefined") {
                    data.title = getData.title;
                }
                if (typeof getData.detail != "undefined") {
                    data.detail = getData.detail;
                }
            } catch (e) {
                saveLog.warn("check version format error!",e)
            }

            if (typeof data.version != "undefined" && data.version) {
                saveLog.warn({
                    cur_version: cur_version,
                    check_version: data.version
                })
                if (cur_version != data.version && versionStringCompare(cur_version,data.version) < 0) {
                    const dialogOpts = {
                        type: 'info',
                        buttons: ['Cancel', "OK"],
                        title: 'Application Update',
                        message: data.title + "("+cur_version+"->"+data.version+")",
                        detail: data.detail
                    }

                    dialog.showMessageBox(dialogOpts).then((diaret) => {
                        if (typeof diaret.response != "undefined" && diaret.response == 1) {
                            shell.openExternal(data.url);
                        }

                    });
                }
            } else {
                saveLog.warn("check version format error!")
            }
        });
    } catch (e) {
        saveLog.error("get update error!", e);
    }
}

