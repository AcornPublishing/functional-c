#include <algorithm>
#include <random>
#include <functional>
#include <numeric>
#include <limits>
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

using namespace std;
using namespace std::placeholders;
using namespace std::chrono;

auto range = [](const int minValue, const int maxValue){
    vector<int> range(maxValue - minValue + 1);
    iota(range.begin(), range.end(), minValue);
    return range;
};

template<typename DestinationType>
auto transformAll = [](const auto& source, const auto& fn){
    DestinationType result;
    result.resize(source.size());
    transform(source.begin(), source.end(), result.begin(), fn);
    return result;
};

auto power = [](const int first, const int second){
    return pow(first, second);
};

auto generate_ints = [](const int min, const int max){
    if(min > max) {
        return vector<int>();
    }

    if(min == max){
        return range(min, min);
    }

    if(max - min <= 100){
        return range(min, max);
    }

    random_device rd;
    mt19937 generator(rd());
    uniform_int_distribution<int> distribution(min, max);
    auto values = transformAll<vector<int>>(range(0, 98), 
            [&distribution, &generator](auto){
                return distribution(generator);
            });
    values.push_back(min);
    values.push_back(max);
    return values;
};

auto logMaxIntBaseX = [](const int x) -> int{
    const int maxInt = numeric_limits<int>::max() ;
    return floor(log(maxInt) / log(x));
};

auto generate_ints_greater_than_1 = bind(generate_ints, 1, numeric_limits<int>::max());
auto generate_ints_greater_than_0 = bind(generate_ints, 0, numeric_limits<int>::max());
auto generate_ints_greater_than_2_less_sqrt_maxInt = bind(generate_ints, 2, sqrt(numeric_limits<int>::max()));
auto generate_ints_greater_than_sqrt_maxInt = bind(generate_ints, sqrt(numeric_limits<int>::max()) + 1, numeric_limits<int>::max());

auto generate_exponent_less_than_log_maxInt = [](const int x){
    return generate_ints(1, logMaxIntBaseX(x));
};

auto all_of_collection = [](const auto& collection, const auto& fn){
    return all_of(collection.begin(), collection.end(), fn);
};

auto printGeneratedValues = [](const string& generatorName, const auto& values){
    cout << "Check generator " << generatorName << endl;
    for_each(values.begin(), values.end(), [](auto value) { cout << value << ", ";});
    cout << endl;
 };

auto check_property = [](const auto& generator, const auto& property, const string& generatorName){
    auto values = generator();
    printGeneratedValues(generatorName, values);
    bool pass = all_of_collection(values, property);
    CHECK(pass);
    return pass;
};

auto property_0_to_power_0_is_1 = [](){
    return power(0, 0) == 1;
};

auto prop_0_to_any_nonzero_int_is_0= [](const int exponent){
    CHECK(exponent > 0);
    return power(0, exponent) == 0;
};

auto prop_anyIntToPower0Is1 = [](const int base){
    CHECK(base > 0);
    return power(base, 0) == 1;
};

auto prop_any_int_to_power_1_is_the_value = [](const int base){
    return power(base, 1) == base;
};

auto prop_nextPowerOfXIsPreviousPowerOfXMultipliedByX = [](const int x){
    auto exponents = bind(generate_exponent_less_than_log_maxInt, x);
    return check_property(exponents, [x](auto y){ return power(x, y) == power(x, y - 1) * x;}, "generate exponents for " + to_string(x));
};

TEST_CASE("Properties"){
    cout << "Property: 0 to power 0 is 1" << endl;
    CHECK(property_0_to_power_0_is_1);

    cout << "Property: 0 to any non-zero power is 0" << endl;
    check_property(generate_ints_greater_than_1, prop_0_to_any_nonzero_int_is_0, "generate ints");

    cout << "Property: any int to power 0 is 1" << endl;
    check_property(generate_ints_greater_than_1, prop_anyIntToPower0Is1, "generate ints");

    cout << "Property: any int to power 1 is the value" << endl;
    check_property(generate_ints_greater_than_0, prop_any_int_to_power_1_is_the_value, "generate ints");

    cout << "Property: next power of x is previous power of x multiplied by x" << endl;
    check_property(generate_ints_greater_than_2_less_sqrt_maxInt, prop_nextPowerOfXIsPreviousPowerOfXMultipliedByX, "generate greater than 2 and less than sqrt of maxInt");
    check_property(generate_ints_greater_than_sqrt_maxInt, prop_nextPowerOfXIsPreviousPowerOfXMultipliedByX, "generate greater than sqrt of maxInt");
}
