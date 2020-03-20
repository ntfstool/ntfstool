var Queue = [];
var storeList = null;
var _ = require('lodash');

import {newDevEvent} from "./eventDisk"

function queueList(){
    Queue === null ? [] : Queue;
}

export function queueListAdd(name){
    Queue.push({
        name:name,
        type:"",
        try_times:0,
    });
    newDevEvent();
}


export function queueListRemove(name){
    _.remove(Queue, function (data) {
        return data.name == name
    });
    newDevEvent();
}

// export function retryQueue(name){
//     _.map(Queue, function (data) {
//         if(data.name == name){
//             data.try_times = data.try_times + 1;
//         }
//         return data;
//     });
//     newDevEvent();
// }

export function sendGlobalUpdate(name){
    //save store
    //send global update event
}




