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
var reMountLock = [];//global lock

/**
 * _ignore
 * @param disk_list
 * @returns {*}
 */
function _ignore(disk_list) {
    return disk_list.filter(function (list) {
        try {
            //APFS: Preboot Recovery VM ignore
            if (typeof list.type != "undefined" && list.type.toLowerCase().indexOf("apfs") >= 0) {
                if (typeof list.name != "undefined") {
                    if (list.name.toLowerCase().indexOf("preboot") == 0) {
                        return false;
                    }

                    if (list.name.toLowerCase().indexOf("recovery") == 0) {
                        return false;
                    }

                    if (list.name.toLowerCase().indexOf("vm") == 0) {
                        return false;
                    }

                    // Apple_APFS Container disk1 type
                    if (list.type.toLowerCase().indexOf("container") >= 0 && list.name.toLowerCase().indexOf("disk") >= 0) {
                        return false;
                    }
                }
            }

            //EFI: efi Ignore
            if (typeof list.type != "undefined" && list.type.toLowerCase().indexOf("efi") >= 0) {
                if (typeof list.name != "undefined") {
                    if (list.name.toLowerCase().indexOf("efi") == 0) {
                        return false;
                    }
                }
            }
            return true;
        } catch (e) {
            saveLog.error(e, "_ignore");
        }
    });
}

/**
 * show_type:  image ext  inner
 * @param disk_list
 * @returns {*}
 */
function _marktype(disk_list) {
    try {
        var disk_list_group = {
            inner: [],
            ext: [],
            image: [],
        };
        for (var i in disk_list) {
            if (disk_list[i]['type'].indexOf("APFS") >= 0) {
                disk_list[i]["group"] = "inner";
                disk_list_group.inner.push(disk_list[i]);
                continue;
            }

            if (disk_list[i]['disk_mount'][0].indexOf("image") >= 0) {
                disk_list[i]["group"] = "image";
                disk_list_group.image.push(disk_list[i]);
                continue;
            }


            //The rest are in ext mode
            disk_list[i]["group"] = "ext";
            disk_list_group.ext.push(disk_list[i]);
        }
        return disk_list_group;
    } catch (e) {
        saveLog.error(e, "_marktype");
    }
}

/**
 * Is it possible to push
 * @param disk_list
 * @returns {*}
 */
