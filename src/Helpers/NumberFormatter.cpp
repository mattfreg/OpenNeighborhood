#include "pch.h"
#include "Helpers/NumberFormatter.h"

std::string NumberFormatter::FileSize(uint64_t number)
{
    std::stringstream result;
    std::string unit;

    const uint64_t oneKiloByte = 1024;
    const uint64_t oneMegaByte = oneKiloByte * 1024;
    const uint64_t oneGigaByte = oneMegaByte * 1024;

    result.setf(std::ios::fixed);
    result.precision(2);

    if (number < oneKiloByte)
    {
        result << number;
        return result.str();
    }

    if (number >= oneKiloByte && number < oneMegaByte)
    {
        result << (static_cast<float>(number) / oneKiloByte);
        unit = "KB";
    }
    else if (number >= oneMegaByte && number < oneGigaByte)
    {
        result << (static_cast<float>(number) / oneMegaByte);
        unit = "MB";
    }
    else if (number >= oneGigaByte)
    {
        result << (static_cast<float>(number) / oneGigaByte);
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
