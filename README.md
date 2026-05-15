# firmstrip

Tool to extract and analyze filesystem contents from IoT firmware images.

---

## Installation

```bash
make && sudo make install
```

## Usage

Extract and analyze a firmware image:

```bash
firmstrip -i firmware.bin -o ./output/
```

Options:

| Flag | Description |
|------|-------------|
| `-i <file>` | Input firmware image |
| `-o <dir>` | Output directory for extracted contents |
| `-a` | Run analysis report after extraction |
| `-v` | Verbose output |

Example with analysis:

```bash
firmstrip -i router_fw_v2.1.bin -o ./extracted/ -a
```

This will extract the embedded filesystem and generate a report listing file structure, permissions, hardcoded strings, and potential points of interest.

## Building from Source

```bash
git clone https://github.com/youruser/firmstrip.git
cd firmstrip
make
```

Requires `gcc` and `libz`. On Debian/Ubuntu:

```bash
sudo apt install build-essential zlib1g-dev
```

## License

MIT © 2024 — see [LICENSE](LICENSE) for details.