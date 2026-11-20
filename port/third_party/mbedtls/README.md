# Mbed TLS

A TLS library in C99, by the Mbed TLS contributors, licensed under the
Apache License 2.0 or the GNU General Public License 2.0 or later (see
`LICENSE`).

Upstream: https://github.com/Mbed-TLS/mbedtls, release `v3.6.8` (the 3.6
long-term support branch), commit `82e69999f91126589a28b70dd75b69fbcabe2711`.
GitHub's source archive for this commit has SHA-256
`49562d7a841503708ab8a8d2ef4939e655dfa3d2363ab8400c2f426684f60492`.

Only `library/`, `include/` and `LICENSE` of the release are kept, without
their build files, and nothing in them is changed. The default configuration
(`include/mbedtls/mbedtls_config.h`) is used, with TLS 1.2 and 1.3, and with
its locks: the builds define `MBEDTLS_THREADING_C` for every unit that
includes its headers, with `MBEDTLS_THREADING_PTHREAD` on Linux, macOS and
Android (`tools/linux_build.py` `MBEDTLS_DEFINES`) and, on Windows,
`MBEDTLS_THREADING_ALT` with slim reader/writer locks
(`port/windows/mbedtls/threading_alt.h`, `port/windows/src/win32_mbedtls.c`).
Several threads use it at once: the game thread's DTLS, and the HTTPS of the
game list, Delta Stats, the dedicated server's control link and the
downloads, which share its PSA crypto (its setup, random generator and key
slots).

Every native build's internet play uses it for DTLS with browsers
(`port/linux/src/posix_dtls.c`: WebRTC's DTLS 1.2 server, with a
self-signed certificate made each run): the Linux and Windows builds, and
the Android app's host library. The web build has none of it (a browser has
its own WebRTC).

The Linux build's self-updater (`port/linux/src/posix_update.c`) uses it to
download new builds over HTTPS, with the system's certificate authorities.
The Windows build uses WinHTTP instead, and the Android app Java's HTTPS.

The consoles (original Xbox, Xbox 360, Wii U) build it with their own
configuration, `port/console/https/console_mbedtls_config.h`, for the shared
client in `port/console/https` (docs/console-https.md).
