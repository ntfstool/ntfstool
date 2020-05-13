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

const {getDiskList} = require('diskutil')
const {setStoreForDiskList, getStoreForDiskList, watchStatus, delAllIgnore, getStore} = require("@/common/utils/AlfwStore")
const {autoMountNtfsDisk} = require("@/common/utils/AlfwDisk")
import {unitTimesToRun, queueExec, filterNtfsNeedMountByDiskList} from '@/common/utils/AlfwCommon.js'
import {AlConst} from '@/common/utils/AlfwConst'

const {_} = require('lodash')

import {ipcRenderer, remote} from 'electron'

const watchmac = require("watch-mac")

export function fsListenMount() {
    var path = '/Volumes/';
    watchmac(path, function (data) {
        var device_file = typeof data.File != "undefined" ? data.File : "";

        if (typeof data.Event != "undefined" && data.Event == "CreteFileEvent") {
            console.warn("Start CreteFileEvent...");
            ipcRenderer.send("IPCMain", {
                name: "CreteFileEvent",
                data: device_file
            });

            //TODO clear all ignore disk
            delAllIgnore();
        }

        if (watchStatus() === true) {
            console.warn(data, "watchmac")
            updateDisklist();
        } else {
            console.warn("watchmac ignore[watchStatus False]");
        }
    })
}


export function updateDisklist(callback) {
    console.warn("updateDisklist Start +++");
    unitTimesToRun("getDiskList", function () {
        // console.warn("updateDisklist Start +++ ok 0");
        getDiskList().then((diskList) => {
            console.warn(diskList,"updateDisklist0");
            //filter inner unmounted
            if (typeof diskList.inner != "undefined") {
                diskList.inner = diskList.inner.filter(function (item) {
                    if (_.get(item, "info.readonly") === true) {
                        item.info.readonly = false;
                    }

                    if(!_.get(item, "info.typebundle")){
                        return false;
                    }

                    if (_.get(item, "info.typebundle").toLocaleLowerCase() === "apfs" && _.get(item, "info.mountpoint").toLocaleLowerCase().indexOf("/system/volumes/") >= 0) {
                        return false;
                    }

                    if (_.get(item, "type").toLocaleLowerCase().indexOf("boot") >= 0  || _.get(item, "type").toLocaleLowerCase() == "efi" || _.get(item, "info.typebundle").toLocaleLowerCase() == "efi" || _.get(item, "info.typebundle").toLocaleLowerCase() == "msr") {
                        return false;
                    }

                    return true;
                });
            }

            //filter ext unmounted
            if (typeof diskList.ext != "undefined") {
                diskList.ext = diskList.ext.filter(function (item) {
                    if (_.get(item, "info.volumename").replace(/\s+/g, "").indexOf("nofilesystem") > 0) {
                        return false;
                    }

                    if (_.get(item, "type").toLocaleLowerCase() == "efi" || _.get(item, "info.typebundle").toLocaleLowerCase() == "efi" || _.get(item, "info.typebundle").toLocaleLowerCase() == "msr") {
                        return false;
                    }

                    return true;
                });
            }

            //filter image unmounted
            if (typeof diskList.image != "undefined") {
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
                if (typeof callback == "function") callback()
                //send the global view update
                ipcRenderer.send("IPCMain", AlConst.GlobalViewUpdate);

                // if (typeof data.Event != "undefined" && data.Event == "CreteFileEvent") {
                //filter the ntfs to remount
                var needReMountList = filterNtfsNeedMountByDiskList(diskList);
                console.warn(needReMountList, "needReMountList");
                if (needReMountList && needReMountList.length > 0) {
                    for (var i in needReMountList) {
                        //exec one by one
                        queueExec("autoMountNtfsDisk", function (cb) {
                            //set is auto_mount
                            if (getStore("auto_mount") == false) {
                                console.warn("Set Can't AutoMount...");
                            } else {
                                autoMountNtfsDisk(needReMountList[i], function () {
                                    cb();
                                });
                            }
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