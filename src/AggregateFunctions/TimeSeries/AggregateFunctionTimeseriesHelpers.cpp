#include <AggregateFunctions/TimeSeries/AggregateFunctionTimeseriesCreate.h>
#include <AggregateFunctions/TimeSeries/AggregateFunctionTimeseriesInstantValue.h>
#include <AggregateFunctions/TimeSeries/AggregateFunctionTimeseriesExtrapolatedValue.h>

namespace DB
{

using namespace detail;

void registerAggregateFunctionTimeseries(AggregateFunctionFactory & factory)
{
    /// timeSeriesRateToGrid documentation
    FunctionDocumentation::Description description_timeSeriesRateToGrid = R"(
Aggregate function that takes time series data as pairs of timestamps and values and calculates [PromQL-like rate](https://prometheus.io/docs/prometheus/latest/querying/functions/#rate) from this data on a regular time grid described by start timestamp, end timestamp and step. For each point on the grid the samples for calculating `rate` are considered within the specified time window.

:::warning
This function is experimental, enable it by setting `allow_experimental_ts_to_grid_aggregate_function=true`.
:::
    )";
    FunctionDocumentation::Syntax syntax_timeSeriesRateToGrid = R"(
timeSeriesRateToGrid(start_timestamp, end_timestamp, grid_step, staleness)(timestamp, value)
    )";
    FunctionDocumentation::Parameters parameters_timeSeriesRateToGrid = {
        {"start_timestamp", "Specifies start of the grid.", {"UInt32", "DateTime"}},
        {"end_timestamp", "Specifies end of the grid.", {"UInt32", "DateTime"}},
        {"grid_step", "Specifies step of the grid in seconds.", {"UInt32"}},
        {"staleness", "Specifies the maximum staleness in seconds of the considered samples. The staleness window is a left-open and right-closed interval.", {"UInt32"}}
    };
    FunctionDocumentation::Arguments arguments_timeSeriesRateToGrid = {
        {"timestamp", "Timestamp of the sample. Can be individual values or arrays.", {"UInt32", "DateTime", "Array(UInt32)", "Array(DateTime)"}},
        {"value", "Value of the time series corresponding to the timestamp. Can be individual values or arrays.", {"Float*", "Array(Float*)"}}
    };
    FunctionDocumentation::ReturnedValue returned_value_timeSeriesRateToGrid = {"Returns rate values on the specified grid. The returned array contains one value for each time grid point. The value is NULL if there are not enough samples within the window to calculate the rate value for a particular grid point.", {"Array(Nullable(Float64))"}};
    FunctionDocumentation::Examples examples_timeSeriesRateToGrid = {
    {
        "Basic usage with individual timestamp-value pairs",
        R"(
WITH
    -- NOTE: the gap between 140 and 190 is to show how values are filled for ts = 150, 165, 180 according to window parameter
    [110, 120, 130, 140, 190, 200, 210, 220, 230]::Array(DateTime) AS timestamps,
    [1, 1, 3, 4, 5, 5, 8, 12, 13]::Array(Float32) AS values, -- array of values corresponding to timestamps above
    90 AS start_ts,       -- start of timestamp grid
    90 + 120 AS end_ts,   -- end of timestamp grid
    15 AS step_seconds,   -- step of timestamp grid
    45 AS window_seconds  -- "staleness" window
SELECT timeSeriesRateToGrid(start_ts, end_ts, step_seconds, window_seconds)(timestamp, value)
FROM
(
    -- This subquery converts arrays of timestamps and values into rows of `timestamp`, `value`
    SELECT
        arrayJoin(arrayZip(timestamps, values)) AS ts_and_val,
        ts_and_val.1 AS timestamp,
        ts_and_val.2 AS value
);
        )",
        R"(
┌─timeSeriesRateToGrid(start_ts, end_ts, step_seconds, window_seconds)(timestamp, value)─┐
│ [NULL,NULL,0,0.06666667,0.1,0.083333336,NULL,NULL,0.083333336]                        │
└───────────────────────────────────────────────────────────────────────────────────────┘
        )"
    },
    {
        "Using array arguments",
        R"(
WITH
    [110, 120, 130, 140, 190, 200, 210, 220, 230]::Array(DateTime) AS timestamps,
    [1, 1, 3, 4, 5, 5, 8, 12, 13]::Array(Float32) AS values,
    90 AS start_ts,
    90 + 120 AS end_ts,
    15 AS step_seconds,
    45 AS window_seconds
SELECT timeSeriesRateToGrid(start_ts, end_ts, step_seconds, window_seconds)(timestamps, values);
        )",
        R"(
