---
type: "query"
date: "2026-09-08T09:56:12.503997+00:00"
question: "Study minima-core upstream architecture"
contributor: "graphify"
outcome: "useful"
source_nodes: ["Main", "CommandRunner", "TxPoWChecker", "TxPoWProcessor"]
---

# Q: Study minima-core upstream architecture

## Answer

Expanded via graph vocabulary: minima main processor checker network database wallet contract command mmr. Existing graph includes fork-specific MDS, so verified upstream 5564b9d directly. Main owns lifecycle; CommandRunner dispatches; NIOMessage admits and relays TxPoW; TxPoWChecker validates; TxPoWProcessor updates tree/cascade. Source study notes in graphify-out/UPSTREAM_STUDY.md. No tests run.

## Outcome

- Signal: useful

## Source Nodes

- Main
- CommandRunner
- TxPoWChecker
- TxPoWProcessor