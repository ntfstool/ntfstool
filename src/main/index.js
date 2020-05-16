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
import {app, ipcMain, ipcRenderer, Notification, dialog, shell, powerMonitor} from 'electron'

const saveLog = require('electron-log');

import {checkNeedInitStore, setDefaultStore, InitSystemInfo, getStore} from '../common/utils/AlfwStore.js'

import {
    openPages,
    openPageByName,
    exitAll,
    doChangeLangEvent,
    doDesktopAppEvent,
    doUpdateViewEvent,
    doCreteFileEvent,
    doNotSudoerEvent,
    goSleep,
    goResume
} from '../main/lib/PageConfig.js'
import {AlConst} from "@/common/utils/AlfwConst";
import {checkUpdate} from "@/common/utils/AlfwCommon";

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

        if (getStore("update.auto_check")) {
            checkUpdate();
        }

        openPages();

        powerMonitor.on('suspend', () => {
            saveLog.warn("++++++++++++++++++++++ system sleep ++++++++++++++++++++++");
            goSleep();
        })

        powerMonitor.on('resume', () => {
            saveLog.warn("++++++++++++++++++++++ system resume ++++++++++++++++++++++");
            goResume();
        })
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
    ipcMain.on('IPCMain', function (event, arg) {
        console.warn({arg}, "++++++++++++++++++ IPCMain ++++++++++++++++++");
        var chanelName, actionData = "";
        if (typeof arg.name != "undefined") {
            chanelName = arg.name;
        } else {
            chanelName = arg;
        }

        if (typeof arg.data != "undefined") {
            actionData = arg.data;
        }

        if (chanelName == "exitAll") {
            exitAll();
        } else if (chanelName == "resetConf") {
            setDefaultStore();
            setTimeout(function () {
                app.relaunch()
                exitAll();
            }, 1000);
            event.returnValue = 'succ';
        } else if (chanelName == "openPageByName") {
            openPageByName(actionData);
        } else if (chanelName == "ChangeLangEvent") {
            doChangeLangEvent(actionData);
        } else if (chanelName == "AutoRunEvent") {
            console.warn(actionData, "Main AutoRunEvent");
            try {
                app.setLoginItemSettings({
                    openAtLogin: actionData,
                    openAsHidden: true
                })
            } catch (e) {
                console.error(e, "changeAutoRun Error");
            }
        } else if (chanelName == AlConst.SudoPwdEvent) {
            console.warn("Main SudoPwdEvent Start >>>>>>>>>>>>>>>>>>>>")
            openPageByName("openSudoPage");
        } else if (chanelName == AlConst.InstallFuseEvent) {
            console.warn("Main InstallFuseEvent Start >>>>>>>>>>>>>>>>>>>>")
            openPageByName("openInstallFusePage");
        } else if (chanelName == AlConst.GlobalViewUpdate) {
            console.warn("Main GlobalViewUpdate Start >>>>>>>>>>>>>>>>>>>>")
            doUpdateViewEvent(actionData);
        } else if (chanelName == "CreteFileEvent") {
            console.warn(actionData, "Main CreteFileEvent Start >>>>>>>>>>>>>>>>>>>>")
            doCreteFileEvent(actionData);
        } else if (chanelName == AlConst.NotSudoerEvent) {
           doNotSudoerEvent(actionData);

        }
    })
} catch (e) {
    saveLog.error(e, "mainError exitAll");
    exitAll();
}




