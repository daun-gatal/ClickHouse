#include <AggregateFunctions/TimeSeries/AggregateFunctionTimeseriesCreate.h>
#include <AggregateFunctions/TimeSeries/AggregateFunctionTimeseriesLinearRegression.h>
#include <AggregateFunctions/TimeSeries/AggregateFunctionTimeseriesChanges.h>
#include <AggregateFunctions/TimeSeries/AggregateFunctionTimeseriesToGridSparse.h>

namespace DB
{

using namespace detail;

void registerAggregateFunctionTimeseries2(AggregateFunctionFactory & factory)
{
    /// timeSeriesDerivToGrid documentation
    FunctionDocumentation::Description description_timeSeriesDerivToGrid = R"(
Aggregate function that takes time series data as pairs of timestamps and values and calculates [PromQL-like derivative](https://prometheus.io/docs/prometheus/latest/querying/functions/#deriv) from this data on a regular time grid described by start timestamp, end timestamp and step. For each point on the grid the samples for calculating `deriv` are considered within the specified time window.

:::note
This function is experimental, enable it by setting `allow_experimental_ts_to_grid_aggregate_function=true`.
:::
    )";
    FunctionDocumentation::Syntax syntax_timeSeriesDerivToGrid = R"(
timeSeriesDerivToGrid(start_timestamp, end_timestamp, grid_step, staleness)(timestamp, value)
    )";
    FunctionDocumentation::Parameters parameters_timeSeriesDerivToGrid = {
        {"start_timestamp", "Specifies start of the grid.", {}},
        {"end_timestamp", "Specifies end of the grid.", {}},
        {"grid_step", "Specifies step of the grid in seconds.", {}},
        {"staleness", "Specifies the maximum \"staleness\" in seconds of the considered samples. The staleness window is a left-open and right-closed interval.", {}}
    };
    FunctionDocumentation::Arguments arguments_timeSeriesDerivToGrid = {
        {"timestamp", "Timestamp of the sample. Can be individual values or arrays.", {}},
        {"value", "Value of the time series corresponding to the timestamp. Can be individual values or arrays.", {}}
    };
    FunctionDocumentation::ReturnedValue returned_value_timeSeriesDerivToGrid = {"`deriv` values on the specified grid as an `Array(Nullable(Float64))`. The returned array contains one value for each time grid point. The value is NULL if there are not enough samples within the window to calculate the derivative value for a particular grid point.", {}};
    FunctionDocumentation::Examples examples_timeSeriesDerivToGrid = {
    {
        "Calculate derivative values on the grid [90, 105, 120, 135, 150, 165, 180, 195, 210]",
        R"(
WITH
    -- NOTE: the gap between 140 and 190 is to show how values are filled for ts = 150, 165, 180 according to window parameter
    [110, 120, 130, 140, 190, 200, 210, 220, 230]::Array(DateTime) AS timestamps,
    [1, 1, 3, 4, 5, 5, 8, 12, 13]::Array(Float32) AS values, -- array of values corresponding to timestamps above
    90 AS start_ts,       -- start of timestamp grid
    90 + 120 AS end_ts,   -- end of timestamp grid
    15 AS step_seconds,   -- step of timestamp grid
    45 AS window_seconds  -- "staleness" window
SELECT timeSeriesDerivToGrid(start_ts, end_ts, step_seconds, window_seconds)(timestamp, value)
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
┌─timeSeriesDerivToGrid(start_ts, end_ts, step_seconds, window_seconds)(timestamp, value)─┐
│ [NULL,NULL,0,0.1,0.11,0.15,NULL,NULL,0.15]                                              │
└─────────────────────────────────────────────────────────────────────────────────────────┘
        )"
    },
    {
        "Same query with array arguments",
        R"(
WITH
    [110, 120, 130, 140, 190, 200, 210, 220, 230]::Array(DateTime) AS timestamps,
    [1, 1, 3, 4, 5, 5, 8, 12, 13]::Array(Float32) AS values,
    90 AS start_ts,
    90 + 120 AS end_ts,
    15 AS step_seconds,
    45 AS window_seconds
SELECT timeSeriesDerivToGrid(start_ts, end_ts, step_seconds, window_seconds)(timestamps, values);
        )",
        R"(
┌─timeSeriesDerivToGrid(start_ts, end_ts, step_seconds, window_seconds)(timestamps, values)─┐
│ [NULL,NULL,0,0.1,0.11,0.15,NULL,NULL,0.15]                                                │
└───────────────────────────────────────────────────────────────────────────────────────────┘
        )"
    }
    };
    FunctionDocumentation::IntroducedIn introduced_in_timeSeriesDerivToGrid = {25, 6};
    FunctionDocumentation::Category category_timeSeriesDerivToGrid = FunctionDocumentation::Category::AggregateFunction;
    FunctionDocumentation documentation_timeSeriesDerivToGrid = {description_timeSeriesDerivToGrid, syntax_timeSeriesDerivToGrid, arguments_timeSeriesDerivToGrid, parameters_timeSeriesDerivToGrid, returned_value_timeSeriesDerivToGrid, examples_timeSeriesDerivToGrid, introduced_in_timeSeriesDerivToGrid, category_timeSeriesDerivToGrid};

    factory.registerFunction("timeSeriesDerivToGrid",
        {createAggregateFunctionTimeseries<false, false, AggregateFunctionTimeseriesLinearRegressionTraits, AggregateFunctionTimeseriesLinearRegression>, {}, documentation_timeSeriesDerivToGrid});
    /// timeSeriesPredictLinearToGrid documentation
    FunctionDocumentation::Description description_timeSeriesPredictLinearToGrid = R"(
