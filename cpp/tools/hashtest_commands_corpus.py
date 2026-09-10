"""Actual hashing, bounded wall clocks and independent decimal rate checks."""
from decimal import Decimal,localcontext,ROUND_DOWN
from main_runtime_differential import op

def cases():
    result=[dict(ops=[op('meta',name='hashtest')])]
    def add(*steps):result.append(dict(ops=list(steps)))
    values=[None,False,True,0,1,{},[],'',' ','invalid','NaN','Infinity','0x01',
            '0','-0','-1','-100','0.5','-0.5','1','2','3.125','0.0001','100','1234',
            '10000','1e3',' 2 ','\t3\r','١٢','2147483648','4294967296','4294967297',
            '18446744073709551616','-18446744073709551616']
    for value in values:add(op('direct',name='hashtest',params={'amount':value}))
    add(op('direct',name='hashtest',params={},expected_hashes='1000000'))
    add(op('direct',name='hashtest',params={'amount':'12.5','ignored':'x'},expected_hashes='12.5'))
    for caller in ['0x00','0x1234']:
        for text in ['hashtest amount:0','hashtest amount:-1','hashtest amount:1.5',
                     'hashtest amount:" 3 "','hashtest amount:{}','hashtest amount:[]',
                     'hashtest amount:invalid','hashtest amount:""','hashtest extra:1',
                     'hashtest amount:0;hashtest amount:2;hashtest amount:-3']:
            add(op('run',caller=caller,text=text))
    add(op('clear'),op('direct',name='hashtest',params={'amount':'2'},expected_hashes='2'),op('publish'))
    return result

def number(value):
    if value.as_tuple().exponent < -44:value=value.quantize(Decimal('1e-44'),rounding=ROUND_DOWN)
    return value

def plain(value):
    if not value:return '0'
    text=format(value,'f');return text.rstrip('0').rstrip('.') if '.' in text else text

def project(raw,request):
    from main_runtime_differential import project as base_project
    value=base_project(raw,request)
    for original,row,step in zip(raw['ops'],value['ops'],request['ops']):
        speed_logs=[(i,text[8:]) for i,text in enumerate(row['logs']) if text.startswith('Speed : ')]
        replies=row.get('value');replies=replies if isinstance(replies,list) else [replies]
        good=[r for r in replies if isinstance(r,dict) and r.get('command')=='hashtest' and r.get('status')]
        assert len(speed_logs)==len(good),(step,row)
        for reply,(position,logged) in zip(good,speed_logs):
            response=reply['response'];assert set(response)=={'hashes','millitime','speed'},response
            elapsed=response['millitime'];assert isinstance(elapsed,int) and 0<=elapsed<=original['after']-original['before'],(original,elapsed)
            with localcontext() as context:
                context.prec=64;context.rounding=ROUND_DOWN
                assert type(response['hashes']) in (int,float),response
                hashes=Decimal(str(response['hashes']));amount=reply.get('params',{}).get('amount','1000000')
                assert hashes==number(+Decimal(amount.strip())),(hashes,amount)
                speed=Decimal(logged);assert logged==plain(speed),(logged,speed)
                if hashes==0:assert speed==0,(hashes,speed)
                else:
                    assert speed!=0 and speed.is_signed()==hashes.is_signed(),(hashes,speed)
                    estimate=int(hashes*1000/speed)
                    candidates=range(max(1,estimate-2),min(max(1,elapsed),estimate+2)+1)
                    assert any(number(hashes/(Decimal(t)/1000))==speed for t in candidates),(hashes,speed,elapsed,estimate)
                megaspeed=number(speed/1000000);context.prec=4;megaspeed=+megaspeed
                expected=plain(megaspeed)+' MH/s';assert response['speed']==expected,(response,logged,expected)
            response['millitime']='<bounded elapsed>';response['speed']='<verified MH/s>';row['logs'][position]='Speed : <verified rate>'
    return value

def validate(request,output):
    for step,row in zip(request['ops'],output['ops']):
        if step['cmd']=='meta':assert row['value']['name']=='hashtest' and row['value']['valid']==['amount'] and row['value']['fresh'],row
        if 'expected_hashes' in step:assert row['value']['status'] and Decimal(str(row['value']['response']['hashes']))==Decimal(step['expected_hashes']),row