┌─timeSeriesRateToGrid(start_ts, end_ts, step_seconds, window_seconds)(timestamps, values)─┐
│ [NULL,NULL,0,0.06666667,0.1,0.083333336,NULL,NULL,0.083333336]                          │
└─────────────────────────────────────────────────────────────────────────────────────────┘
        )"
    }
    };
    FunctionDocumentation::IntroducedIn introduced_in_timeSeriesRateToGrid = {25, 6};
    FunctionDocumentation::Category category_timeSeriesRateToGrid = FunctionDocumentation::Category::AggregateFunction;
    FunctionDocumentation documentation_timeSeriesRateToGrid = {description_timeSeriesRateToGrid, syntax_timeSeriesRateToGrid, arguments_timeSeriesRateToGrid, parameters_timeSeriesRateToGrid, returned_value_timeSeriesRateToGrid, examples_timeSeriesRateToGrid, introduced_in_timeSeriesRateToGrid, category_timeSeriesRateToGrid};

    factory.registerFunction("timeSeriesRateToGrid",
        {createAggregateFunctionTimeseries<true, false, AggregateFunctionTimeseriesExtrapolatedValueTraits, AggregateFunctionTimeseriesExtrapolatedValue>, {}, documentation_timeSeriesRateToGrid});

    /// timeSeriesDeltaToGrid documentation
    FunctionDocumentation::Description description_timeSeriesDeltaToGrid = R"(
Aggregate function that takes time series data as pairs of timestamps and values and calculates [PromQL-like delta](https://prometheus.io/docs/prometheus/latest/querying/functions/#delta) from this data on a regular time grid described by start timestamp, end timestamp and step.
For each point on the grid the samples for calculating `delta` are considered within the specified time window.

:::warning
This function is experimental, enable it by setting `allow_experimental_ts_to_grid_aggregate_function=true`.
:::
    )";
    FunctionDocumentation::Syntax syntax_timeSeriesDeltaToGrid = R"(
timeSeriesDeltaToGrid(start_timestamp, end_timestamp, grid_step, staleness)(timestamp, value)
    )";
    FunctionDocumentation::Parameters parameters_timeSeriesDeltaToGrid = {
        {"start_timestamp", "Specifies start of the grid.", {"UInt32", "DateTime"}},
        {"end_timestamp", "Specifies end of the grid.", {"UInt32", "DateTime"}},
        {"grid_step", "Specifies step of the grid in seconds.", {"UInt32"}},
        {"staleness", "Specifies the maximum staleness in seconds of the considered samples. The staleness window is a left-open and right-closed interval.", {"UInt32"}}
    };
    FunctionDocumentation::Arguments arguments_timeSeriesDeltaToGrid = {
        {"timestamp", "Timestamp of the sample. Can be individual values or arrays.", {"UInt32", "DateTime", "Array(UInt32)", "Array(DateTime)"}},
        {"value", "Value of the time series corresponding to the timestamp. Can be individual values or arrays.", {"Float*", "Array(Float*)"}}
    };
    FunctionDocumentation::ReturnedValue returned_value_timeSeriesDeltaToGrid = {"Returns delta values on the specified grid. The returned array contains one value for each time grid point. The value is NULL if there are not enough samples within the window to calculate the delta value for a particular grid point.", {"Array(Nullable(Float64))"}};
    FunctionDocumentation::Examples examples_timeSeriesDeltaToGrid = {
    {
        "Basic usage with individual timestamp-value pairs",
        R"(
WITH
    -- NOTE: the gap between 140 and 190 is to show how values are filled for ts = 150, 165, 180 according to window parameter
    [110, 120, 130, 140, 190, 200, 210, 220, 230]::Array(DateTime) AS timestamps,
    [1, 1, 3, 4, 5, 5, 8, 12, 13]::Array(Float32) AS values, -- array of values corresponding to timestamps above
    90 AS start_ts,      -- start of timestamp grid
    90 + 120 AS end_ts,  -- end of timestamp grid
    15 AS step_seconds,  -- step of timestamp grid
    45 AS window_seconds -- "staleness" window
SELECT timeSeriesDeltaToGrid(start_ts, end_ts, step_seconds, window_seconds)(timestamp, value)
FROM
(
    -- This subquery converts arrays of timestamps and values into rows of `timestamp`, `value`
    SELECT
        arrayJoin(arrayZip(timestamps, values)) AS ts_and_val,
        ts_and_val.1 AS timestamp,
        ts_and_val.2 AS value
);
        )",
        R"(
┌─timeSeriesDeltaToGrid(start_ts, end_ts, step_seconds, window_seconds)(timestamp, value)─┐
│ [NULL,NULL,0,3,4.5,3.75,NULL,NULL,3.75]                                               │
└───────────────────────────────────────────────────────────────────────────────────────┘
        )"
    },
    {
        "Using array arguments",
        R"(
-- it is possible to pass multiple samples of timestamps and values as Arrays of equal size
WITH
    [110, 120, 130, 140, 190, 200, 210, 220, 230]::Array(DateTime) AS timestamps,
    [1, 1, 3, 4, 5, 5, 8, 12, 13]::Array(Float32) AS values,
    90 AS start_ts,
    90 + 120 AS end_ts,
    15 AS step_seconds,
    45 AS window_seconds
SELECT timeSeriesDeltaToGrid(start_ts, end_ts, step_seconds, window_seconds)(timestamps, values);
        )",
        R"(