Aggregate function that takes time series data as pairs of timestamps and values and calculates a [PromQL-like linear prediction](https://prometheus.io/docs/prometheus/latest/querying/functions/#predict_linear) with a specified prediction timestamp offset from this data on a regular time grid described by start timestamp, end timestamp and step. For each point on the grid the samples for calculating `predict_linear` are considered within the specified time window.

:::note
This function is experimental, enable it by setting `allow_experimental_ts_to_grid_aggregate_function=true`.
:::
    )";
    FunctionDocumentation::Syntax syntax_timeSeriesPredictLinearToGrid = R"(
timeSeriesPredictLinearToGrid(start_timestamp, end_timestamp, grid_step, staleness, predict_offset)(timestamp, value)
    )";
    FunctionDocumentation::Parameters parameters_timeSeriesPredictLinearToGrid = {
        {"start_timestamp", "Specifies start of the grid.", {}},
        {"end_timestamp", "Specifies end of the grid.", {}},
        {"grid_step", "Specifies step of the grid in seconds.", {}},
        {"staleness", "Specifies the maximum \"staleness\" in seconds of the considered samples. The staleness window is a left-open and right-closed interval.", {}},
        {"predict_offset", "Specifies number of seconds of offset to add to prediction time.", {}}
    };
    FunctionDocumentation::Arguments arguments_timeSeriesPredictLinearToGrid = {
        {"timestamp", "Timestamp of the sample. Can be individual values or arrays.", {}},
        {"value", "Value of the time series corresponding to the timestamp. Can be individual values or arrays.", {}}
    };
    FunctionDocumentation::ReturnedValue returned_value_timeSeriesPredictLinearToGrid = {"`predict_linear` values on the specified grid as an `Array(Nullable(Float64))`. The returned array contains one value for each time grid point. The value is NULL if there are not enough samples within the window to calculate the rate value for a particular grid point.", {}};
    FunctionDocumentation::Examples examples_timeSeriesPredictLinearToGrid = {
    {
        "Calculate predict_linear values on the grid [90, 105, 120, 135, 150, 165, 180, 195, 210] with a 60 second offset",
        R"(
WITH
    -- NOTE: the gap between 140 and 190 is to show how values are filled for ts = 150, 165, 180 according to window parameter
    [110, 120, 130, 140, 190, 200, 210, 220, 230]::Array(DateTime) AS timestamps,
    [1, 1, 3, 4, 5, 5, 8, 12, 13]::Array(Float32) AS values, -- array of values corresponding to timestamps above
    90 AS start_ts,       -- start of timestamp grid
    90 + 120 AS end_ts,   -- end of timestamp grid
    15 AS step_seconds,   -- step of timestamp grid
    45 AS window_seconds, -- "staleness" window
    60 AS predict_offset  -- prediction time offset
SELECT timeSeriesPredictLinearToGrid(start_ts, end_ts, step_seconds, window_seconds, predict_offset)(timestamp, value)
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
┌─timeSeriesPredictLinearToGrid(start_ts, end_ts, step_seconds, window_seconds, predict_offset)(timestamp, value)─┐
│ [NULL,NULL,1,9.166667,11.6,16.916666,NULL,NULL,16.5]                                                            │
└─────────────────────────────────────────────────────────────────────────────────────────────────────────────────┘
        )"
    },
    {
        "Same query with array arguments",
        R"(
WITH
    [110, 120, 130, 140, 190, 200, 210, 220, 230]::Array(DateTime) AS timestamps,
    [1, 1, 3, 4, 5, 5, 8, 12, 13]::Array(Float32) AS values,
    90 AS start_ts,
    90 + 120 AS end_ts,
    15 AS step_seconds,
    45 AS window_seconds,
    60 AS predict_offset
SELECT timeSeriesPredictLinearToGrid(start_ts, end_ts, step_seconds, window_seconds, predict_offset)(timestamps, values);
        )",
        R"(