function _checkPushable(disk_list) {
    try {
        for (var i in disk_list) {
            if (disk_list[i]['disk_mount'][0].indexOf("image") >= 0 || disk_list[i]['disk_mount'][0].indexOf("ext") >= 0) {
                disk_list[i]["canPush"] = true;
                continue;
            }
        }
        return disk_list;
    } catch (e) {
        saveLog.error(e, "_marktype");
    }
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
 * Get the disk list
 * @returns {Promise<any>}
 */
export function getDiskList() {
    return new Promise((resolve, reject) => {
        execShell(`diskutil list`).then(async (res) => {
            try {
                var disk_list = [];
                let diskArr = res.split("/dev/disk");
                for (var key in diskArr) {
                    if (diskArr[key].trim()) {
                        var diskArr2 = diskArr[key].split("\n").map(item => {
                            return item.trim();
                        }).filter(function (s) {
                            s = s.trim();
                            //Must not be empty
                            if (s) {
                                //Remove 0: #: line
                                if (s.indexOf("0:") !== 0 && s.indexOf("#:") !== 0) {
                                    //Remove the line without the :
                                    if (s.indexOf(":") >= 0) {
                                        return true;
                                    }
                                }
                            }
                        });

                        let disk_mount = "";

                        if (typeof diskArr2[0] != "undefined") {
                            disk_mount = diskArr2[0].replace(/.*\((.*)\).*/i, "$1").split(",").map(item => {
                                return item.trim()
                            });
                        }

                        for (var i = 1; i < diskArr2.length; i++) {
                            if (diskArr2[i]) {
                                let val = diskArr2[i].split("  ").map(item => {
                                    return item.trim()
                                }).filter(function (s) {
                                    return s && s.trim();
                                });

                                let disk_map = {
                                    disk_mount: disk_mount,
                                    canPush: false,
                                    type: "",
                                    name: "",
                                    size: "",
                                    size_wei: "",
                                    index: "",
                                    info: [],
                                };

                                if (val.length == 4) {
                                    let val1 = val[1].split(" ").map(item => {
                                        return item.trim()
                                    });
                                    // console.log(val1, "val1");
                                    if (val1.length > 1) {
                                        disk_map.name = val1.pop();
                                        disk_map.type = val1.join(" ");
                                    } else {
                                        disk_map.name = "";
                                        disk_map.type = val1.join(" ");
                                    }

                                    let val2 = val[2].split(" ").map(item => {
                                        return item.trim()
                                    });
                                    if (val2.length == 2) {
                                        disk_map.size = val2[0];
                                        disk_map.size_wei = val2[1];
                                    }

                                    disk_map.index = val[3];
                                }

                                disk_list.push(disk_map);
                            }
                        }
                    }

                }
                disk_list = _ignore(disk_list);
                disk_list = _checkPushable(disk_list);
                let disk_list_group = _marktype(disk_list);


                //Update details
                getDiskFullInfo(disk_list_group).then((diskList) => {
                    resolve(diskList)
                }).catch((err) => {
                    reject(err)
                });
            } catch (e) {
                saveLog.error(e, "getDiskList");
            }

        }).catch((e) => {
            saveLog.error(e, "getDiskList");
            reject(e)
        })
    })
}

/**
 * getDiskFullInfo
 * @param disklist
 */
export async function getDiskFullInfo(disklist) {
    try {
        for (var key in disklist) {
            for (var disk_index in disklist[key]) {
                let info = await getDiskInfo(disklist[key][disk_index]["index"]);
                disklist[key][disk_index]["info"] = info;
                if (!disklist[key][disk_index]["name"] && info.mountpoint) {
                    disklist[key][disk_index]["name"] = info.mountpoint.replace(/\/Volumes\/(.*)/i, "$1");
                }

                //NTFS needs to be remounted
                if (disklist[key][disk_index]["info"]["readonly"] && disklist[key][disk_index]["info"]["typebundle"] == "ntfs") {
                    var _index = disklist[key][disk_index].index;

                    if (typeof reMountLock[_index] != "undefined" && reMountLock[_index]) {
                        console.log(_index + " is busy... +++++++++++TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT++++++++++");
                    } else {
                        reMountNtfs(_index).then((res) => {
                            alEvent.$emit('doRefreshEvent');//Send the refresh event
                            console.warn(res, ">>>  reMountNtfs then +++++++++++TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT++++++++++");
                        }).catch((err) => {
                            console.warn(err, ">>>  reMountNtfs catch +++++++++++TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT++++++++++");
                        });
                    }
                }
            }
        }
        return disklist;
    } catch (e) {
        saveLog.error(e, "getDiskFullInfo");
    }
}


/**
 * getDiskInfo
 * @param index
 */
export function getDiskInfo(index) {
    let disk_path = "/dev/" + index;
    return new Promise((resolve, reject) => {
        execShell("diskutil info " + disk_path).then(async (info) => {
            try {
                var infoArr = info.split("\n").map(item => {
                    return item.trim();
                }).filter((item) => {
                    return item;
                });
                var infoArr2 = [];
                for (let i in infoArr) {
                    infoArr[i] = infoArr[i].split(":").map(item => {
                        return item.trim();
                    });
                    infoArr2[infoArr[i][0]] = infoArr[i][1];
                }

                //Filter the key information
                var infoArr3 = {
                    "volumename": "",
                    "mounted": "",
                    "mountpoint": "",
                    "typebundle": "",
                    "protocol": "",
                    "uuid": "",
                    "total_size": "",
                    "total_size_wei": "",
                    "used_size": "",
                    "used_size_wei": "",
                    "readonly": "",
                    "percentage": ""
                };
                var disk_dize;
                var disk_size_wei;
                for (let i in infoArr2) {
                    let key = i.toLowerCase().replace(/\s+/g, "");
                    // console.warn(key,"key")
                    if (key.indexOf("volumename") >= 0) {
                        infoArr3.volumename = infoArr2[i].toLowerCase();
                    }
                    if (key.indexOf("mounted") >= 0) {
                        infoArr3.mounted = infoArr2[i].toLowerCase() == "yes" ? true : false;
                    }
                    if (key.indexOf("mountpoint") >= 0) {
                        infoArr3.mountpoint = infoArr2[i];
                    }
                    if (key.indexOf("filesystempersonality") >= 0) {
                        infoArr3.typebundle = infoArr2[i].toLowerCase();
                    }
                    if (key.indexOf("Type (Bundle)") >= 0) {
                        infoArr3.typebundle = infoArr2[i].toLowerCase();
                    }
                    if (key.indexOf("uuid") >= 0) {
                        infoArr3.uuid = infoArr2[i];
                    }
                    if (key.indexOf("protocol") >= 0) {
                        infoArr3.protocol = infoArr2[i];
                    }
                    if (key.indexOf("totalspace") >= 0) {
                        let total_size_arr = infoArr2[i].replace(/([^\(]*).*/, "$1").trim().split(" ").map(item => {
                            return item.trim();
                        });
                        infoArr3.total_size = Math.round(parseFloat(total_size_arr[0]) * 100) / 100;
                        infoArr3.total_size_wei = total_size_arr[1];
                    }
                    if (key.indexOf("disksize") >= 0) {
                        let total_size_arr = infoArr2[i].replace(/([^\(]*).*/, "$1").trim().split(" ").map(item => {
                            return item.trim();
                        });
                        disk_dize = Math.round(parseFloat(total_size_arr[0]) * 100) / 100;
                        disk_size_wei = total_size_arr[1];
                    }


                    if (key.indexOf("usedspace") >= 0) {
                        let used_size_arr = infoArr2[i].replace(/([^\(]*).*/, "$1").trim().split(" ").map(item => {
                            return item.trim();
                        });
                        infoArr3.used_size = Math.round(parseFloat(used_size_arr[0]) * 100) / 100;
                        infoArr3.used_size_wei = used_size_arr[1];
                    }
                    if (infoArr3.total_size > 0) {
                        infoArr3.percentage = Math.round((infoArr3.used_size / infoArr3.total_size * 100) * 100) / 100;
                    }


                    if (key.indexOf("read-onlyvolume") >= 0) {
                        infoArr3.readonly = infoArr2[i].toLowerCase() == "yes" ? true : false;
                    }
                }
                if (!infoArr3.total_size && disk_dize) {
                    infoArr3.total_size = disk_dize;
                    infoArr3.total_size_wei = disk_size_wei;
                }
                //If disk information has not been obtained
                if ((!infoArr3.total_size || !infoArr3.used_size) && info.length > 20) {
                    var sizeData = formatDiskSize(info);
                    if (sizeData["total"]) {
                        infoArr3.total_size = sizeData["total"];
                        infoArr3.total_size_wei = sizeData["wei"];
                    }

                    if (!infoArr3.used_size && sizeData["used"]) {
                        infoArr3.used_size = sizeData["used"];
                        infoArr3.used_size_wei = sizeData["wei"];
                    }

                    if (!infoArr3.percentage && sizeData["percentage"]) {
                        infoArr3.percentage = sizeData["percentage"];
                    }
                }
                resolve(infoArr3);
            } catch (e) {
                saveLog.error(e, "getDiskInfo");
            }
        })
    })
}


/**
 * reMountNtfs
 * @param index
 * @param force
 * @returns {Promise<any>}
 */
export function reMountNtfs(index, force = false) {
    // console.warn(index, "reMountNtfs start +++++++++++TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT++++++++++");
    reMountLock[index] = true;
    var link_dev = "/dev/" + index;
    return new Promise(async (resolve, reject) => {
        try {
            var info = await getDiskInfo(index);
            // console.log(info, "info");
            if (info.typebundle != "ntfs") {
                reMountLock[index] = false;
                reject("not is ntfs disk[" + index + "]!");
                return;
            }
            //Check if overload is needed
            var check_res1 = await execShell("mount |grep '" + index + "'");
            if (check_res1) {
                if (force === true || check_res1.indexOf("read-only") >= 0) {
                    await execShellSudo("umount " + link_dev);
                } else {
                    reMountLock[index] = false;
                    reject("disk is already mounted.[" + index + "]");
                    return;
                }

            }

            var volumename = info.volumename ? info.volumename : "AUntitled";
            var mount_path = '/Volumes/' + volumename;

            var run_res = await execShellSudo("mkdir -p " + mount_path);

            var run_res = await execShellSudo(`mount_ntfs -o rw,auto,nobrowse,noowners,noatime ${link_dev} ${mount_path}`);
            console.log(run_res, "run_res mount_ntfs");


            var check_res2 = await execShell("mount |grep '" + index + "'");
            if (check_res2 && check_res2.indexOf("read-only") <= 0) {
                reMountLock[index] = false;
                resolve("succ[" + index + "]");
            } else {
                reMountLock[index] = false;
                reject("mount fail[" + index + "]");
            }
        } catch (e) {
            reMountLock[index] = false;
            saveLog.error(e, "reMountNtfs");
            reject(e)
        }
    })
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

/////////////////////////////////////////////////////////////////////////////////////////////////////////

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
            var volumename = typeof item.info.volumename != "undefined" && item.info.volumename ? item.info.volumename : "AUntitled";
            var mount_path = '/Volumes/' + volumename;
            var dev_path = "/dev/" + item.index;
            //Determine the mount method
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
            var dev_path = "/dev/" + item.index;
            //NTFS
            if (typeof item.info.typebundle != "undefined" && item.info.typebundle == "ntfs") {
                console.warn(item, "[NTFS]uMountDisk start +++++++++++++++++++++TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT")
                resolve(await execShellSudo(`umount ${dev_path}`));
                alEvent.$emit('doRefreshEvent');//Send the refresh event
                return;
            } else {
                console.warn(item, "eject uMountDisk start +++++++++++++++++++++TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT")
                resolve(await execShellSudo(`diskutil eject ${get_safe_ejst_disk_name(dev_path)}`));
                alEvent.$emit('doRefreshEvent');//Send the refresh event
                return;
            }
        } catch (e) {
            saveLog.error(e, "uMountDisk");
            reject(e)
        }
    })
}

/**
 * check the disk is ntfs format
 * @param mount_path
 * @returns {Promise<any>}
 */
export function checkDevicesIsNtfs(mount_path) {
    return new Promise((resolve, reject) => {
        execShell('mount |grep "' + mount_path + '"|grep "read-only"').then((res) => {
            console.warn(res, "checkDevicesIsNtfs RES");
            if (res) {
                resolve(true)
            } else {
                resolve(false)
            }
        }).catch((e) => {
            saveLog.error(e, "checkDevicesIsNtfs");
            reject(e)
        })
    })
}

/**
 * open the system diskUtils
 * @returns {Promise<any>}
 */
export function openSysDiskUtils() {
    return new Promise(async (resolve, reject) => {
        try {
            await execShell(`open "/Applications/Utilities/Disk Utility.app" `);
            resolve();
        } catch (e) {
            saveLog.error(e, "openSysDiskUtils");
            reject(e)
        }
    })
}

/**
 * Analysis and filtering out disk data
 * @param str
 */
function formatDiskSize(str) {
    var data = _formatDiskSizeGb(str);
    if (!data.total) {
        var data = _formatDiskSizeMb(str);
    }

    console.warn("formatDiskSize", {data, str});
    return data;
}

function _formatDiskSizeGb(str) {
    var data = str.split("\n");
    //Get the possible data set
    var matchData = [];
    for (var key in data) {
        if (data[key].trim().length > 10 && data[key].toLowerCase().indexOf("gb") >= 0) {
            //Rule algorithm: 1.Remove all characters after gb 2.Backtrack 3.Gb truncates everything except. And digits 4.Count down 5 trim away.
            var _match_value = data[key];
            _match_value = _match_value.toLowerCase().replace(/\s+/g, "");//Remove all spaces, lower case
            _match_value = _match_value.replace(/(.*[\d\.]*gb).*/i, "$1");//Remove all characters after gb
            _match_value = _match_value.split("").reverse().join("");//flashback
            _match_value = _match_value.replace(/(bg[\d.]*).*/g, "$1");
            _match_value = _match_value.split("").reverse().join("").trim();//Flashback
            _match_value = _match_value.replace("gb", "");//Remove gb string

            if ((_match_value.lastIndexOf('.') + 1) == _match_value.length) {
                // Trim off the last possible occurrence .
                _match_value = _match_value.substring(0, _match_value.lastIndexOf('.') - 1);
            }

            if (typeof matchData[_match_value] != "undefined") {
                matchData[_match_value] = matchData[_match_value] + "|" + data[key];
            } else {
                matchData[_match_value] = data[key];
            }

            matchData[_match_value] = matchData[_match_value].toLowerCase().replace(/\s+/g, "");
        }
    }

    //Possible data set to filter out the exact number
    var resData = {total: 0, used: 0, free: 0, percentage: 0, wei: "GB"};
    for (var j in matchData) {
        if (matchData[j].indexOf("total") >= 0 || matchData[j].indexOf("disksize") >= 0) {
            //Possible keywords, get here
            resData["total"] = formatSize(j);
        }

        if (matchData[j].indexOf("free") >= 0) {
            //Possible keywords, get here
            resData["free"] = formatSize(j);
        }
        if (matchData[j].indexOf("used") >= 0) {
            //Possible keywords, get here
            resData["used"] = formatSize(j);
        }
    }
    if (resData["total"]) {
        if (!resData["free"] && resData["used"] && resData["used"] <= resData["total"]) {
            resData["free"] = formatSize(resData["total"] - resData["used"]);
        }
        if (!resData["used"] && resData["free"] && resData["free"] <= resData["total"]) {
            resData["used"] = formatSize(resData["total"] - resData["free"]);
        }
    }
    if (!resData["total"] && resData["used"] && resData["free"]) {
        resData["total"] = formatSize(resData["used"] + resData["free"]);
    }
    if (!resData["percentage"] && resData["used"] && resData["total"]) {
        resData["percentage"] = formatSize(resData["used"] / resData["total"] * 100);
    }

    return resData;
}

function _formatDiskSizeMb(str) {
    var data = str.split("\n");
    //Get the possible data set
    var matchData = [];
    for (var key in data) {
        if (data[key].trim().length > 10 && data[key].toLowerCase().indexOf("mb") >= 0) {
            //Rule algorithm: 1.Remove all characters after mb 2.Flashback 3.Except for. And numbers, all truncated after mb 4.Count down 5 trim away.
            var _match_value = data[key];
            _match_value = _match_value.toLowerCase().replace(/\s+/g, "");//Remove all spaces, lower case
            _match_value = _match_value.replace(/(.*[\d\.]*mb).*/i, "$1");//Remove all characters after gb
            _match_value = _match_value.split("").reverse().join("");//reverse
            _match_value = _match_value.replace(/(bm[\d.]*).*/g, "$1");
            _match_value = _match_value.split("").reverse().join("").trim();//reverse
            _match_value = _match_value.replace("mb", "");

            if ((_match_value.lastIndexOf('.') + 1) == _match_value.length) {
                _match_value = _match_value.substring(0, _match_value.lastIndexOf('.') - 1);
            }

            if (typeof matchData[_match_value] != "undefined") {
                matchData[_match_value] = matchData[_match_value] + "|" + data[key];
            } else {
                matchData[_match_value] = data[key];
            }

            matchData[_match_value] = matchData[_match_value].toLowerCase().replace(/\s+/g, "");//Remove all spaces, lower case
        }
    }

    var resData = {total: 0, used: 0, free: 0, percentage: 0, wei: "MB"};
    for (var j in matchData) {
        if (matchData[j].indexOf("total") >= 0 || matchData[j].indexOf("disksize") >= 0) {
            resData["total"] = formatSize(j);
        }

        if (matchData[j].indexOf("free") >= 0) {
            resData["free"] = formatSize(j);
        }
        if (matchData[j].indexOf("used") >= 0) {
            resData["used"] = formatSize(j);
        }
    }
    if (resData["total"]) {
        if (!resData["free"] && resData["used"] && resData["used"] <= resData["total"]) {
            resData["free"] = formatSize(resData["total"] - resData["used"]);
        }
        if (!resData["used"] && resData["free"] && resData["free"] <= resData["total"]) {
            resData["used"] = formatSize(resData["total"] - resData["free"]);
        }
    }
    if (!resData["total"] && resData["used"] && resData["free"]) {
        resData["total"] = formatSize(resData["used"] + resData["free"]);
    }
    if (!resData["percentage"] && resData["used"] && resData["total"]) {
        resData["percentage"] = formatSize(resData["used"] / resData["total"] * 100);
    }

    return resData;
}

function formatSize(num) {
    var res = Math.round(parseFloat(num) * 100) / 100;
    if (isNaN(res)) {
        res = 0;
    }
    return res;
}