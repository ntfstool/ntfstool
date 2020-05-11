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

import {app, ipcRenderer, remote} from 'electron'
import {getPackageVersion, disableZoom, choseDefaultNode, getSystemInfo} from '@/common/utils/AlfwCommon.js'
import {
    clearPwd,
    getStore,
    getStoreForDiskList,
    getMountType,
    getMountNotifyStatus,
    fixUnclear,
    ignoreUSB
} from '@/common/utils/AlfwStore'
import {
    uMountDisk,
    mountDisk,
    openInFinder
} from '@/common/utils/AlfwDisk'

const {_} = require('lodash')

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
            firstTime:true
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
            console.warn("Home GlobalViewUpdate Come")
            this.diskList = getStoreForDiskList();
            if (!_.get(this.select_item, "name") && !_.get(this.select_item, "type")) {
                if (_.get(this.diskList, "inner[0]")) {
                    this.select_item = _.get(this.diskList, "inner[0]");
                    this.select_disk_key = _.get(this.diskList, "inner[0].index");
                }
            }
        });

        ipcRenderer.on("CreteFileEvent", (event, arg) => {
            console.warn("Home on CreteFileEvent...")
            var _this = this;
            setTimeout(function () {
                console.warn(arg,"start CreteFileEvent refreshDevice ... (3)")
                _this.refreshDevice();
            }, 3000)

            setTimeout(function () {
                console.warn(arg,"start CreteFileEvent refreshDevice ... (8)")
                _this.refreshDevice();
            }, 8000)

            setTimeout(function () {
                console.warn(arg,"start CreteFileEvent refreshDevice ... (18)")
                _this.refreshDevice();
            }, 18000);
        });


        var UsbNotice = function (title, device) {
            console.warn(device,"UsbNotice")
            if (ignoreUSB(device.serialNumber) !== true) {
                new window.Notification(title, {body: device.deviceName + "("+_this.$i18n.t('click_can_forbid')+")"}).onclick = function () {
                    remote.getCurrentWindow().show();
                    var confirm_status = confirm(device.deviceName + ": "+ _this.$i18n.t('cancel_usb_notify'))
                    if (confirm_status) {
                        ignoreUSB(device.serialNumber,true);
                        alert(_this.$i18n.t('canceled_usb_notify'));
                    }
                };
            } else {
                console.warn(device, "device [in ignoreUSB]");
            }
        }

        ipcRenderer.on("UsbDeleteFileEvent", (event, device) => {
            if (getMountNotifyStatus()) {
                UsbNotice(this.$i18n.t('remove_device_event'), device)
            }
        });

        ipcRenderer.on("UsbAddFileEvent", (event, device) => {
            if (getMountNotifyStatus()) {
                UsbNotice(this.$i18n.t('new_device_event'), device)
            }
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
            var confirm_status = confirm(this.$i18n.t('Submittherunninglog'))
            if (confirm_status) {
                var confirm_status = confirm(this.$i18n.t('Theanalysisdata'))
                if (confirm_status) {

                }
            } else {
                alert(this.$i18n.t('Youhavegivenup'));
            }
        },
        refreshDevice() {
            try {
                var _this = this;
                _this.loading = -1;
                updateDisklist(function () {
                    _this.loading = 0;

                    _this.firstTime = false;
                });
            } catch (e) {
                console.warn(e, "refreshDevice");
            }

            //first time to show
            if(this.firstTime){
                this.firstTime = false;
                //show cache
                var cacheValue = getStoreForDiskList();
                if(cacheValue && typeof cacheValue.inner != "undefined"){
                    this.diskList = cacheValue;
                    if (!_.get(this.select_item, "name") && !_.get(this.select_item, "type")) {
                        if (_.get(this.diskList, "inner[0]")) {
                            this.select_item = _.get(this.diskList, "inner[0]");
                            this.select_disk_key = _.get(this.diskList, "inner[0].index");
                        }
                    }
                }else{
                    console.warn("firstTime show cache [fail]");
                }
            }
        },
        changeVolumeName(select_item) {
            this.$prompt(this.$i18n.t('Pleaseenteranewname'), '', {
                showClose: false,
                inputValue: select_item.volume_name,
                confirmButtonText: this.$i18n.t('Confirm'),
                cancelButtonText: this.$i18n.t('Cancel'),
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
                alert(_this.$i18n.t('Internaldiskcannotbeunmounted') + ":" + item.name);
                return;
            }


            var confirm_status = confirm(_this.$i18n.t('OKtounmountthedisk') + ":" + item.name)
            console.warn(confirm_status, "confirm confirm_status")

            if (confirm_status) {
                uMountDisk(item).then(res => {
                    console.warn("uMountDisk res", res);
                    let option = {
                        title: "NTFSTool",
                        body: item.name + " " + _this.$i18n.t('Diskuninstallsucceeded'),
                    };
                    new window.Notification(option.title, option);
                    _this.refreshDevice();
                }).catch(err => {
                    alert(_this.$i18n.t('Uninstallfailed'));
                })
            }
        },
        mountDisk(item) {
            var _this = this;
            mountDisk(item).then(res => {
                console.warn("mountDisk res", res)
                let option = {
                    title: "NTFSTool",
                    body: item.name + " " + _this.$i18n.t('Diskmountedsuccessfully'),
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
                alert(this.$i18n.t('Thediskisnotmounted'));
                return;
            }
            openInFinder(item.info.mountpoint).catch(() => {
                alert("openDisk fail!");
            });
        },
        openSysSetting() {
            console.warn(" openSysSetting click");
            ipcRenderer.send('IPCMain', {
                name:"openPageByName",
                data:"openSettingPage"
            })
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
            // ipcRenderer.send('MainMsgFromRender', 'openSettingPage')
            ipcRenderer.send('IPCMain', {
                name:"openPageByName",
                data:"openSettingPage"
            })
        },
        openAboutPage() {
            this.menu_box1 = false;
            // ipcRenderer.send('MainMsgFromRender', 'openAboutPage')
            ipcRenderer.send('IPCMain', {
                name:"openPageByName",
                data:"openAboutPage"
            })
        },
        openFeedBackPage() {
            this.menu_box1 = false;
            // ipcRenderer.send('MainMsgFromRender', 'openFeedBackPage')
            ipcRenderer.send('IPCMain', {
                name:"openPageByName",
                data:"openFeedBackPage"
            })
        },
        exitAll() {
            this.menu_box1 = false;
            // ipcRenderer.send('MainMsgFromRender', 'exitAll')
            ipcRenderer.send('IPCMain',"exitAll")
        },
        clearPwd() {
            clearPwd();
            this.menu_box1 = false;
        },
        showFreeSpace(total_size, total_size_wei, used_size, used_size_wei) {
            let wei_to_num = function (wei) {
                if (!wei || typeof wei != "string") {
                    return 1;
                }

                wei = wei.toLowerCase();
                if (wei.indexOf("tb") >= 0) {
                    return 1024 * 1024 * 1024;
                }

                if (wei.indexOf("gb") >= 0) {
                    return 1024 * 1024;
                }

                if (wei.indexOf("mb") >= 0) {
                    return 1024;
                }

                return 1;
            }

            let num_to_weinum = function (num) {
                var num = parseFloat(num).toFixed(0);
                if (isNaN(num)) {
                    return "-";
                }
                if (!num) {
                    return "--";
                }
                if (num <= 1024) {
                    return num + " KB";
                }

                if (num > 1024 && num <= 1024 * 1024) {
                    return (num / 1024).toFixed(2) + " MB";
                }

                if (num > 1024 * 1024 && num <= 1024 * 1024 * 1024) {
                    return (num / (1024 * 1024)).toFixed(2) + " GB";
                }

                if (num > 1024 * 1024 * 1024 && num <= 1024 * 1024 * 1024 * 1024) {
                    return (num / (1024 * 1024 * 1024)).toFixed(2) + " TB";
                }

            }

            var free_num = total_size * wei_to_num(total_size_wei) - used_size * wei_to_num(used_size_wei);
            return num_to_weinum(free_num);
            //
            //
            // return total_size +"|"+total_size_wei +
            //     "#"+used_size+"|"+used_size_wei;
        },
        test() {
            ipcRenderer.send('IPCMain', {
                "name":"name1",
                "data":"data1"
            })
        }
    }
}