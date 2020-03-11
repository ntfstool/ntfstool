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
import {getDiskList, listenSudoPwd, getDiskFullInfo, uMountDisk, mountDisk, openInFinder,getPackageVersion,disableZoom} from '@/utils/utils'
import {ipcRenderer,shell} from 'electron'
import { alEvent } from '@/utils/alevent.js';
const Store = require('electron-store');
const store = new Store();
const {remote} = require('electron')

export default {
    components: {},
    data() {
        const generateData = _ => {
            const data = [];
            for (let i = 1; i <= 15; i++) {
                data.push({
                    key: i,
                    label: `磁盘 ${i}`,
                    disabled: i % 4 === 0
                });
            }
            return data;
        };

        return {
            auto_run:store.get("auto_run") == false ? false : true,
            theme: store.get("theme") != "undefined" ? store.get("theme") : 1,
            lang: store.get("lang") != "undefined" ? store.get("lang") : "english",
            show_menu:store.get("show_menu") == false ? false : true,
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

            lang_list:this.$t('languages'),//多语言列表


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


        console.warn(this.lang_list,"lang_list")
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
                    this.remote_size = [650, 185];
                    break;
                case 5:
                    this.remote_size = [530, 330];
                    break;
            }

            remote.getCurrentWindow().setSize(this.remote_size[0], this.remote_size[1])
            this.select_block = select_block_id;
        },
        onSubmit() {
            console.log('submit!');
        },
        setToggleTrayMenu(){
            this.show_menu = ipcRenderer.sendSync('toggleTrayMenu') == "destroy" ? false : true;
            store.set("show_menu",this.show_menu);
        },
        changeTheme() {
            console.warn("set theme",this.theme);
            store.set("theme",this.theme);
            this.$refs.carouselObj.setActiveItem(this.theme);//轮播图切换
        },
        changeLang() {
            store.set("lang",this.lang);
            this.$i18n.locale = this.lang;
            ipcRenderer.send('ChangeLangEvent',this.lang);

        },
        changeInstallBugType() {
            console.warn(this.install_bug_type,"install_bug_type");
            store.set('common.install_bug_type',this.install_bug_type);
        },
        changeHowRestartWindow() {
            store.set("common.how_restart_window",this.how_restart_window);
        },
        changeAutoRun(){
            store.set("auto_run",this.auto_run);
        },


        changeMountShowMsg() {
            store.set("message.mount_show_msg",this.mount_show_msg);
        },
        changeUpdateShowMsg() {
            store.set("message.update_show_msg",this.update_show_msg);
        },
        changeErrorDiskMsg() {
            store.set("message.error_disk_msg",this.error_disk_msg);
        },

        changeAutoCheck() {
            store.set("update.auto_check",this.auto_check);
        },
        changeAutoBetaUpdate() {
            store.set("update.auto_beta_update",this.auto_beta_update);
        },
        openPrivacyUrl(){
            shell.openExternal(this.privacy_url);
        },
        checkSoftUpdate(){
            alert("检查更新");
        },
        resetConf(){
            var confirm_status = confirm("确认重置所有配置为默认?")
            if (confirm_status) {
                if(ipcRenderer.sendSync('MainMsgFromRender',"resetConf") == "succ" ? true : false){
                    alert("重置成功,请重新开启配置页面");
                    remote.getCurrentWindow();
                    remote.getCurrentWindow().close();
                }else{
                    alert("重置失败");
                }
            }
        }
    }
}