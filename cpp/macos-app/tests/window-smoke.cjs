// Exercise the actual window/preload/main adapter; no test-only IPC is shipped.
const {app}=require('electron');const fs=require('node:fs');const path=require('node:path');const net=require('node:net');
const testRoot=process.env.MINIMA_CPP_APP_TEST_ROOT;if(!testRoot)throw new Error('An isolated test root is required.');fs.mkdirSync(testRoot,{recursive:true});
const pause=ms=>new Promise(r=>setTimeout(r,ms));
async function freePort(){const s=net.createServer();await new Promise(r=>s.listen(0,'127.0.0.1',r));const port=s.address().port;await new Promise(r=>s.close(r));if(port>65531)return freePort();return port;}
app.once('browser-window-created',(_,win)=>{win.hide();win.webContents.once('did-finish-load',async()=>{
 const evaluate=code=>win.webContents.executeJavaScript(code,true);
 async function wait(code,timeout=120000){const end=Date.now()+timeout;while(!await evaluate(code)){if(Date.now()>end)throw new Error('Window observation timeout: '+code);await pause(50);}}
 const report={passed:false,checks:[]};
 try{
  await wait("document.getElementById('identity').textContent.includes('OPTIMIZED')");
  const port=await freePort();await evaluate(`document.getElementById('mode').value='solo';document.getElementById('mode').onchange();document.getElementById('port').value=${port};document.getElementById('extraArgs').value='-hashtest 1000';`);
  fs.writeFileSync(path.join(testRoot,'startup.png'),(await win.webContents.capturePage()).toPNG());
  for(const step of ['fresh','reopen']){
   await evaluate("document.getElementById('start').click()");
   await wait("document.getElementById('state').textContent==='RUNNING'&&!document.getElementById('command').disabled");
   const live=await evaluate('window.preview.status()');win.webContents.send('preview:status',{...live,state:'starting',revision:0});await pause(50);
   if(!await evaluate("document.getElementById('state').textContent==='RUNNING'&&!document.getElementById('command').disabled"))throw new Error('A stale Starting event disabled the console');
   await evaluate("document.getElementById('command').focus()");await win.webContents.insertText('hash data:hello');
   if(!await evaluate("document.getElementById('command').value==='hash data:hello'"))throw new Error('Typing into the terminal field failed');
   await evaluate("document.getElementById('send').click()");
   await wait("document.getElementById('output').textContent.includes('0x3338BE694F50C5F338814986CDF0686453A888B84F424D792AF4B9202398F392')");
   if(!await evaluate("document.getElementById('command').value===''") )throw new Error('Submitted command was not cleared');
   fs.writeFileSync(path.join(testRoot,step+'.png'),(await win.webContents.capturePage()).toPNG());
   await evaluate("document.getElementById('stop').click()");await wait("document.getElementById('state').textContent==='STOPPED'&&document.getElementById('output').textContent.includes('Shut down completed OK..')");
   report.checks.push({step,terminalTyping:true,staleStatusIgnored:true,commandResult:true,cleanStop:true});
  }
  report.passed=true;
 }catch(error){report.error=error.message;process.exitCode=1;}
 fs.writeFileSync(path.join(testRoot,'window-results.json'),JSON.stringify(report,null,2)+'\n');console.log(JSON.stringify(report));app.quit();
});});
require('../main/main');
