<template>
    <el-container class="al-main">
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
    </el-container>
</template>
<script>
    const {shell, ipcRenderer, remote} = require('electron')

    import {getPackageVersion, disableZoom, getSystemInfo,POST_LOG_URL} from '@/utils/utils'

    export default {
        components: {},
        data() {
            return {
                version: "-",
                statisticsImg: "",
                serial_number: "--",
                os_version: "",
            }
        },
        mounted() {
            this.statistics();
            disableZoom(require('electron').webFrame);
            this.version = getPackageVersion();
        },
        methods: {
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
            }
        }
    }

</script>

<style scoped>
    .main_about {
        background: #f1f1f1;
        width: 100%;
        height: 100vh;
        display: flex;
        flex-direction: column;
        padding-top: 10px;
    }

    .main_about div {
        display: flex;
        justify-content: center;
        margin: 5px;
        font-size: 14px;
        user-select: text;
    }
</style>