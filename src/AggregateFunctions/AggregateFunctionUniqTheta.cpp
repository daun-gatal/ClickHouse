#include <AggregateFunctions/AggregateFunctionFactory.h>
#include <AggregateFunctions/AggregateFunctionUniq.h>
#include <AggregateFunctions/Helpers.h>
#include <AggregateFunctions/FactoryHelpers.h>

#include <DataTypes/DataTypeDate.h>
#include <DataTypes/DataTypeDate32.h>
#include <DataTypes/DataTypeDateTime.h>
#include <DataTypes/DataTypeUUID.h>
#include <DataTypes/DataTypeIPv4andIPv6.h>


namespace DB
{

namespace ErrorCodes
{
    extern const int NUMBER_OF_ARGUMENTS_DOESNT_MATCH;
}

namespace
{

#if USE_DATASKETCHES

template <typename Data, template <bool, bool> typename DataForVariadic>
AggregateFunctionPtr
createAggregateFunctionUniq(const std::string & name, const DataTypes & argument_types, const Array & params, const Settings *)
{
    assertNoParameters(name, params);

    if (argument_types.empty())
        throw Exception(ErrorCodes::NUMBER_OF_ARGUMENTS_DOESNT_MATCH,
            "Incorrect number of arguments for aggregate function {}", name);

    bool use_exact_hash_function = !isAllArgumentsContiguousInMemory(argument_types);

    if (argument_types.size() == 1)
    {
        const IDataType & argument_type = *argument_types[0];

        AggregateFunctionPtr res(createWithNumericType<AggregateFunctionUniq, Data>(*argument_types[0], argument_types));

        WhichDataType which(argument_type);
        if (res)
            return res;
        if (which.isDate())
            return std::make_shared<AggregateFunctionUniq<DataTypeDate::FieldType, Data>>(argument_types);
        if (which.isDate32())
            return std::make_shared<AggregateFunctionUniq<DataTypeDate32::FieldType, Data>>(argument_types);
        if (which.isDateTime())
            return std::make_shared<AggregateFunctionUniq<DataTypeDateTime::FieldType, Data>>(argument_types);
        if (which.isStringOrFixedString())
            return std::make_shared<AggregateFunctionUniq<String, Data>>(argument_types);
        if (which.isUUID())
            return std::make_shared<AggregateFunctionUniq<DataTypeUUID::FieldType, Data>>(argument_types);
        if (which.isIPv4())
            return std::make_shared<AggregateFunctionUniq<DataTypeIPv4::FieldType, Data>>(argument_types);
        if (which.isIPv6())
            return std::make_shared<AggregateFunctionUniq<DataTypeIPv6::FieldType, Data>>(argument_types);
        if (which.isTuple())
        {
            if (use_exact_hash_function)
                return std::make_shared<AggregateFunctionUniqVariadic<DataForVariadic<true, true>>>(argument_types);
            return std::make_shared<AggregateFunctionUniqVariadic<DataForVariadic<false, true>>>(argument_types);
        }
    }

    /// "Variadic" method also works as a fallback generic case for single argument.
    if (use_exact_hash_function)
        return std::make_shared<AggregateFunctionUniqVariadic<DataForVariadic<true, false>>>(argument_types);
    return std::make_shared<AggregateFunctionUniqVariadic<DataForVariadic<false, false>>>(argument_types);
}

#endif

}

void registerAggregateFunctionUniqTheta(AggregateFunctionFactory & factory)
{
#if USE_DATASKETCHES
    AggregateFunctionProperties properties = { .returns_default_when_only_null = true, .is_order_dependent = false };

    FunctionDocumentation::Description description = R"(
Calculates the approximate number of different argument values, using the [Theta Sketch Framework](https://datasketches.apache.org/docs/Theta/ThetaSketches.html#theta-sketch-framework).

<details>
<summary>Implementation details</summary>

This function calculates a hash for all parameters in the aggregate, then uses it in calculations.
It uses the [KMV](https://datasketches.apache.org/docs/Theta/InverseEstimate.html) algorithm to approximate the number of different argument values.

4096(2^12) 64-bit sketch are used.
The size of the state is about 41 KB.

The relative error is 3.125% (95% confidence), see the [relative error table](https://datasketches.apache.org/docs/Theta/ThetaErrorTable.html) for detail.
</details>
    )";
    FunctionDocumentation::Syntax syntax = R"(
uniqTheta(x[, ...])
    )";
    FunctionDocumentation::Arguments arguments = {
        {"x", "The function takes a variable number of parameters.", {"Tuple(T)", "Array(T)", "Date", "DateTime", "String", "(U)Int*", "Float*", "Decimal"}}
    };
    FunctionDocumentation::ReturnedValue returned_value = {"Returns a UInt64-type number representing the approximate number of different argument values.", {"UInt64"}};
    FunctionDocumentation::Examples examples = {
    {
        "Basic usage",
        R"(
CREATE TABLE example_theta
(
    id UInt32,
    category String
)
ENGINE = Memory;

INSERT INTO example_theta VALUES
(1, 'A'), (2, 'B'), (3, 'A'), (4, 'C'), (5, 'B'), (6, 'A');

SELECT uniqTheta(category) as theta_unique_categories
FROM example_theta;
        )",
        R"(
┌─theta_unique_categories─┐
│                       3 │
└─────────────────────────┘
        )"
    }
    };
    FunctionDocumentation::IntroducedIn introduced_in = {21, 6};
    FunctionDocumentation::Category category = FunctionDocumentation::Category::AggregateFunction;
    FunctionDocumentation documentation = {description, syntax, arguments, {}, returned_value, examples, introduced_in, category};

    factory.registerFunction("uniqTheta",
        {createAggregateFunctionUniq<AggregateFunctionUniqThetaData, AggregateFunctionUniqThetaDataForVariadic>, properties, documentation});
#else
    UNUSED(factory);
#endif
}

}
