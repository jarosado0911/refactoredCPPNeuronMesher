#pragma once
#include <string>
#include <vector>

std::string getExecutableDir();
void checkFolder(std::string& folderPath);
void deleteFolder(const std::string& path);
std::vector<std::string> listFilesInDirectory(const std::string& path);

