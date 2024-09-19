#pragma once

#include <string>
#include <array>
#include <memory>

class Utils
{
public:

#ifdef __linux__
    inline static std::string exec(const std::string str) {
        std::array<char, 128> buffer;
        std::string result;
        std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(str.c_str(), "r"), pclose);
        if (!pipe) {
            return "";
        }
        while (fgets(buffer.data(), static_cast<int>(buffer.size()), pipe.get()) != nullptr) {
            std::string line = buffer.data();
            line.erase(std::remove(line.begin(), line.end(), '\n'), line.cend());
            result += line;
        }
        return result;
    }
#endif
};