# Iskander Core — Codebase Structure Overview

## Project Overview

**Iskander (ISK)** is a custom cryptocurrency forked from **Litecoin Core** (which itself descends from Bitcoin Core). It's a C++ full-node implementation with several notable extensions: AuxPoW merged mining, Omni token layer, and MWEB (inactive).

---

## Top-Level Directory Layout

```
iskandercoin/
├── src/                    # All C++ source code
├── test/                   # Python-based functional/regression tests
├── doc/                    # Documentation (build guides, developer notes)
├── contrib/                # Auxiliary tools, Docker, gitian, deployment helpers
│   └── docker/             # Dockerfiles for Ubuntu 22.04/24.04 builds
├── depends/                # Cross-compilation dependency build system
├── build-aux/              # Autotools helpers
├── build_msvc/             # MSVC (Windows) build files
├── build/                  # Build output directory
├── share/                  # Shared resources (man pages, pixmaps, etc.)
├── ci/                     # CI scripts
├── .github/                # GitHub workflows and templates
├── configure.ac            # Autotools root config
├── Makefile.am             # Top-level Makefile template
├── autogen.sh              # Bootstrap script for autotools
├── .travis.yml             # Travis CI config
├── .appveyor.yml           # AppVeyor CI config
└── .cirrus.yml             # Cirrus CI config
```

---

## Core Layers (`src/`)

### Consensus & Validation

| File(s) | Purpose |
|---------|---------|
| `consensus/params.h` | Chain consensus parameters struct |
| `consensus/consensus.h` | Block size, weight limits |
| `consensus/tx_check.cpp` | Context-free transaction checks |
| `consensus/tx_verify.cpp` | Context-dependent transaction verification |
| `consensus/merkle.cpp` | Merkle root computation |
| `validation.cpp/h` | Block & mempool validation, chain state management (largest core file) |
| `pow.cpp/h` | Proof-of-work difficulty adjustment |
| `versionbits.cpp/h` | BIP9 soft fork deployment tracking |
| `chainparams.cpp/h` | Network identity: genesis block, ports, address prefixes, seed nodes |
| `chainparamsbase.cpp/h` | Base network name definitions (main, test, regtest) |

### Chain Parameters (Mainnet)

| Parameter | Value |
|-----------|-------|
| Block time | **3 minutes** |
| Retarget window | **120 minutes** (40 blocks) |
| Block reward | **312 ISK**, halving every 695,662 blocks |
| Address prefix | `K` (base58), `isk` (bech32) |
| P2P port | **25366** |
| Genesis timestamp | Feb 3, 2026 |
| SegWit / Taproot | Active from block 0 |
| DNS seeds | `node1.iskandercoin.com`, `node2.iskandercoin.com` |

### Networking

| File(s) | Purpose |
|---------|---------|
| `net.cpp/h` | P2P connection management, peer discovery |
| `net_processing.cpp/h` | Protocol message handling (inv, getdata, block, tx, etc.) |
| `netaddress.cpp/h` | Network address types (IPv4, IPv6, Tor) |
| `netbase.cpp/h` | Low-level network utilities, proxy support |
| `net_permissions.cpp/h` | Per-peer permission flags |
| `torcontrol.cpp/h` | Tor control port integration |
| `banman.cpp/h` | Peer banning logic |
| `addrman.cpp/h` | Address manager (peer address database) |
| `addrdb.cpp/h` | Serialization of peer/ban databases |

### Script Engine

| File(s) | Purpose |
|---------|---------|
| `script/script.cpp/h` | Script opcodes, CScript class |
| `script/interpreter.cpp/h` | Script execution engine |
| `script/standard.cpp/h` | Standard transaction type detection (P2PKH, P2SH, P2WPKH, etc.) |
| `script/sign.cpp/h` | Transaction signing |
| `script/sigcache.cpp/h` | Signature verification cache |
| `script/descriptor.cpp/h` | Output descriptor parsing |

### Transaction & Block Primitives

