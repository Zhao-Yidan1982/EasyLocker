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
lock -k keyfile -i infile -o out   # 指定输出文件
lock -k keyfile -i infile -m 512   # 每次读取 512 KB 块进行处理（默认 1024 KB）
lock -k keyfile -i infile -f       # 强制覆盖输出文件
lock -k keyfile -i infile -p       # 处理时显示进度
```

行为说明：
- 使用 `-k` 指定钥匙文件，钥匙内容按字节循环用于对输入文件进行异或处理。
- 如果 `-o` 缺失，输出文件将放在与输入文件相同目录，文件名加前缀 `out_`。
- `-f` 强制覆盖输出文件，无需确认。
- `-p` 显示处理进度。
- `-m` 指定每次处理的块大小，单位 KB，默认 `1024`（即 1 MiB）。
- 输入文件或钥匙文件为空时会报错并退出。

示例：

```bash
./lock -k 1.key -i random.bin            # 使用 1.key 对 random.bin 加密，输出 out_random.bin
./lock -k 1.key -i random.bin -o enc.bin # 输出到 enc.bin
./lock -k 1.key -i enc.bin -o dec.bin    # 再次相同操作可解密回原文
```
本文与代码由AI生成进攻参考，
