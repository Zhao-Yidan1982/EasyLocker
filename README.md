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
randomkey -v                 # 显示版本
randomkey -s 1024            # 生成 1024 字节到默认文件 random.bin
randomkey -s 4096 -o out.bin # 生成 4096 字节到 out.bin
```

参数说明：
- `-v` 显示版本并退出
- `-h` 显示帮助
- `-o` 输出路径，默认 `random.bin`
- `-s` 要生成的大小（字节），必需且为正整数
