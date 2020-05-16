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
import {getStoreForDiskList,setStoreForDiskList,getMountType,watchStatus,ignoreItem,delIgnoreItem,fixUnclear} from '@/common/utils/AlfwStore'
import {AlConst} from "@/common/utils/AlfwConst";
const saveLog = require('electron-log');
const {getDiskInfo} = require('diskutil')
const {ntfstool_bin} = require('ntfstool')
const {_} = require('lodash')
import {ipcRenderer, remote} from 'electron'
// var reMountLock = [];//global lock
var fs= require("fs")
import {noticeTheSystemError} from '@/common/utils/AlfwCommon'


export function autoMountNtfsDisk(mountInfo,cb) {
    try{
        console.warn(mountInfo,"mountInfo")
        reMountNtfs(mountInfo).then(function () {
            typeof cb == "function" && cb();
        }).catch(function () {
            typeof cb == "function" && cb();
        })
    }catch (e) {
        console.error(e,"autoMountNtfsDisk");
        typeof cb == "function" && cb();
        //send log
    }
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
            delIgnoreItem(item.bsd_name);
            reMountNtfs(item, true).then((res) => {
                resolve(res);
            }).catch((err) => {
                reject(err);
            });
        } catch (e) {
            saveLog.error(e, "mountDisk");
            reject(e)
        }
    })
}


function sendReMountNtfsEvent(item,type) {
    typeof ipcRenderer != "undefined" && ipcRenderer.send('IPCMain', {
        name:"MountStatusEvent",
        data:{
            bsd_name: _.get(item,"bsd_name",""),
            type:type
        }
    })
}

/**
 * reMountNtfs
 * @param index
 * @param force
 * @returns {Promise<any>}
 */
