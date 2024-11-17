#include "Files.h"

#include <fstream>

std::string Files::readFileToString(const std::filesystem::path &path)
{
    auto a_path = std::filesystem::absolute(path);

    std::ifstream file_stream;
    std::string file_str;

    file_stream.open(a_path.string());

    std::stringstream str_stream;
    str_stream << file_stream.rdbuf();

    file_stream.close();

    return str_stream.str();
}