┌─timeSeriesPredictLinearToGrid(start_ts, end_ts, step_seconds, window_seconds, predict_offset)(timestamp, value)─┐
│ [NULL,NULL,1,9.166667,11.6,16.916666,NULL,NULL,16.5]                                                            │
└─────────────────────────────────────────────────────────────────────────────────────────────────────────────────┘
        )"
    }
    };
    FunctionDocumentation::IntroducedIn introduced_in_timeSeriesPredictLinearToGrid = {25, 6};
    FunctionDocumentation::Category category_timeSeriesPredictLinearToGrid = FunctionDocumentation::Category::AggregateFunction;
    FunctionDocumentation documentation_timeSeriesPredictLinearToGrid = {description_timeSeriesPredictLinearToGrid, syntax_timeSeriesPredictLinearToGrid, arguments_timeSeriesPredictLinearToGrid, parameters_timeSeriesPredictLinearToGrid, returned_value_timeSeriesPredictLinearToGrid, examples_timeSeriesPredictLinearToGrid, introduced_in_timeSeriesPredictLinearToGrid, category_timeSeriesPredictLinearToGrid};

    factory.registerFunction("timeSeriesPredictLinearToGrid",
        {createAggregateFunctionTimeseries<false, true, AggregateFunctionTimeseriesLinearRegressionTraits, AggregateFunctionTimeseriesLinearRegression>, {}, documentation_timeSeriesPredictLinearToGrid});

    /// timeSeriesChangesToGrid documentation
    FunctionDocumentation::Description description_timeSeriesChangesToGrid = R"(
Aggregate function that takes time series data as pairs of timestamps and values and calculates [PromQL-like changes](https://prometheus.io/docs/prometheus/latest/querying/functions/#changes) from this data on a regular time grid described by start timestamp, end timestamp and step. For each point on the grid the samples for calculating `changes` are considered within the specified time window.

:::note
This function is experimental, enable it by setting `allow_experimental_ts_to_grid_aggregate_function=true`.
:::
    )";
    FunctionDocumentation::Syntax syntax_timeSeriesChangesToGrid = R"(
