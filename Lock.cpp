#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <filesystem>
#include <cctype>
#include <cstdint>

static const char *VERSION = "1.0.0";

void print_usage(const char *prog) {
    std::cout << "Usage: " << prog << " -k key_path -i input_file [-o output_file] [-v] [-f] [-p] [-m block_kb]\n";
    std::cout << "  -v            Show version and exit\n";
    std::cout << "  -k <path>     Key file path\n";
    std::cout << "  -i <path>     Input file to encrypt/decrypt\n";
    std::cout << "  -o <path>     Output file path (default: same directory, prefix out_)\n";
    std::cout << "  -f            Force overwrite output file if it exists\n";
    std::cout << "  -p            Show progress while processing\n";
    std::cout << "  -m <KB>       Block size in KB for processing (default 1024 KB)\n";
}

int main(int argc, char **argv) {
    std::string key_path;
    std::string in_path;
    std::string out_path;
    size_t block_kb = 1024;
    bool show_version = false;
    bool force_overwrite = false;
    bool show_progress = false;

    for (int i = 1; i < argc; ++i) {
        std::string a = argv[i];
        if (a == "-v") {
            show_version = true;
        } else if (a == "-f") {
            force_overwrite = true;
        } else if (a == "-p") {
            show_progress = true;
        } else if (a == "-k") {
            if (i + 1 >= argc) { std::cerr << "Error: -k requires a path\n"; return 2; }
            key_path = argv[++i];
        } else if (a == "-i") {
            if (i + 1 >= argc) { std::cerr << "Error: -i requires a path\n"; return 2; }
            in_path = argv[++i];
        } else if (a == "-o") {
            if (i + 1 >= argc) { std::cerr << "Error: -o requires a path\n"; return 2; }
            out_path = argv[++i];
        } else if (a == "-m") {
            if (i + 1 >= argc) { std::cerr << "Error: -m requires a value\n"; return 2; }
            try { block_kb = static_cast<size_t>(std::stoul(argv[++i])); } catch (...) { std::cerr << "Error: invalid -m value\n"; return 2; }
        } else if (a == "-h") {
            print_usage(argv[0]);
            return 0;
        } else {
            std::cerr << "Unknown option: " << a << "\n";
            print_usage(argv[0]);
            return 2;
        }
    }

    if (show_version) {
        std::cout << "Lock version " << VERSION << "\n";
        return 0;
    }

    if (key_path.empty()) {
        std::cerr << "Error: key file path (-k) is required\n";
        return 2;
    }
    if (in_path.empty()) {
        std::cerr << "Error: input file (-i) is required\n";
        return 2;
    }

    try {
        // read key file
        std::ifstream kf(key_path, std::ios::binary);
        if (!kf) { std::cerr << "Error: cannot open key file: " << key_path << "\n"; return 3; }
        std::vector<uint8_t> key((std::istreambuf_iterator<char>(kf)), std::istreambuf_iterator<char>());
        kf.close();
        if (key.empty()) { std::cerr << "Error: key file is empty\n"; return 4; }

        // open input
        std::ifstream inf(in_path, std::ios::binary);
        if (!inf) { std::cerr << "Error: cannot open input file: " << in_path << "\n"; return 3; }

        // prepare output path
        if (out_path.empty()) {
            std::filesystem::path p(in_path);
            std::filesystem::path dir = p.parent_path();
            std::string name = p.filename().string();
            out_path = (dir / (std::string("out_") + name)).string();
        }

        if (std::filesystem::exists(out_path) && !force_overwrite) {
            std::cout << "Output file exists: " << out_path << ". Overwrite? Type 'yes' or 'on' to confirm [no]: ";
            std::string ans;
            if (!std::getline(std::cin, ans)) {
                std::cerr << "No input, aborting.\n";
                return 6;
            }
            for (char &c : ans) c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
            if (!(ans == "yes" || ans == "on")) {
                std::cerr << "Aborted: not overwriting.\n";
                return 6;
            }
        }

        std::ofstream outf(out_path, std::ios::binary | std::ios::trunc);
        if (!outf) { std::cerr << "Error: cannot open output file: " << out_path << "\n"; return 3; }

        const size_t BUF_SIZE = block_kb * 1024;
        std::vector<uint8_t> buf;
        buf.resize(BUF_SIZE);

        size_t key_len = key.size();
        size_t key_pos = 0;

        uint64_t processed = 0;
        while (inf) {
            inf.read(reinterpret_cast<char *>(buf.data()), static_cast<std::streamsize>(BUF_SIZE));
            std::streamsize got = inf.gcount();
            if (got <= 0) break;
            for (std::streamsize i = 0; i < got; ++i) {
                buf[i] ^= key[key_pos];
                key_pos = (key_pos + 1) % key_len;
            }
            outf.write(reinterpret_cast<const char *>(buf.data()), got);
            if (!outf) { std::cerr << "Error: write failed\n"; return 5; }
            if (show_progress) {
                processed += static_cast<uint64_t>(got);
                std::cout << "\rProcessed " << processed << " bytes" << std::flush;
            }
        }

        inf.close();
        outf.close();
        std::cout << "Processed " << in_path << " -> " << out_path << "\n";
    } catch (const std::exception &e) {
        std::cerr << "Exception: " << e.what() << "\n";
        return 6;
    }

    return 0;
}
