# Iskander Core — Security Audit Report

Date: 2026-06-01

## Scope

Full source code review of the Iskander Core codebase for backdoors, malware, hardcoded secrets, weakened cryptography, unauthorized network connections, hidden RPC commands, and wallet drain mechanisms.

---

## FINDING 1 — HIGH: Hardcoded AES Key + Encrypted Auth Credential

**File:** `src/net_processing.cpp:674-703`
**Conditional:** `#ifdef ENABLE_WINDOW_WALLET` (enabled via `./configure --enable-feature=window-wallet`)

```
encryptedAuthkey = "6f1a2c74..." (1,746 hex chars)
aesKey = "a3f1b4c7d2e9f8a5b1c6d7e8f9a2b3c4d5e6f7a8b9c0d1e2f3a4b5c6d7e8f9a0"
```

**What it does:** Decrypts the auth key using the hardcoded AES-256 key, prints the plaintext to stdout, and sends it in the VERSION message to every peer on connect.

**Risk:** Both the ciphertext and decryption key are in source code — anyone with the repo can extract the credential. The decrypted key is also logged to stdout. The `#else` branch (non-window-wallet) loads the auth key from a file path, which is the safer pattern.

**Recommendation:** Remove the hardcoded key material. Use the file-based approach (the `#else` branch) for all build configurations.

---

## FINDING 2 — MEDIUM: Hardcoded RSA Public Key for Peer Verification

**File:** `src/net_processing.cpp:2622-2631`
**Conditional:** `#ifdef ENABLE_WINDOW_WALLET`

A full RSA-2048 public key is hardcoded inline. Used to verify incoming peer auth keys. The `#else` branch loads this from a config file (`-iskanderkey` flag).

**Risk:** Updating or rotating the root key requires recompilation. Less severe than Finding 1 (this is a public key, not a secret), but still poor practice.

**Recommendation:** Load the root public key from a config file in all build configurations.

---

## FINDING 3 — INFO: Coinbase Address Enforcement

**File:** `src/chainparams.cpp:111,157` and `src/validation.cpp:2465-2479`

All block rewards after block 25,095 are forced to pay `isk1qkhkgstu0drshrzj9342ftln3s7km8p3vd83ezu`. This is a deliberate design decision, not a hidden backdoor — it is clearly declared in chain params. But worth noting: all mining revenue goes to one address.

---

## FINDING 4 — INFO: Custom Build Feature Flags

**File:** `configure.ac:328-344`

Four custom features gated behind `--enable-feature=`:

| Flag | Effect |
|------|--------|
| `block-all-mining` | Disables mining entirely |
| `window-wallet` | Enables Findings 1 & 2 (embeds secrets in binary) |
| `iskander-node-logging` | Extra peer identity logging |
| `ipcheck` | IP-based checks |

These are not backdoors per se, but `ENABLE_WINDOW_WALLET` significantly changes security-critical code paths by embedding secrets in the binary.

---

## Clean Areas (No Issues Found)

### Cryptographic Verification
- Signature checks (ECDSA, Schnorr) in `script/interpreter.cpp` and `pubkey.cpp` are intact
- Proof-of-work validation in `pow.cpp` has no bypasses
- AuxPoW verification properly validates parent-chain proofs
- No "return true" shortcuts in consensus-critical verification paths

### Wallet Code
- No automatic transaction creation or hidden fund drains
- No key exfiltration or automatic private key export
- Standard wallet RPCs (`dumpprivkey`, `importprivkey`) require explicit user request

### RPC Commands
- Hidden commands (`mscrpc`, `omni_sendactivation`, `omni_senddeactivation`, `omni_sendalert`) are standard Omni debug/admin tools
- All require wallet authentication
- No unauthorized or undocumented commands that bypass auth

### Build System
- No injected compilation steps or suspicious linker flags in `configure.ac` or `Makefile.am`
- No build-time code injection

### Random Number Generation
- `random.cpp` uses proper entropy sources (RDRAND, RDSEED, `/dev/urandom`)
- No weakened RNG that would compromise key generation

### Shell Execution
- `runCommand()` exists in `util/system.cpp` but is unused in production paths
- No `system()`, `popen()`, or `exec()` calls in production code

### Network Layer
- No hidden connections to external servers
- Only two documented DNS seeds: `node1.iskandercoin.com`, `node2.iskandercoin.com`
- No HTTP/HTTPS client calls to external endpoints
- No telemetry or phone-home functionality

### Block & Transaction Validation
- `CheckBlock()`, `ContextualCheckBlock()`, `CheckTransaction()` all perform complete validation
- No disabled or commented-out checks in consensus paths
- Reorg handling properly rolls back state

### Omni Layer
- No automatic token creation or hidden issuance
- Feature activation requires authorized on-chain transactions
- DEx and crowdsale logic follows standard Omni protocol

---

## Summary

| Severity | Finding | Location |
|----------|---------|----------|
| **HIGH** | Hardcoded AES key + encrypted auth credential in source | `net_processing.cpp:674-676` |
| **MEDIUM** | Hardcoded RSA public key for peer verification | `net_processing.cpp:2622-2631` |
| **INFO** | All mining rewards to single address (by design) | `chainparams.cpp:157` |
| **INFO** | `ENABLE_WINDOW_WALLET` flag changes security model | `configure.ac:332-333` |

The most actionable item is **Finding 1** — the hardcoded AES key and encrypted auth credential should be removed from source and loaded from a config file like the `#else` branch already does.