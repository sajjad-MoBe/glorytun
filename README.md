# Glorytun

Glorytun is a small, simple and secure multipath UDP tunnel.

Please use the [stable branch](https://github.com/angt/glorytun/tree/stable).
Visit the [wiki](https://github.com/angt/glorytun/wiki) for how-to guides,
tutorials, etc.

## Features

The key features of Glorytun come directly from [mud](https://github.com/angt/mud).

 * **Fast and highly secure**

   When AES-NI is available, the new and extremely fast AEAD construction
   [AEGIS-256](https://github.com/angt/aegis256) is used.
   Otherwise, an automatic fallback to ChaCha20-Poly1305 is done in both peers.
   All messages are encrypted, authenticated and timestamped to mitigate a
   large set of attacks.
   This implies that the client and the server must be synchronized,
   an offset of 10min is accepted by default.
   Perfect forward secrecy is also implemented with ECDH over Curve25519.
   Keys are rotated every hours.

 * **Multipath and failover**

   Connectivity is now crucial, especially in the SD-WAN world.
   This feature allows a TCP connection (and all other protocols) to explore
   and exploit all available links without being disconnected.
   Aggregation should work on all conventional links.
   Only very high latency (+500ms) links are not recommended for now.
   Backup paths are also supported, they will be used only in case of emergency,
   it is useful when aggregation is not your priority.

 * **Traffic shaping**

   Shaping is very important in network, it allows to keep a low latency
   without sacrificing the bandwidth.
   It also helps the multipath scheduler to make better decisions.
   Currently it must be configured by hand, but soon Glorytun will do it
   for you.

 * **Path MTU discovery without ICMP**

   Bad MTU configuration is a very common problem in the world of VPN.
   As it is critical, Glorytun will try to setup it correctly by guessing
   its value.
   It doesn't rely on Next-hop MTU to avoid ICMP black holes.
   In asymmetric situations the minimum MTU is selected.

## Quick Start

### 1. Generate a Key
On either the server or client, generate a secret key:
```bash
./glorytun keygen > gt.key
```
Copy this `gt.key` to both machines.

### 2. Server Setup
Run Glorytun on the server (replace `SERVER_IP` with your public IP):
```bash
./glorytun bind SERVER_IP 55055 dev tun0 keyfile gt.key chacha
```
Configure the tunnel interface and NAT:
```bash
ifconfig tun0 10.0.1.1 pointopoint 10.0.1.2 up
iptables -t nat -I POSTROUTING -s 10.0.1.0/24 -o eth0 -j MASQUERADE
iptables -I INPUT -i tun0 -j ACCEPT
iptables -I FORWARD -i eth0 -o tun0 -j ACCEPT
iptables -I FORWARD -i tun0 -o eth0 -j ACCEPT
```

### 3. Client Setup
Run Glorytun on the client:
```bash
./glorytun bind 0.0.0.0 to SERVER_IP 55055 dev tun0 keyfile gt.key chacha
```
Configure the tunnel interface:
```bash
ifconfig tun0 10.0.1.2 pointopoint 10.0.1.1 up
```

### 4. Adding Multipath Paths
To add additional paths (e.g., using different gateways or local IPs), use the `path` command:
```bash
./glorytun path up 192.168.70.2 rate tx 50mbit rx 50mbit
./glorytun path up 192.168.70.3 rate tx 50mbit rx 50mbit
```
The version included in this repository supports `IP_FREEBIND`, allowing you to bind to non-local IP addresses for these paths if they are reachable via your routing table.

### 5. Status and Monitoring
Check the status of the tunnel and paths:
```bash
./glorytun show
./glorytun path show
```

## Compatibility

Glorytun only depends on [libsodium](https://github.com/jedisct1/libsodium)
version >= 1.0.4.
Which can be installed on a wide variety of systems.

Linux is the platform of choice but the code is standard so it should be
easily ported on other posix systems.
It was successfully tested on OpenBSD, FreeBSD and MacOS.

IPv4 and IPv6 are supported.
On Linux you can have both at the same time by binding `::`.

---
For feature requests and bug reports,
please create an [issue](https://github.com/angt/glorytun/issues).