function reMountNtfs(item, force = false) {
    var index = item.bsd_name;
    console.warn(index, "reMountNtfs start +++++++++++TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT++++++++++");
    // reMountLock[index] = true;
    var link_dev = "/dev/" + index;
    return new Promise(async (resolve, reject) => {
        try {
            //判断如果不是ntfs格式无需卸载挂载[插拔设备即可]
            if (_.get(item,"file_system","").toLowerCase().indexOf('ntfs') < 0) {
                reject("not is ntfs disk[" + index + "]!");
                return;
            }

            //手动点击是强制性的,自动挂载是判断性的
            if(force == true){
                sendReMountNtfsEvent(item,AlConst.MountStatus.UMOUNT_OK);
                // 先卸载再挂载 [手动模式]
                if(_.get(item,"mounted") == true){
                    uMountDisk(item,true);
                }
            }else{
                // 必须命令系统检查(脏消息问题) [自动挂载模式]
                var check_res = await execShell("mount | grep '"+link_dev+"'");
                var check_str = check_res.toString() + "";
                if(check_str){
                    if(check_str.toLowerCase().indexOf("read-only") >= 0){
                        //检查如果已经[只读]挂载,则卸载磁盘
                        console.warn(check_str.toLowerCase(),"reMountNtfs [只读]挂载,则卸载磁盘.....");
                        uMountDisk(item);
                    }else{
                        //检查如果已经[可读写]挂载,则退出
                        console.warn(check_str.toLowerCase(),"reMountNtfs [可读写]挂载,退出.....");
                        return;
                    }
                }
            }



            // here should deeply notice the recursive for the watch for the [/Volumes]
            // WatchStatus = false
            watchStatus(false);
            if(_.get(item,"_name") == _.get(item,"bsd_name")){
                console.error(item,"_name === bsd_name,reset _name null");
                item._name = "";
            }

            var volumename = item._name ? item._name : getAutoVolumeName(item);
            volumename = volumename.replace( /volumes/gi , '').replace( /\//gi , '');
            var mount_path = '/Volumes/' + volumename;

            if(getMountType() == "inner"){
                if (!fs.existsSync(mount_path)) {
                    await execShellSudo("mkdir '" + mount_path + "'");
                    //TODO ======================= this should be ignore
                }else{
                    //the same name volumes
                    var samename_res = await execShell("mount |grep '" + mount_path + "'");

                    console.warn(samename_res,"samename_res");
                    if(samename_res && samename_res.indexOf(index) <= 0){
                        console.warn("reMountNtfs not found index",index);
                        volumename = volumename + "1";//rename
                        var mount_path = '/Volumes/' + volumename;
                        if (!fs.existsSync(mount_path)) {
                            await execShellSudo("mkdir -p " + mount_path);
                        }
                    }
                }

                sendReMountNtfsEvent(item,AlConst.MountStatus.MOUNT_ON);

                var run_res = await execShellSudo(`mount_ntfs -o rw,auto,nobrowse,noowners,noatime ${link_dev} '${mount_path}'`);

                sendReMountNtfsEvent(item,AlConst.MountStatus.MOUNT_OK);
            }else{
                console.warn("reMountNtfs UseMountType:Outer");

                sendReMountNtfsEvent(item,AlConst.MountStatus.MOUNT_ON);

                // unclear -o remove_hiberfile
                if(fixUnclear(index) === true){
                    sendReMountNtfsEvent(item,AlConst.MountStatus.MOUNT_FIX_ON);
                    console.warn("reMountNtfs fixUnclear mode to mount",index);
                    var run_res = await execShellSudo(`${ntfstool_bin} ${link_dev} '${mount_path}' -o volname='${volumename}' -o remove_hiberfile -olocal -oallow_other   -o auto_xattr -o hide_hid_files`);
                }else{
                    var run_res = await execShellSudo(`${ntfstool_bin} ${link_dev} '${mount_path}' -o volname='${volumename}'  -olocal -oallow_other   -o auto_xattr -o hide_hid_files`);
                }

                sendReMountNtfsEvent(item,AlConst.MountStatus.MOUNT_OK);
            }

            watchStatus(true);

            console.log(run_res, "reMountNtfs run_res mount_ntfs");


            var check_res2 = await execShell("mount |grep '" + index + "'");
            if (check_res2 && check_res2.indexOf("read-only") <= 0) {
                // reMountLock[index] = false;
                // setDiskMountPrending(index,0)
                console.warn("reMountNtfs start to mount disk...[ok]",link_dev)

                resolve("succ[" + index + "]");
            } else {
                // setDiskMountPrending(index,-99)
                // reMountLock[index] = false;
                reject("mount fail[" + index + "]");
            }
        } catch (e) {
            sendReMountNtfsEvent(item,AlConst.MountStatus.MOUNT_OK);

            // reMountLock[index] = false;
            watchStatus(true);
            if(typeof e == "string" && e.indexOf("itself") >= 0 && e.indexOf("OSXFUSE") >= 0){
                console.warn("reMountNtfs busy try ...",e);
                resolve("succ2[" + index + "]");
            }

            if(typeof e == "string" && e.indexOf("unclean") >= 0){
                //The disk contains an unclean file system (0, 0).
                // Metadata kept in Windows cache, refused to mount.
                // Falling back to read-only mount because the NTFS partition is in an
                // unsafe state. Please resume and shutdown Windows fully (no hibernation
                // or fast restarting.)
                console.warn("reMountNtfs Catch unclean");
                // noticeTheSystemError("UNCLEANERROR",e);

                fixUnclear(index,true);
            }

            saveLog.error(e, "reMountNtfs Error");
            reject(e)
        }
    })
}

//直接和卷名关联
function getAutoVolumeName(item) {
    if(typeof item.bsd_name != "undefined" && item.bsd_name){
        return "NT" + item.bsd_name.replace("disk","").toUpperCase();
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
        ipcRenderer.send("IPCMain",AlConst.GlobalViewUpdate);
    })
}



/**
 * umount the disk
 * @param item
 * @returns {Promise<any>}
 */
export function uMountDisk(item,user_action = false) {
    console.warn(item, "mountDisk start +++++++++++++++++++++TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT")
    return new Promise(async (resolve, reject) => {
        try {
            //add ignore item
            if(user_action == true){
                ignoreItem(item.bsd_name);
            }

            var dev_path = "/dev/" + item.bsd_name;
            //NTFS
            sendReMountNtfsEvent(item,AlConst.MountStatus.UMOUNT_ON);
            if (typeof item.file_system != "undefined" && item.file_system.toLowerCase().indexOf('ntfs') >= 0) {
                console.warn(item, "[NTFS]uMountDisk start +++++++++++++++++++++TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT")
                var res = await execShellSudo(`umount -f '${dev_path}'`);
            } else {
                console.warn(item, "eject uMountDisk start +++++++++++++++++++++TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT")
                var res = await execShellSudo(`diskutil eject '${get_safe_ejst_disk_name(dev_path)}'`);
            }
            sendReMountNtfsEvent(item,AlConst.MountStatus.UMOUNT_OK);

            resolve(res);
        } catch (e) {
            saveLog.error(e, "uMountDisk");
            sendReMountNtfsEvent(item,AlConst.MountStatus.UMOUNT_OK);
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