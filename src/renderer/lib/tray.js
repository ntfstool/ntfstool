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
const {shell,ipcRenderer,remote} = require('electron')

export default {
    components: {},
    data() {
        return {
            title:"AlNtfs",
            menu_box1:false,
        }
    },
    mounted() {
        this._title = this.title;
        console.warn(this.$refs, "this.$refsa")
        this.resetSize();

        remote.getCurrentWindow().on('blur', () => {
            this.menu_box1 = false;
        })
    },
    methods: {
        test(){
          console.warn("ASDFASFDSF")
        },
        openMenuBox(id){
            this[id] = this[id] ? false : true
        },
        setTitle(title){
            if(typeof title != "undefined"){
                this.title = title;
            }else{
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
        pushAll: () => {
            alert("ok");

            let option = {
                title: "title",
                body: "body",
                icon: "../static/hhw.ico",
                href: 'https://www.iqiyi.com/v_19rqz6uit0.html'
            };

            // 创建通知并保存
            let hhwNotication = new window.Notification(option.title, option);

            // 当通知被点击时, 用默认浏览器打开链接
            hhwNotication.onclick = function () {
                shell.openExternal(option.href)
            }
        },
        openSettingPage(){
            this.menu_box1 = false;
            ipcRenderer.send('MainMsgFromRender', 'openSettingPage')
        },
        openAboutPage(){
            this.menu_box1 = false;
            ipcRenderer.send('MainMsgFromRender', 'openAboutPage')
        },
        openFeedBackPage(){
            this.menu_box1 = false;
            ipcRenderer.send('MainMsgFromRender', 'openFeedBackPage')
        },
        openHomePage(){
            this.menu_box1 = false;
            ipcRenderer.send('MainMsgFromRender', 'openHomePage')
        },
        exitAll(){
            this.menu_box1 = false;
            ipcRenderer.send('MainMsgFromRender', 'exitAll')
        }
    }
}