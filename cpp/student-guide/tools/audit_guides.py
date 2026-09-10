#!/usr/bin/env python3
"""Audit guide presence/links/statuses, and regenerate a labelled file index.
An authored marker is a documentation state, never a port/parity certification.
"""
import collections,hashlib,json,os,re
from pathlib import Path
from urllib.parse import unquote
BASE=Path(__file__).resolve().parents[1];CPP=BASE.parent;ROOT=CPP.parent
source_map=json.loads((CPP/'source-map.json').read_text());rows=source_map['files']
errors=[];entries=[];link_count=0;line_count=0

def teaching(text,is_test=False):
 match=re.search(r'^\*\*Teaching status:\*\*\s*(.*)',text,re.M)
 if match:
  return (('source_test_explanation' if is_test else 'authored_lesson') if match[1].startswith('Authored') else 'source_reading_draft'),match[1]
 if '## Purpose and logic' in text and '## Concrete test evidence' in text:
  return 'source_test_explanation','Source-derived purpose, logic and assertion explanation; not an exhaustive behavior audit or fresh test run.'
 return 'unclassified','No recognized teaching marker.'

for row in rows:
 path=BASE/'files'/Path(row['java']).with_suffix('.md')
 if not path.exists():errors.append({'type':'missing_guide','java':row['java']});continue
 s=path.read_text();state,note=teaching(s,row['java'].startswith('test/'))
 statusline=next((l for l in s.splitlines() if l.startswith(('**Port status:**','Mapping status:'))),'')
 if row['status'] not in statusline or row['parity'] not in statusline:
  errors.append({'type':'status_mismatch','guide':str(path.relative_to(BASE)),'expected':[row['status'],row['parity']],'line':statusline})
 if row['cpp_exists']!=(CPP/row['cpp']).exists():errors.append({'type':'stale_source_map','java':row['java']})
 source=ROOT/'upstream-study'/row['java']
 if hashlib.sha256(source.read_bytes()).hexdigest()!=row['java_sha256']:errors.append({'type':'source_hash_mismatch','java':row['java']})
 if row['java_sha256'] not in s:errors.append({'type':'guide_source_hash_mismatch','java':row['java']})
 entries.append({'java':row['java'],'cpp':row['cpp'],'guide':str(path.relative_to(BASE)),'port_status':row['status'],'parity':row['parity'],'native_exists':row['cpp_exists'],'teaching_status':state,'teaching_note':note})

counts=collections.Counter(x['teaching_status'] for x in entries)
index=['# All 457 source-file lessons','',
 'Each link corresponds to exactly one pinned Java file. **Authored** means the page has a student-level explanation; **reading draft** still needs that work. **Test explanation** describes source tests and their observed assertions, with its own limits. None of these labels certifies full C++ parity.','',
 f'Current documentation: {counts["authored_lesson"]} authored production lessons, {counts["source_reading_draft"]} production reading drafts, {counts["source_test_explanation"]} original-test explanations. See [audit data](coverage-audit.json).','',
 '[Course introduction](README.md) · [How to study](how-to-study-this-port.md) · [Checker rejection lab](new-tests/checker-rejections.md) · [Command runner error lab](new-tests/command-runner-errors.md) · [Wallet command lab](new-tests/wallet-commands.md) · [Address command lab](new-tests/address-commands.md) · [Witness command lab](new-tests/witness-commands.md) · [Coin ownership lab](new-tests/coin-ownership.md) · [Coin and proof reader lab](new-tests/coin-streams.md) · [Chain command lab](new-tests/chain-commands.md) · [Transaction-input lab](new-tests/transaction-inputs.md) · [Transaction-check lab](new-tests/transaction-check.md) · [Increment/decrement lab](new-tests/increment-decrement.md) · [Mining/posting lab](new-tests/transaction-mining.md) · [Powers/significant digits lab](new-tests/power-significant-digits.md) · [Bit-counting lab](new-tests/bit-counting.md) · [Vault/signing lab](new-tests/vault-and-signing.md) · [Bit-get/set lab](new-tests/bit-get-set.md) · [Wallet-key lab](new-tests/wallet-keys.md) · [Coin-funding lab](new-tests/coin-selection-and-funding.md) · [Reverse/length lab](new-tests/reverse-and-length.md) · [Offline-file lab](new-tests/offline-send-files.md) · [Unsigned-file lab](new-tests/unsigned-transaction-files.md) · [Send-polling lab](new-tests/send-polling-command.md) · [Wallet orchestration lab](new-tests/wallet-command-orchestration.md) · [Raw-transaction lab](new-tests/raw-transaction-commands.md) · [Custom-address sending lab](new-tests/custom-address-send-commands.md) · [Consolidation lab](new-tests/consolidate-custom-address.md) · [Wallet consolidation lab](new-tests/wallet-consolidation.md) · [Wallet balance lab](new-tests/wallet-balance.md) · [Coin-search lab](new-tests/coin-search-command.md) · [Transaction-history lab](new-tests/transaction-history.md) · [TxPoW-search lab](new-tests/txpow-search.md) · [Node-state lab](new-tests/node-state-commands.md) · [Seed derivation lab](new-tests/seed-random-command.md) · [Original cast tests](new-tests/cast-original-tests.md) · [MySQL connection lab](new-tests/mysql-connection.md) · [Main runtime lab](new-tests/main-runtime.md)','']
