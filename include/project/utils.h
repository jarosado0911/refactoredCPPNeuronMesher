/**
 * @file utils.h
 * @brief Utility functions for file system operations
 * 
 * This header provides cross-platform utility functions for common file system
 * operations used throughout the CPPNeuronMesher project. These utilities
 * handle path manipulation, directory creation, and file listing.
 *
 * @author CPPNeuronMesher Team
 * @date 2025-07-27
 * @version 1.0
 * @copyright MIT License
 */

#pragma once
#include <string>
#include <vector>

/**
 * @brief Gets the directory containing the current executable
 * @return Absolute path to the directory containing the executable
 * 
 * This function retrieves the full path to the directory containing the
 * currently running executable. The path is returned with a trailing
 * directory separator.
 * 
 * @note On Linux, this uses /proc/self/exe to determine the path
 * @note The returned path is guaranteed to be absolute
 * @note The function may return an empty string if the path cannot be determined
 */
std::string getExecutableDir();
/**
 * @brief Ensures a directory exists, creating it if necessary
 * @param[in,out] folderPath Path to the directory to check/create
 * 
 * This function checks if the specified directory exists and creates it
 * (including any necessary parent directories) if it doesn't exist.
 * The folderPath parameter is modified to contain an absolute path.
 * 
 * @throws std::runtime_error if the directory cannot be created
 * @note Uses std::filesystem::create_directories()
 */
void checkFolder(std::string& folderPath);
/**
 * @brief Recursively deletes a directory and all its contents
 * @param[in] path Path to the directory to delete
 * 
 * This function removes the specified directory and all files and subdirectories
 * it contains. If the path does not exist, the function does nothing.
 * 
 * @throws std::runtime_error if the directory cannot be deleted
 * @note Uses std::filesystem::remove_all()
 * @warning This operation is not reversible
 */
void deleteFolder(const std::string& path);
/**
 * @brief Lists all regular files in a directory
 * @param[in] path Path to the directory to scan
 * @return Vector of filenames (not including directory path)
 * 
 * This function returns a list of all regular files (not directories)
 * in the specified directory. The returned filenames do not include
 * the directory path.
 * 
 * @throws std::runtime_error if the directory cannot be accessed
 * @note The order of files in the result is filesystem-dependent
 * @note Symbolic links are not followed
 */
std::vector<std::string> listFilesInDirectory(const std::string& path);

