# Omni Core Token Layer — Architecture

## Origin

This is a port of **OmniLite** (Litecoin's fork of Omni Layer / "Mastercoin"), version **0.9.1**. It provides a full token platform running as a "meta-protocol" on top of the Iskander base chain — tokens exist as specially-encoded data embedded in ISK transactions.

---

## Architecture

### Hook Points into Base Chain

The Omni layer is **not** a consensus-level fork. It hooks into the base chain via four callback functions wired into `validation.cpp`:

| Hook | Called When | Purpose |
|------|-----------|---------|
| `mastercore_handler_block_begin` | Start of `ConnectBlock` | Initialize per-block Omni state |
| `mastercore_handler_tx` | For each tx in a block | Scan for Omni marker, parse & execute |
| `mastercore_handler_block_end` | End of `ConnectBlock` | Finalize state, persist, expire DEx offers |
| `mastercore_handler_disc_begin` | Block disconnect (reorg) | Roll back Omni state |

Lifecycle: `mastercore_init()` at node startup (`init.cpp:1930`), `mastercore_shutdown()` at exit.

### Transaction Encoding

Omni payloads are embedded in ISK transactions using two encoding classes:

- **Class B** (`encoding.cpp`): Obfuscated multisig outputs + Exodus marker output
- **Class C** (`encoding.cpp`): `OP_RETURN` output with `omni` marker prefix (preferred)

The sender/receiver are derived from the ISK transaction inputs/outputs; the Omni-specific data (type, property ID, amount, etc.) is in the payload.

---

## Transaction Types

The `CMPTransaction` class (`tx.h/tx.cpp`) is the central interpreter. Each tx type has an `interpret_*` method (payload parsing) and a `logicMath_*` method (state effects):

### Token Transfers

| Type ID | Name | Description |
|---------|------|-------------|
| 0 | `SIMPLE_SEND` | Transfer tokens from sender to receiver |
| 3 | `SEND_TO_OWNERS` | Distribute tokens pro-rata to all holders of a property |
| 4 | `SEND_ALL` | Transfer all tokens of all properties to receiver |
| 5 | `SEND_NONFUNGIBLE` | Transfer a range of NFT token IDs |

### Token Creation & Management

| Type ID | Name | Description |
|---------|------|-------------|
| 50 | `CREATE_PROPERTY_FIXED` | Create a token with a fixed supply |
| 51 | `CREATE_PROPERTY_VARIABLE` | Create a crowdsale token |
| 53 | `CLOSE_CROWDSALE` | End a crowdsale early |
| 54 | `CREATE_PROPERTY_MANUAL` | Create a managed (mintable/burnable) token |
| 55 | `GRANT_PROPERTY_TOKENS` | Mint additional tokens (managed only) |
| 56 | `REVOKE_PROPERTY_TOKENS` | Burn tokens (managed only) |
| 70 | `CHANGE_ISSUER_ADDRESS` | Transfer issuer role |

### Freezing (Managed Properties Only)

| Type ID | Name | Description |
|---------|------|-------------|
| 71 | `ENABLE_FREEZING` | Turn on address freezing for a property |
| 72 | `DISABLE_FREEZING` | Turn off address freezing |
| 185 | `FREEZE_PROPERTY_TOKENS` | Freeze an address's balance |
| 186 | `UNFREEZE_PROPERTY_TOKENS` | Unfreeze an address's balance |

### Decentralized Exchange (DEx)

| Type ID | Name | Description |
|---------|------|-------------|
| 20 | `TRADE_OFFER` | Create/update/cancel a sell offer (tokens for ISK) |
| 22 | `ACCEPT_OFFER_BTC` | Accept a DEx sell offer |

### NFTs & Data

| Type ID | Name | Description |
|---------|------|-------------|
| 200 | `ANYDATA` | Embed arbitrary data in the Omni layer |
| 201 | `NONFUNGIBLE_DATA` | Set issuer/holder data on NFT ranges |

### System

| Type ID | Name | Description |
|---------|------|-------------|
| 65533 | `DEACTIVATION` | Deactivate a feature (authorized only) |
| 65534 | `ACTIVATION` | Activate a feature (authorized only) |
| 65535 | `ALERT` | Broadcast an alert message |

---

## Property Types

Tokens come in several flavors (`prop_type` field):

| Type | Value | Behavior |
|------|-------|----------|
| Indivisible | 1 | Integer units only |
| Divisible | 2 | 8 decimal places (like satoshis) |
| Non-Fungible | 5 | Each token has a unique ID in a range |
| Indivisible Replacing | 65 | Crowdsale, replaces existing tokens |
| Divisible Replacing | 66 | Crowdsale, replaces existing tokens |
| Indivisible Appending | 129 | Crowdsale, appends to existing tokens |
| Divisible Appending | 130 | Crowdsale, appends to existing tokens |

---

## State Management

### In-Memory State

- **`mp_tally_map`** (`omnicore.h:177`): `address -> CMPTally` — every address's balance for every property. The `CMPTally` tracks 4 sub-balances per property: `BALANCE`, `SELLOFFER_RESERVE`, `ACCEPT_RESERVE`, `PENDING`.
- **`my_offers`** / **`my_accepts`** (`dex.h`): In-memory DEx order book.
- **`my_crowds`** (`sp.h`): Active crowdsales.

### LevelDB Databases

| Database Class | Header | Purpose |
|-------------|------|---------|
| `CMPSPInfo` | `dbspinfo.h` | Smart property definitions (name, type, issuer, supply, crowdsale params, historical issuers) |
| `CMPNonFungibleTokensDB` | `nftdb.h` | NFT ownership ranges and metadata (issuer data, holder data, grant data) |
| `CMPTxList` | `dbtxlist.h` | Index of all Omni transactions |
| `CMPSTOList` | `dbstolist.h` | Send-To-Owners distribution records |
| `CMPTradeList` | `dbtransaction.h` | DEx trade history |

### Persistence & Reorgs

- State is snapshotted to flat files every 5,000 blocks (`STORE_EVERY_N_BLOCK`) via `PersistInMemoryState` (`persistence.cpp`)
- The last 200 block states are kept (`MAX_STATE_HISTORY`) for fast rollback on reorgs
- On crash recovery, `LoadMostRelevantInMemoryState` restores from the latest snapshot and replays forward

---

## Consensus Rules

Source: `rules.cpp`

### Mainnet Activation Schedule

| Feature | Activation Block |
|---------|-----------------|
| Omni Genesis | **120,000** |
| Simple Send, DEx, Smart Properties, Managed Properties, STO, Send All, Free DEx | **120,000** (all at genesis) |
| NFTs | **2,262,000** |
| STO v1 (cross-property), Freeze Notice, NFT Issuer Data | **100,000,000** (effectively disabled — must be activated via on-chain `ACTIVATION` tx) |

Features can also be turned on/off dynamically via on-chain `ACTIVATION`/`DEACTIVATION` transactions from authorized addresses (`activation.cpp`).

---

## RPC Interface

The Omni layer exposes **60+ RPC commands** across four files:

| File | Commands | Category |
|------|----------|----------|
| `rpc.cpp` (102KB) | `omni_getbalance`, `omni_getproperty`, `omni_listproperties`, `omni_gettransaction`, `omni_getactivedexsells`, `omni_getinfo`, NFT queries, etc. | Read/query |
| `rpctx.cpp` (94KB) | `omni_send`, `omni_sendall`, `omni_senddexsell`, `omni_sendissuancefixed`, `omni_sendgrant`, `omni_sendrevoke`, NFT sends, etc. | Write/transact |
| `rpcpayload.cpp` (36KB) | `omni_createpayload_*` | Build raw payloads |
| `rpcrawtx.cpp` (20KB) | Raw transaction helpers | Low-level tx construction |

---

## Wallet Integration

- **`wallettxbuilder.cpp`**: `WalletTxBuilder()` creates the ISK transaction embedding an Omni payload, handles coin selection, and broadcasts
- **`CreateFundedTransaction()`**: Variant that selects all sender coins + fee from a separate address
- **`walletcache.cpp`** / **`walletfetchtxs.cpp`**: Cache wallet-relevant Omni transactions for the GUI
- **`walletutils.cpp`**: Helper utilities for wallet property lists

---

## Key Design Points

1. **Meta-protocol**: Omni state is entirely derived from ISK transactions. Any full node that runs the Omni parser arrives at the same state. No base-layer consensus changes needed.

2. **Two ecosystems**: Production (property IDs 1+) and test (property IDs starting at `0x80000003`). The test ecosystem exists for experimentation.

3. **Reserved property IDs**: `0` = ISK (native), `1` = OMNI (the layer's own token), `2` = TOMNI (test).

4. **NFT model**: Non-fungible tokens use contiguous ID ranges within a property. Ownership is tracked as `(propertyId, startId, endId) -> address` in LevelDB. Ranges split/merge on transfer.

5. **DEx**: A simple on-chain order book for trading Omni tokens against native ISK. Offers have a time limit (in blocks); accepts must be paid within that window or they expire.