timeSeriesChangesToGrid(start_timestamp, end_timestamp, grid_step, staleness)(timestamp, value)
    )";
    FunctionDocumentation::Parameters parameters_timeSeriesChangesToGrid = {
        {"start_timestamp", "Specifies start of the grid.", {}},
        {"end_timestamp", "Specifies end of the grid.", {}},
        {"grid_step", "Specifies step of the grid in seconds.", {}},
        {"staleness", "Specifies the maximum \"staleness\" in seconds of the considered samples.", {}}
    };
    FunctionDocumentation::Arguments arguments_timeSeriesChangesToGrid = {
        {"timestamp", "Timestamp of the sample. Can be individual values or arrays.", {}},
        {"value", "Value of the time series corresponding to the timestamp. Can be individual values or arrays.", {}}
    };
    FunctionDocumentation::ReturnedValue returned_value_timeSeriesChangesToGrid = {"`changes` values on the specified grid as an `Array(Nullable(Float64))`. The returned array contains one value for each time grid point. The value is NULL if there are no samples within the window to calculate the changes value for a particular grid point.", {}};
    FunctionDocumentation::Examples examples_timeSeriesChangesToGrid = {
    {
        "Calculate changes values on the grid [90, 105, 120, 135, 150, 165, 180, 195, 210, 225]",
        R"(
WITH
    -- NOTE: the gap between 130 and 190 is to show how values are filled for ts = 180 according to window parameter
    [110, 120, 130, 190, 200, 210, 220, 230]::Array(DateTime) AS timestamps,
    [1, 1, 3, 5, 5, 8, 12, 13]::Array(Float32) AS values, -- array of values corresponding to timestamps above
    90 AS start_ts,       -- start of timestamp grid
    90 + 135 AS end_ts,   -- end of timestamp grid
    15 AS step_seconds,   -- step of timestamp grid
    45 AS window_seconds  -- "staleness" window
SELECT timeSeriesChangesToGrid(start_ts, end_ts, step_seconds, window_seconds)(timestamp, value)
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
┌─timeSeriesChangesToGrid(start_ts, end_ts, step_seconds, window_seconds)(timestamp, value)─┐
│ [NULL,NULL,0,1,1,1,NULL,0,1,2]                                                            │
└───────────────────────────────────────────────────────────────────────────────────────────┘
        )"
    },
    {
        "Same query with array arguments",
        R"(
WITH
    [110, 120, 130, 190, 200, 210, 220, 230]::Array(DateTime) AS timestamps,
    [1, 1, 3, 5, 5, 8, 12, 13]::Array(Float32) AS values,
    90 AS start_ts,
    90 + 135 AS end_ts,
    15 AS step_seconds,
    45 AS window_seconds
SELECT timeSeriesChangesToGrid(start_ts, end_ts, step_seconds, window_seconds)(timestamps, values);
        )",
        R"(
┌─timeSeriesChangesToGrid(start_ts, end_ts, step_seconds, window_seconds)(timestamp, value)─┐
│ [NULL,NULL,0,1,1,1,NULL,0,1,2]                                                            │
└───────────────────────────────────────────────────────────────────────────────────────────┘
        )"
    }
    };
    FunctionDocumentation::IntroducedIn introduced_in_timeSeriesChangesToGrid = {25, 6};
    FunctionDocumentation::Category category_timeSeriesChangesToGrid = FunctionDocumentation::Category::AggregateFunction;
    FunctionDocumentation documentation_timeSeriesChangesToGrid = {description_timeSeriesChangesToGrid, syntax_timeSeriesChangesToGrid, arguments_timeSeriesChangesToGrid, parameters_timeSeriesChangesToGrid, returned_value_timeSeriesChangesToGrid, examples_timeSeriesChangesToGrid, introduced_in_timeSeriesChangesToGrid, category_timeSeriesChangesToGrid};

    factory.registerFunction("timeSeriesChangesToGrid",
        {createAggregateFunctionTimeseries<false, false, AggregateFunctionTimeseriesChangesTraits, AggregateFunctionTimeseriesChanges>, {}, documentation_timeSeriesChangesToGrid});
    /// timeSeriesResetsToGrid documentation
    FunctionDocumentation::Description description_timeSeriesResetsToGrid = R"(
Aggregate function that takes time series data as pairs of timestamps and values and calculates [PromQL-like resets](https://prometheus.io/docs/prometheus/latest/querying/functions/#resets) from this data on a regular time grid described by start timestamp, end timestamp and step. For each point on the grid the samples for calculating `resets` are considered within the specified time window.

:::note
This function is experimental, enable it by setting `allow_experimental_ts_to_grid_aggregate_function=true`.
:::
    )";
    FunctionDocumentation::Syntax syntax_timeSeriesResetsToGrid = R"(
