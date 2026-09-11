#!/usr/bin/env python3
"""Render the maintained team outline as an offline, editable HTML presentation."""
from pathlib import Path
import html,re
ROOT=Path(__file__).resolve().parents[1]
source=ROOT/'delivery/PRESENTATION.md'
sections=re.split(r'^## ',source.read_text(),flags=re.M)[1:]
slides=[]
for i,section in enumerate(sections,1):
    title,body=section.split('\n',1);content,_,notes=body.partition('Speaker note:')
    title=re.sub(r'^\d+\.\s*','',title)
    paragraphs=[' '.join(p.split()) for p in content.strip().split('\n\n')]
    slides.append('<section class="slide" id="slide-'+str(i)+'" tabindex="-1"><p class="eyebrow">MinimaCoreC++</p><h1>'+html.escape(title)+'</h1>'+''.join('<p>'+html.escape(p)+'</p>' for p in paragraphs)+'<p class="folio">'+str(i)+' / '+str(len(sections))+'</p></section><aside class="notes"><strong>Presenter notes</strong><p>'+html.escape(' '.join(notes.split()))+'</p></aside>')
page='''<!doctype html><html lang="en"><meta charset="utf-8"><meta name="viewport" content="width=device-width,initial-scale=1"><title>MinimaCoreC++ team presentation</title><style>
:root{font-family:Arial,Helvetica,sans-serif;color:#192e3b;background:#f7f6f2}*{box-sizing:border-box}body{margin:0}nav{padding:16px 5vw;background:#192e3b;color:white;font-size:15px}nav a{color:white;margin-right:24px}.slide{min-height:90vh;padding:9vh 10vw 7vh;position:relative;max-width:1500px;margin:auto;border-bottom:1px solid #cbd0cf}.eyebrow{font-size:20px;color:#366b67;letter-spacing:.08em}h1{font-size:clamp(34px,4.8vw,64px);line-height:1.08;max-width:1100px;margin:28px 0 48px}p{font-size:clamp(22px,2.3vw,32px);line-height:1.5;max-width:1050px}.folio{font-size:15px;margin-top:70px;color:#47636a}.notes{display:none;padding:25px 10vw;background:#e8eeeb}.notes p{font-size:19px;line-height:1.6}body.show-notes .notes{display:block}a:focus-visible,button:focus-visible{outline:3px solid #ffcd75}button{font:inherit;background:transparent;color:white;border:1px solid white;padding:6px 12px;cursor:pointer}@media print{@page{size:landscape;margin:0}nav,.notes{display:none!important}.slide{height:100vh;min-height:0;break-after:page;padding:7vh 8vw;border:0}h1{font-size:36pt;margin:18pt 0 28pt}p{font-size:22pt}.eyebrow{font-size:14pt}.folio{font-size:11pt;margin-top:30pt}}@media(max-width:600px){.slide{padding:7vh 7vw}h1{margin-bottom:28px}p{font-size:23px}}
</style><nav aria-label="Presentation controls"><a href="START_HERE.md">Handover</a><a href="PRESENTATION.md">Editable outline</a><button id="notes" type="button" aria-pressed="false">Presenter notes</button><span> &nbsp; Arrow keys navigate slides</span></nav>'''+''.join(slides)+'''<script>
const slides=[...document.querySelectorAll('.slide')];document.querySelector('#notes').onclick=function(){const on=document.body.classList.toggle('show-notes');this.setAttribute('aria-pressed',String(on))};document.addEventListener('keydown',e=>{if(!['ArrowRight','ArrowLeft','PageDown','PageUp'].includes(e.key))return;e.preventDefault();let i=slides.reduce((best,s,j)=>Math.abs(s.getBoundingClientRect().top)<Math.abs(slides[best].getBoundingClientRect().top)?j:best,0);i=Math.max(0,Math.min(slides.length-1,i+(['ArrowRight','PageDown'].includes(e.key)?1:-1)));slides[i].scrollIntoView();slides[i].focus({preventScroll:true})});
</script></html>'''
(ROOT/'delivery/presentation.html').write_text(page)
print(f'Wrote {len(slides)} slides and {len(slides)} presenter-note sections')
