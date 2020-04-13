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

const {getDiskInfo, getDiskList} = require('diskutil')
const {setStoreForDiskList, getStoreForDiskList,watchStatus} = require("@/common/utils/AlfwStore")
const {autoMountNtfsDisk} = require("@/common/utils/AlfwDisk")
import {unitTimesToRun, queueExec, filterNtfsNeedMountByDiskList} from '@/common/utils/AlfwCommon.js'
import {alEvent} from '@/common/utils/alEvent'
import {AlConst} from '@/common/utils/AlfwConst'
const {_} = require('lodash')

import {ipcRenderer, remote} from 'electron'

const watchmac = require("watch-mac")


export function test() {
    console.warn(getStoreForDiskList(), "getStoreForDiskList")
    //
    // getDiskList().then((diskList) => {
    //     console.log(diskList, "getDiskList");
    // });
}

export function fsListenMount() {
    var path = '/Volumes/';
    watchmac(path, function (data) {
        if(watchStatus() === true){
            console.warn(data, "watchmac")
            updateDisklist();
        }else{
            console.warn("watchmac ignore[watchStatus False]");
        }
    })

}


export function updateDisklist(callback) {
    unitTimesToRun("getDiskList", function () {

        getDiskList().then((diskList) => {
            //filter inner unmounted
            if(typeof diskList.inner != "undefined") {
                diskList.inner = diskList.inner.filter(function (item) {
                    if(_.get(item, "info.readonly") === true){
                        item.info.readonly = false;
                    }
                    if (_.get(item, "info.typebundle") === "apfs" && _.get(item, "info.mountpoint").indexOf("/System/Volumes/") >= 0) {
                        return false;
                    } else {
                        return true;
                    }
                });
            }


            //filter image unmounted
            if(typeof diskList.image != "undefined") {
                diskList.image = diskList.image.filter(function (item) {
                    if (_.get(item, "info.mounted") === false) {
                        return false;
                    } else {
                        return true;
                    }
                });
            }

            console.log(diskList, "getDiskList");
            setStoreForDiskList(diskList, function () {
                if(typeof callback == "function") callback()
                //send the global view update
                ipcRenderer.send(AlConst.GlobalViewUpdate, "");

                // if (typeof data.Event != "undefined" && data.Event == "CreteFileEvent") {
                //filter the ntfs to remount
                var needReMountList = filterNtfsNeedMountByDiskList(diskList);
                console.warn(needReMountList, "needReMountList");
                if (needReMountList && needReMountList.length > 0) {
                    for (var i in needReMountList) {
                        //exec one by one
                        queueExec("autoMountNtfsDisk", function (cb) {
                            autoMountNtfsDisk(needReMountList[i], function () {
                                cb();
                            });
                        })
                    }
                }
            });
        });
    })
}

// export function globalUpdate() {
//     ipcRenderer.on("GlobalUpdateExent", (event, arg) => {
//         saveLog.info(arg, "GlobalUpdateExent Come");
//         var diskList = getStoreForDiskList();
//
//     });
// }