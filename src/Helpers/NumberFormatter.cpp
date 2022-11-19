#include "pch.h"
#include "Helpers/NumberFormatter.h"

std::string NumberFormatter::FileSize(uint64_t fileSize)
{
    std::stringstream result;
    std::string unit;

    const uint64_t oneKiloByte = 1024;
    const uint64_t oneMegaByte = oneKiloByte * 1024;
    const uint64_t oneGigaByte = oneMegaByte * 1024;

    result.setf(std::ios::fixed);
    result.precision(2);

    if (fileSize < oneKiloByte)
    {
        result << fileSize;
        unit = "B";
    }
    else if (fileSize >= oneKiloByte && fileSize < oneMegaByte)
    {
        result << (static_cast<float>(fileSize) / oneKiloByte);
        unit = "KB";
    }
    else if (fileSize >= oneMegaByte && fileSize < oneGigaByte)
    {
        result << (static_cast<float>(fileSize) / oneMegaByte);
        unit = "MB";
    }
    else if (fileSize >= oneGigaByte)
    {
        result << (static_cast<float>(fileSize) / oneGigaByte);
        unit = "GB";
    }

    result << " " << unit;

    return result.str();
}

std::string NumberFormatter::Decimal(uint64_t number)
{
    std::stringstream result;
    std::string numberAsString = std::to_string(number);

    for (size_t i = 0; i < numberAsString.size(); i++)
    {
        if (i != 0 && (numberAsString.size() - i) % 3 == 0)
            result << ',';

        result << numberAsString[i];
    }

    return result.str();
}
