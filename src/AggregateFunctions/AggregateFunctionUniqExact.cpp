#include <AggregateFunctions/AggregateFunctionFactory.h>
#include <AggregateFunctions/AggregateFunctionUniq.h>
#include <AggregateFunctions/Helpers.h>
#include <AggregateFunctions/FactoryHelpers.h>

#include <DataTypes/DataTypeDate.h>
#include <DataTypes/DataTypeDate32.h>
#include <DataTypes/DataTypeDateTime.h>
#include <DataTypes/DataTypeUUID.h>
#include <DataTypes/DataTypeIPv4andIPv6.h>

#include <Core/Settings.h>


namespace DB
{

namespace Setting
{
    extern const SettingsMaxThreads max_threads;
}

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

void registerAggregateFunctionUniqExact(AggregateFunctionFactory & factory)
{
    AggregateFunctionProperties properties = { .returns_default_when_only_null = true, .is_order_dependent = false };

    auto assign_bool_param = [](const std::string & name, const DataTypes & argument_types, const Array & params, const Settings * settings)
    {
        /// Using two level hash set if we wouldn't be able to merge in parallel can cause ~10% slowdown.
        if (settings && (*settings)[Setting::max_threads] > 1)
            return createAggregateFunctionUniq<
                true, AggregateFunctionUniqExactData, AggregateFunctionUniqExactDataForVariadic, true /* is_able_to_parallelize_merge */>(name, argument_types, params, settings);
        return createAggregateFunctionUniq<
            true,
            AggregateFunctionUniqExactData,
            AggregateFunctionUniqExactDataForVariadic,
            false /* is_able_to_parallelize_merge */>(name, argument_types, params, settings);
    };

    FunctionDocumentation::Description description = R"(
Calculates the exact number of different argument values.

:::warning
The `uniqExact` function uses more memory than `uniq`, because the size of the state has unbounded growth as the number of different values increases.
Use the `uniqExact` function if you absolutely need an exact result.
Otherwise use the [`uniq`](https://clickhouse.com/docs/sql-reference/aggregate-functions/reference/uniq) function.
:::
    )";
    FunctionDocumentation::Syntax syntax = R"(
uniqExact(x[, ...])
    )";
    FunctionDocumentation::Arguments arguments = {
        {"x", "The function takes a variable number of parameters.", {"Tuple(T)", "Array(T)", "Date", "DateTime", "String", "(U)Int*", "Float*", "Decimal"}}
    };
    FunctionDocumentation::ReturnedValue returned_value = {"Returns the exact number of different argument values as a UInt64.", {"UInt64"}};
    FunctionDocumentation::Examples examples = {
    {
        "Basic usage",
        R"(
CREATE TABLE example_data
(
    id UInt32,
    category String
)
ENGINE = Memory;

INSERT INTO example_data VALUES
(1, 'A'), (2, 'B'), (3, 'A'), (4, 'C'), (5, 'B'), (6, 'A');

SELECT uniqExact(category) as exact_unique_categories
FROM example_data;
        )",
        R"(
┌─exact_unique_categories─┐
│                       3 │
└─────────────────────────┘
        )"
    },
    {
        "Multiple arguments",
        R"(
SELECT uniqExact(id, category) as exact_unique_combinations
FROM example_data;
        )",
        R"(
┌─exact_unique_combinations─┐
│                         6 │
└───────────────────────────┘
        )"
    }
    };
    FunctionDocumentation::IntroducedIn introduced_in = {1, 1};
    FunctionDocumentation::Category category = FunctionDocumentation::Category::AggregateFunction;
    FunctionDocumentation documentation = {description, syntax, arguments, {}, returned_value, examples, introduced_in, category};

    factory.registerFunction("uniqExact", {assign_bool_param, properties, documentation});
}

}
