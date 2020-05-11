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
import {getPackageVersion, disableZoom, getSystemInfo} from '@/common/utils/AlfwCommon.js'
import {ipcRenderer, shell} from 'electron'

const Store = require('electron-store');
const store = new Store();
const {remote} = require('electron')
var get = require('get');
const saveLog = require('electron-log');

export default {
    components: {},
    data() {
        const generateData = _ => {
            const data = [];
            for (let i = 1; i <= 15; i++) {
                data.push({
                    key: i,
                    label: `Disk ${i}`,
                    disabled: i % 4 === 0
                });
            }
            return data;
        };

        return {
            auto_run: store.get("auto_run") == false ? false : true,
            auto_mount: store.get("auto_mount") == false ? false : true,
            theme: store.get("theme") != "undefined" ? store.get("theme") : 1,
            lang: store.get("lang") != "undefined" ? store.get("lang") : "english",
            show_menu: store.get("show_menu") == false ? false : true,
            install_bug_type: store.get("common.install_bug_type") != "undefined" ? store.get("common.install_bug_type") : "auto_solve",
            how_restart_window: store.get("common.how_restart_window") != "undefined" ? store.get("common.how_restart_window") : "change_to_bacground",
            //message
            mount_show_msg: store.get("message.mount_show_msg") != "undefined" ? store.get("message.mount_show_msg") : "",
            update_show_msg: store.get("message.update_show_msg") != "undefined" ? store.get("message.update_show_msg") : "",
            error_disk_msg: store.get("message.error_disk_msg") != "undefined" ? store.get("message.error_disk_msg") : "",

            //disk_list
            history_list: store.get("disk_list.history_list") != "undefined" ? store.get("disk_list.history_list") : [],
            ignore_list: store.get("disk_list.ignore_list") != "undefined" ? store.get("disk_list.ignore_list") : [],

            //update
            auto_check: store.get("update.auto_check") != "undefined" ? store.get("update.auto_check") : [],
            auto_beta_update: store.get("update.auto_beta_update") != "undefined" ? store.get("update.auto_beta_update") : [],

            update_url: store.get("update.update_url") != "undefined" ? store.get("update.update_url") : "",
            privacy_url: store.get("privacy_url") != "undefined" ? store.get("privacy_url") : "",
            update_beta_url: store.get("update.update_beta_url") != "undefined" ? store.get("update.update_beta_url") : "",

            lang_list: this.$t('languages'),


            remote_size: [],

            data: generateData(),
            value: [1, 4],


            select_block: 1,
            form: {
                name: '',
                region: '',
                date1: '',
                date2: '',
                delivery: false,
                type: [],
                resource: '',
                desc: ''
            }
        }
    },
    mounted() {
        disableZoom(require('electron').webFrame);


        console.warn(this.lang_list, "lang_list")
        // this.remote_size = remote.getCurrentWindow().getSize();
        // console.warn(this.remote_size,"remote_size");
        //
        // remote.getCurrentWindow().on('resize', () => {
        //     this.remote_size = remote.getCurrentWindow().getSize();
        //     console.warn(this.remote_size,"remote_size");
        //
        // })
    },
    methods: {
        chose_block(select_block_id) {
            switch (select_block_id) {
                case 1:
                    this.remote_size = [750, 500];
                    break;
                case 2:
                    this.remote_size = [675, 285];
                    break;
                case 3:
                    this.remote_size = [625, 495];
                    break;
                case 4:
                    this.remote_size = [680, 185];
                    break;
                case 5:
                    this.remote_size = [530, 250];
                    break;
            }

            remote.getCurrentWindow().setSize(this.remote_size[0], this.remote_size[1])
            this.select_block = select_block_id;
        },
        onSubmit() {
            console.log('submit!');
        },
        changeTheme() {
            console.warn("set theme", this.theme);
            store.set("theme", this.theme);
            this.$refs.carouselObj.setActiveItem(this.theme);
        },
        changeLang() {
            store.set("lang", this.lang);
            this.$i18n.locale = this.lang;
            // ipcRenderer.send('ChangeLangEvent', this.lang);
            ipcRenderer.send('IPCMain', {
                name:"ChangeLangEvent",
                data:this.lang,
            });

        },
        changeInstallBugType() {
            console.warn(this.install_bug_type, "install_bug_type");
            store.set('common.install_bug_type', this.install_bug_type);
        },
        changeHowRestartWindow() {
            store.set("common.how_restart_window", this.how_restart_window);
        },
        changeAutoRun() {
            store.set("auto_run", this.auto_run);
            // ipcRenderer.send('AutoRunEvent', this.auto_run);
            ipcRenderer.send("IPCMain",{
                name:"AutoRunEvent",
                data:this.auto_run
            });
        },
        changeAutoMount() {
            store.set("auto_mount", this.auto_mount);
            console.warn(store.get("auto_mount"),"changeAutoMount")
        },
        changeMountShowMsg() {
            store.set("message.mount_show_msg", this.mount_show_msg);
        },
        changeUpdateShowMsg() {
            store.set("message.update_show_msg", this.update_show_msg);
        },
        changeErrorDiskMsg() {
            store.set("message.error_disk_msg", this.error_disk_msg);
        },

        changeAutoCheck() {
            store.set("update.auto_check", this.auto_check);
        },
        changeAutoBetaUpdate() {
            store.set("update.auto_beta_update", this.auto_beta_update);
        },
        openPrivacyUrl() {
            shell.openExternal(this.privacy_url);
        },
        versionStringCompare(preVersion = '', lastVersion = '') {
            var sources = preVersion.split('.');
            var dests = lastVersion.split('.');
            var maxL = Math.max(sources.length, dests.length);
            var result = 0;
            for (let i = 0; i < maxL; i++) {
                let preValue = sources.length > i ? sources[i] : 0;
                let preNum = isNaN(Number(preValue)) ? preValue.charCodeAt() : Number(preValue);
                let lastValue = dests.length > i ? dests[i] : 0;
                let lastNum = isNaN(Number(lastValue)) ? lastValue.charCodeAt() : Number(lastValue);
                if (preNum < lastNum) {
                    result = -1;
                    break;
                } else if (preNum > lastNum) {
                    result = 1;
                    break;
                }
            }
            return result;
        },
        checkSoftUpdate() {
            var _this = this;
            var cur_version = getPackageVersion();
            try {
                get('https://ntfstool.com/version.json').asString(function (err, ret) {
                    if (err) throw err;
                    var data = {
                        "version": "",
                        "url": "https://ntfstool.com/",
                        "title": "New Version Found",
                        "detail": "update"
                    };

                    try {
                        var getData = JSON.parse(ret);
                        if (!getData || typeof getData.version == "undefined" || !getData.version) {
                            saveLog.error("not found version!")
                            return;
                        }
                        if (typeof getData.version != "undefined") {
                            data.version = getData.version;
                        }
                        if (typeof getData.url != "undefined") {
                            data.url = getData.url;
                        }
                        if (typeof getData.title != "undefined") {
                            data.title = getData.title;
                        }
                        if (typeof getData.detail != "undefined") {
                            data.detail = getData.detail;
                        }
                    } catch (e) {
                        saveLog.warn("check version format error!", e)
                    }

                    if (typeof data.version != "undefined" && data.version) {
                        saveLog.warn({
                            cur_version: cur_version,
                            check_version: data.version
                        })
                        if (cur_version != data.version && _this.versionStringCompare(cur_version, data.version) < 0) {
                            var confirm_status = confirm(data.title + "(" + cur_version + "->" + data.version + ")" + "\n" + data.detail);
                            if (confirm_status) {
                                shell.openExternal(data.url);
                            }
                        } else {
                            alert("Already the latest version!");
                        }
                    } else {
                        saveLog.warn("check version format error!")
                    }
                });
            } catch (e) {
                saveLog.error("get update error!", e);
            }
        },
        resetConf() {
            var confirm_status = confirm(this.$i18n.t('ConfirmConfigtoreset'))
            if (confirm_status) {
                if (ipcRenderer.sendSync('IPCMain', "resetConf") == "succ" ? true : false) {
                    alert(this.$i18n.t('Theresetissuccessful'));
                    remote.getCurrentWindow().close();
                } else {
                    alert(this.$i18n.t('Resetfailed'));
                }
            }
        },
        openTransLang(){
            alert(this.$i18n.t('Viewthetranslation') + " service@ntfstool.com");
            shell.openExternal("https://github.com/ntfstool/ntfstool/blob/master/src/common/lang/en.js")
            shell.openExternal("https://github.com/ntfstool/ntfstool/blob/master/src/common/lang/"+this.lang+".js")
            shell.openExternal("mailto:service@ntfstool.com")
        }
    }
}