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

import {app,ipcRenderer, remote} from 'electron'
import {getPackageVersion, disableZoom, choseDefaultNode, getSystemInfo} from '@/common/utils/AlfwCommon.js'
import {clearPwd, getStoreForDiskList, getMountType} from '@/common/utils/AlfwStore'
import {
    getDiskList,
    getDiskFullInfo,
    uMountDisk,
    mountDisk,
    openInFinder
} from '@/common/utils/AlfwDisk'
import {alEvent} from '@/common/utils/alEvent'


import {fsListenMount, updateDisklist, test} from '@/renderer/lib/diskMonitor'
import {AlConst} from "@/common/utils/AlfwConst";

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
        var _this = this;



        console.warn("getMountType", getMountType());

        this.refreshDevice();
        this.setVersion();
        disableZoom(require('electron').webFrame);


        //background event [past it in dialog]
        fsListenMount();

        ipcRenderer.on(AlConst.GlobalViewUpdate, () => {
            console.warn(`${AlConst.GlobalViewUpdate} come home ...`);
            this.diskList = getStoreForDiskList();
        });


        window.addEventListener('beforeunload', () => {
            remote.getCurrentWindow().on('blur', () => {
                this.menu_box1 = false;
            })
        });


        ipcRenderer.on("ChangeLangEvent", (e, lang) => {
            console.warn("main wind ChangeLangEvent", lang);
            this.$i18n.locale = lang;
        });


        remote.getCurrentWindow().on('focus', function () {
            console.warn("currentWindow focus[todo: check diskutils <=> mount]");
            _this.refreshDevice();
        })
    },
    methods: {

        help() {
            var confirm_status = confirm($t('Submittherunninglog'))
            if (confirm_status) {
                var confirm_status = confirm($t('Theanalysisdata'))
                if (confirm_status) {

                }
            } else {
                alert($t('Youhavegivenup'));
            }
        },
        refreshDevice() {
            try {
                var _this = this;
                _this.loading = -1;
                updateDisklist(function () {
                    _this.loading = 0;
                });
            } catch (e) {
                console.warn(e, "refreshDevice");
            }
        },
        changeVolumeName(select_item) {
            this.$prompt($t('Pleaseenteranewname'), '', {
                showClose: false,
                inputValue: select_item.volume_name,
                confirmButtonText: $t('Confirm'),
                cancelButtonText: $t('Cancel'),
            }).then(({value}) => {
                this.$alert("ok " + value);
            })
        },
        clearDisk(item) {
            console.warn("clearDisk click", this.$i18n.t('Erasingthediskwilldelete'))
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
                        body: item.name + " " + $t('Diskuninstallsucceeded'),
                    };
                    new window.Notification(option.title, option);
                    _this.refreshDevice();
                }).catch(err => {
                    alert($t('Uninstallfailed'));
                })
            }
        },
        mountDisk(item) {
            var _this = this;
            mountDisk(item).then(res => {
                console.warn("mountDisk res", res)
                let option = {
                    title: "NTFSTool",
                    body: item.name + " "+$t('Diskmountedsuccessfully'),
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
                console.warn(item, "choseDisk Error");
            }
        },
        openDisk(item) {
            console.warn("dbclick ", item);
            if (!item.info.mountpoint) {
                alert($t('Thediskisnotmounted'));
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
        exitAll() {
            this.menu_box1 = false;
            ipcRenderer.send('MainMsgFromRender', 'exitAll')
        },
        clearPwd() {
            clearPwd();
            this.menu_box1 = false;
        }
    }
}