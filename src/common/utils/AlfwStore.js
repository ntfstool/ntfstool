const saveLog = require('electron-log');
const Store = require('electron-store');
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

