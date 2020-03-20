var event = null;

function event(){
    return event === null ? new EventEmitter() : event;
}


export function newDevEvent(){
    console.warn("send " + eventType.newDev)
    event().emit(eventType.newDev);
}

export function onNewDevEvent(callback){
    event.on(eventType.newDev, function(data) {
        console.warn("on " + eventType.newDev)
        callback(data);
    });
}


