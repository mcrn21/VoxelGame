#ifndef FILES_H
#define FILES_H

#include <filesystem>
#include <string>

class Files
{
public:
    static std::string readFileToString(const std::filesystem::path &path);
};

#endif // FILES_H
