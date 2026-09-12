// BrowserWindow/IPC and awaited quit adapted from minimacore-desktop; see REUSE.md.
const {app,BrowserWindow,ipcMain,dialog,Menu}=require('electron');
const fs=require('node:fs');const path=require('node:path');const {pathToFileURL}=require('node:url');
const {NodeManager,validate}=require('./node-manager');
app.setName('MinimaCoreC++ Preview');
const fixtureRoot=process.env.MINIMA_CPP_APP_TEST_ROOT;
const userData=fixtureRoot||path.join(app.getPath('appData'),'MinimaCoreCppPreview');
app.setPath('userData',userData);
const root=path.join(__dirname,'..');const page=path.join(root,'renderer/index.html');
const runtime=app.isPackaged?path.join(process.resourcesPath,'runtime'):process.env.MINIMA_CPP_APP_RUNTIME;
if(!runtime)throw new Error('Set MINIMA_CPP_APP_RUNTIME to a verified native runtime folder for development.');
const node=new NodeManager(path.join(runtime,'bin/MinimaCoreC++'));
const configPath=path.join(userData,'startup.json');
const defaults={dataFolder:path.join(userData,'node-data'),port:21001,peer:'eurobuddha.com:9001',host:'127.0.0.1',mode:'mainnet',role:'desktop',extraArgs:''};
function load(){try{return {...defaults,...JSON.parse(fs.readFileSync(configPath,'utf8')),extraArgs:''};}catch{return {...defaults};}}
function save(input){const {value}=validate(input);const {extraArgs,...safe}=value;fs.mkdirSync(userData,{recursive:true,mode:0o700});const temp=configPath+'.tmp';fs.writeFileSync(temp,JSON.stringify(safe,null,2)+'\n',{mode:0o600});fs.renameSync(temp,configPath);}
let win;let quitting=false;
if(!app.requestSingleInstanceLock()){app.quit();}else{
app.on('second-instance',()=>{if(win){if(win.isMinimized())win.restore();win.show();win.focus();}});
function createWindow(){
 win=new BrowserWindow({width:1180,height:820,minWidth:900,minHeight:650,title:'MinimaCoreC++ Preview',backgroundColor:'#eeeae3',webPreferences:{preload:path.join(__dirname,'preload.js'),contextIsolation:true,nodeIntegration:false,sandbox:true}});
 win.webContents.setWindowOpenHandler(()=>({action:'deny'}));win.webContents.on('will-navigate',e=>e.preventDefault());win.loadFile(page);
}
function handle(name,fn){ipcMain.handle(name,async(event,...args)=>{if(event.sender!==win?.webContents||event.senderFrame?.url!==pathToFileURL(page).href)throw new Error('Untrusted window.');return fn(...args);});}
handle('preview:settings',()=>({settings:load(),identity:JSON.parse(fs.readFileSync(path.join(runtime,'manifest.json'),'utf8'))}));
handle('preview:start',async settings=>{validate(settings);const result=node.start(settings);try{save(settings);}catch(error){await node.stop();throw error;}return result;});
handle('preview:stop',async()=>{await node.stop();return node.snapshot();});
handle('preview:command',text=>{node.command(text);return true;});
handle('preview:status',()=>node.snapshot());handle('preview:logs',()=>node.logs);
handle('preview:folder',async()=>{const result=await dialog.showOpenDialog(win,{properties:['openDirectory','createDirectory']});return result.canceled?null:result.filePaths[0];});
handle('preview:help',()=>{node.command('help');return true;});
node.on('status',state=>{if(win&&!win.isDestroyed())win.webContents.send('preview:status',state);});
let queued=false;node.on('log',()=>{if(queued)return;queued=true;setTimeout(()=>{queued=false;if(win&&!win.isDestroyed())win.webContents.send('preview:logs',node.logs);},100);});
app.whenReady().then(()=>{Menu.setApplicationMenu(Menu.buildFromTemplate([{label:app.name,submenu:[{role:'about'},{type:'separator'},{role:'quit'}]},{role:'editMenu'},{role:'windowMenu'}]));createWindow();});
app.on('activate',()=>{if(!win||win.isDestroyed())createWindow();else win.show();});
app.on('window-all-closed',()=>app.quit());
app.on('before-quit',async event=>{if(quitting)return;event.preventDefault();quitting=true;await node.stop();app.quit();});
app.on('will-quit',()=>node.killNow());process.on('exit',()=>node.killNow());
}
