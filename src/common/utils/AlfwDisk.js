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
import {savePassword,execShell,execShellSudo,checkSudoPassword} from '@/common/utils/AlfwShell'
import {getStoreForDiskList,setStoreForDiskList,getMountType,watchStatus,ignoreItem,delIgnoreItem} from '@/common/utils/AlfwStore'
import {AlConst} from "@/common/utils/AlfwConst";
const saveLog = require('electron-log');
const {getDiskInfo, getDiskList} = require('diskutil')
const {ntfstool_bin} = require('ntfstool')
const {_} = require('lodash')
import {ipcRenderer, remote} from 'electron'
var reMountLock = [];//global lock
var fs= require("fs")
import {noticeTheSystemError} from '@/common/utils/AlfwCommon'


export function autoMountNtfsDisk(mountInfo,cb) {
    try{
        console.warn(mountInfo,"mountInfo")
        reMountNtfs(mountInfo.index).then(function () {
            cb();
        }).catch(function () {
            cb();
        })
    }catch (e) {
        console.error(e,"autoMountNtfsDisk");
        cb();
        //send log
    }
}

/**
 * reMountNtfs
 * @param index
 * @param force
 * @returns {Promise<any>}
 */
function reMountNtfs(index, force = false) {
    console.warn(index, "reMountNtfs start +++++++++++TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT++++++++++");
    reMountLock[index] = true;
    var link_dev = "/dev/" + index;
    return new Promise(async (resolve, reject) => {
        try {
            var info = await getDiskInfo(index);
            console.log(info, "reMountNtfs info");

            if (info.typebundle != "ntfs") {
                reMountLock[index] = false;
                reject("not is ntfs disk[" + index + "]!");
                return;
            }

           // if(_.get(info,"readonly") == true || force == true){
           //      var check_res1 = await execShell("mount |grep '" + link_dev + "'");
           //      if (check_res1) {
           //          if (force === true || check_res1.indexOf("read-only") >= 0) {
           //              console.warn("start to mount disk...",link_dev)
           //              setDiskMountPrending(index,-1)
           //
           //              await execShellSudo("diskutil unmount " + link_dev);
           //
           //          } else {
           //              reMountLock[index] = false;
           //              reject("disk is already mounted.[" + index + "]");
           //              return;
           //          }
           //      }
           //  }

            if(_.get(info,"mounted") == true){
                if(force === false && _.get(info,"readonly") != true){
                    reMountLock[index] = false;
                    console.warn("succ[" + index + "] is Already Moubted!");
                    resolve("succ[" + index + "] is Already Moubted!");
                    return ;
                }

                await execShellSudo("diskutil unmount " + link_dev);
            }


            // here should deeply notice the recursive for the watch for the [/Volumes]
            // WatchStatus = false
            watchStatus(false);

            var volumename = info.volumename ? info.volumename : getAutoVolumeName();
            volumename = volumename.replace( /volumes/gi , '').replace( /\//gi , '');
            var mount_path = '/Volumes/' + volumename;
            if (!fs.existsSync(mount_path)) {
                await execShellSudo("mkdir -p " + mount_path);
                //TODO ======================= this should be ignore
            }else{
                //the same name volumes
                var samename_res = await execShell("mount |grep '" + mount_path + " '");

                console.warn(samename_res,"samename_res");
                if(samename_res && samename_res.indexOf(index) <= 0){
                    console.warn("not found index");
                    volumename = volumename + "1";//rename
                    var mount_path = '/Volumes/' + volumename;
                    if (!fs.existsSync(mount_path)) {
                        await execShellSudo("mkdir -p " + mount_path);
                    }
                }
            }

            if(getMountType() == "inner"){
                console.warn("UseMountType:Inner")
                var run_res = await execShellSudo(`mount_ntfs -o rw,auto,nobrowse,noowners,noatime ${link_dev} ${mount_path}`);
            }else{
                console.warn("UseMountType:Outer")
                var run_res = await execShellSudo(`${ntfstool_bin} ${link_dev} ${mount_path} -o volname=${volumename}  -olocal -oallow_other   -o auto_xattr -o hide_hid_files`);
            }

            watchStatus(true);

            console.log(run_res, "run_res mount_ntfs");


            var check_res2 = await execShell("mount |grep '" + index + "'");
            if (check_res2 && check_res2.indexOf("read-only") <= 0) {
                reMountLock[index] = false;
                setDiskMountPrending(index,0)
                console.warn("start to mount disk...[ok]",link_dev)

                resolve("succ[" + index + "]");
            } else {
                setDiskMountPrending(index,-99)
                reMountLock[index] = false;
                reject("mount fail[" + index + "]");
            }
        } catch (e) {
            reMountLock[index] = false;
            watchStatus(true);
            if(typeof e == "string" && e.indexOf("unclean") >= 0){
                //The disk contains an unclean file system (0, 0).
                // Metadata kept in Windows cache, refused to mount.
                // Falling back to read-only mount because the NTFS partition is in an
                // unsafe state. Please resume and shutdown Windows fully (no hibernation
                // or fast restarting.)
                noticeTheSystemError("UNCLEANERROR",e);
            }

            saveLog.error(e, "reMountNtfs Error");
            reject(e)
        }
    })
}


function getAutoVolumeName() {
    if(typeof global["AutoVolumeNameTimes"] == "undefined"){
        global["AutoVolumeNameTimes"] = 0;
        return "AUntitled";
    }else{
        global["AutoVolumeNameTimes"]++;
        return "AUntitled" + global["AutoVolumeNameTimes"];
    }
}


function setDiskMountPrending(index,setStatus) {
    var diskList = getStoreForDiskList();
    if(typeof diskList["ext"] != "undefined"){
        for(var i in diskList["ext"]){
            if(diskList["ext"][i].index == index){
                diskList["ext"][i].status =  setStatus;
            }
        }
    }

    setStoreForDiskList(diskList,function () {
        ipcRenderer.send(AlConst.GlobalViewUpdate);
    })
}

/**
 * mountDisk
 * @param mount_path
 * @param link_path
 * @returns {Promise<any>}
 */
export function mountDisk(item) {
    console.warn(item, "mountDisk start +++++++++++++++++++++TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT")
    return new Promise(async (resolve, reject) => {
        try {
            //del ignore item
            delIgnoreItem(item.index);
            if (typeof item.info.typebundle != "undefined" && item.info.typebundle == "ntfs") {
                console.warn(item.index, "[ntfs mount]mountDisk start +++++++++++++++++++++TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT")
                reMountNtfs(item.index, true).then((res) => {
                    resolve(res);
                }).catch((err) => {
                    reject(err);
                });
                return;
            }
            //No other disks need to be mounted temporarily
            reject("not need mount");
        } catch (e) {
            saveLog.error(e, "mountDisk");
            reject(e)
        }
    })
}

/**
 * umount the disk
 * @param item
 * @returns {Promise<any>}
 */
export function uMountDisk(item) {
    console.warn(item, "mountDisk start +++++++++++++++++++++TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT")
    return new Promise(async (resolve, reject) => {
        try {
            //add ignore item
            ignoreItem(item.index);

            var dev_path = "/dev/" + item.index;
            //NTFS
            if (typeof item.info.typebundle != "undefined" && item.info.typebundle == "ntfs") {
                console.warn(item, "[NTFS]uMountDisk start +++++++++++++++++++++TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT")
                resolve(await execShellSudo(`umount ${dev_path}`));
            } else {
                console.warn(item, "eject uMountDisk start +++++++++++++++++++++TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT")
                resolve(await execShellSudo(`diskutil eject ${get_safe_ejst_disk_name(dev_path)}`));
            }
        } catch (e) {
            saveLog.error(e, "uMountDisk");
            reject(e)
        }
    })
}


/**
 * Returns the strict parent disk node
 * @param dev_path
 * @returns {string}
 */
function get_safe_ejst_disk_name(dev_path) {
    try {
        var safe_dev = dev_path.substring(0, 9);//Make sure /dev/disk exists
        var safe_dev2 = dev_path.substring(9);//Make sure  /dev/disk exists
        var find_index = safe_dev2.lastIndexOf('s');
        if (find_index >= 0) {
            var safe_path = safe_dev + safe_dev2.substring(0, find_index);
        } else {
            var safe_path = safe_dev + safe_dev2;
        }
        return safe_path;
    } catch (e) {
        saveLog.error(e, "_marktype");
    }
}

/**
 * openInFinder
 * @param path
 * @returns {Promise<any>}
 */
export function openInFinder(path) {
    return new Promise((resolve, reject) => {
        execShell(`open "${path}"`).then((res, err) => {
            console.log({
                res: res,
                err: err
            }, "openInFinder")
            if (res.indexOf("exist") >= 0) {
                reject()
            } else {
                resolve()
            }
        }).catch((e) => {
            saveLog.error(e, "openInFinder ok");
            reject(e)
        })
    })
}