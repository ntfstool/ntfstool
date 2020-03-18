<template>
    <el-container class="al-main">
        <div class="sudo_box" v-show="show_index == 'sudo_box'">
            <div class="left">
                <img src="../../../static/lock.svg">
            </div>
            <div class="right">
                <div class="title">
                    <span>“NtfsTool” 正在尝试调用系统磁盘操作权限。</span>
                    <span>输入密码以允许此次操作。</span>
                </div>
                <div class="input">
                    <table>
                        <tr>
                            <td class="aleft">用户名：</td>
                            <td>
                                <input v-if="workname" name="username" v-model="workname" disabled>
                                <input v-else name="username" v-model="workname">
                            </td>
                        </tr>
                        <tr>
                            <td class="aleft">密码：</td>
                            <td><input name="workpwd" v-on:keyup.enter="checkSudoPwd"
                                       ref="workpwd" type="password" v-model="workpwd"></td>
                        </tr>
                    </table>
                </div>
                <div class="btn-box">
                    <span :class="[btnDisable ? 'btn-disable' : 'btn-default']" @click="cancel()">取消</span>

                    <button :class="[btnDisable ? 'btn-active-disable' :  'btn-active']" @click="checkSudoPwd()">解锁
                    </button>
                </div>
            </div>
        </div>

        <div class="about_box" v-show="show_index == 'about_box'">
            <div class="main_about">
                <div>
                    <img @click="openMainsite()" style="width: 80px;height: 80px;user-select: none;"
                         src="../assets/logo.png">
                </div>
                <div style="display: none;">
                    <img :src="statisticsImg">
                </div>
                <div style="font-weight: bold;">
                    NTFSTool for MAC
                </div>

                <div style="font-size: 10px;">
                    系统 {{os_version}}
                </div>

                <div style="font-size: 10px;">
                    系列号 {{serial_number}}
                </div>

                <div style="font-size: 10px;">
                    版本 {{version}} (3245) <i @click="openGitHub()" class="iconfont" style="margin-left:5px;font-size: 12px;
    color: black;">&#xe691;</i>
                </div>
                <div style="    font-size: 11px;">
                    Copyright © 2020 <u style="margin: 0 5px;cursor: pointer" @click="openMainsite()">NtfsTool</u> Inc.
                </div>
            </div>
        </div>
    </el-container>
</template>
<script>
    const {shell, ipcRenderer, remote} = require('electron')
    const saveLog = require('electron-log');
    import {disableZoom, noticeTheSystemError, getSystemInfo, getPackageVersion} from '@/common/utils/AlfwCommon'
    import {savePassword, checkSudoPassword, systemName} from '@/common/utils/AlfwShell'
    import {POST_LOG_URL} from '@/common/utils/AlfwConst'

    const CurWin = remote.getCurrentWindow();

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

                    function animate() {
                        if (_index === routeArray.length) return
                        setTimeout(() => {
                            curWin.setPosition(routeArray[_index].x, startY)
                            _index++
                            animate()
                        }, 60)
                    }

                    animate()
                }
            }
        }
    }

</script>

<style scoped src="../theme/dialog.scss" lang="scss"></style>