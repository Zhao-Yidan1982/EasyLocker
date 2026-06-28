#include <iostream>
#include <fstream>
#include <random>
#include <vector>
#include <string>
#include <filesystem>
#include <cstdint>

static const char *VERSION = "1.0.0";

void print_usage(const char *prog) {
	std::cout << "Usage: " << prog << " [-v] [-h] [-o output_path] [-s size_in_bytes]\n";
	std::cout << "  -v            Show version and exit\n";
	std::cout << "  -h            Show this help\n";
	std::cout << "  -o <path>     Output file path (default: random.bin)\n";
	std::cout << "  -s <bytes>    Output size in bytes (required unless -h or -v)\n";
}

int main(int argc, char **argv) {
	std::string out_path = "random.bin";
	uint64_t size = 0;
	bool show_version = false;

	for (int i = 1; i < argc; ++i) {
		std::string a = argv[i];
		if (a == "-v") {
			show_version = true;
		} else if (a == "-h") {
			print_usage(argv[0]);
			return 0;
		} else if (a == "-o") {
			if (i + 1 >= argc) {
				std::cerr << "Error: -o requires a path\n";
				return 2;
			}
			out_path = argv[++i];
		} else if (a == "-s") {
			if (i + 1 >= argc) {
				std::cerr << "Error: -s requires a size in bytes\n";
				return 2;
			}
			try {
				size = std::stoull(argv[++i]);
			} catch (...) {
				std::cerr << "Error: invalid size\n";
				return 2;
			}
		} else {
			std::cerr << "Unknown option: " << a << "\n";
			print_usage(argv[0]);
			return 2;
		}
	}

	if (show_version) {
		std::cout << "RandomKey version " << VERSION << "\n";
		return 0;
	}

	if (size == 0) {
		std::cerr << "Error: size must be specified with -s and > 0\n";
		print_usage(argv[0]);
		return 2;
	}

	try {
		std::filesystem::path p(out_path);

		if (std::filesystem::exists(p)) {
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

		if (p.has_parent_path()) {
			std::filesystem::create_directories(p.parent_path());
		}

		std::ofstream ofs(out_path, std::ios::binary | std::ios::trunc);
		if (!ofs) {
			std::cerr << "Error: cannot open output file: " << out_path << "\n";
			return 3;
		}

		const size_t BUF_SIZE = 64u * 1024u; // 64 KiB buffer
		std::vector<uint8_t> buf;
		buf.resize(BUF_SIZE);

		std::random_device rd;
		std::mt19937_64 gen(rd());
		std::uniform_int_distribution<uint32_t> dist(0, 0xFFFFFFFFu);

		uint64_t remaining = size;
		while (remaining > 0) {
			size_t to_write = static_cast<size_t>(std::min<uint64_t>(BUF_SIZE, remaining));
			// fill buffer with random bytes 4 bytes at a time
			uint8_t *ptr = buf.data();
			size_t written = 0;
			while (written + 4 <= to_write) {
				uint32_t v = dist(gen);
				ptr[written + 0] = static_cast<uint8_t>(v & 0xFF);
				ptr[written + 1] = static_cast<uint8_t>((v >> 8) & 0xFF);
				ptr[written + 2] = static_cast<uint8_t>((v >> 16) & 0xFF);
				ptr[written + 3] = static_cast<uint8_t>((v >> 24) & 0xFF);
				written += 4;
			}
			// remaining bytes
			if (written < to_write) {
				uint32_t v = dist(gen);
				for (size_t j = written; j < to_write; ++j) {
					ptr[j] = static_cast<uint8_t>(v & 0xFF);
					v >>= 8;
				}
			}

			ofs.write(reinterpret_cast<const char *>(buf.data()), to_write);
			if (!ofs) {
				std::cerr << "Error: write failed\n";
				return 4;
			}
			remaining -= to_write;
		}
		ofs.close();
		std::cout << "Wrote " << size << " bytes to " << out_path << "\n";
	} catch (const std::exception &e) {
		std::cerr << "Exception: " << e.what() << "\n";
		return 5;
	}

	return 0;
}
