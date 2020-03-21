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
//
// import {getPackageVersion, disableZoom, getSystemInfo} from '@/common/utils/AlfwCommon.js'
//
// import {getDiskList,getDiskInfo} from 'diskutil'

const {getDiskInfo,getDiskList} = require('diskutil')
const {setStoreForDiskList,getStoreForDiskList} = require("@/common/utils/AlfwStore")

import {alEvent} from '@/common/utils/alEvent'
import {AlConst} from '@/common/utils/AlfwConst'
const watch = require('node-watch');
import {ipcRenderer, remote} from 'electron'
const fs = require('fs');
const watchmac = require("watch-mac")


export function test() {
    console.warn(getStoreForDiskList(),"getStoreForDiskList")
    //
    // getDiskList().then((diskList) => {
    //     console.log(diskList, "getDiskList");
    // });
}

export function fsListenMount() {
    var path = '/Volumes/';
    watchmac(path,function (data) {
        console.warn(data,"watchmac")
        if(typeof data.Event != "undefined" && data.Event == "CreteFileEvent"){
            getDiskList().then((diskList) => {
                console.log(diskList, "getDiskList");
                setStoreForDiskList(diskList);
            });
        }

    })

    diskLiskChange();

    return;
    //
    // try {
    //     fs.watch(path, function (event, filename) {
    //         console.warn(filename, "Fs watch Volumes")
    //     });
    // } catch (e) {
    //     getDiskList().then((diskList) => {
    //         console.log(diskList, "getDiskList");
    //         setStoreForDiskList(diskList);
    //     });
    // }
    //
    //
    // try {
    //     var monitorPath = '/Volumes/';
    //     watch(monitorPath, {recursive: false}, function (evt, name) {
    //         console.log("evt=%s|name=%s \n", evt, name);
    //         name = name.replace(monitorPath, "");
    //         if (evt != "remove") {
    //             //add new volume
    //             getDiskList().then((diskList) => {
    //                 console.log(diskList, "getDiskList");
    //                 setStoreForDiskList(diskList);
    //             });
    //         }
    //     });
    // } catch (e) {
    //     console.warn(e, "fsListenMount");
    // }
}

export function diskLiskChange() {
    console.warn("diskLiskChange listen")
    alEvent().on(AlConst.DiskListEvent,function (data) {
        console.warn(AlConst.DiskListEvent + " DiskListEvent")
        //send the global view update


        //filter the ntfs to remount
    })
}
