#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include <sstream>
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>

std::string encodeBase64(const std::vector<unsigned char> &data) {
    static const char* base64_chars = 
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

    std::string encoded;
    int i = 0;
    int j = 0;
    unsigned char char_array_3[3];
    unsigned char char_array_4[4];

    for (unsigned char byte : data) {
        char_array_3[i++] = byte;
        if (i == 3) {
            char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
            char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
            char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
            char_array_4[3] = char_array_3[2] & 0x3f;

            for(i = 0; i < 4; i++)
                encoded += base64_chars[char_array_4[i]];
            i = 0;
        }
    }

    if (i) {
        for(j = i; j < 3; j++)
            char_array_3[j] = '\0';

        char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
        char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
        char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
        char_array_4[3] = char_array_3[2] & 0x3f;

        for (j = 0; (j < i + 1); j++)
            encoded += base64_chars[char_array_4[j]];

        while((i++ < 3))
            encoded += '=';
    }

    return encoded;
}

std::string imageToBase64(const std::string &filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return "";
    }

    std::vector<unsigned char> buffer(std::istreambuf_iterator<char>(file), {});
    return encodeBase64(buffer);
}

std::vector<std::string> getImagesInDirectory(const std::string &directory) {
    std::vector<std::string> images;

    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir(directory.c_str())) != nullptr) {
        while ((ent = readdir(dir)) != nullptr) {
            std::string filename = ent->d_name;
            if (filename.length() > 4 &&
                (filename.substr(filename.length() - 4) == ".jpg" ||
                 filename.substr(filename.length() - 4) == ".png" ||
                 filename.substr(filename.length() - 4) == ".jpeg")) {
                images.push_back(filename);
            }
        }
        closedir(dir);
    } else {
        std::cerr << "Failed to open directory: " << directory << std::endl;
    }

    return images;
}

int main() {
    std::vector<std::string> images = getImagesInDirectory(".");

    std::ofstream outfile("images.txt");
    if (!outfile) {
        std::cerr << "Failed to create output file." << std::endl;
        return 1;
    }

    int imgNum = 1;
    for (const auto &image : images) {
        std::string base64 = imageToBase64(image);
        if (!base64.empty()) {
            outfile << "[img" << imgNum++ << "]:data:image/" << image.substr(image.find_last_of(".") + 1) << ";base64," << base64 << std::endl;
        }
    }

    std::cout << "Successfully generated images.txt" << std::endl;

    return 0;
}

