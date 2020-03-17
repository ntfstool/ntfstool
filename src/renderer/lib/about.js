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

const {shell} = require('electron')
import {getPackageVersion, disableZoom, getSystemInfo} from '@/common/utils/AlfwCommon.js'
import {POST_LOG_URL} from '@/common/utils/AlfwConst.js'



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
        },
    }
}