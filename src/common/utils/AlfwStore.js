const saveLog = require('electron-log');
const Store = require('electron-store');
import {alEvent} from '@/common/utils/alEvent'
import {AlConst} from '@/common/utils/AlfwConst'
const store = new Store();


const alfwStore = {
    name: "Alntfs",
    auto_run: true,
    theme: "",
    lang: "en",
    show_menu: true,
    common: {
        website_url: "",
        install_bug_type: "auto_solve",
        how_restart_window: "change_to_bacground",
    },
    message: {
        mount_show_msg: "",
        update_show_msg: "",
        error_disk_msg: "",
    },
    disk_list: {
        history_list: [],
        ignore_list: [],
    },
    privacy_url: 'https://github.com',
    update: {
        auto_check: "",
        auto_beta_update: "",
        update_url: "",
        update_beta_url: "",
    },
    sudoPwd: false,
};


export function checkNeedInitStore() {
    if (!store.get("name") || store.get("name") == "undefined") {
        setDefaultStore();
    }
}


export function setDefaultStore() {
    store.set(alfwStore);
    saveLog.info("initStore alfwStore");
}

export function setStore(key,value) {
    store.set(key,value);
}

export function getStore(key) {
    store.get(key);
}


export function setStoreForDiskList(value) {
    console.warn("setStoreForDiskList",value)
    store.set(AlConst.DiskList,value);
    alEvent().emit(AlConst.DiskListEvent,"SADASFDS");//update the DiskListEvent
}

export function getStoreForDiskList() {
    return store.get(AlConst.DiskList);
}


