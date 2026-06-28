# 更新记录

## v1.0.0

本节记录 v1.0.0 版本中 `RandomKey` 与 `Lock` 的具体参数与示例（当前实现）。

RandomKey (v1.0.0):

- 构建：

```bash
g++ -std=c++17 RandomKey.cpp -O2 -o randomkey
```

- 命令行参数（v1.0.0 实现）：
  - `-v` 显示版本并退出
  - `-h` 显示帮助
  - `-f` 强制覆盖输出文件（跳过确认）
  - `-p` 显示写入进度
  - `-o <path>` 指定输出路径（默认 `random.bin`）
  - `-s <size>` 要生成的大小，支持无后缀表示字节，或后缀 `K`/`M`/`G`（例如 `2K`, `1M`）

示例：

```bash
./randomkey -s 1024                 # 生成 1024 字节
./randomkey -s 2K -o out.bin        # 生成 2048 字节
./randomkey -s 1M -f -p             # 生成 1 MiB，强制覆盖并显示进度
```

Lock (v1.0.0):

- 构建：

```bash
g++ -std=c++17 Lock.cpp -O2 -o lock
```

- 命令行参数（v1.0.0 实现）：
  - `-v` 显示版本并退出
  - `-h` 显示帮助
  - `-f` 强制覆盖输出文件（跳过确认）
  - `-p` 显示处理进度
  - `-k <path>` 从文件读取钥匙（按字节循环）
  - `-x <hex>` 从十六进制字符串读取钥匙（字符串长度必须为偶数，支持混写大小写 `0-9a-fA-F`）
  - `-i <path>` 要处理的输入文件（必需）
  - `-o <path>` 输出文件路径（缺失时写入输入同目录并在文件名前加 `out_` 前缀）
  - `-m <KB>` 每次读取的块大小，单位 KB（默认 `1024`，即 1 MiB）

示例：

```bash
./lock -k key.bin -i secret.dat            # 使用 key.bin 对 secret.dat 异或处理，输出 out_secret.dat
./lock -x a1b2c3d4 -i secret.dat -o out.dat # 使用 hex 字符串作为 key
```

注意：后续版本可能会对参数做调整，README 保持通用说明，具体版本差异请以 `update.md` 中对应版本章节为准。
# RandomKey

生成指定字节数的随机二进制文件。

## 构建

在支持 C++17 的编译器上编译，例如 g++ 或 clang++：

```bash
g++ -std=c++17 RandomKey.cpp -O2 -o randomkey
```

Windows + MinGW/MSYS2 同样适用：

```powershell
g++ -std=c++17 RandomKey.cpp -O2 -o randomkey.exe
```

## 用法

```text
randomkey -v                   # 显示版本
randomkey -s 1024              # 生成 1024 字节到默认文件 random.bin
randomkey -s 2K -o out.bin     # 生成 2048 字节到 out.bin
randomkey -s 1M -f             # 生成 1 MiB，强制覆盖输出文件
randomkey -s 1G -p             # 生成 1 GiB，并显示写入进度
```

参数说明：
- `-v` 显示版本并退出
- `-h` 显示帮助
- `-f` 强制覆盖输出文件，无需确认
- `-p` 显示写入进度
- `-o` 输出路径，默认 `random.bin`
- `-s` 要生成的大小，支持 `K`、`M`、`G` 后缀

---

# Lock

对文件进行基于钥匙的循环异或加密/解密工具。

## 构建

在支持 C++17 的编译器上编译：

```bash
g++ -std=c++17 Lock.cpp -O2 -o lock
```

Windows:

```powershell
g++ -std=c++17 Lock.cpp -O2 -o lock.exe
```

## 用法

```text
lock -v                            # 显示版本
lock -k keyfile -i infile          # 使用 keyfile 对 infile 进行异或处理，输出到同目录下的 out_infile
lock -x a1b2c3d4 -i infile          # 使用十六进制钥匙对 infile 进行异或处理
lock -k keyfile -i infile -o out   # 指定输出文件
lock -k keyfile -i infile -m 512   # 每次读取 512 KB 块进行处理（默认 1024 KB）
lock -k keyfile -i infile -f       # 强制覆盖输出文件
lock -k keyfile -i infile -p       # 处理时显示进度
```

行为说明：
- 使用 `-k` 指定钥匙文件，或使用 `-x` 指定十六进制钥匙字符串。
- `-x` 的十六进制字符串长度必须为偶数，支持混写大小写 `0-9 a-f A-F`。
- 如果 `-o` 缺失，输出文件将放在与输入文件相同目录，文件名加前缀 `out_`。
- `-f` 强制覆盖输出文件，无需确认。
- `-p` 显示处理进度。
- `-m` 指定每次处理的块大小，单位 KB，默认 `1024`（即 1 MiB）。
- 输入文件或钥匙文件/钥匙串为空时会报错并退出。

示例：

```bash
./lock -k 1.key -i random.bin            # 使用 1.key 对 random.bin 加密，输出 out_random.bin
./lock -k 1.key -i random.bin -o enc.bin # 输出到 enc.bin
./lock -k 1.key -i enc.bin -o dec.bin    # 再次相同操作可解密回原文
```
本文与代码由AI生成进攻参考，