| File(s) | Purpose |
|---------|---------|
| `primitives/transaction.h` | `CTransaction`, `CTxIn`, `CTxOut` |
| `primitives/block.h` | `CBlock`, `CBlockHeader` |
| `primitives/pureheader.h` | Base header class (supports AuxPoW extension) |
| `amount.h` | `CAmount` type (satoshi-level precision) |
| `coins.cpp/h` | UTXO set representation |
| `compressor.cpp/h` | Compact serialization for UTXOs |
| `undo.h` | Block undo data for reorgs |

### Mining

| File(s) | Purpose |
|---------|---------|
| `miner.cpp/h` | Block template assembly, `CreateNewBlock()` |
| `pow.cpp/h` | `GetNextWorkRequired()` difficulty adjustment |
| `rpc/mining.cpp` | `getblocktemplate`, `submitblock` RPCs |

### Wallet

| File(s) | Purpose |
|---------|---------|
| `wallet/wallet.cpp/h` | Core wallet: key management, tx creation, balance tracking |
| `wallet/coinselection.cpp/h` | UTXO coin selection algorithms |
| `wallet/coincontrol.cpp/h` | Manual coin control |
| `wallet/crypter.cpp/h` | Wallet encryption (AES-256-CBC) |
| `wallet/bdb.cpp/h` | Berkeley DB wallet backend |
| `wallet/sqlite.cpp/h` | SQLite wallet backend |
| `wallet/scriptpubkeyman.cpp/h` | Key/script management abstraction |
| `wallet/rpcdump.cpp` | `dumpprivkey`, `importprivkey` etc. |
| `wallet/rpcwallet.cpp` | Wallet RPC commands |
| `wallet/fees.cpp/h` | Fee estimation |
| `wallet/feebumper.cpp/h` | RBF fee bumping |

### RPC Interface

| File(s) | Purpose |
|---------|---------|
| `rpc/server.cpp/h` | RPC server framework |
| `rpc/client.cpp/h` | RPC client argument conversion |
| `rpc/blockchain.cpp` | `getblock`, `getblockchaininfo`, chain query RPCs |
| `rpc/mining.cpp` | Mining-related RPCs |
| `rpc/net.cpp` | `getpeerinfo`, `addnode`, network RPCs |
| `rpc/rawtransaction.cpp` | `createrawtransaction`, `sendrawtransaction` |
| `rpc/misc.cpp` | `validateaddress`, `signmessage`, utility RPCs |

### Qt GUI

| Directory | Purpose |
|-----------|---------|
| `qt/` | Full desktop wallet UI built with Qt |
| `qt/bitcoingui.cpp/h` | Main window |
| `qt/walletview.cpp/h` | Wallet tab views |
| `qt/sendcoinsdialog.cpp/h` | Send coins form |
| `qt/receivecoinsdialog.cpp/h` | Receive coins / address generation |
| `qt/transactionview.cpp/h` | Transaction history |
| `qt/overviewpage.cpp/h` | Dashboard/overview tab |
| `qt/balancesdialog.cpp/h` | Omni token balances display |

### Storage

| File(s) | Purpose |
|---------|---------|
| `dbwrapper.cpp/h` | LevelDB wrapper used across the codebase |
| `txdb.cpp/h` | UTXO set database, block index database |
| `flatfile.cpp/h` | Flat file storage for blocks (`blk*.dat`, `rev*.dat`) |
| `index/` | Optional indexes (tx index, block filter index) |

### Daemon & CLI Entrypoints

| File | Purpose |
|------|---------|
| `bitcoind.cpp` | `iskanderd` daemon main() |
| `bitcoin-cli.cpp` | `iskander-cli` command-line RPC client |
| `bitcoin-tx.cpp` | `iskander-tx` offline transaction tool |
| `bitcoin-wallet.cpp` | `iskander-wallet` offline wallet tool |
| `qt/bitcoin.cpp` | `iskander-qt` GUI entrypoint |

---

## Custom Extensions

### AuxPoW (Merged Mining) — `src/auxpow.cpp/h`

- Chain ID: `0x4953` (ISK in hex)
- Enabled from block 1
- Allows ISK to be merge-mined alongside another Scrypt-based chain
- Extends `CBlockHeader` via `primitives/pureheader.h` to carry a parent-chain coinbase proof

