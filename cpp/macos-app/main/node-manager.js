// Adapted from eurobuddha's MIT-licensed minimacore-desktop/main/node-manager.js.
const {spawn,execFileSync}=require('node:child_process');
const EventEmitter=require('node:events');
const fs=require('node:fs');
const path=require('node:path');
const {StringDecoder}=require('node:string_decoder');
const LOG_MAX_LINES=800;
function tokenizeArgs(s){
  if(!s||typeof s!=='string')return [];
  // Preserve the existing quoted-token grammar; reject ambiguous/unclosed input.
  if(!/^(?:\s*(?:"[^"\n]*"|'[^'\n]*'|[^\s"']+)(?=\s|$))*\s*$/.test(s))throw new Error('Use spaces between arguments and matching quotes around values containing spaces.');
  const out=[];const re=/"([^"]*)"|'([^']*)'|(\S+)/g;let m;
  while((m=re.exec(s)))out.push(m[1]??m[2]??m[3]);
  return out;
}
function listeners(port){
  if(!port)return [];
  try{return [...new Set(execFileSync('/usr/sbin/lsof',['-nP','-t','-iTCP:'+port,'-sTCP:LISTEN'],{encoding:'utf8',stdio:['ignore','pipe','ignore']}).split(/\s+/).filter(Boolean).map(Number))];}
  catch(e){if(e.status===1)return [];throw new Error('Cannot check whether the selected port is available.');}
}
const MANAGED=new Set(['-data','-port','-host','-connect','-daemon','-noshutdownhook','-conf','-desktop','-server','-isclient','-solo']);
function validate(settings,{allowEphemeral=false}={}){
  if(!settings||typeof settings!=='object')throw new Error('Missing startup settings.');
  const value={};
  for(const key of ['dataFolder','peer','host','extraArgs','mode','role']){
    if(typeof settings[key]!=='string'||settings[key].includes('\0')||settings[key].length>16384)throw new Error('Invalid '+key+'.');
    value[key]=settings[key].trim();
  }
  value.port=Number(settings.port);
  if(!Number.isInteger(value.port)||value.port<(allowEphemeral?0:1)||value.port>65531)throw new Error('Choose a base port from 1 to 65531.');
  if(!path.isAbsolute(value.dataFolder))throw new Error('Choose an absolute data folder.');
  if(!['mainnet','solo'].includes(value.mode)||!['desktop','server','isclient'].includes(value.role))throw new Error('Invalid network or node role.');
  if(!value.host||/[\r\n]/.test(value.host+value.peer))throw new Error('Enter a valid host and initial peer.');
  const extra=tokenizeArgs(value.extraArgs);
  for(const token of extra)if(MANAGED.has(token))throw new Error(token+' is controlled by the settings fields or required by the app console.');
  return {value,extra};
}
function buildArgs(settings,options){
  const {value:c,extra}=validate(settings,options);
  const args=['-data',c.dataFolder,'-port',String(c.port),'-host',c.host,'-nossl','-'+c.role];
  if(c.mode==='solo')args.push('-solo','-noconnect','-connect','');
  else if(c.peer)args.push('-connect',c.peer);
  args.push(...extra);return args;
}
class NodeManager extends EventEmitter{
  constructor(binary,options={}){super();this.binary=binary;this.options=options;this.proc=null;this.state='stopped';this.revision=0;this.logs=[];this.lastError=null;this.stopPromise=null;this.secrets=[];}
  snapshot(){return {state:this.state,revision:this.revision,lastError:this.lastError,pid:this.proc?.pid||null};}
  setState(s){this.state=s;++this.revision;this.emit('status',this.snapshot());}
  log(line){
    let text=String(line);
    for(const secret of this.secrets)if(secret)text=text.split(secret).join('•••');
    text=text.replace(/phrase:"[^"]*"/g,'phrase:"•••"').replace(/privatekey:0x[0-9A-Fa-f]+/g,'privatekey:•••');
    for(const l of text.split('\n'))if(l.trim())this.logs.push(l.length>8000?l.slice(0,8000)+'…':l);
    if(this.logs.length>LOG_MAX_LINES)this.logs.splice(0,this.logs.length-LOG_MAX_LINES);
    this.emit('log');
  }
  start(settings){
    if(this.proc||this.state==='starting'||this.stopPromise)throw new Error('Stop the current node before changing startup settings.');
    const args=buildArgs(settings,this.options);
    if(listeners(Number(settings.port)).length)throw new Error('Port '+settings.port+' is already in use. Choose another port or stop that node.');
    fs.mkdirSync(settings.dataFolder,{recursive:true,mode:0o700});
    this.secrets=[];
    for(let i=0;i<args.length-1;i++)if(['-dbpassword','-rpcpassword','-mdspassword','-mysqldb','-seed','-anyseed'].includes(args[i]))this.secrets.push(args[i+1]);
    this.logs=[];this.lastError=null;this.setState('starting');this.log('[app] Starting bundled C++ node.');
    const env=Object.fromEntries(Object.entries(process.env).filter(([k])=>!k.toLowerCase().startsWith('minima_')&&!k.toLowerCase().startsWith('dyld_')));
    const p=spawn(this.binary,args,{stdio:['pipe','pipe','pipe'],shell:false,env,cwd:settings.dataFolder});this.proc=p;
    const attach=(stream)=>{const decoder=new StringDecoder('utf8');let pending='';stream.on('data',d=>{pending+=decoder.write(d);let at;while((at=pending.indexOf('\n'))>=0){this.log(pending.slice(0,at));pending=pending.slice(at+1);}if(pending.length>65536){this.log(pending);pending='';}});stream.on('end',()=>{pending+=decoder.end();if(pending)this.log(pending);});};
    attach(p.stdout);attach(p.stderr);
    p.stdin.on('error',e=>{if(e.code!=='EPIPE')this.log('[app] Console input: '+e.message);});
    p.on('spawn',()=>{if(this.proc===p&&this.state==='starting')this.setState('running');});
    p.on('error',e=>{this.lastError=e.message;this.log('[app] '+e.message);});
    p.on('close',(code,signal)=>{if(this.proc!==p)return;this.proc=null;this.log('[app] Node exited: '+(signal||code));if(code===0){this.setState('stopped');}else{this.lastError=this.lastError||'Node exited with '+(signal||code)+'. See console output.';this.setState('error');}this.emit('closed',{code,signal});});
    return this.snapshot();
  }
  command(value){
    if(typeof value!=='string'||!value.trim()||/[\r\n\0]/.test(value)||value.length>65536)throw new Error('Enter one console command.');
    if(!this.proc||this.state==='stopping')throw new Error('The node is not accepting commands.');
    this.proc.stdin.write(value+'\n');
  }
  stop(){
    if(this.stopPromise)return this.stopPromise;
    const p=this.proc;if(!p)return Promise.resolve();this.setState('stopping');
    this.stopPromise=new Promise(resolve=>{
      const t=setTimeout(()=>{if(this.proc===p){this.log('[app] Graceful stop is taking longer; requesting shutdown with SIGTERM.');p.kill('SIGTERM');}},45000);
      const t2=setTimeout(()=>{if(this.proc===p){this.log('[app] Shutdown timed out; stopping the owned process.');p.kill('SIGKILL');}},90000);
      p.once('close',()=>{clearTimeout(t);clearTimeout(t2);resolve();});
      if(p.stdin.writable)p.stdin.write('quit\n');else p.kill('SIGTERM');
    }).finally(()=>{this.stopPromise=null;});return this.stopPromise;
  }
  killNow(){if(this.proc){try{this.proc.kill('SIGTERM');}catch{} }}
}
module.exports={NodeManager,tokenizeArgs,buildArgs,validate,listeners};
