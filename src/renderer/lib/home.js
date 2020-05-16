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
import {getPackageVersion, disableZoom,filter_menu_show} from '@/common/utils/AlfwCommon.js'
import {
    clearPwd,
    getMountNotifyStatus,
    getMenuShowConf,
    setStore
} from '@/common/utils/AlfwStore'
import {
    uMountDisk,
    mountDisk,
    openInFinder,
    autoMountNtfsDisk
} from '@/common/utils/AlfwDisk'

const {_} = require('lodash')

import {fsListenMount, updateDisklist} from '@/renderer/lib/diskMonitor'
import {AlConst} from "@/common/utils/AlfwConst";

export default {
    components: {},
    data() {
        return {
            diskMap:[],
            loading: 0,
            version: "-",
            menu_box: false,
            menu_top_box: false,
            select_item: {
                "_name": "",
                "bsd_name": "",
                "group": "-",
                "file_system": "-",
                "free_space_in_bytes": 0,
                "ignore_ownership": "-",
                "mounted":true,
                "mount_status":"",
                "mount_point": "-",
                "physical_drive": {
                    "device_name": "-",
                    "is_internal_disk": "no",
                    "media_name": "-",
                    "partition_map_type": "-",
                    "protocol": "-"
                },
                "size_in_bytes": 0,
                "total_size": "",
                "percentage": "",
                "volume_uuid": "-",
                "writable": "-"
            },
            firstTime:true,
            showDebugMenu: process.env.NODE_ENV === 'development' ? true : false,
            scrollShow:false,
            menu_show_conf:{
                inner:true,
                ext:true,
                image:true
            },
        }
    },
    mounted() {
        var _this = this;
        window.addEventListener('scroll',this.handleScroll,true)

        this.setVersion();

        this.refreshDevice();

        this.menu_show_conf = getMenuShowConf();
        console.warn(this.menu_show_conf,"init this.menu_show_conf")

        disableZoom(require('electron').webFrame);

        fsListenMount();

        ipcRenderer.on("HomeEvent", (event,args) => {
            console.warn("HomeEvent",args);

            if(_.get(args,"type") == AlConst.GlobalViewUpdate){
                this.diskMap = filter_menu_show(_.get(args,"data"));

                console.warn( this.diskMap,"Home diskMap");

                if(!_.get(_this.select_item, "_name") && !_.get(_this.select_item, "bsd_name") ||
                    -1 == _.findIndex(_.get(this.diskMap,_this.select_item.group), function(o) { return o.bsd_name == _this.select_item.bsd_name; })){
                    if (_.get(this.diskMap, "inner[0]")) {
                        _this.select_item = _.get(this.diskMap, "inner[0]");
                        console.warn(_this.select_item,"Home  _this.select_item +++++++++++++++++")
                    }
                }
            }

            if(_.get(args,"type") == "CreteFileEvent"){
                // console.warn("Home on CreteFileEvent...")
                setTimeout(function () {
                    console.warn(args,"start CreteFileEvent refreshDevice ... (3)")
                    _this.refreshDevice();
                }, 3000)

                setTimeout(function () {
                    console.warn(args,"start CreteFileEvent refreshDevice ... (8)")
                    _this.refreshDevice();
                }, 8000)

                setTimeout(function () {
                    console.warn(args,"start CreteFileEvent refreshDevice ... (18)")
                    _this.refreshDevice();
                }, 18000);
            }

            if(_.get(args,"type") == "ChangeLangEvent"){
                console.warn("home wind ChangeLangEvent", _.get(args,"data"));
                this.$i18n.locale = _.get(args,"data");
            }

            if(_.get(args,"type") == "MountStatusEvent"){
                var for_bsd_name = _.get(args,"data.bsd_name");
                var type = _.get(args,"data.type");
                if(typeof this.diskMap["ext"] != "undefined" && this.diskMap["ext"].length > 0){
                    for(var i in this.diskMap["ext"]){
                        if(this.diskMap["ext"][i]["bsd_name"] == for_bsd_name){
                            var mount_status = "";
                            if(type == AlConst.MountStatus.UMOUNT_ON){
                                mount_status = "&#xe676;";//-> 1/4
                            }else if(type == AlConst.MountStatus.UMOUNT_OK){
                                mount_status = "&#xe679;";//-> 1/3
                            }else if(type == AlConst.MountStatus.MOUNT_ON){
                                mount_status = "&#xe67e;"; //-> 1/2
                            }else if(type == AlConst.MountStatus.MOUNT_FIX_ON){
                                mount_status = "&#xe680;"; //-> 3/4
                            }else if(type == AlConst.MountStatus.MOUNT_OK){
                                mount_status = "&#xe67a;"; // 1
                            }
                            this.diskMap["ext"][i]["mount_status"] = mount_status;
                            this.diskMap.__ob__.dep.notify()
                        }
                    }
                }
            }
        });

        // remote.getCurrentWindow().on('focus', function () {
        //     console.warn("currentWindow focus[todo: check diskutils <=> mount]");
        //     _this.refreshDevice();
        // })
    },
    methods: {
        handleScroll(e){
            var _this = this;
            if(!this.scrollShow){
                _this.scrollShow = true;
                setTimeout(function () {
                    _this.scrollShow = false;
                },1000)
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
        submit_change_name(){
            if(typeof this.$refs.name_input != "undefined" && this.$refs.name_input.getAttribute("pre_value") != this.select_item._name){
                console.warn(this.select_item._name,"submit_change_name");
                alert(this.select_item._name);
            }else{
                console.warn("Same submit_change_name");
            }

        },
        changeVolumeName(select_item) {
            if(select_item.group != "ext"){
                alert("该磁盘不支持重命名");
                return;
            }

            // name_input
            console.warn(this.$refs.name_input,"this.$ref")

            console.warn(this.$refs.name_input.getAttribute("pre_value"),"submit_change_name prename attr");

            this.$refs.name_input.focus();

            return;
            this.$prompt("", this.$i18n.t('Pleaseenteranewname'), {
                showClose: false,
                inputValue: select_item._name,
                confirmButtonText: this.$i18n.t('Confirm'),
                cancelButtonText: this.$i18n.t('Cancel'),
            }).then(({value}) => {
                this.$alert("ok " + value);
            })
        },
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
                    // console.warn("uMountDisk res", res);
                    // let option = {
                    //     title: "NTFSTool",
                    //     body: item._name + " " + _this.$i18n.t('Diskuninstallsucceeded'),
                    // };
                    // new window.Notification(option.title, option);
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
                // let option = {
                //     title: "NTFSTool",
                //     body: item._name + " " + _this.$i18n.t('Diskmountedsuccessfully'),
                // };
                // new window.Notification(option.title, option);
            })
        },
        choseDisk(item) {
            console.warn(item, "choseDisk");

            this.submit_change_name();

            if (typeof item.bsd_name != "undefined") {
                this.select_item = item;
            } else {
                console.warn(item, "choseDisk Error");
            }
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
        openSysSetting() {
            console.warn(" openSysSetting click");
            ipcRenderer.send('IPCMain', {
                name:"openPageByName",
                data:"openSettingPage"
            })
        },
        setVersion() {
            this.version = getPackageVersion();
        },
        openSettingPage() {
            this.menu_box = false;
            ipcRenderer.send('IPCMain', {
                name:"openPageByName",
                data:"openSettingPage"
            })
        },
        openAboutPage() {
            this.menu_box = false;
            ipcRenderer.send('IPCMain', {
                name:"openPageByName",
                data:"openAboutPage"
            })
        },
        openFeedBackPage() {
            this.menu_box = false;
            ipcRenderer.send('IPCMain', {
                name:"openPageByName",
                data:"openFeedBackPage"
            })
        },
        exitAll() {
            this.menu_box = false;
            ipcRenderer.send('IPCMain',"exitAll")
        },
        clearPwd() {
            this.menu_box = false;
            clearPwd();
        },
        menu_show_set(group){
            if(typeof  this.menu_show_conf[group] != "undefined"){
                this.menu_show_conf[group] = !this.menu_show_conf[group];
                setStore("menu_show_conf",this.menu_show_conf);
                this.refreshDevice();

                ipcRenderer.send('IPCMain', {
                    name:AlConst.GlobalViewUpdate,
                    data:this.diskMap
                })

                this.menu_top_box = false;
            }else{
                console.error({group:group,menu_show_conf:this.menu_show_conf},"menu_show_c group err");
            }
        },
        writeable_fix(item){
            console.warn(item,"writeable_fix")
            autoMountNtfsDisk(item);
        }
    }
}