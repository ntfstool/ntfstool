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
const Store = require('electron-store');
import {exec} from 'child_process'
import {t} from 'element-ui/lib/locale'

const store = new Store();

export function savePassword(password) {
    try {
        store.set("sudoPwd", password);
        if (password != store.get("sudoPwd")) {
            noticeTheSystemError("savePassword");
            return false;
        } else {
            return true;
        }
    } catch (e) {
        noticeTheSystemError("savePassword2");
        return false;
    }
}

function getSudoPwd() {
    try {
        return store.get("sudoPwd");
    } catch (e) {
        noticeTheSystemError("getSudoPwdError");
        return false;
    }
}

export function systemName() {
    return new Promise((resolve, reject) => {
        execShell("whoami").then(res => {
            resolve(res ? res : "root")
        });
    })
}

export function execShell(shell) {
    return new Promise((resolve, reject) => {
        try {
            exec(shell, (error, stdout, stderr) => {
                saveLog.warn("execShell", {
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
 * force 强制不论结果
 * @param shell
 * @param force
 * @returns {Promise}
 */
export function execShellSudo(shell, force = false) {
    return new Promise((resolve, reject) => {
        var password = getSudoPwd();
        try {
            exec(`echo ${password}|sudo -S ${shell}`, (error, stdout, stderr) => {
                saveLog.warn("execShellSudo", {
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
                    if (stderr.toLowerCase().indexOf("password") >= 0) {
                        checkSudoPassword().then(res => {
                            if (!res) {
                                reject(stderr);
                                return;
                            }
                        });
                    } else {
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
            exec(`echo ${password}|sudo -Sk ls /usr`, (error, stdout, stderr) => {
                saveLog.warn({
                    error, stdout, stderr
                }, "checkSudoPassword res");

                if (checkIncorrectPasswordStr(stderr)) {
                    //Todo

                    // alEvent.$emit('SudoPWDEvent');//Send the refresh event
                    // saveLog.warn("checkSudoPassword error password")


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