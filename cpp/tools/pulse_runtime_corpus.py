"""Pulse construction and retained list/reader behavior in the real DB fixture."""
import itertools

def cases():
    result=[]
    def op(cmd,**kw):return dict(cmd=cmd,rows=[],columns=[],**kw)
    def add(*ops):result.append(dict(ops=list(ops)))
    snap=op('pulseSnapshot');hold=op('pulseHold');pulse=op('pulse');wire=op('pulseWire')
    for count in [0,1,2,8,511,512,513]:
        add(op('chain',count=count),pulse,hold,snap,wire,
            op('pulseMutate',target='tree',id='0xAB'),snap,
            op('pulseRecalc'),snap,wire,
            op('pulseMutate',target='held',id='0xCD'),snap,pulse,snap)
    for target,action,recalc in itertools.product(['tree','pulse','held'],['add','clear'],[False,True]):
        add(op('chain'),pulse,hold,*( [op('pulseRecalc')] if recalc else []),op('pulseMutate',target=target,action=action),snap,wire)
    for mode in ['held','tree','null','new']:
        add(op('chain'),pulse,hold,op('pulseSet',target=mode),snap,wire,op('pulseMutate'),snap,op('pulseRecalc'),snap)
    add(op('chain'),pulse,hold,op('hold'),op('create'),op('useHeld'),pulse,snap,op('pulseSet',target='tree'),snap)
    add(op('chain'),pulse,hold,op('reset'),snap,pulse,snap,op('pulseSet'),snap)
    for version in ['0','1','2','-1','1.1','256','9223372036854775808']:
        add(op('chain'),pulse,op('pulseVersion',version=version),wire)
    def nn(n):
        length=max(1,(n.bit_length()+8)//8);raw=n.to_bytes(length,'big',signed=True)
        while len(raw)>1 and (raw[:2]==b'\0\0' or raw[0]==255 and raw[1]>=128):raw=raw[1:]
        return bytes([0,len(raw)])+raw
    def data(raw):return len(raw).to_bytes(4,'big')+raw
    raw=nn(1)+nn(3)+data(b'\xAB')+data(b'\xCD\xEF')+data(b'')
    samples=[raw[:i] for i in range(len(raw)+1)]+[raw+b'extra']
    samples += [nn(v)+nn(count)+data(b'\x01') for v,count in itertools.product([-1,0,1,2],[-1,0,1,2,2**32,2**31])]
    samples += [bytes.fromhex(s) for s in ['00','0000','0080','00010100ff','000101000101ffffffff','00010100010120000001']]
    for payload in samples:
        add(op('chain'),pulse,hold,op('pulseRead',wire='0x'+payload.hex()),snap,wire,op('pulseMutate',target='held'),snap)
    for fail,error,null,clear in itertools.product([-1,1,2,3],[False,True],[False,True],[False,True]):
        add(op('chain'),op('pulseFault'),op('pulse',pulseFail=fail,pulseError=error,pulseNullTree=null,pulseClearDB=clear),snap,op('pulseRestore'),op('safe'),snap)
    for length,expected in itertools.product([-2147483648,-1,0,1,2,536870913,2147483647],[-2,-1,0,1,2]):
        payload=length.to_bytes(4,'big',signed=True)+b'\xAB'
        add(op('pulseDataRead',wire='0x'+payload.hex(),expected=expected))
    return result

def validate(request,output):
    for step,result in zip(request['ops'],output['ops']):
        if step['cmd']=='pulseSnapshot':
            value=result['value']
            if value['pulseTree']:assert value['pulse']==value['tree']
            if value['pulseHeld']:assert value['pulse']==value['held']
            if value['heldTree']:assert value['held']==value['tree']
    # Independent witness for the replacement that motivated this increment.
    if request['ops'][0]['cmd']=='chain' and len(request['ops'])==14:
        rows=output['ops'];count=request['ops'][0]['count'];ids=rows[3]['value']['tree']
        assert len(ids)==min(count,512)
        assert rows[3]['value']['pulseTree'] and rows[3]['value']['heldTree']
        assert rows[6]['value']['tree']==ids+['0xAB']
        assert rows[8]['value']['tree']==ids and not rows[8]['value']['pulseTree']
        assert rows[8]['value']['pulse']==ids+['0xAB']
        assert rows[11]['value']['pulse']==ids+['0xAB','0xCD']
        assert rows[13]['value']['pulseTree'] and rows[13]['value']['pulse']==ids
