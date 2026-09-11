# Delivery deadline

User instruction received at approximately 23:30 Europe/London on 11 September 2026:
cap efforts at three hours, with delivery by **02:30 Europe/London on 12 September 2026**
(**2026-09-12 01:30 UTC**). Do not extend development past this cutoff.

The full 457-file working port remains the requested outcome. A runnable build and
passing selected tests do not establish that outcome. Record actual limitations in
the delivered handover if they remain at the cutoff.

Reserve the final hour (from 01:30 London / 00:30 UTC) for delivery assembly,
relocation/startup verification, source/docs/presentation checks, final local commit,
and verified recovery. Prefer ending implementation earlier if broad testing needs
more time. Do not let repeated already-passing demonstrations consume this window.

Delivery must include runnable macOS package, complete checkpoint source including
pinned upstream, build/test instructions and evidence, all 457 student lessons,
team presentation materials, known gaps with reproduction/next steps, licenses,
checksums, and local recovery information. No wallet/chain databases or credentials
in the shareable package. Existing port21001 preview/user data remain untouched.

Current implementation checkpoint: 8e83642713a4467c8b1957263e88b22a532d15cd.
Current inventory: 456 counterparts, one missing JNI file, full parity unverified.
Previous turn was progress: 3 entry points + exception correction + tested commit.
Next implementation batch: send, Contract, TxPoWTreeNode, P2PFunctions, then broad tests.

Reuse package_macos_preview.py and verify_macos_preview.py. Extend their port
configuration for fixture validation to avoid touching any user node. Source
packaging should use tracked Git objects and the explicit upstream submodule,
not a recursive workspace copy (which would include test wallets/build caches).