timeSeriesResetsToGrid(start_timestamp, end_timestamp, grid_step, staleness)(timestamp, value)
    )";
    FunctionDocumentation::Parameters parameters_timeSeriesResetsToGrid = {
        {"start_timestamp", "Specifies start of the grid.", {}},
        {"end_timestamp", "Specifies end of the grid.", {}},
        {"grid_step", "Specifies step of the grid in seconds.", {}},
        {"staleness", "Specifies the maximum \"staleness\" in seconds of the considered samples.", {}}
    };
    FunctionDocumentation::Arguments arguments_timeSeriesResetsToGrid = {
        {"timestamp", "Timestamp of the sample. Can be individual values or arrays.", {}},
        {"value", "Value of the time series corresponding to the timestamp. Can be individual values or arrays.", {}}
    };
    FunctionDocumentation::ReturnedValue returned_value_timeSeriesResetsToGrid = {"`resets` values on the specified grid as an `Array(Nullable(Float64))`. The returned array contains one value for each time grid point. The value is NULL if there are no samples within the window to calculate the resets value for a particular grid point.", {}};
    FunctionDocumentation::Examples examples_timeSeriesResetsToGrid = {
    {
        "Calculate resets values on the grid [90, 105, 120, 135, 150, 165, 180, 195, 210, 225]",
        R"(
WITH
    -- NOTE: the gap between 130 and 190 is to show how values are filled for ts = 180 according to window parameter
    [110, 120, 130, 190, 200, 210, 220, 230]::Array(DateTime) AS timestamps,
    [1, 3, 2, 6, 6, 4, 2, 0]::Array(Float32) AS values, -- array of values corresponding to timestamps above
    90 AS start_ts,       -- start of timestamp grid
    90 + 135 AS end_ts,   -- end of timestamp grid
    15 AS step_seconds,   -- step of timestamp grid
    45 AS window_seconds  -- "staleness" window
SELECT timeSeriesResetsToGrid(start_ts, end_ts, step_seconds, window_seconds)(timestamp, value)
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
┌─timeSeriesResetsToGrid(start_ts, end_ts, step_seconds, window_seconds)(timestamp, value)─┐
│ [NULL,NULL,0,1,1,1,NULL,0,1,2]                                                           │
└──────────────────────────────────────────────────────────────────────────────────────────┘
        )"
    },
    {
        "Same query with array arguments",
        R"(
WITH
    [110, 120, 130, 190, 200, 210, 220, 230]::Array(DateTime) AS timestamps,
    [1, 3, 2, 6, 6, 4, 2, 0]::Array(Float32) AS values,
    90 AS start_ts,
    90 + 135 AS end_ts,
    15 AS step_seconds,
    45 AS window_seconds
SELECT timeSeriesResetsToGrid(start_ts, end_ts, step_seconds, window_seconds)(timestamps, values);
        )",
        R"(
┌─timeSeriesResetsToGrid(start_ts, end_ts, step_seconds, window_seconds)(timestamp, value)─┐
│ [NULL,NULL,0,1,1,0,NULL,0,1,2]                                                           │
└──────────────────────────────────────────────────────────────────────────────────────────┘
        )"
    }
    };
    FunctionDocumentation::IntroducedIn introduced_in_timeSeriesResetsToGrid = {25, 6};
    FunctionDocumentation::Category category_timeSeriesResetsToGrid = FunctionDocumentation::Category::AggregateFunction;
    FunctionDocumentation documentation_timeSeriesResetsToGrid = {description_timeSeriesResetsToGrid, syntax_timeSeriesResetsToGrid, arguments_timeSeriesResetsToGrid, parameters_timeSeriesResetsToGrid, returned_value_timeSeriesResetsToGrid, examples_timeSeriesResetsToGrid, introduced_in_timeSeriesResetsToGrid, category_timeSeriesResetsToGrid};

    factory.registerFunction("timeSeriesResetsToGrid",
        {createAggregateFunctionTimeseries<true, false, AggregateFunctionTimeseriesChangesTraits, AggregateFunctionTimeseriesChanges>, {}, documentation_timeSeriesResetsToGrid});

    /// timeSeriesResampleToGridWithStaleness documentation
    FunctionDocumentation::Description description_timeSeriesResampleToGridWithStaleness = R"(
Aggregate function that takes time series data as pairs of timestamps and values and re-samples this data to a regular time grid described by start timestamp, end timestamp and step. For each point on the grid the most recent (within the specified time window) sample is chosen.

Alias: `timeSeriesLastToGrid`.

:::warning
This function is experimental, enable it by setting `allow_experimental_ts_to_grid_aggregate_function=true`.
:::
    )";
    FunctionDocumentation::Syntax syntax_timeSeriesResampleToGridWithStaleness = R"(
