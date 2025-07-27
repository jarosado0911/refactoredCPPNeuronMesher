/**
 * @file utils.cpp
 * @brief Implementation of utility functions for file system operations
 * 
 * This file contains general-purpose utility functions for file system operations
 * used throughout the CPPNeuronMesher project. It provides cross-platform
 * functionality for working with files and directories.
 *
 * Key features include:
 * - Directory management (creation, deletion, checking existence)
 * - File system path manipulation
 * - Directory listing and file enumeration
 * - Executable path resolution
 *
 * The implementation uses the C++17 filesystem library for cross-platform
 * compatibility and modern C++ practices.
 *
 * @author CPPNeuronMesher Team
 * @date 2025-07-27
 * @version 1.0
 * @copyright MIT License
 * @see https://github.com/yourusername/refactoredCPPNeuronMesher
 */

#include "utils.h"
#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;

/**
 * @brief Gets the directory containing the currently running executable
 * @return std::string The absolute path to the directory containing the executable
 * 
 * This function determines the directory containing the currently running executable
 * by resolving the symbolic link at "/proc/self/exe" and returning its parent directory.
 *
 * @note This implementation is Linux-specific and uses /proc filesystem
 * @note The returned path is always absolute and canonical (no . or .. components)
 * @note On error, the function may return an empty string
 * @see https://man7.org/linux/man-pages/man5/proc.5.html for /proc/self/exe details
 */
std::string getExecutableDir() {
    return fs::canonical("/proc/self/exe").parent_path().string();
}

/**
 * @brief Checks if a folder exists and creates it if it doesn't
 * @param[in,out] folderPath Path to the folder to check/create
 * 
 * This function checks if the specified folder exists. If it doesn't exist,
 * it attempts to create the folder and all necessary parent directories.
 * Status messages are printed to standard output/error streams.
 *
 * @note The function modifies the input string to ensure it's in a canonical form
 * @note If folder creation fails, an error message is printed to stderr
 * @note On success, the folderPath parameter will be updated to the canonical path
 * @see std::filesystem::create_directories() for details on directory creation
 * @see std::filesystem::exists() for details on existence checking
 */
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

/**
 * @brief Recursively deletes a directory and all its contents
 * @param[in] path Path to the directory to be deleted
 * 
 * This function attempts to delete the specified directory and all its contents
 * recursively. It uses std::filesystem::remove_all() for the operation.
 * Status messages are printed to standard output/error streams.
 *
 * @note The operation is not atomic - some files might be deleted even if others fail
 * @note If the operation fails, an error message is printed to stderr
 * @note The function will not throw exceptions on filesystem errors
 * @see std::filesystem::remove_all() for details on the underlying operation
 * @see std::error_code for error handling details
 */
void deleteFolder(const std::string& path) {
    std::error_code ec;
    fs::remove_all(path, ec);
    if (ec) {
        std::cerr << "Failed to delete directory: " << ec.message() << std::endl;
    } else {
        std::cout << "Directory '" << path <<  "' deleted successfully.\n";
    }
}

/**
 * @brief Lists all regular files in the specified directory
 * @param[in] path Path to the directory to scan
 * @return std::vector<std::string> Vector containing absolute paths to all regular files
 * 
 * This function scans the specified directory and returns a list of all regular files
 * it contains. The function only includes files (not directories) in the results.
 *
 * @note The function returns absolute file paths by default
 * @note The function does not recurse into subdirectories
 * @note If the directory cannot be accessed, an empty vector is returned
 * @note The order of files in the result is filesystem-dependent
 * @see std::filesystem::directory_iterator for iteration details
 * @see std::filesystem::is_regular_file() for file type checking
 */
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
