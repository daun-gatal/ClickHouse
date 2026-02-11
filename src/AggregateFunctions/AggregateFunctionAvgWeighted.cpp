#include <AggregateFunctions/AggregateFunctionAvgWeighted.h>
#include <AggregateFunctions/AggregateFunctionFactory.h>
#include <AggregateFunctions/FactoryHelpers.h>


namespace DB
{

struct Settings;

namespace ErrorCodes
{
    extern const int ILLEGAL_TYPE_OF_ARGUMENT;
}

namespace
{

bool allowTypes(const DataTypePtr & left, const DataTypePtr & right) noexcept
{
    const WhichDataType l_dt(left);
    const WhichDataType r_dt(right);

    constexpr auto allow = [](WhichDataType t)
    {
        return t.isInt() || t.isUInt() || t.isNativeFloat();
    };

    return allow(l_dt) && allow(r_dt);
}

/// First-level dispatch: first half of value types.
template <class... TArgs>
IAggregateFunction * createFirstHalf(const IDataType & first_type, const IDataType & second_type, TArgs && ... args)
{
    const WhichDataType which(first_type);

#define LINE(Type) \
    case TypeIndex::Type: return createAvgWeightedSecondType<Type, TArgs...>(second_type, std::forward<TArgs>(args)...)
    switch (which.idx)
    {
        LINE(Int8); LINE(Int16); LINE(Int32); LINE(Int64);
        LINE(UInt8); LINE(UInt16); LINE(UInt32);
        default: return nullptr;
    }
#undef LINE
}

}

/// Defined in AggregateFunctionAvgWeighted2.cpp.
IAggregateFunction * createAvgWeightedSecondHalf(const IDataType & first_type, const IDataType & second_type, const DataTypes & argument_types);

namespace
{

AggregateFunctionPtr
createAggregateFunctionAvgWeighted(const std::string & name, const DataTypes & argument_types, const Array & parameters, const Settings *)
{
    assertNoParameters(name, parameters);
    assertBinary(name, argument_types);

    const auto data_type = static_cast<const DataTypePtr>(argument_types[0]);
    const auto data_type_weight = static_cast<const DataTypePtr>(argument_types[1]);

    if (!allowTypes(data_type, data_type_weight))
        throw Exception(ErrorCodes::ILLEGAL_TYPE_OF_ARGUMENT,
                        "Types {} and {} are non-conforming as arguments for aggregate function {}",
                        data_type->getName(), data_type_weight->getName(), name);

    IAggregateFunction * result = createFirstHalf(*data_type, *data_type_weight, argument_types);
    if (!result)
        result = createAvgWeightedSecondHalf(*data_type, *data_type_weight, argument_types);
    return AggregateFunctionPtr(result);
}

}

void registerAggregateFunctionAvgWeighted(AggregateFunctionFactory & factory)
{
    FunctionDocumentation::Description description = R"(
Calculates the [weighted arithmetic mean](https://en.wikipedia.org/wiki/Weighted_arithmetic_mean).
    )";
    FunctionDocumentation::Syntax syntax = "avgWeighted(x, weight)";
    FunctionDocumentation::Arguments arguments = {
        {"x", "Values.", {"(U)Int*", "Float*"}},
        {"weight", "Weights of the values.", {"(U)Int*", "Float*"}}
    };
    FunctionDocumentation::Parameters parameters = {};
    FunctionDocumentation::ReturnedValue returned_value = {"Returns `NaN` if all the weights are equal to 0 or the supplied weights parameter is empty, or the weighted mean otherwise.", {"Float64"}};
    FunctionDocumentation::Examples examples = {
    {
        "Usage example",
        R"(
SELECT avgWeighted(x, w)
FROM VALUES('x Int8, w Int8', (4, 1), (1, 0), (10, 2))
        )",
        R"(
┌─avgWeighted(x, w)─┐
│                 8 │
└───────────────────┘
        )"
    },
    {
        "Mixed integer and float weights",
        R"(
SELECT avgWeighted(x, w)
FROM VALUES('x Int8, w Float64', (4, 1), (1, 0), (10, 2))
        )",
        R"(
┌─avgWeighted(x, w)─┐
│                 8 │
└───────────────────┘
        )"
    },
    {
        "All weights are zero returns NaN",
        R"(
SELECT avgWeighted(x, w)
FROM VALUES('x Int8, w Int8', (0, 0), (1, 0), (10, 0))
        )",
        R"(
┌─avgWeighted(x, w)─┐
│               nan │
└───────────────────┘
        )"
    },
    {
        "Empty table returns NaN",
        R"(
CREATE TABLE test (t UInt8) ENGINE = Memory;
SELECT avgWeighted(t, t) FROM test
        )",
        R"(
┌─avgWeighted(t, t)─┐
│               nan │
└───────────────────┘
        )"
    }
    };
    FunctionDocumentation::Category category = FunctionDocumentation::Category::AggregateFunction;
    FunctionDocumentation::IntroducedIn introduced_in = {20, 1};
    FunctionDocumentation documentation = {description, syntax, arguments, parameters, returned_value, examples, introduced_in, category};
    factory.registerFunction("avgWeighted", {createAggregateFunctionAvgWeighted, AggregateFunctionProperties{}, documentation });
}

}