groups=collections.defaultdict(list)
for entry in entries:groups[str(Path(entry['java']).parent)].append(entry)
labels={'authored_lesson':'authored','source_reading_draft':'reading draft','source_test_explanation':'test explanation','unclassified':'unclassified'}
for group,rs in groups.items():
 index+=['## '+group,'','| File | Teaching state | Native state |','|---|---|---|']
 for r in rs:index+=[f'| [{Path(r["java"]).name}]({r["guide"]}) | {labels[r["teaching_status"]]} | {r["port_status"]} |']
 index+=['']
(BASE/'INDEX.md').write_text('\n'.join(index))
# Validate all authored/draft markdown links after the index exists.
for path in sorted(BASE.rglob('*.md')):
 s=path.read_text();fenced=False
 for number,line in enumerate(s.splitlines(),1):
  if line.startswith('```'):fenced=not fenced;continue
  if fenced:continue
  visible=re.sub(r'`[^`]*`','',line)
  for target in re.findall(r'\[[^\]]*\]\(([^)]+)\)',visible):
   if target.startswith(('http:','https:','mailto:','#')):continue
   link_count+=1;target=target.strip('<>');filepart,_,fragment=target.partition('#');candidate=(path.parent/unquote(filepart)).resolve()
   if candidate==BASE/'coverage-audit.json':continue
   if not candidate.exists():errors.append({'type':'broken_link','page':str(path.relative_to(BASE)),'line':number,'target':target});continue
   if fragment and re.fullmatch(r'L\d+',fragment):
    line_count+=1;n=int(fragment[1:])
    if not 1<=n<=len(candidate.read_text(errors='replace').splitlines()):errors.append({'type':'bad_line_anchor','page':str(path.relative_to(BASE)),'line':number,'target':target})
 if fenced:errors.append({'type':'unbalanced_fence','page':str(path.relative_to(BASE))})

if len(entries)!=457 or len({r['java'] for r in entries})!=457:errors.append({'type':'inventory_count','entries':len(entries)})
expected={x['guide'] for x in entries}
extra=[str(p.relative_to(BASE)) for p in (BASE/'files').rglob('*.md') if str(p.relative_to(BASE)) not in expected]
if extra:errors.append({'type':'extra_file_guides','paths':extra})
report={'source_pin':source_map['commit'],'source_files':len(rows),'unique_guides':len(entries),'native_counterparts':sum(r['native_exists'] for r in entries),'native_absent':sum(not r['native_exists'] for r in entries),'mapping_counts_by_source_set':{group:{'total':sum(r['java'].startswith(group+'/') for r in entries),'counterparts':sum(r['java'].startswith(group+'/') and r['native_exists'] for r in entries),'absent':sum(r['java'].startswith(group+'/') and not r['native_exists'] for r in entries)} for group in ['src','test']},'teaching_counts':dict(counts),'links_checked':link_count,'line_anchors_checked':line_count,'errors':errors,'ownership_exclusions':['new-tests/command-runner-errors.md','new-tests/checker-rejections.md','how-to-study-this-port.md','files/src/org/minima/kissvm/functions/number/FLOOR.md','files/src/org/minima/kissvm/functions/number/CEIL.md','files/test/org/minima/tests/kissvm/functions/maths/FLOORTests.md','files/test/org/minima/tests/kissvm/functions/maths/CEILTests.md','files/src/org/minima/system/commands/CommandRunner.md','files/src/org/minima/Minima.md','files/src/org/minima/system/Main.md','files/src/org/minima/system/brains/TxPoWChecker.md','files/src/org/minima/system/brains/TimedChecker.md','files/src/org/minima/objects/Transaction.md','files/src/org/minima/objects/TxPoW.md','files/src/org/minima/kissvm/Contract.md','files/src/org/minima/utils/ZipExtractor.md','files/src/org/minima/utils/Streamable.md','files/test/org/minima/tests/kissvm/values/ScriptValueTests.md','files/test/org/minima/tests/kissvm/values/HexValueTests.md','files/src/org/minima/kissvm/values/HexValue.md','files/test/org/minima/tests/kissvm/values/NumberValueTests.md','files/src/org/minima/objects/Coin.md','files/src/org/minima/kissvm/values/NumberValue.md','files/src/org/minima/system/params/GeneralParams.md','files/src/org/minima/system/params/GlobalParams.md','files/src/org/minima/system/params/ParamConfigurer.md','files/src/org/minima/system/params/TestParams.md','files/src/org/minima/kissvm/functions/MinimaFunction.md','files/src/org/minima/kissvm/functions/number/ABS.md','files/test/org/minima/tests/kissvm/functions/DUMMYFUNCTION.md','files/test/org/minima/tests/kissvm/functions/MinimaFunctionTests.md','files/test/org/minima/tests/kissvm/functions/maths/ABSTests.md'],'note':'Structural audit only. Authored markers and test explanations do not prove exhaustive teaching review or port parity. Drafts remain unfinished teaching work.','files':entries}
(BASE/'coverage-audit.json').write_text(json.dumps(report,indent=2)+'\n')
# Replace the initial generation snapshot with the current state, including hand-authored edits.
(BASE/'production-coverage.json').write_text(json.dumps([r for r in entries if r['java'].startswith('src/')],indent=2)+'\n')
print(json.dumps({k:v for k,v in report.items() if k not in ['files','ownership_exclusions']},indent=2))
raise SystemExit(bool(errors))
