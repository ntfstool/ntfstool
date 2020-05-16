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
const saveLog = require('electron-log');
import {exec} from 'child_process'
import {t} from 'element-ui/lib/locale'
import {ipcRenderer, remote} from 'electron'
import {noticeTheSystemError} from '@/common/utils/AlfwCommon'
import {savePassword,getSudoPwd} from '@/common/utils/AlfwStore'
import {AlConst} from '@/common/utils/AlfwConst'

/**
 * get the system name
 * @returns {Promise<any>}
 */
export function systemName() {
    return new Promise((resolve, reject) => {
        execShell("whoami").then(res => {
            resolve(res ? res : "root")
        });
    })
}

/**
 * exec the shell code by common user
 * @param shell
 * @returns {Promise<any>}
 */
export function execShell(shell) {
    return new Promise((resolve, reject) => {
        try {
            exec(shell, (error, stdout, stderr) => {
                saveLog.log("execShell", {
                    code: shell,
                    stdout: stdout,
                    stderr: stderr,
                })
                if (stderr) {
                    reject(stdout + error);
                    return;
                }

                if (!stdout && stderr) {
                    stdout = stderr;
                }
                resolve(stdout, stderr)
            });
        } catch (e) {
            saveLog.error(e, "execShell");
        }
    })
}


/**
 * exec the shell code by root
 * force Ignore result
 * @param shell
 * @param force
 * @returns {Promise}
 */
export function execShellSudo(shell, force = false) {
    return new Promise((resolve, reject) => {
        var password = getSudoPwd();
        try {
            exec(`echo '${password}'|sudo  -Sk ${shell}`, (error, stdout, stderr) => {
                stderr = stderr.replace( /^Password:/gi , '')
                saveLog.log("execShellSudo", {
                    code: "[SUDO]" + shell,
                    stdout: stdout,
                    stderr: stderr,
                    // error:error
                })
                if (force == true) {
                    resolve();
                    return;
                }
                if (stderr) {
                    if (checkIncorrectPasswordStr(stderr)) {
                        checkSudoPassword().then(res => {
                            if (!res) {
                                reject(stderr);
                                return;
                            }
                        });
                    }else if(!checkFuseStr(stderr)){
                        ipcRenderer.send("IPCMain",AlConst.InstallFuseEvent);
                    }else if(!checkNotSudoer(stderr)){
                        console.warn("checkNotSudoer ok");
                        ipcRenderer.send("IPCMain",{
                            name:AlConst.NotSudoerEvent,
                            data:stderr
                        });
                    }  else {
                        reject(stderr);
                        return;
                    }
                } else {
                    resolve(stdout, stderr)
                }
            });
        } catch (e) {
            saveLog.error(e, "execShellSudo");
        }
    })
}


/**
 * Check for password error messages
 * @param stderr
 * @returns {boolean}
 */
function checkIncorrectPasswordStr(stderr) {
    try {
        var errorPwd = false;
        if (stderr) {
            if (stderr.toLowerCase().indexOf("password") >= 0) {
                if (stderr.toLowerCase().indexOf("try") >= 0 || stderr.toLowerCase().indexOf("incorrect") >= 0) {
                    errorPwd = true;
                }
            }
        }

        return errorPwd;
    } catch (e) {
        saveLog.error(e, "checkIncorrectPasswordStr error");
    }
}

/**
 * Check for fuse install
 * @param stderr
 * @returns {boolean}
 */
function checkFuseStr(stderr) {
    try {
        var fuseStatus = true;
        if (stderr) {
            if (stderr.toLowerCase().indexOf("fuse") >= 0) {
                if (stderr.toLowerCase().indexOf("not") >= 0) {
                    fuseStatus = false;
                }
            }
        }

        // return fuseStatus;
        return true;
    } catch (e) {
        saveLog.error(e, "checkFuseStr error");
    }
}

function checkNotSudoer(stderr) {
    try {
        var status = true;
        if (stderr) {
            if (stderr.toLowerCase().indexOf("no") >= 0) {
                if (stderr.toLowerCase().indexOf("sudoers") >= 0) {
                    status = false;
                }
            }
        }

        return status;
    } catch (e) {
        saveLog.error(e, "checkNotSudoer error");
    }
}

/**
 * check the {{work}} password
 * @returns {Promise}
 */
export function checkSudoPassword(setPwd = false) {
    if (setPwd !== false) {
        var password = setPwd
    } else {
        var password = getSudoPwd();
    }
    return new Promise((resolve, reject) => {
        try {
            //sudo -Sk Force password
            exec(`echo '${password}'|sudo -Sk ls /usr`, (error, stdout, stderr) => {
                saveLog.log({
                    error, stdout, stderr
                }, "checkSudoPassword res");

                if (checkIncorrectPasswordStr(stderr)) {
                    console.log("start send SudoPWDEvent >>>>>>>>>>>>>>>>>>>>>>>>>>>>")
                    ipcRenderer.send("IPCMain",AlConst.SudoPwdEvent);

                    resolve(false);
                } else {
                    resolve(true);

                }
            });
        } catch (e) {
            saveLog.error(e, "checkSudoPassword catch error");
            noticeTheSystemError("checkSudoPasswordError");
            resolve(true);
        }
    });
}