"""Real chain/cascade state with independent expectations for the healthcheck wrapper."""
import itertools
from decimal import Decimal,localcontext
from main_runtime_differential import op


def number(value):
    text=format(value,'f')
    return text.rstrip('0').rstrip('.') if '.' in text else text


def seed(blocks=(),cascade=(),**kw):
    return op('healthChain',blocks=list(blocks),cascade=list(cascade),**kw)


def call(**kw):
    return op('direct',name='healthcheck',params={},**kw)


def expected(root,tip,cascade):
    with localcontext() as context:
        context.prec=100
        return dict(chain=dict(root=root,tip=tip,chainlength=number(Decimal(tip)-Decimal(root))),cascade=dict(tip=cascade[-1],tipcorrect=Decimal(cascade[-1])==Decimal(root)-1,cascadelength=len(cascade)) if cascade else 'nocacade')


def cases():
    result=[dict(ops=[op('meta',name='healthcheck')])]
    def add(*ops):result.append(dict(ops=list(ops)))
    with localcontext() as context:
        context.prec=100
        for start,count,mode in itertools.product(['0','1','100','-10','9223372036854775808','18446744073709551606','1.5'],[0,1,2,5],['empty','match','mismatch','multi','null']):
            root=Decimal(start);blocks=[number(root+i) for i in range(count)]
            cascade=[] if mode in ['empty','null'] else [number(root-1)] if mode=='match' else [number(root)] if mode=='mismatch' else [number(root-i) for i in range(4,0,-1)]
            error='org.minima.system.commands.CommandException: No TIP block found.. ?' if not count else 'java.lang.NullPointerException' if mode=='null' else None
            check={'error':error} if error else {'expect':expected(blocks[0],blocks[-1],cascade)}
            add(seed(blocks,cascade,nullCascade=mode=='null'),call(**check),op('run',text='healthcheck',caller='0x00'),call(**check))
    # Out-of-range construction fails before a tree tip can be installed.
    huge='10000000000000000000000000000000000000000'
    add(seed([huge],error='java.lang.NumberFormatException: MiniNumber too large - outside allowed range 2^64 '+huge),call(error='org.minima.system.commands.CommandException: No TIP block found.. ?'))
    # The response measures height difference, not the number of links.
    for blocks in [['100','150','900'],['5','4','3'],['1.5','2.25'],['100','100','100']]:
        add(seed(blocks,['98','99']),call(expect=expected(blocks[0],blocks[-1],['98','99'])))
    # Equal-difficulty branches: the root's second child has more descendants.
    add(seed(['100','101','102','103'],['99'],parents=[-1,0,0,2]),call(expect=expected('100','103',['99'])))
    # Fresh state and no cached response across replacements.
    add(seed(['5'],[]),call(expect=expected('5','5',[])),seed(['20','21'],['18','19']),call(expect=expected('20','21',['18','19'])),seed(),call(error='org.minima.system.commands.CommandException: No TIP block found.. ?'))
    for caller,text,has_tip in itertools.product(['0x00','0x1234'],['healthcheck','healthcheck extra:true','healthcheck extra:{}','healthcheck;healthcheck','healthcheck;hash data:0xAA'],[False,True]):
        add(seed(['100'] if has_tip else [],['99']),op('run',text=text,caller=caller))
    for raw in [None,False,True,0,{},[],'','ignored']:
        add(seed(['100']),op('direct',name='healthcheck',params={'extra':raw},expect=expected('100','100',[])))
    return result


def validate(request,output):
    for step,row in zip(request['ops'],output['ops']):
        if step['cmd']=='healthChain' and 'error' not in step:assert 'exception' not in row,row
        if step['cmd']=='meta':
            assert row['value']['name']=='healthcheck' and row['value']['fresh'] and row['value']['valid']==[],row
        if 'expect' in step:
            assert 'exception' not in row and row['value']['status'] is True,row
            assert row['value']['response']==step['expect'],(step,row)
            assert 'maxima' not in row['value']['response'],row
        if 'error' in step:assert row.get('exception')==step['error'],(step,row)
        if step['cmd']=='direct':
            assert row['events']==[] and row['timers']==[] and row['listeners']==[],row
            assert row['state']['queue']==0,row