### Omni Layer (Token Platform) — `src/omnicore/` (86 files)

Full Omni protocol implementation for issuing/trading custom tokens, NFTs, DEX, smart properties, crowdsales, and more. Ported from OmniLite.

See [omnicore-architecture.md](omnicore-architecture.md) for the detailed deep dive.

### MWEB (MimbleWimble Extension Blocks)

| Directory | Purpose |
|-----------|---------|
| `src/mweb/` | MWEB integration: mining, wallet, node, policy, DB |
| `src/libmw/` | MimbleWimble cryptographic library |

Currently **deactivated** on mainnet (`NEVER_ACTIVE` in chain params). The infrastructure exists but the feature is not enabled.

### Coinbase Address Enforcement

Starting at block **25,095** (`consensus.nCoinbaseAddressEnforcementHeight`), all coinbase outputs must pay to the address `isk1qkhkgstu0drshrzj9342ftln3s7km8p3vd83ezu`.

---

## Build System

### Autotools

- `autogen.sh` → `configure` → `make`
- `configure.ac`: Feature flags (`--enable-wallet`, `--with-gui`, etc.)
- `Makefile.am` + `src/Makefile.am`: Source file lists, library targets
- `src/Makefile.omnicore.include`: Omni layer source files

### Cross-Compilation

- `depends/`: Reproducible dependency builds for Linux, macOS, Windows, ARM
- `build_msvc/`: Visual Studio project files

### Docker

7 Dockerfiles in `contrib/docker/`:

| File | Purpose |
|------|---------|
| `isk-deps-amd-ubuntu22.Dockerfile` | Build dependencies (Ubuntu 22.04) |
| `isk-deps-amd-ubuntu24.Dockerfile` | Build dependencies (Ubuntu 24.04) |
| `isk-amd-ubuntu22.Dockerfile` | Full node build (Ubuntu 22.04) |
| `isk-amd-ubuntu24.Dockerfile` | Full node build (Ubuntu 24.04) |
| `isk-auxpow-amd-ubuntu24.Dockerfile` | AuxPoW-enabled build (Ubuntu 24.04) |
| `txc-runtime-amd-ubuntu22.Dockerfile` | Runtime image (Ubuntu 22.04) |
| `txc-runtime-amd-ubuntu24.Dockerfile` | Runtime image (Ubuntu 24.04) |

---

## Bundled Dependencies (`src/`)

| Directory | Library | Purpose |
|-----------|---------|---------|
| `leveldb/` | LevelDB | Key-value storage (UTXO set, indexes, Omni DBs) |
| `crc32c/` | CRC32C | Checksum for LevelDB |
| `secp256k1-zkp/` | libsecp256k1-zkp | Elliptic curve crypto (with zero-knowledge proof extensions for MWEB) |
| `univalue/` | UniValue | JSON parsing/serialization for RPC |
| `crypto/` | Internal | SHA-256, RIPEMD-160, AES, HMAC, siphash |

---

## Testing

| Directory / File | Purpose |
|-----------------|---------|
| `src/test/` | C++ unit tests (Boost.Test framework) |
| `src/bench/` | C++ microbenchmarks |
| `test/functional/` | Python functional/regression tests |
| `test/lint/` | Code style and lint checks |
| `src/omnicore/test/` | Omni layer unit tests |

Run unit tests: `make check`
Run functional tests: `test/functional/test_runner.py`

---

## CI Configuration

| File | Service |
|------|---------|
| `.travis.yml` | Travis CI |
| `.appveyor.yml` | AppVeyor (Windows) |
| `.cirrus.yml` | Cirrus CI |
| `.fuzzbuzz.yml` | Fuzz testing |

---

## Git History Summary

The project evolved through these major phases:

1. **Litecoin Core base** — initial fork
2. **Bidirectional node authorization** — custom peer auth
3. **AuxPoW merged mining** — Scrypt merge-mining support
4. **Omni token layer** — OmniLite port for token issuance, DEx, NFTs
5. **Difficulty & reward tuning** — custom chain economics
6. **Coinbase address enforcement** — mandatory payout address
7. **Docker packaging** — containerized builds and runtime images