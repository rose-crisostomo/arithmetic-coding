#include <map>

class Model
{
public:
    struct Range
    {
        double lower;
        double upper;
    };

    Model()
    {
        probability['$'] = Range{ 0.0, 0.2 };
        probability['a'] = Range{ 0.2, 0.6 };
        probability['b'] = Range{ 0.6, 1.0 };
    }

    Range GetProbability(char c)
    {
        return probability[c];
    }

private:
    std::map<char, Range> probability;
};