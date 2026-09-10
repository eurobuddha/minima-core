"""Small Markdown editing helper; does not infer facts or run on import."""
from pathlib import Path
import os,re
BASE=Path(__file__).resolve().parents[1];CPP=BASE.parent

def author(relative,purpose,steps,adaptation,tests,exercise,limits):
 p=BASE/'files'/relative;s=p.read_text()
 def link(path,label):return f'[{label}]({os.path.relpath(path,p.parent)})'
 def section(title,body):
  nonlocal s
  s=re.sub(r'(## '+re.escape(title)+r'\n\n).*?(?=\n## |\Z)',lambda m:m[1]+body+'\n',s,flags=re.S)
 s=re.sub(r'\*\*Teaching status:\*\*[^\n]+','**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.',s)
 s=s.replace('— source reading notes','— implementation lesson')
 section('What this file does',purpose)
 section('Follow the logic','\n'.join(f'{i}. {t}' for i,t in enumerate(steps,1)))
 section('How the translation is approached',adaptation)
 section('What the tests tell us',tests(link))
 section('Check your understanding',exercise)
 sourcehash=s[s.index('Pinned Java SHA-256:'):].strip()
 section('Limits and traceability',limits+'\n\n'+sourcehash)
 p.write_text(s)
