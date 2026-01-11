#include "url_decode.hpp"

static int hex2int(char c)
{
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    return 0;
}

std::string url_decode(const std::string& src)
{
    std::string out;
    for (size_t i = 0; i < src.length(); i++)
    {
        if (src[i] == '%' && i + 2 < src.length())
        {
            char h = hex2int(src[i + 1]) << 4 | hex2int(src[i + 2]);
            out += h;
            i += 2;
        }
        else if (src[i] == '+')
        {
            out += ' ';
        }
        else
        {
            out += src[i];
        }
    }
    return out;
}
