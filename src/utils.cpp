#include "utils.h"
#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;

std::string getExecutableDir() {
    return fs::canonical("/proc/self/exe").parent_path().string();
}

void checkFolder(std::string& folderPath) {
    if (fs::exists(folderPath)) {
        std::cout << "Folder already exists: " << folderPath << std::endl;
    } else {
        if (fs::create_directory(folderPath)) {
            std::cout << "Folder created: " << folderPath << std::endl;
        } else {
            std::cerr << "Failed to create folder: " << folderPath << std::endl;
        }
    }
}

void deleteFolder(const std::string& path) {
    std::error_code ec;
    fs::remove_all(path, ec);
    if (ec) {
        std::cerr << "Failed to delete directory: " << ec.message() << std::endl;
    } else {
        std::cout << "Directory '" << path <<  "' deleted successfully.\n";
    }
}

std::vector<std::string> listFilesInDirectory(const std::string& path) {
    std::vector<std::string> files;

    for (const auto& entry : fs::directory_iterator(path)) {
        if (entry.is_regular_file()) {
            files.push_back(entry.path().string());  // full path
            // Or use entry.path().filename().string() for just the file name
        }
    }

    return files;
}
