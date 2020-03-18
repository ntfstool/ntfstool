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

import {ipcRenderer, remote} from 'electron'

import {getPackageVersion, disableZoom, getSystemInfo} from '@/common/utils/AlfwCommon.js'

import {
    getDiskList,
    getDiskFullInfo,
    uMountDisk,
    mountDisk,
    openInFinder} from '@/common/utils/AlfwDisk.js'
import {alEvent} from '@/common/utils/alevent.js'

const log = require('electron-log');
const fs = require('fs');
export default {
    components: {},
    data() {
        return {
            activeTab: 'tab1',
            devices: [],
            diskList: [],
            loading: 0,
            version: "1.0.0",
            menu_box1: false,


            select_item: {
                disk_mount: "",
                canPush: false,
                type: "",
                name: "",
                size: "",
                size_wei: "",
                index: "",
                info: [],
                _test: ""
            },
            select_disk_key: "",
            atest_lasttime: 0,
            atest_times: 0,
            sudoDialog: false,
        }
    },
    mounted() {

        this.fsListenMount();
        this.refreshDevice();
        this.setVersion();
        disableZoom(require('electron').webFrame);

        window.addEventListener('beforeunload', ()=>{
            remote.getCurrentWindow().on('blur', () => {
                this.menu_box1 = false;
            })
        });

        //Admin password
        alEvent.$on('SudoPWDEvent', args => {
            this.changePwdEvent(args);
        });

        alEvent.$on('doRefreshEvent', filename => {
            this.refreshDevice();
        });

        //监听语言切换
        ipcRenderer.on("ChangeLangEvent", (e, lang) => {
            console.warn("main wind ChangeLangEvent", lang);
            this.$i18n.locale = lang;
        });

        //监听挂载事件
        ipcRenderer.on("MountEvent", (e, filename) => {
            console.warn("main wind acive MountEvent", filename);
            this.refreshDevice();
        });

        remote.getCurrentWindow().on('focus', function() {
           console.warn("currentWindow focus");
        })
    },
    methods: {

        help() {
            var confirm_status = confirm("提交NTFSTool应用的运行日志，从而帮助开发者改善他们的应用")
            if (confirm_status) {
                var confirm_status = confirm("分析数据已提交,即将跳转在线帮助页面...")
                if (confirm_status) {

                }
            } else {
                alert("您已放弃提交分析数据,暂无法提供免费技术支持");
            }
        },
        refreshDevice() {
            try {
                this.loading = -1;
                //更新列表
                getDiskList().then((diskList) => {
                    console.log(diskList, "getDiskList");
                    this.diskList = diskList;
                    if (!this.select_disk_key) {
                        try {
                            this.choseDisk(diskList["inner"][0]);
                        } catch (e) {
                            log.warn(e, "refreshDevice choseDisk");
                        }
                    }
                    this.loading = 0;
                });
            } catch (e) {
                log.warn(e, "refreshDevice");
            }
        },
        changeVolumeName(select_item) {
            this.$prompt('请输入新名称', '', {
                showClose: false,
                inputValue: select_item.volume_name,
                confirmButtonText: '确定',
                cancelButtonText: '取消',
            }).then(({value}) => {
                this.$alert("ok " + value);
            })
        },
        clearDisk(item) {
            console.warn("clearDisk click", this.$i18n.t('Erasingthediskwilldelete'))
            // 抹掉磁盘将删除 的所有资料
            var confirm_status = confirm(this.$i18n.t('Erasingthediskwilldelete') + " (" + item.name + ")")
            console.warn(confirm_status, "confirm confirm_status")
            if (confirm_status) {
                if (confirm(this.$i18n.t('Detectsystemdisktoolisabouttojump'))) {
                    openSysDiskUtils().then(res => {
                        console.warn("openSysDiskUtils res", res)
                    })
                }
            }
        },
        uMountDisk(item) {
            var _this = this;
            console.warn(item, "select_item");
            if (item.group == 'inner') {
                alert(this.$i18n.t('Internaldiskcannotbeunmounted') + ":" + item.name);
                return;
            }
            var confirm_status = confirm(this.$i18n.t('OKtounmountthedisk') + ":" + item.name)
            console.warn(confirm_status, "confirm confirm_status")
            if (confirm_status) {
                uMountDisk(item).then(res => {
                    console.warn("uMountDisk res", res);
                    let option = {
                        title: "NTFSTool",
                        body: item.name + " 磁盘推出成功",
                    };
                    new window.Notification(option.title, option);
                    _this.refreshDevice();
                })
            }
        },
        mountDisk(item) {
            var _this = this;
            mountDisk(item).then(res => {
                console.warn("mountDisk res", res)
                let option = {
                    title: "NTFSTool",
                    body: item.name + " 磁盘挂载成功",
                };
                new window.Notification(option.title, option);
                _this.refreshDevice();
            })
        },
        choseDisk(item) {
            if (typeof item.index != "undefined" && typeof item.info != "undefined" && typeof item.info.protocol != "undefined") {
                this.select_item = item;
                this.select_disk_key = item.index;
            } else {
                log.warn(item, "choseDisk Error");
            }
        },
        openDisk(item) {
            console.warn("dbclick ", item);
            if (!item.info.mountpoint) {
                alert("该磁盘没有挂载");
                return;
            }
            openInFinder(item.info.mountpoint).catch(() => {
                alert("openDisk fail!");
            });
        },
        openSysSetting() {
            console.warn(" openSysSetting click");
            ipcRenderer.send('MainMsgFromRender', 'openSettingPage')
        },
        altest() {
            console.warn("altest");
            var cur_time = new Date().getTime();


            if (cur_time - this.atest_lasttime > 1000) {
                this.atest_times = 0;

            } else {
                this.atest_times++;
                // console.warn(this.atest_times, "this.atest_times");

                if (this.atest_times > 5) {
                    this.atest_times = 0;
                    remote.getCurrentWindow().webContents.openDevTools();
                    console.warn("open debug!....");
                }
            }
            this.atest_lasttime = cur_time;
        },
        setVersion() {
            this.version = getPackageVersion();
        },
        fsListenMount() {
            var _this = this;
            //监听挂载事件
            try {
                var path = '/Volumes/';
                fs.watch(path, function (event, filename) {
                    console.warn(filename, "home watch Volumes")
                    _this.refreshDevice();
                });
            } catch (e) {
                log.warn(e, "watch Volumes");
            }
        },
        changePwdEvent(args) {
            if (_this.sudoDialog) {
                return;
            }
            _this.sudoDialog = true;

            var title = '磁盘挂载需要管理员权限,请输入密码';
            if (args == "invalid password") {
                title = '密码验证错误,请重新输入';
            }
            this.$prompt(title, '', {
                showClose: false,
                inputType: "password",
                confirmButtonText: '确定',
                cancelButtonText: '取消',
            }).then(({value}) => {
                _this.sudoDialog = false;
                alEvent.$emit('setPWDEvent', value);//发送刷新事件
                _this.refreshDevice();
            }).catch(err => {
                _this.sudoDialog = false;
            })
        },
        openMenuBox(id) {
            this[id] = this[id] ? false : true
        },
        openSettingPage() {
            this.menu_box1 = false;
            ipcRenderer.send('MainMsgFromRender', 'openSettingPage')
        },
        openAboutPage() {
            this.menu_box1 = false;
            ipcRenderer.send('MainMsgFromRender', 'openAboutPage')
        },
        openFeedBackPage() {
            this.menu_box1 = false;
            ipcRenderer.send('MainMsgFromRender', 'openFeedBackPage')
        },
        exitAll(){
            this.menu_box1 = false;
            ipcRenderer.send('MainMsgFromRender', 'exitAll')
        }
    }
}