#include <AggregateFunctions/AggregateFunctionSparkbar.h>


namespace DB
{

struct Settings;

namespace ErrorCodes
{
    extern const int NUMBER_OF_ARGUMENTS_DOESNT_MATCH;
    extern const int ILLEGAL_TYPE_OF_ARGUMENT;
    extern const int BAD_ARGUMENTS;
}

AggregateFunctionPtr createSparkbarHalf2(const std::string & name, const IDataType & x_type, const IDataType & y_type, const DataTypes & args, const Array & params);

namespace
{

AggregateFunctionPtr createAggregateFunctionSparkbarImpl1(const std::string & name, const IDataType & x_type, const IDataType & y_type, const DataTypes & args, const Array & params)
{
    WhichDataType which(y_type);
    if (which.idx == TypeIndex::UInt8) return AggregateFunctionPtr(createWithUIntegerOrTimeType<AggregateFunctionSparkbar, UInt8>(name, x_type, args, params));
    if (which.idx == TypeIndex::UInt16) return AggregateFunctionPtr(createWithUIntegerOrTimeType<AggregateFunctionSparkbar, UInt16>(name, x_type, args, params));
    if (which.idx == TypeIndex::UInt32) return AggregateFunctionPtr(createWithUIntegerOrTimeType<AggregateFunctionSparkbar, UInt32>(name, x_type, args, params));
    if (which.idx == TypeIndex::UInt64) return AggregateFunctionPtr(createWithUIntegerOrTimeType<AggregateFunctionSparkbar, UInt64>(name, x_type, args, params));
    if (which.idx == TypeIndex::UInt128) return AggregateFunctionPtr(createWithUIntegerOrTimeType<AggregateFunctionSparkbar, UInt128>(name, x_type, args, params));
    if (which.idx == TypeIndex::UInt256) return AggregateFunctionPtr(createWithUIntegerOrTimeType<AggregateFunctionSparkbar, UInt256>(name, x_type, args, params));
    if (which.idx == TypeIndex::Int8) return AggregateFunctionPtr(createWithUIntegerOrTimeType<AggregateFunctionSparkbar, Int8>(name, x_type, args, params));
    if (which.idx == TypeIndex::Int16) return AggregateFunctionPtr(createWithUIntegerOrTimeType<AggregateFunctionSparkbar, Int16>(name, x_type, args, params));
    return nullptr;
}

AggregateFunctionPtr createAggregateFunctionSparkbar(const std::string & name, const DataTypes & arguments, const Array & params, const Settings *)
{
    assertBinary(name, arguments);

    if (params.size() != 1 && params.size() != 3)
        throw Exception(ErrorCodes::NUMBER_OF_ARGUMENTS_DOESNT_MATCH,
            "The number of params does not match for aggregate function '{}', expected 1 or 3, got {}", name, params.size());

    if (params.size() == 3)
    {
        if (params.at(1).getType() != arguments[0]->getDefault().getType() ||
            params.at(2).getType() != arguments[0]->getDefault().getType())
        {
            throw Exception(ErrorCodes::ILLEGAL_TYPE_OF_ARGUMENT,
                            "The second and third parameters are not the same type as the first arguments for aggregate function {}", name);
        }
    }

    auto res = createAggregateFunctionSparkbarImpl1(name, *arguments[0], *arguments[1], arguments, params);
    if (!res)
        res = createSparkbarHalf2(name, *arguments[0], *arguments[1], arguments, params);
    if (!res)
        throw Exception(ErrorCodes::ILLEGAL_TYPE_OF_ARGUMENT, "The second argument type must be numeric for aggregate function {}", name);
    return res;
}

}

void registerAggregateFunctionSparkbar(AggregateFunctionFactory & factory)
{

    FunctionDocumentation::Description description_sparkbar = R"(
The function plots a frequency histogram for values `x` and the repetition rate `y` of these values over the interval `[min_x, max_x]`.
Repetitions for all `x` falling into the same bucket are averaged, so data should be pre-aggregated.
Negative repetitions are ignored.

If no interval is specified, then the minimum `x` is used as the interval start, and the maximum `x` — as the interval end.
Otherwise, values outside the interval are ignored.
    )";
    FunctionDocumentation::Syntax syntax_sparkbar = R"(
sparkbar(buckets[, min_x, max_x])(x, y)
    )";
    FunctionDocumentation::Parameters parameters_sparkbar = {
        {"buckets", "The number of segments.", {"(U)Int*"}},
        {"min_x", "Optional. The interval start.", {"(U)Int*", "Float*", "Decimal"}},
        {"max_x", "Optional. The interval end.", {"(U)Int*", "Float*", "Decimal"}}
    };
    FunctionDocumentation::Arguments arguments_sparkbar = {
        {"x", "The field with values.", {"const String"}},
        {"y", "The field with the frequency of values.", {"const String"}}
    };
    FunctionDocumentation::ReturnedValue returned_value_sparkbar = {"Returns the frequency histogram.", {"String"}};
    FunctionDocumentation::Examples examples_sparkbar = {
    {
        "Without interval specification",
        R"(
CREATE TABLE spark_bar_data (`value` Int64, `event_date` Date) ENGINE = MergeTree ORDER BY event_date;

INSERT INTO spark_bar_data VALUES (1,'2020-01-01'), (3,'2020-01-02'), (4,'2020-01-02'), (-3,'2020-01-02'), (5,'2020-01-03'), (2,'2020-01-04'), (3,'2020-01-05'), (7,'2020-01-06'), (6,'2020-01-07'), (8,'2020-01-08'), (2,'2020-01-11');

SELECT sparkbar(9)(event_date, cnt) FROM (SELECT sum(value) AS cnt, event_date FROM spark_bar_data GROUP BY event_date);
        )",
        R"(
┌─sparkbar(9)(event_date, cnt)─┐
│ ▂▅▂▃▆█  ▂                    │
└──────────────────────────────┘
        )"
    },
    {
        "With interval specification",
        R"(
SELECT sparkbar(9, toDate('2020-01-01'), toDate('2020-01-10'))(event_date, cnt) FROM (SELECT sum(value) AS cnt, event_date FROM spark_bar_data GROUP BY event_date);
        )",
        R"(
┌─sparkbar(9, toDate('2020-01-01'), toDate('2020-01-10'))(event_date, cnt)─┐
│ ▂▅▂▃▇▆█                                                                  │
└──────────────────────────────────────────────────────────────────────────┘
        )"
    }
    };
    FunctionDocumentation::IntroducedIn introduced_in_sparkbar = {21, 11};
    FunctionDocumentation::Category category_sparkbar = FunctionDocumentation::Category::AggregateFunction;
    FunctionDocumentation documentation_sparkbar = {description_sparkbar, syntax_sparkbar, arguments_sparkbar, parameters_sparkbar, returned_value_sparkbar, examples_sparkbar, introduced_in_sparkbar, category_sparkbar};

    factory.registerFunction("sparkbar", {createAggregateFunctionSparkbar, {}, documentation_sparkbar});
    factory.registerAlias("sparkBar", "sparkbar");
}

}
