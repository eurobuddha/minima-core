const {contextBridge,ipcRenderer}=require('electron');
contextBridge.exposeInMainWorld('preview',{
 settings:()=>ipcRenderer.invoke('preview:settings'),start:settings=>ipcRenderer.invoke('preview:start',settings),stop:()=>ipcRenderer.invoke('preview:stop'),
 command:text=>ipcRenderer.invoke('preview:command',text),status:()=>ipcRenderer.invoke('preview:status'),logs:()=>ipcRenderer.invoke('preview:logs'),folder:()=>ipcRenderer.invoke('preview:folder'),
 onStatus:fn=>ipcRenderer.on('preview:status',(_,value)=>fn(value)),onLogs:fn=>ipcRenderer.on('preview:logs',(_,value)=>fn(value))
});