┌─timeSeriesDeltaToGrid(start_ts, end_ts, step_seconds, window_seconds)(timestamps, values)─┐
│ [NULL,NULL,0,3,4.5,3.75,NULL,NULL,3.75]                                                 │
└─────────────────────────────────────────────────────────────────────────────────────────┘
        )"
    }
    };
    FunctionDocumentation::IntroducedIn introduced_in_timeSeriesDeltaToGrid = {25, 6};
    FunctionDocumentation::Category category_timeSeriesDeltaToGrid = FunctionDocumentation::Category::AggregateFunction;
    FunctionDocumentation documentation_timeSeriesDeltaToGrid = {description_timeSeriesDeltaToGrid, syntax_timeSeriesDeltaToGrid, arguments_timeSeriesDeltaToGrid, parameters_timeSeriesDeltaToGrid, returned_value_timeSeriesDeltaToGrid, examples_timeSeriesDeltaToGrid, introduced_in_timeSeriesDeltaToGrid, category_timeSeriesDeltaToGrid};

    factory.registerFunction("timeSeriesDeltaToGrid",
        {createAggregateFunctionTimeseries<false, false, AggregateFunctionTimeseriesExtrapolatedValueTraits, AggregateFunctionTimeseriesExtrapolatedValue>, {}, documentation_timeSeriesDeltaToGrid});

    /// timeSeriesInstantRateToGrid documentation
    FunctionDocumentation::Description description_timeSeriesInstantRateToGrid = R"(
Aggregate function that takes time series data as pairs of timestamps and values and calculates [PromQL-like irate](https://prometheus.io/docs/prometheus/latest/querying/functions/#irate) from this data on a regular time grid described by start timestamp, end timestamp and step. For each point on the grid the samples for calculating `irate` are considered within the specified time window.

:::warning
This function is experimental, enable it by setting `allow_experimental_ts_to_grid_aggregate_function=true`.
:::
    )";
    FunctionDocumentation::Syntax syntax_timeSeriesInstantRateToGrid = R"(
