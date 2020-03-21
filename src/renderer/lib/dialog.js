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
const {shell, ipcRenderer, remote} = require('electron')
const saveLog = require('electron-log');
import {disableZoom, noticeTheSystemError, getSystemInfo, getPackageVersion} from '@/common/utils/AlfwCommon'
import {savePassword, checkSudoPassword, systemName} from '@/common/utils/AlfwShell'
import {POST_LOG_URL} from '@/common/utils/AlfwConst'

const CurWin = remote.getCurrentWindow();

import {fsListenMount,test} from '@/renderer/lib/diskMonitor'

export default {
    components: {},
    data() {
        return {
            show_index: "about_box",
            //about_box
            version: "-",
            statisticsImg: "",
            serial_number: "--",
            os_version: "",

            //sudo_box
            workname: "root",
            workpwd: "",
            btnDisable: false,
        }
    },
    mounted() {
        disableZoom(require('electron').webFrame);

        // //background event
        // fsListenMount();


        ipcRenderer.on("ShowDialogEvent", (event, arg) => {
            saveLog.info(arg, "Dialog ShowDialogEvent");
            if (arg == "showSudo") {
                this.showSudo();
            } else {
                this.showAbout();
            }
        });
    },
    methods: {
        test(){
            test()
        },
        showAbout() {
            this.show_index = "about_box";
            this.statistics();
            this.version = getPackageVersion();

            CurWin.setSize(400, 245);
            CurWin.setAlwaysOnTop(false);
            CurWin.show();
        },
        showSudo() {
            console.warn("start showSudo")
            this.show_index = "sudo_box";

            systemName().then(name => {
                this.workname = name;
            });

            CurWin.setSize(500, 210);
            CurWin.setAlwaysOnTop(true);
            CurWin.center();

            //Todo
            CurWin.show();

            checkSudoPassword().then(res => {
                if (!res) {
                    CurWin.show();
                } else {
                    console.log("ShowSudo password corrent!")
                }
            })
        },
        //about_box method
        openMainsite() {
            shell.openExternal("https://www.ntfstool.com")
        },
        openGitHub() {
            shell.openExternal("https://github.com/ntfstool/ntfstool")
        },
        statistics() {
            getSystemInfo().then(json => {
                var pasysteminfo_parmrams = Object.keys(json).map(function (key) {
                    return encodeURIComponent(key) + "=" + encodeURIComponent(json[key]);
                }).join("&");
                this.serial_number = json.serial_number;
                this.os_version = json.os_version;

                console.warn(pasysteminfo_parmrams, "pasysteminfo_parmrams");

                this.statisticsImg = POST_LOG_URL + ".gif?APIVersion=0.6.0&altype=start&" + pasysteminfo_parmrams;
            });
        },
        //sudo_box method
        checkSudoPwd() {
            if (this.btnDisable) {
                return;
            }

            if (!this.workpwd) {
                shell.beep()
                this.$refs.workpwd.focus()
                this.sharkWin();
                return;
            }

            this.btnDisable = true;

            setTimeout(function () {
                this.btnDisable = false;
            }, 10000);

            checkSudoPassword(this.workpwd).then(res => {
                this.btnDisable = false;
                if (!res) {
                    shell.beep()
                    this.$refs.workpwd.focus()
                    this.sharkWin();
                } else {
                    console.warn("pwd ok");
                    if (savePassword(this.workpwd)) {
                        remote.getCurrentWindow().hide();
                    } else {
                        shell.beep()
                        this.sharkWin();
                        noticeTheSystemError("dialog_save_err");
                    }
                }
            }).catch(err => {
                shell.beep()
                this.btnDisable = false;
                this.sharkWin();
                noticeTheSystemError("dialog");
            })
        },
        cancel() {
            remote.getCurrentWindow().hide();
        },
        sharkWin() {
            var curWin = remote.getCurrentWindow();
            var winPos = curWin.getBounds();
            if (typeof winPos.x != "undefined") {
                const startX = winPos.x;
                const startY = winPos.y;
                const moveArrry = [-10, 10, -7, 7, -3, 3, 0]
                const routeArray = moveArrry.map(item => ({x: item + startX}))
                let _index = 0
                this.animate(_index, routeArray, curWin)
            }
        },
        animate(_index, routeArray, curWin) {
            if (_index === routeArray.length) return
            setTimeout(() => {
                curWin.setPosition(routeArray[_index].x, startY)
                _index++
                this.animate(_index, routeArray, curWin)
            }, 60)
        }
    }
}
