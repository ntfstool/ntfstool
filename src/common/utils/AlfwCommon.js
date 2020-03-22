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
// import {exec} from 'child_process'
import {t} from 'element-ui/lib/locale'
import {remote} from 'electron'

const {shell} = require('electron')
const {getAspInfo} = require('ntfstool')
const {_} = require('lodash')
const saveLog = require('electron-log');

/**
 * check is dev
 */
export function isDev() {
    return process.env.NODE_ENV === 'development' ? true : false;
}

/**
 * get the ntfstool version
 * @returns {*}
 */
export function getPackageVersion() {
    try {
        let curVersion = process.env.NODE_ENV === 'development' ? process.env.npm_package_version : require('electron').remote.app.getVersion();
        saveLog.log(curVersion, "curVersion");
        return curVersion;
    } catch (e) {
        saveLog.error(e, "getPackageVersion error");
        return "45.00";
    }
}

/**
 * get the system info
 * @returns {Promise<any>}
 */
export function getSystemInfo() {
    return new Promise((resolve, reject) => {
        try {
            getAspInfo({
                dataTypes: [
                    'SPSoftwareDataType',
                    'SPHardwareDataType',
                ]
            }, (error, stdout) => {
                if (error) throw error;
                console.warn(stdout,"getSystemInfo stdout");

                var sysinfo = {
                    os_version:_.get(stdout,'SPSoftwareDataType.os_version'),
                    user_name:_.get(stdout,'SPSoftwareDataType.user_name'),
                    machine_name:_.get(stdout,'SPHardwareDataType.machine_name'),
                    physical_memory:_.get(stdout,'SPHardwareDataType.physical_memory'),
                    serial_number:_.get(stdout,'SPHardwareDataType.serial_number'),
                };

                saveLog.warn(sysinfo,"getSystemInfo sysinfo");

                resolve(sysinfo, error)
            });
        } catch (e) {
            saveLog.error(e, "getSystemInfo error");
            reject(stdout + error);
        }

    })
}

/**
 * open the log file
 */
export function openLog() {
    var logObj = saveLog.transports.file.getFile();
    console.warn(logObj, "log getFile");
    if (typeof logObj.path != "undefined") {
        try {
            execShell("open " + logObj.path);
        } catch (e) {
            saveLog.error(e, "log getFile error");
        }
    }
}

/**
 * notice the system error
 * @param _error
 * @param setOption
 */
export function noticeTheSystemError(_error, setOption) {
    var errorMap = {
        system: 10000,
        dialog: 10010,
        dialog_save_err: 10011,
        savePassword: 10020,
        savePassword2: 10021,
        getSudoPwdError:10031,
        checkSudoPasswordError:10041,
        opendevmod: 10030,
        FEEDBACK_ERROR:10040
    };
    var error = (typeof _error != "undefined") ? _error : "system";
    console.warn(error, "error")
    var errorNo = (typeof errorMap[error] != "undefined") ? errorMap[error] : 1000;
    var option = {
        title: "System Error: " + errorNo,
        body: "please contact official technical support",
        href: 'https://www.ntfstool.com'
    };

    if (typeof setOption == "object") {
        option = setOption;
    }
    if (typeof setOption == "string") {
        option.body = setOption;
    }

    saveLog.error({name: _error, text: JSON.stringify(option)}, "noticeTheSystemError");

    new window.Notification(option.title, option).onclick = function () {
        shell.openExternal(option.href)
    }
}

/**
 * disableZoom
 */
export function disableZoom(webFrame) {
    try {
        webFrame.setVisualZoomLevelLimits(1, 1);
        webFrame.setLayoutZoomLevelLimits(0, 0);
    } catch (e) {
        saveLog.error(e.getError(), "disableZoom error");
    }
}

/**
 * In a time_snap,call one,exec one,call n(n>1),exec the start,and exec the time_snap end.
 * @param callback
 */
export function unitTimesToRun(run_type,callback,time_snap){
    if(typeof time_snap == "undefined"){
        time_snap = 1000;
    }

    if(typeof global[run_type] == "undefined"){
        global[run_type] = 0;
    }

    var execHandle = function () {
        callback();
    }

    if(global[run_type] == 0){
        execHandle();
        global[run_type]++;
        setTimeout(function () {
            if(global[run_type] >= 2){
                execHandle();
            }

            global[run_type] = 0;
        },time_snap);
    }else{
        global[run_type]++;
        console.warn(global[run_type],"unitTimesToRun more times,exec nothing...")
    }
}

/**
 * Queue list to exec the callback
 * when timeout Queue will auto to exec next if callback no back to exec
 * @param type
 * @param callback
 * @param timeout
 */
export function queueExec(type,callback,timeout) {
    var global_key = "queueExec_" + type;
    if(typeof global[global_key] == "undefined"){
        global[global_key] = [];
    }
    if(typeof timeout != "undefined"){
        global["queueExec_Timeout"] = timeout;
    }

    if(typeof global["queueExec_Timeout"] == "undefined"){
        global["queueExec_Timeout"] = 3000;//3秒钟执行不完毕，新建执行
    }

    if(callback === null){
        var _callback = global[global_key].pop();
        if(typeof _callback == "function"){
            _callback(function () {
                clearTimeout(global["queueExec_TimeoutHandle"]);

                queueExec(type,null)
            });

            clearTimeout(global["queueExec_TimeoutHandle"]);
            global["queueExec_TimeoutHandle"] = setTimeout(function () {
                clearTimeout(global["queueExec_TimeoutHandle"]);

                queueExec(type,null)
            },global["queueExec_Timeout"])

        }else{
            global["queueExecStatus"] = 0;
        }
    }else{
        if(typeof global["queueExecStatus"] == "undefined" || global["queueExecStatus"] == 0){
            global["queueExecStatus"] = 1;

            callback(function () {
                clearTimeout(global["queueExec_TimeoutHandle"]);

                queueExec(type,null)
            });

            clearTimeout(global["queueExec_TimeoutHandle"]);
            global["queueExec_TimeoutHandle"] = setTimeout(function () {
                clearTimeout(global["queueExec_TimeoutHandle"]);

                queueExec(type,null)
            },global["queueExec_Timeout"])


        }else{
            global["queueExecStatus"]++;
            global[global_key].push(callback);
        }
    }
}