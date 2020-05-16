import {AlConst} from "@/common/utils/AlfwConst";

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

import {openLog, getPackageVersion,filter_menu_show} from '@/common/utils/AlfwCommon.js'

const {_} = require('lodash')
import {
    uMountDisk,
    mountDisk,
    openInFinder
} from '@/common/utils/AlfwDisk'
import {updateDisklist} from '@/renderer/lib/diskMonitor'

export default {
    components: {},
    data() {
        return {
            title: "NTFS Tool",
            menu_box1: false,
            diskList: [],
            diskMap:[],
            showDebugMenu: process.env.NODE_ENV === 'development' ? true : false,
        }
    },
    mounted() {
        this._title = this.title;

        ipcRenderer.on("TrayEvent", (event,args) => {
            console.warn("TrayEvent",args);

            //GlobalViewUpdate
            if(_.get(args,"type") == AlConst.GlobalViewUpdate){
                this.diskMap = filter_menu_show(_.get(args,"data"));
                this.resetSize();
            }

            if(_.get(args,"type") == "ChangeLangEvent"){
                console.warn("tray wind ChangeLangEvent", _.get(args,"data"));
                this.$i18n.locale = _.get(args,"data");
            }
        });


        remote.getCurrentWindow().on('focus', function () {
            console.warn("TrayWindow focus");
            var _this = this;
            updateDisklist(function () {

            });
        })


        ipcRenderer.on("OpenShare", () => {
            this.openShare();
        });


    },
    methods: {
        uMountDisk(item) {
            var _this = this;
            console.warn(item, "select_item");
            if (item.group == 'inner') {
                alert(_this.$i18n.t('Internaldiskcannotbeunmounted') + ":" + item._name);
                return;
            }

            var confirm_status = confirm(item._name ? _this.$i18n.t('OKtounmountthedisk') + ": " + item._name + "?" : _this.$i18n.t('OKtounmountthedisk'))
            console.warn(confirm_status, "confirm confirm_status")

            if (confirm_status) {
                uMountDisk(item,true).then(res => {
                    console.warn("uMountDisk res", res);
                    let option = {
                        title: "NTFSTool",
                        body: item._name + " " + _this.$i18n.t('Diskuninstallsucceeded'),
                    };
                    new window.Notification(option.title, option);
                }).catch(err => {
                    console.warn(err,"uMountDisk");
                    alert(_this.$i18n.t('Uninstallfailed'));
                })
            }
        },
        mountDisk(item) {
            console.warn("start mountDisk_______________________")
            var _this = this;
            mountDisk(item).then(res => {
                console.warn("mountDisk res", res)
                let option = {
                    title: "NTFSTool",
                    body: item._name + " " + _this.$i18n.t('Diskmountedsuccessfully'),
                };
                new window.Notification(option.title, option);
            })
        },
        openDisk(item) {
            console.warn("openDisk ", item);
            if (!item.mount_point) {
                alert(this.$i18n.t('Thediskisnotmounted'));
                return;
            }
            openInFinder(item.mount_point).catch(() => {
                alert("openDisk fail!");
            });
        },
        test() {
            console.warn("ASDFASFDSF")
        },
        openMenuBox(id) {
            this[id] = this[id] ? false : true
        },
        setTitle(title) {
            if (typeof title != "undefined") {
                this.title = title;
            } else {
                this.title = this._title;
            }
        },
        resetSize: function () {
            var nums = parseInt(_.get(this.diskMap,"inner",[]).length + _.get(this.diskMap,"ext",[]).length + _.get(this.diskMap,"image",0).length);

            if(isNaN(nums)){
                nums = 1;
            }

            remote.getCurrentWindow().setSize(380, nums * 55 + 120);
        },
        openWebsite: () => {
            shell.openExternal("http://www.ntfstool.com")
        },
        openSettingPage() {
            this.menu_box1 = false;
            // ipcRenderer.send('MainMsgFromRender', 'openSettingPage')
            ipcRenderer.send('IPCMain', {
                name:"openPageByName",
                data:"openFeedBackPage"
            })
        },
        openDialog() {
            this.menu_box1 = false;
            // ipcRenderer.send('MainMsgFromRender', 'openDialogPage')
            ipcRenderer.send('IPCMain', {
                name:"openPageByName",
                data:"openDialogPage"
            })
        },
        openLog() {
            this.menu_box1 = false;
            openLog();
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
        openHomePage() {
            this.menu_box1 = false;
            // ipcRenderer.send('MainMsgFromRender', 'openHomePage')
            ipcRenderer.send('IPCMain', {
                name:"openPageByName",
                data:"openHomePage"
            })
        },
        exitAll() {
            this.menu_box1 = false;
            // ipcRenderer.send('MainMsgFromRender', 'exitAll')
            ipcRenderer.send('IPCMain', "exitAll")
        },
        startDebug() {
            console.warn("click startDebug");
            var cur_time = new Date().getTime();

            if (cur_time - this.atest_lasttime > 1000) {
                this.atest_times = 0;

            } else {
                this.atest_times++;
                // console.warn(this.atest_times, "this.atest_times");

                if (this.atest_times > 5) {
                    this.atest_times = 0;
                    this.showDebugMenu = true;
                    // remote.getCurrentWindow().webContents.openDevTools();
                    // this.showDebugMenu = true;
                    // noticeTheSystemError("opendevmod");
                }
            }
            this.atest_lasttime = cur_time;
        },
        openShare() {
            let subject = this.$i18n.t('RecommendUsing');
            let body = "Hi!%0d%0a "+this.$i18n.t('AlreadyUsing')+"%0d%0a"+this.$i18n.t('FindMore')
                +"%0d%0ahttps://ntfstool.com/?tellfriends";
            shell.openExternal("mailto:?cc=service@ntfstool.com&subject=" + subject + "&body=" + body)
        }
    }
}