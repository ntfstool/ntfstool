import {app, BrowserWindow, Menu, Tray, ipcMain,globalShortcut} from 'electron'
/**
 * Set `__static` path to static files in production
 * https://simulatedgreg.gitbooks.io/electron-vue/content/en/using-static-assets.html
 */
if (process.env.NODE_ENV !== 'development') {
    global.__static = require('path').join(__dirname, '/static').replace(/\\/g, '\\\\')
}
const fs = require('fs');
const Store = require('electron-store');
const store = new Store();
app.disableHardwareAcceleration();//disable gpu

const default_store = {
    name: "Alntfs",
    auto_run:true,
    theme: "",//主题
    lang: "en",//语言设置
    show_menu: true,//是否显示菜单栏
    common: {
        website_url: "",//官方网站
        install_bug_type: "auto_solve",//安装坏卷
        how_restart_window: "change_to_bacground", //如何处理休眠
    },
    message: {
        mount_show_msg: "",//磁盘挂载通知通知
        update_show_msg: "",//更新通知
        error_disk_msg: "",//卷宗异常通知
    },
    disk_list: {
        history_list: [],//磁盘列表
        ignore_list: [],//磁盘忽略列表
    },
    privacy_url: 'https://github.com',//隐私链接
    update: {
        auto_check: "",//自动检查更新
        auto_beta_update: "",//自动检查beta更新
        update_url: "",//更新链接
        update_beta_url: "",//beta更新链接
    },
    sudoPwd:false,
};


var settingWin = null;
var trayWin = null;
var tray = null
let mainWindow
//设置路径
const winURL = process.env.NODE_ENV === 'development'
    ? `http://localhost:9080`
    : `file://${__dirname}/index.html`

//默认主窗口
const initWindow = () => {

    mainWindow = new BrowserWindow({
        show: false,
        fullscreen: false,
        height: 600,
        minHeight: 600,
        minWidth: 800,
        width: 900,
        maxWidth: 900,
        useContentSize: true,
        // center: true,
        frame: false,
        titleBarStyle: 'hidden',
        webPreferences: {
            experimentalFeatures: true,
            nodeIntegration: true
        },
        transparent: true
    })

    console.log(winURL, "winURL");
    mainWindow.loadURL(winURL);

    mainWindow.once('ready-to-show', () => {
        mainWindow.show()
    })

    mainWindow.on('closed', () => {
        mainWindow = null
    })




    // //左上角菜单栏
    // const contextMenu = Menu.buildFromTemplate([
    //     {
    //         label: '退出', click: function () {
    //             app.quit()
    //         }
    //     },
    //     {
    //         label: '退出2', click: function () {
    //             app.quit()
    //         }
    //     },
    //     {
    //         label: 'Edit',
    //         submenu: [
    //             { role: 'undo' },
    //             { role: 'redo' },
    //             { type: 'separator' },
    //             { role: 'cut' },
    //             { role: 'copy' },
    //             { role: 'paste' },
    //             [
    //                 { role: 'pasteAndMatchStyle' },
    //                 { role: 'delete' },
    //                 { role: 'selectAll' },
    //                 { type: 'separator' },
    //                 {
    //                     label: 'Speech',
    //                     submenu: [
    //                         { role: 'startspeaking' },
    //                         { role: 'stopspeaking' }
    //                     ]
    //                 }
    //             ]
    //         ]
    //     }
    // ])
    // tray.setToolTip('测试气泡提示文字')
    // tray.setContextMenu(contextMenu)


    //左上角菜单栏
    var template = [
        {
            label: '关闭',
            click: function () {
                win.close();
                console.log("关闭")
            },
            submenu: [
                {
                    label: '关于AlNtfs',
                    click: async () => {
                        const {shell} = require('electron')
                        await shell.openExternal('https://baidu.org')
                    }
                },
                {type: 'separator'},
                {
                    label: '偏好设置',
                    click: async () => {
                        openSysSeeting();
                    }
                },
                {role: 'services'},
                {type: 'separator'},
                {
                    label: '退出AlNtfs',
                    accelerator: 'CmdOrCtrl+Z',
                    role: 'quit'
                },
            ],
        },
        {
            label: 'Edit',
            submenu: [
                {role: 'undo'},
                {role: 'redo'},
                {type: 'separator'},
                {role: 'cut'},
                {role: 'copy'},
                {role: 'paste'},
                {role: 'delete'},
                {role: 'selectAll'},
                {type: 'separator'},
            ]
        }


    ]
    var menu = Menu.buildFromTemplate(template)
    Menu.setApplicationMenu(menu);

}

