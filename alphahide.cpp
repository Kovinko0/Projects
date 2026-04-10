#include <iostream>
#include <vector>
#include <fstream>
#include <iterator>
#include <string>
#include "lodepng.h"

// 1-byte delimiter (simpler + matches decoder)
const std::string DELIM = "11111110";

// --- utils ---

std::string bytes_to_bits(const std::vector<unsigned char>& data) {
    std::string bits;
    for (auto b : data) {
        for (int i = 7; i >= 0; --i) {
            bits += ((b >> i) & 1) ? '1' : '0';
        }
    }
    return bits;
}

std::vector<unsigned char> bits_to_bytes(const std::string& bits) {
    std::vector<unsigned char> out;

    for (size_t i = 0; i + 8 <= bits.size(); i += 8) {
        std::string byte = bits.substr(i, 8);

        if (byte == DELIM) break;

        unsigned char val = 0;
        for (int j = 0; j < 8; ++j) {
            val = (val << 1) | (byte[j] - '0');
        }

        out.push_back(val);
    }

    return out;
}

std::vector<unsigned char> read_stdin() {
    return std::vector<unsigned char>(
        std::istreambuf_iterator<char>(std::cin),
        std::istreambuf_iterator<char>()
    );
}

std::vector<unsigned char> read_file(const std::string& path) {
    std::ifstream f(path, std::ios::binary);
    return std::vector<unsigned char>(
        std::istreambuf_iterator<char>(f),
        std::istreambuf_iterator<char>()
    );
}

void write_stdout(const std::vector<unsigned char>& data) {
    std::cout.write(reinterpret_cast<const char*>(data.data()), data.size());
}

// --- encode ---

int encode(const std::string& in_png, const std::string& out_png, std::vector<unsigned char>& data) {
    std::vector<unsigned char> image;
    unsigned w, h;

    unsigned error = lodepng::decode(image, w, h, in_png);
    if (error) {
        std::cerr << "Decode error: " << error << "\n";
        return 1;
    }

    std::string bits = bytes_to_bits(data) + DELIM;

    // RGBA = 4 bytes per pixel → 1 alpha per pixel
    size_t capacity = image.size() / 4;

    if (bits.size() > capacity) {
        std::cerr << "Not enough capacity\n";
        return 1;
    }

    size_t idx = 0;

    for (size_t i = 0; i < image.size(); i += 4) {
        if (idx < bits.size()) {
            image[i + 3] = (image[i + 3] & ~1) | (bits[idx++] - '0');
        }
    }

    error = lodepng::encode(out_png, image, w, h);
    if (error) {
        std::cerr << "Encode error: " << error << "\n";
        return 1;
    }

    return 0;
}

// --- decode ---

int decode(const std::string& in_png) {
    std::vector<unsigned char> image;
    unsigned w, h;

    unsigned error = lodepng::decode(image, w, h, in_png);
    if (error) {
        std::cerr << "Decode error: " << error << "\n";
        return 1;
    }

    std::string bits;

    for (size_t i = 0; i < image.size(); i += 4) {
        bits += (image[i + 3] & 1) ? '1' : '0';
    }

    auto data = bits_to_bytes(bits);
    write_stdout(data);

    return 0;
}

// --- main ---

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Usage:\n";
        std::cerr << "  encode input.png output.png [file]\n";
        std::cerr << "  decode input.png\n";
        return 1;
    }

    std::string cmd = argv[1];

    if (cmd == "encode") {
        if (argc < 4) {
            std::cerr << "Missing arguments for encode\n";
            return 1;
        }

        std::vector<unsigned char> data;

        if (argc >= 5) {
            data = read_file(argv[4]);
        } else {
            data = read_stdin();
        }

        return encode(argv[2], argv[3], data);
    }

    else if (cmd == "decode") {
        return decode(argv[2]);
    }

    std::cerr << "Unknown command\n";
    return 1;
}