timeSeriesInstantRateToGrid(start_timestamp, end_timestamp, grid_step, staleness)(timestamp, value)
    )";
    FunctionDocumentation::Parameters parameters_timeSeriesInstantRateToGrid = {
        {"start_timestamp", "Specifies start of the grid.", {"UInt32", "DateTime"}},
        {"end_timestamp", "Specifies end of the grid.", {"UInt32", "DateTime"}},
        {"grid_step", "Specifies step of the grid in seconds.", {"UInt32"}},
        {"staleness", "Specifies the maximum staleness in seconds of the considered samples. The staleness window is a left-open and right-closed interval.", {"UInt32"}}
    };
    FunctionDocumentation::Arguments arguments_timeSeriesInstantRateToGrid = {
        {"timestamp", "Timestamp of the sample. Can be individual values or arrays.", {"UInt32", "DateTime", "Array(UInt32)", "Array(DateTime)"}},
        {"value", "Value of the time series corresponding to the timestamp. Can be individual values or arrays.", {"Float*", "Array(Float*)"}}
    };
    FunctionDocumentation::ReturnedValue returned_value_timeSeriesInstantRateToGrid = {"Returns irate values on the specified grid. The returned array contains one value for each time grid point. The value is NULL if there are not enough samples within the window to calculate the instant rate value for a particular grid point.", {"Array(Nullable(Float64))"}};
    FunctionDocumentation::Examples examples_timeSeriesInstantRateToGrid = {
    {
        "Basic usage with individual timestamp-value pairs",
        R"(
WITH
    -- NOTE: the gap between 140 and 190 is to show how values are filled for ts = 150, 165, 180 according to window parameter
    [110, 120, 130, 140, 190, 200, 210, 220, 230]::Array(DateTime) AS timestamps,
    [1, 1, 3, 4, 5, 5, 8, 12, 13]::Array(Float32) AS values, -- array of values corresponding to timestamps above
    90 AS start_ts,       -- start of timestamp grid
    90 + 120 AS end_ts,   -- end of timestamp grid
    15 AS step_seconds,   -- step of timestamp grid
    45 AS window_seconds  -- "staleness" window
SELECT timeSeriesInstantRateToGrid(start_ts, end_ts, step_seconds, window_seconds)(timestamp, value)
FROM
(
    -- This subquery converts arrays of timestamps and values into rows of `timestamp`, `value`
    SELECT
        arrayJoin(arrayZip(timestamps, values)) AS ts_and_val,
        ts_and_val.1 AS timestamp,
        ts_and_val.2 AS value
);
        )",
        R"(
┌─timeSeriesInstantRateToGrid(start_ts, end_ts, step_seconds, window_seconds)(timestamp, value)─┐
│ [NULL,NULL,0,0.2,0.1,0.1,NULL,NULL,0.3]                                                      │
└──────────────────────────────────────────────────────────────────────────────────────────────┘
        )"
    },
    {
        "Using array arguments",
        R"(
-- it is possible to pass multiple samples of timestamps and values as Arrays of equal size
WITH
    [110, 120, 130, 140, 190, 200, 210, 220, 230]::Array(DateTime) AS timestamps,
    [1, 1, 3, 4, 5, 5, 8, 12, 13]::Array(Float32) AS values,
    90 AS start_ts,
    90 + 120 AS end_ts,
    15 AS step_seconds,
    45 AS window_seconds
SELECT timeSeriesInstantRateToGrid(start_ts, end_ts, step_seconds, window_seconds)(timestamps, values);
        )",
        R"(
┌─timeSeriesInstantRateToGrid(start_ts, end_ts, step_seconds, window_seconds)(timestamps, values)─┐
│ [NULL,NULL,0,0.2,0.1,0.1,NULL,NULL,0.3]                                                        │
└────────────────────────────────────────────────────────────────────────────────────────────────┘
        )"
    }
    };
    FunctionDocumentation::IntroducedIn introduced_in_timeSeriesInstantRateToGrid = {25, 6};
    FunctionDocumentation::Category category_timeSeriesInstantRateToGrid = FunctionDocumentation::Category::AggregateFunction;
    FunctionDocumentation documentation_timeSeriesInstantRateToGrid = {description_timeSeriesInstantRateToGrid, syntax_timeSeriesInstantRateToGrid, arguments_timeSeriesInstantRateToGrid, parameters_timeSeriesInstantRateToGrid, returned_value_timeSeriesInstantRateToGrid, examples_timeSeriesInstantRateToGrid, introduced_in_timeSeriesInstantRateToGrid, category_timeSeriesInstantRateToGrid};

    factory.registerFunction("timeSeriesInstantRateToGrid",
        {createAggregateFunctionTimeseries<true, false, AggregateFunctionTimeseriesInstantValueTraits, AggregateFunctionTimeseriesInstantValue>, {}, documentation_timeSeriesInstantRateToGrid});

    /// timeSeriesInstantDeltaToGrid documentation
    FunctionDocumentation::Description description_timeSeriesInstantDeltaToGrid = R"(
Aggregate function that takes time series data as pairs of timestamps and values and calculates [PromQL-like idelta](https://prometheus.io/docs/prometheus/latest/querying/functions/#idelta) from this data on a regular time grid described by start timestamp, end timestamp and step.
For each point on the grid the samples for calculating `idelta` are considered within the specified time window.

:::warning
This function is experimental, enable it by setting `allow_experimental_ts_to_grid_aggregate_function=true`.
:::
    )";
    FunctionDocumentation::Syntax syntax_timeSeriesInstantDeltaToGrid = R"(
