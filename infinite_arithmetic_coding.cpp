#include <stdio.h>
#include <iostream>
#include <cmath>
#include <algorithm>
#include <fstream>
#include "model.h"

const double HALF = 0.5;
const double QUARTER = 0.25;
const double PACMAN = 0.75;

void output(std::bitset<16>& out_bits, short val, uint& bit_count)
{
    out_bits <<= 1;
    out_bits.set(0, val);
    ++bit_count;
}

void writeToFile(const std::bitset<16>& out_bits)
{
    std::ofstream os("compressed.txt", std::ios::binary);
    auto value = out_bits.to_ulong();
    os.write(reinterpret_cast<const char*>(&value), sizeof(value));
    os.close();
}

uint16_t readBits()
{
    std::ifstream is("compressed.txt", std::ios::binary);
    uint16_t value;
    is.read(reinterpret_cast<char *>(&value), sizeof(value));
    is.close();
    return value;
}

uint encode(Model model, std::string input)
{
    std::bitset<16> out_bits;

    double low = 0.0;
    double high = 1.0;
    uint bit_count = 0;

    for (const auto& s : input)
    {
        double range = high - low;
        const auto& p = model.GetProbability(s);
        high = low + range * p.upper;
        low = low + range * p.lower;
    }

    int rem_bits = 0;
    while (high < HALF || low > HALF)
    {
        if (low > HALF)
        {
            output(out_bits, 1, bit_count);
            high = 2 * (high - HALF);
            low = 2 * (low - HALF);
        }
        else if (high < HALF)
        {
            output(out_bits, 0, bit_count);
            high = 2 * high;
            low = 2 * low;
        }
    }

    while (low > QUARTER && high < PACMAN)
    {
        rem_bits++;
        high = 2 * (high - QUARTER);
        low = 2 * (low - QUARTER);
    }

    rem_bits++;
    if (low <= QUARTER)
    {
        output(out_bits, 0, bit_count);
        for (int i = 0; i < rem_bits; i++)
        {
            output(out_bits, 1, bit_count);
        }
    }
    else
    {
        output(out_bits, 1, bit_count);
        for (int i = 0; i < rem_bits; i++)
            output(out_bits, 0, bit_count);
    }

    std::cout << "-- encoding " << out_bits.to_string() << std::endl;
    writeToFile(out_bits);
    return bit_count;
}

void decode(Model model, double input, std::string symbols)
{
    double low = 0.0;
    double high = 1.0;
    bool cont = true;

    while (cont)
    {
        for (const auto& s : symbols)
        {
            double range = high - low;
            const auto& p = model.GetProbability(s);
            double low_temp = low + range * p.lower;
            double high_temp = low + range * p.upper;

            if (low_temp <= input && input < high_temp)
            {
                std::cout << s;
                if (s == '$') // end of file
                {
                    cont = false;
                    break;
                }

                low = low_temp;
                high = high_temp;
            }
        }
    }
}

double toBinaryFraction(uint binary_number)
{
    unsigned bits, var = binary_number;
    for(bits = 0; var != 0; ++bits) var >>= 1;

    return binary_number / pow(2, bits);
}

int main()
{
    std::string strInput;
    std::string symbols = "ab$";
    std::cout << "String input: " << std::endl;
    std::cin >> strInput;

    Model model;

    std::cout << "Encode with infinite precision: " << strInput << std::endl;
    uint bits = encode(model, strInput);
    uint binInput = readBits();
    double lowInput = binInput / pow(2, bits);
    std::cout << "Decode with infinite precision: " << lowInput << std::endl;
    decode(model, lowInput, symbols);

    return 0;
}