//右上角菜单栏
const openTrayMenu = () => {
    // const path = require('path');
    // tray = new Tray(path.join(__dirname, "../renderer/assets/menu/AINTFS18.png"));
    // tray.setPressedImage(path.join(__dirname, "../renderer/assets/menu/AINTFS_active18.png"));
   // tray.on('click', () => toggleTrayWindow())
}

const toggleTrayWindow = () => {
    trayWin.isFocused() ? trayWin.hide() : showWindow()
}

const showWindow = () => {
    const position = getWindowPosition()
    trayWin.setPosition(position.x, position.y, false)
    trayWin.show()
    trayWin.focus()
}

const getWindowPosition = () => {
    const windowBounds = trayWin.getBounds()
    const trayBounds = tray.getBounds()

    // Center window horizontally below the tray icon
    const x = Math.round(trayBounds.x + (trayBounds.width / 2) - (windowBounds.width / 2))
    // Position window 4 pixels vertically below the tray icon
    const y = Math.round(trayBounds.y + trayBounds.height + 4)
    return {x, y}
}

//默认菜单栏
const initTray = () => {
    if (trayWin == null) {
        //已下为插入内容
        const path = require('path')
        trayWin = new BrowserWindow({
            height: 100,
            width: 380,
            frame: false,
            resizable: false,
            show: false,
            transparent: true,
            webPreferences: {
                nodeIntegration: true,
                backgroundThrottling: false
            }
        })

        trayWin.loadURL(winURL + "#tray")
        // settingWin.webContents.closeDevTools()

        trayWin.on('closed', () => {
            trayWin = null
        })

        trayWin.on('blur', () => {
            trayWin.hide();
        })

    } else {
        trayWin.show()
    }
}
//设置进程调用
const openSysSeeting = () => {
    if (settingWin == null) {
        //已下为插入内容
        const path = require('path')
        settingWin = new BrowserWindow({
            title: "设置",
            fullscreen: false,
            height: 500,
            width: 750,
            useContentSize: true,
            center: true,
            frame: false,
            titleBarStyle: 'hidden',
            show: false,
            resizable: false,
            minimizable: false,
            maximizable: false,
            skipTaskbar: true,
            webPreferences: {
                nodeIntegration: true
            }
        })

        settingWin.loadURL(winURL + "#setting")
        // settingWin.webContents.closeDevTools();//关闭dev工具

        settingWin.once('ready-to-show', () => {
            settingWin.show()
        })

        settingWin.on('closed', () => {
            settingWin = null
        })
    } else {
        settingWin.show()
    }
}

//默认设置全局配置
const initStore = () => {
    store.set(default_store);
    console.warn("initStore run");
}

//
//
// //监听Mount事件
// const listenMount = () => {
//     //监测USB插入事件
//     var path = '/Volumes/';
//     fs.watch(path, function (event, filename) {
//             if(mainWindow){
//                 mainWindow.send("MountEvent",filename);
//             }
//     });
// }

app.on('ready', () => {
    initWindow()
    initTray()
    if (!store.get("name") || store.get("name") == "undefined") {
        initStore();
    }
    if (store.get("show_menu") != false) {
        openTrayMenu();
    }

    // listenMount();


    globalShortcut.register('Command+Shift+J', () => {
        let focusWin = BrowserWindow.getFocusedWindow()
        focusWin && focusWin.toggleDevTools()
    });
})

app.on('activate', () => {
    if (mainWindow === null) {
        initWindow()
    }
})

app.on('window-all-closed', () => {
    if (process.platform !== 'darwin') {
        app.quit()
    }
})

//主进程监听消息
ipcMain.on('MainMsgFromRender', function (event, arg) {
    console.warn(arg, "主进程监听到消息")
    if (arg == "openSysSeeting") {
        openSysSeeting();
        event.sender.send('RenderMsgFromMain', arg + " succ#############")
    }else if(arg == "resetConf"){
        store.set(default_store);
        event.returnValue = 'succ';
    } else {
        event.sender.send('RenderMsgFromMain', arg + " fail##############")
    }
})


//监听关闭开启菜单图标
ipcMain.on('toggleTrayMenu', function (event, arg) {
    if (tray !== null) {
        tray.destroy();//立刻删除 Tray 图标
        tray = null;
        event.returnValue = 'destroy';
    } else {
        openTrayMenu();
        event.returnValue = 'newopen';
    }
})


ipcMain.on('ChangeLangEvent', function (event, arg) {
    console.warn("ChangeLangEvent",arg);
    if(mainWindow){
        mainWindow.send("ChangeLangEvent",arg);
    }
})


