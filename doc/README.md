TexitCoin Core
=============

Setup
---------------------
TexitCoin Core is the original TexitCoin client and it builds the backbone of the network. It downloads and, by default, stores the entire history of TexitCoin transactions, which requires approximately 22 gigabytes of disk space. Depending on the speed of your computer and network connection, the synchronization process can take anywhere from a few hours to a day or more.

To download TexitCoin Core, visit [texitcoin.org](https://texitcoin.org/).

Running
---------------------
The following are some helpful notes on how to run TexitCoin Core on your native platform.

### Unix

Unpack the files into a directory and run:

- `bin/texitcoin-qt` (GUI) or
- `bin/texitcoind` (headless)

### Windows

Unpack the files into a directory, and then run texitcoin-qt.exe.

### macOS

Drag TexitCoin Core to your applications folder, and then run TexitCoin Core.

### Need Help?

* See the documentation at the [TexitCoin Wiki](https://texitcoin.info/) for help and more information.
* Ask for help on [#texitcoin](https://webchat.freenode.net/#texitcoin) on Freenode. If you don't have an IRC client, use [webchat here](https://webchat.freenode.net/#texitcoin).
* Ask for help on the [TexitCoinTalk](https://texitcointalk.io/) forums, in the [Technical Support board](https://texitcointalk.io/c/technical-support).

Building
---------------------
The following are developer notes on how to build TexitCoin Core on your native platform. They are not complete guides, but include notes on the necessary libraries, compile flags, etc.

- [Dependencies](dependencies.md)
- [macOS Build Notes](build-osx.md)
- [Unix Build Notes](build-unix.md)
- [Windows Build Notes](build-windows.md)
- [FreeBSD Build Notes](build-freebsd.md)
- [OpenBSD Build Notes](build-openbsd.md)
- [NetBSD Build Notes](build-netbsd.md)
- [Gitian Building Guide (External Link)](https://github.com/bitcoin-core/docs/blob/master/gitian-building.md)

Development
---------------------
The TexitCoin repo's [root README](/README.md) contains relevant information on the development process and automated testing.

- [Developer Notes](developer-notes.md)
- [Productivity Notes](productivity.md)
- [Release Notes](release-notes.md)
- [Release Process](release-process.md)
- [Source Code Documentation (External Link)](https://doxygen.bitcoincore.org/)
- [Translation Process](translation_process.md)
- [Translation Strings Policy](translation_strings_policy.md)
- [JSON-RPC Interface](JSON-RPC-interface.md)
- [Unauthenticated REST Interface](REST-interface.md)
- [Shared Libraries](shared-libraries.md)
- [BIPS](bips.md)
- [Dnsseed Policy](dnsseed-policy.md)
- [Benchmarking](benchmarking.md)

### Resources
* Discuss on the [TexitCoinTalk](https://texitcointalk.io/) forums.
* Discuss general TexitCoin development on #texitcoin-dev on Freenode. If you don't have an IRC client, use [webchat here](https://webchat.freenode.net/#texitcoin-dev).

### Miscellaneous
- [Assets Attribution](assets-attribution.md)
- [bitcoin.conf Configuration File](bitcoin-conf.md)
- [Files](files.md)
- [Fuzz-testing](fuzzing.md)
- [Reduce Memory](reduce-memory.md)
- [Reduce Traffic](reduce-traffic.md)
- [Tor Support](tor.md)
- [Init Scripts (systemd/upstart/openrc)](init.md)
- [ZMQ](zmq.md)
- [PSBT support](psbt.md)

License
---------------------
Distributed under the [MIT software license](/COPYING).
