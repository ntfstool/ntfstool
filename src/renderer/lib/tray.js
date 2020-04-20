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

import {openLog, getPackageVersion} from '@/common/utils/AlfwCommon.js'
import {getStoreForDiskList} from "@/common/utils/AlfwStore";
import {
    getDiskList,
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
            showDebugMenu: process.env.NODE_ENV === 'development' ? true : false,
        }
    },
    mounted() {
        this._title = this.title;
        console.warn(this.$refs, "this.$refsa")
        this.resetSize();

        ipcRenderer.on("ChangeLangEvent", (e, lang) => {
            console.warn("tray wind ChangeLangEvent", lang);
            this.$i18n.locale = lang;
        });

        // window.addEventListener('beforeunload', ()=>{
        //     remote.getCurrentWindow().on('blur', () => {
        //         this.menu_box1 = false;
        //     })
        // });


        remote.getCurrentWindow().on('focus', function () {
            console.warn("TrayWindow focus");
            var _this = this;
            updateDisklist(function () {

            });
        })

        ipcRenderer.on(AlConst.GlobalViewUpdate, () => {
            this.diskList = getStoreForDiskList();
            console.warn(`${AlConst.GlobalViewUpdate} come tray ...`, this.diskList);
            this.resetSize();
        });

        ipcRenderer.on("OpenShare", () => {
            this.openShare();
        });


    },
    methods: {
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
                        body: item.name + " " + _this.$i18n.t('Diskuninstallsucceeded'),
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
                    body: item.name + " " + _this.$i18n.t('Diskmountedsuccessfully'),
                };
                new window.Notification(option.title, option);
                _this.refreshDevice();
            })
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
            console.warn(this.$refs, "this.$refs")
            let height0 = this.$refs.trayref_h.offsetHeight;  //100
            let height = this.$refs.trayref.offsetHeight;  //100
            console.warn(height, "traywin height");
            remote.getCurrentWindow().setSize(380, height0 + height + 30)
        },
        openWebsite: () => {
            shell.openExternal("http://www.ntfstool.com")
        },
        openSettingPage() {
            this.menu_box1 = false;
            ipcRenderer.send('MainMsgFromRender', 'openSettingPage')
        },
        openDialog() {
            this.menu_box1 = false;
            ipcRenderer.send('MainMsgFromRender', 'openDialogPage')
        },
        openLog() {
            this.menu_box1 = false;
            openLog();
        },
        openAboutPage() {
            this.menu_box1 = false;
            ipcRenderer.send('MainMsgFromRender', 'openAboutPage')
        },
        openFeedBackPage() {
            this.menu_box1 = false;
            ipcRenderer.send('MainMsgFromRender', 'openFeedBackPage')
        },
        openHomePage() {
            this.menu_box1 = false;
            ipcRenderer.send('MainMsgFromRender', 'openHomePage')
        },
        exitAll() {
            this.menu_box1 = false;
            ipcRenderer.send('MainMsgFromRender', 'exitAll')
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