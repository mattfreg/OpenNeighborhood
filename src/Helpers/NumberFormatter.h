#pragma once

class NumberFormatter
{
public:
    static std::string FileSize(uint64_t number);

    static std::string Decimal(uint64_t number);
};
