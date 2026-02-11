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

template <bool is_exact, template <typename, bool> typename Data, template <bool, bool, bool> typename DataForVariadic, bool is_able_to_parallelize_merge>
AggregateFunctionPtr
createAggregateFunctionUniq(const std::string & name, const DataTypes & argument_types, const Array & params, const Settings *)
{
    assertNoParameters(name, params);

    if (argument_types.empty())
        throw Exception(ErrorCodes::NUMBER_OF_ARGUMENTS_DOESNT_MATCH,
            "Incorrect number of arguments for aggregate function {}", name);

    bool use_exact_hash_function = is_exact || !isAllArgumentsContiguousInMemory(argument_types);

    if (argument_types.size() == 1)
    {
        const IDataType & argument_type = *argument_types[0];

        AggregateFunctionPtr res(createWithNumericType<AggregateFunctionUniq, Data, is_able_to_parallelize_merge>(*argument_types[0], argument_types));

        WhichDataType which(argument_type);
        if (res)
            return res;
        if (which.isDate())
            return std::make_shared<
                AggregateFunctionUniq<DataTypeDate::FieldType, Data<DataTypeDate::FieldType, is_able_to_parallelize_merge>>>(
                argument_types);
        if (which.isDate32())
            return std::make_shared<
                AggregateFunctionUniq<DataTypeDate32::FieldType, Data<DataTypeDate32::FieldType, is_able_to_parallelize_merge>>>(
                argument_types);
        if (which.isDateTime())
            return std::make_shared<
                AggregateFunctionUniq<DataTypeDateTime::FieldType, Data<DataTypeDateTime::FieldType, is_able_to_parallelize_merge>>>(
                argument_types);
        if (which.isStringOrFixedString())
            return std::make_shared<AggregateFunctionUniq<String, Data<String, is_able_to_parallelize_merge>>>(argument_types);
        if (which.isUUID())
            return std::make_shared<
                AggregateFunctionUniq<DataTypeUUID::FieldType, Data<DataTypeUUID::FieldType, is_able_to_parallelize_merge>>>(
                argument_types);
        if (which.isIPv4())
            return std::make_shared<
                AggregateFunctionUniq<DataTypeIPv4::FieldType, Data<DataTypeIPv4::FieldType, is_able_to_parallelize_merge>>>(
                argument_types);
        if (which.isIPv6())
            return std::make_shared<
                AggregateFunctionUniq<DataTypeIPv6::FieldType, Data<DataTypeIPv6::FieldType, is_able_to_parallelize_merge>>>(
                argument_types);
        if (which.isTuple())
        {
            if (use_exact_hash_function)
                return std::make_shared<AggregateFunctionUniqVariadic<DataForVariadic<true, true, is_able_to_parallelize_merge>>>(
                    argument_types);
            return std::make_shared<AggregateFunctionUniqVariadic<DataForVariadic<false, true, is_able_to_parallelize_merge>>>(
                argument_types);
        }
    }

    if (use_exact_hash_function)
        return std::make_shared<AggregateFunctionUniqVariadic<DataForVariadic<true, false, is_able_to_parallelize_merge>>>(argument_types);
    return std::make_shared<AggregateFunctionUniqVariadic<DataForVariadic<false, false, is_able_to_parallelize_merge>>>(argument_types);
}

}

void registerAggregateFunctionUniqHLL12(AggregateFunctionFactory & factory)
{
    AggregateFunctionProperties properties = { .returns_default_when_only_null = true, .is_order_dependent = false };

    FunctionDocumentation::Description description = R"(
Calculates the approximate number of different argument values, using the [HyperLogLog](https://en.wikipedia.org/wiki/HyperLogLog) algorithm.

:::warning
We do not recommend using this function. In most cases, use the [uniq](https://clickhouse.com/docs/sql-reference/aggregate-functions/reference/uniq) or [uniqCombined](https://clickhouse.com/docs/sql-reference/aggregate-functions/reference/uniqcombined) function.
:::

<details>
<summary>Implementation details</summary>
This function calculates a hash for all parameters in the aggregate, then uses it in calculations.
It uses the HyperLogLog algorithm to approximate the number of different argument values.

2^12 5-bit cells are used.
The size of the state is slightly more than 2.5 KB.
The result is not very accurate (up to ~10% error) for small data sets (<10K elements).
However, the result is fairly accurate for high-cardinality data sets (10K-100M), with a maximum error of ~1.6%.
Starting from 100M, the estimation error increases, and the function will return very inaccurate results for data sets with extremely high cardinality (1B+ elements).

Provides a determinate result (it does not depend on the query processing order).
</details>
    )";
    FunctionDocumentation::Syntax syntax = R"(
uniqHLL12(x[, ...])
    )";
    FunctionDocumentation::Arguments arguments = {
        {"x", "The function takes a variable number of parameters.", {"Tuple(T)", "Array(T)", "Date", "DateTime", "String", "(U)Int*", "Float*", "Decimal"}}
    };
    FunctionDocumentation::ReturnedValue returned_value = {"Returns a UInt64-type number representing the approximate number of different argument values.", {"UInt64"}};
    FunctionDocumentation::Examples examples = {
    {
        "Basic usage",
        R"(
CREATE TABLE example_hll
(
    id UInt32,
    category String
)
ENGINE = Memory;

INSERT INTO example_hll VALUES
(1, 'A'), (2, 'B'), (3, 'A'), (4, 'C'), (5, 'B'), (6, 'A');

SELECT uniqHLL12(category) AS hll_unique_categories
FROM example_hll;
        )",
        R"(
┌─hll_unique_categories─┐
│                     3 │
└───────────────────────┘
        )"
    }
    };
    FunctionDocumentation::IntroducedIn introduced_in = {1, 1};
    FunctionDocumentation::Category category = FunctionDocumentation::Category::AggregateFunction;
    FunctionDocumentation documentation = {description, syntax, arguments, {}, returned_value, examples, introduced_in, category};

    factory.registerFunction("uniqHLL12",
        {createAggregateFunctionUniq<false, AggregateFunctionUniqHLL12Data, AggregateFunctionUniqHLL12DataForVariadic, false /* is_able_to_parallelize_merge */>, properties, documentation});
}

}