timeSeriesResampleToGridWithStaleness(start_timestamp, end_timestamp, grid_step, staleness_window)(timestamp, value)
    )";
    FunctionDocumentation::Parameters parameters_timeSeriesResampleToGridWithStaleness = {
        {"start_timestamp", "Specifies start of the grid.", {"UInt32", "DateTime"}},
        {"end_timestamp", "Specifies end of the grid.", {"UInt32", "DateTime"}},
        {"grid_step", "Specifies step of the grid in seconds.", {"UInt32"}},
        {"staleness_window", "Specifies the maximum staleness of the most recent sample in seconds.", {"UInt32"}}
    };
    FunctionDocumentation::Arguments arguments_timeSeriesResampleToGridWithStaleness = {
        {"timestamp", "Timestamp of the sample. Can be individual values or arrays.", {"UInt32", "DateTime", "Array(UInt32)", "Array(DateTime)"}},
        {"value", "Value of the time series corresponding to the timestamp. Can be individual values or arrays.", {"Float*", "Array(Float*)"}}
    };
    FunctionDocumentation::ReturnedValue returned_value_timeSeriesResampleToGridWithStaleness = {"Returns time series values re-sampled to the specified grid. The returned array contains one value for each time grid point. The value is NULL if there is no sample for a particular grid point.", {"Array(Nullable(Float64))"}};
    FunctionDocumentation::Examples examples_timeSeriesResampleToGridWithStaleness = {
    {
        "Basic usage with individual timestamp-value pairs",
        R"(
WITH
    -- NOTE: the gap between 140 and 190 is to show how values are filled for ts = 150, 165, 180 according to staleness window parameter
    [110, 120, 130, 140, 190, 200, 210, 220, 230]::Array(DateTime) AS timestamps,
    [1, 1, 3, 4, 5, 5, 8, 12, 13]::Array(Float32) AS values, -- array of values corresponding to timestamps above
    90 AS start_ts,       -- start of timestamp grid
    90 + 120 AS end_ts,   -- end of timestamp grid
    15 AS step_seconds,   -- step of timestamp grid
    30 AS window_seconds  -- "staleness" window
SELECT timeSeriesResampleToGridWithStaleness(start_ts, end_ts, step_seconds, window_seconds)(timestamp, value)
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
┌─timeSeriesResampleToGridWithStaleness(start_ts, end_ts, step_seconds, window_seconds)(timestamp, value)─┐
│ [NULL,NULL,1,3,4,4,NULL,5,8]                                                                           │
└────────────────────────────────────────────────────────────────────────────────────────────────────┘
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
    30 AS window_seconds
SELECT timeSeriesResampleToGridWithStaleness(start_ts, end_ts, step_seconds, window_seconds)(timestamps, values);
        )",
        R"(
┌─timeSeriesResampleToGridWithStaleness(start_ts, end_ts, step_seconds, window_seconds)(timestamps, values)─┐
│ [NULL,NULL,1,3,4,4,NULL,5,8]                                                                             │
└──────────────────────────────────────────────────────────────────────────────────────────────────────┘
        )"
    }
    };
    FunctionDocumentation::IntroducedIn introduced_in_timeSeriesResampleToGridWithStaleness = {25, 6};
    FunctionDocumentation::Category category_timeSeriesResampleToGridWithStaleness = FunctionDocumentation::Category::AggregateFunction;
    FunctionDocumentation documentation_timeSeriesResampleToGridWithStaleness = {description_timeSeriesResampleToGridWithStaleness, syntax_timeSeriesResampleToGridWithStaleness, arguments_timeSeriesResampleToGridWithStaleness, parameters_timeSeriesResampleToGridWithStaleness, returned_value_timeSeriesResampleToGridWithStaleness, examples_timeSeriesResampleToGridWithStaleness, introduced_in_timeSeriesResampleToGridWithStaleness, category_timeSeriesResampleToGridWithStaleness};

    factory.registerFunction("timeSeriesResampleToGridWithStaleness",
        {createAggregateFunctionTimeseries<false, false, AggregateFunctionTimeseriesToGridSparseTraits, AggregateFunctionTimeseriesToGridSparse>, {}, documentation_timeSeriesResampleToGridWithStaleness});
    factory.registerAlias("timeSeriesLastToGrid", "timeSeriesResampleToGridWithStaleness");
}

}
