# miniupnpc

The MiniUPnP project's UPnP IGD client library in C, by Thomas Bernard,
licensed under the BSD 3-Clause License (see `LICENSE`).

Upstream: https://github.com/miniupnp/miniupnp (https://miniupnp.tuxfamily.org),
release `miniupnpc_2_3_5` at commit `e1db7c47295d38b2b416c892779dae9afa434016`.
The GitHub archive for this commit has SHA-256
`0ef406e3d0b4a922100719a4f3457a055a1476d906779901c9ff9f46cf1b827f`.

Only the library's sources in `src/` (not its test and command-line
programs), `include/` and `LICENSE` are kept, without their build files,
and nothing in them is changed. `src/miniupnpcstrings.h`, which the
library's build generates from `miniupnpcstrings.h.in`, is written here.

Internet play (`port/linux/src/p2p.c`, through `port/linux/src/posix_upnp.c`)
uses it to ask the router to forward the tunnel's UDP port
(`network.allow_upnp` in config.toml), for players whose NATs are too
strict for hole punching.
