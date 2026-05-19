# 📁 TFTP — Trivial File Transfer Protocol in C

![Language](https://img.shields.io/badge/Language-C-blue?style=flat-square&logo=c)
![Platform](https://img.shields.io/badge/Platform-Linux-orange?style=flat-square&logo=linux)
![Protocol](https://img.shields.io/badge/Protocol-UDP-purple?style=flat-square)
![Status](https://img.shields.io/badge/Status-Complete-success?style=flat-square)

A client-server file transfer application written in C, built over **UDP sockets**, following the TFTP protocol. Supports uploading and downloading files between client and server with multiple transfer modes and block-level acknowledgment.

---

## 🔍 How It Works

```
Client                          Server
  │                               │
  │──── RRQ / WRQ (filename) ────►│
  │◄─── SUCCESS / FAILURE ────────│
  │                               │
  │◄─── DATA (block 1) ──────────►│  ← send_file() / receive_file()
  │──── ACK  (block 1) ──────────►│
  │         ...                   │
  │──── DATA (block N, 0 bytes) ──│  ← EOF signal
```

Both `send_file()` and `receive_file()` are shared between client and server via `tftp.c` and `tftp.h`.

---

## ✨ Features

- 📤 **PUT** — upload a file from client to server (`WRQ`)
- 📥 **GET** — download a file from server to client (`RRQ`)
- 🔁 **Block-wise transfer** — 512 bytes per block with ACK after each block
- 🔄 **Retransmission** — automatically resends on ACK mismatch
- 🗂️ **Transfer modes** — Default (512 B), Octet (1 B), NetASCII
- ✅ **IP & port validation** — checked before connecting
- 📡 **UDP sockets** — connectionless, lightweight communication
- 🖼️ **Binary file support** — tested with `.txt`, `.c`, `.jpg` files

---

## 🗂️ Project Structure

```
TFTP_project/
├── TFTP_client/
│   ├── tftp_client.c     # Client main — menu, connect, put, get
│   ├── tftp_client.h     # tftp_client_t struct and function declarations
│   ├── tftp.c            # Shared — send_file() and receive_file()
│   └── tftp.h            # Shared — packet structs, opcodes, constants
│
└── TFTP_server/
    ├── tftp_server.c     # Server main — bind, listen, handle_client()
    ├── tftp.c            # Shared — send_file() and receive_file()
    └── tftp.h            # Shared — packet structs, opcodes, constants
```

---

## 🚀 Build & Run

### Compile

```bash
# Terminal 1 — Server
cd TFTP_server
gcc tftp.c tftp_server.c -o server

# Terminal 2 — Client
cd TFTP_client
gcc tftp.c tftp_client.c -o client
```

### Run

```bash
# Start server first
./server

# Then start client
./client
```

---

## 📖 Usage

### Step 1 — Connect to server

```
......................Menu....................
1. Connect
2. Put
3. Get
4. Mode
5. Exit
Enter your option from menu: 1
Enter Server IP Address: 127.0.0.1
Enter Server Port Number: 6969
Connected to IP address 127.0.0.1 and port 6969
```

### Step 2 — Upload a file (PUT)

```
Enter your option from menu: 2
Enter the filename: file1.txt
```

```
# Server output:
Request received: operation = 2
Request received: filename = file1.txt
Started receiving file: file1.txt
File transfer completed...
```

### Step 3 — Download a file (GET)

```
Enter your option from menu: 3
Enter filename: file1.txt
File receive completed...
```

### Step 4 — Change transfer mode

```
Enter your option from menu: 4

Select Mode
0. Default (512 bytes)
1. Octet   (1 byte)
2. NetASCII
Enter mode: 1
```

---

## 📦 TFTP Packet Types

| Opcode | Type | Description |
|--------|------|-------------|
| `1` | `RRQ` | Read request — client wants to download |
| `2` | `WRQ` | Write request — client wants to upload |
| `3` | `DATA` | Data block with block number |
| `4` | `ACK` | Acknowledgment for received block |
| `5` | `ERROR` | Error notification |

---

## ⚙️ Transfer Modes

| Mode | Block Size | Use Case |
|------|-----------|----------|
| Default | 512 bytes | General file transfer |
| Octet | 1 byte | Precise / binary transfer |
| NetASCII | 512 bytes | Text with `\n` → `\r` conversion |

---

## 🧠 Key Concepts Used

| Concept | Purpose |
|---------|---------|
| `socket()` + `bind()` | Create and bind UDP socket on server |
| `sendto()` + `recvfrom()` | Send/receive packets without a connection |
| `htons()` / `ntohs()` | Convert between host and network byte order |
| `fork()` / `open()` / `read()` / `write()` | File handling during transfer |
| `struct sockaddr_in` | Store IP address and port information |
| Block numbering + ACK loop | Reliable transfer over unreliable UDP |

---

## 👤 Author

**Amar C M** — Embedded Systems Engineer

[![LinkedIn](https://img.shields.io/badge/LinkedIn-Connect-blue?style=flat&logo=linkedin)](https://linkedin.com/in/amar-c-m-233375224) · amarcm1502@gmail.com

---

## 📜 License

[MIT License](LICENSE) — free to use and modify with attribution.
