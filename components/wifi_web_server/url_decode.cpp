#include "url_decode.hpp"
#include <cctype>

std::string url_decode(const std::string &src) {
    std::string ret;
    char hex[3] = {0};
    for (size_t i = 0; i < src.length(); ++i) {
        if (src[i] == '%') {
            if (i + 2 < src.length()) {
                hex[0] = src[i + 1];
                hex[1] = src[i + 2];
                ret += static_cast<char>(strtol(hex, nullptr, 16));
                i += 2;
            }
        } else if (src[i] == '+') {
            ret += ' ';
        } else {
            ret += src[i];
        }
    }
    return ret;
}