timeSeriesInstantDeltaToGrid(start_timestamp, end_timestamp, grid_step, staleness)(timestamp, value)
    )";
    FunctionDocumentation::Parameters parameters_timeSeriesInstantDeltaToGrid = {
        {"start_timestamp", "Specifies start of the grid.", {"UInt32", "DateTime"}},
        {"end_timestamp", "Specifies end of the grid.", {"UInt32", "DateTime"}},
        {"grid_step", "Specifies step of the grid in seconds.", {"UInt32"}},
        {"staleness", "Specifies the maximum staleness in seconds of the considered samples. The staleness window is a left-open and right-closed interval.", {"UInt32"}}
    };
    FunctionDocumentation::Arguments arguments_timeSeriesInstantDeltaToGrid = {
        {"timestamp", "Timestamp of the sample. Can be individual values or arrays.", {"UInt32", "DateTime", "Array(UInt32)", "Array(DateTime)"}},
        {"value", "Value of the time series corresponding to the timestamp. Can be individual values or arrays.", {"Float*", "Array(Float*)"}}
    };
    FunctionDocumentation::ReturnedValue returned_value_timeSeriesInstantDeltaToGrid = {"Returns idelta values on the specified grid. The returned array contains one value for each time grid point. The value is NULL if there are not enough samples within the window to calculate the instant delta value for a particular grid point.", {"Array(Nullable(Float64))"}};
    FunctionDocumentation::Examples examples_timeSeriesInstantDeltaToGrid = {
    {
        "Basic usage with individual timestamp-value pairs",
        R"(
WITH
    -- NOTE: the gap between 140 and 190 is to show how values are filled for ts = 150, 165, 180 according to window parameter
    [110, 120, 130, 140, 190, 200, 210, 220, 230]::Array(DateTime) AS timestamps,
    [1, 1, 3, 4, 5, 5, 8, 12, 13]::Array(Float32) AS values, -- array of values corresponding to timestamps above
    90 AS start_ts,       -- start of timestamp grid
    90 + 120 AS end_ts,   -- end of timestamp grid
    15 AS step_seconds,   -- step of timestamp grid
    45 AS window_seconds  -- "staleness" window
SELECT timeSeriesInstantDeltaToGrid(start_ts, end_ts, step_seconds, window_seconds)(timestamp, value)
FROM
(
    -- This subquery converts arrays of timestamps and values into rows of `timestamp`, `value`
    SELECT
        arrayJoin(arrayZip(timestamps, values)) AS ts_and_val,
        ts_and_val.1 AS timestamp,
        ts_and_val.2 AS value
);
        )",
        R"(
┌─timeSeriesInstantDeltaToGrid(start_ts, end_ts, step_seconds, window_seconds)(timestamp, value)─┐
│ [NULL,NULL,0,2,1,1,NULL,NULL,3]                                                               │
└───────────────────────────────────────────────────────────────────────────────────────────────┘
        )"
    },
    {
        "Using array arguments",
        R"(
-- it is possible to pass multiple samples of timestamps and values as Arrays of equal size
WITH
    [110, 120, 130, 140, 190, 200, 210, 220, 230]::Array(DateTime) AS timestamps,
    [1, 1, 3, 4, 5, 5, 8, 12, 13]::Array(Float32) AS values,
    90 AS start_ts,
    90 + 120 AS end_ts,
    15 AS step_seconds,
    45 AS window_seconds
SELECT timeSeriesInstantDeltaToGrid(start_ts, end_ts, step_seconds, window_seconds)(timestamps, values);
        )",
        R"(
┌─timeSeriesInstantDeltaToGrid(start_ts, end_ts, step_seconds, window_seconds)(timestamps, values)─┐
│ [NULL,NULL,0,2,1,1,NULL,NULL,3]                                                                 │
└─────────────────────────────────────────────────────────────────────────────────────────────────┘
        )"
    }
    };
    FunctionDocumentation::IntroducedIn introduced_in_timeSeriesInstantDeltaToGrid = {25, 6};
    FunctionDocumentation::Category category_timeSeriesInstantDeltaToGrid = FunctionDocumentation::Category::AggregateFunction;
    FunctionDocumentation documentation_timeSeriesInstantDeltaToGrid = {description_timeSeriesInstantDeltaToGrid, syntax_timeSeriesInstantDeltaToGrid, arguments_timeSeriesInstantDeltaToGrid, parameters_timeSeriesInstantDeltaToGrid, returned_value_timeSeriesInstantDeltaToGrid, examples_timeSeriesInstantDeltaToGrid, introduced_in_timeSeriesInstantDeltaToGrid, category_timeSeriesInstantDeltaToGrid};

    factory.registerFunction("timeSeriesInstantDeltaToGrid",
        {createAggregateFunctionTimeseries<false, false, AggregateFunctionTimeseriesInstantValueTraits, AggregateFunctionTimeseriesInstantValue>, {}, documentation_timeSeriesInstantDeltaToGrid});
}

}
