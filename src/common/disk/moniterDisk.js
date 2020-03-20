import {event} from "./queueDisk";

var watch = require('node-watch')
import {onNewDevEvent} from "./eventDisk"


//call the listenVolume to monitor the volume mount event
function listenVolume() {
    watch("./data", {recursive: true}, function (evt, name) {
        console.warn({evt, name}, "watchFile");
        if (evt == "remove") {
            queueListRemove(name);
        } else {
            queueListAdd(name);
        }
    })
}


//call the listenVolume to monitor the volume mount event
function mainMonitor() {
    // queueDisk.addEvent()  listen.... todo
    onNewDevEvent(() => {
        console.warn("newDevEvent");

        // //step1 get new dev info and update view
        // sendGlobalUpdate();//global view update（from store）
        //
        // //setp check2 ntfs and update
        // if (action === NTFS_Mount) {
        //     workerDisk.autoMount(function () {
        //         sendGlobalUpdate();//global view update again
        //     });
        // }
    })

};

export function moniterDisk() {

}

