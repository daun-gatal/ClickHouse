#include <AggregateFunctions/AggregateFunctionSparkbar.h>


namespace DB
{

AggregateFunctionPtr createSparkbarHalf2(const std::string & name, const IDataType & x_type, const IDataType & y_type, const DataTypes & args, const Array & params)
{
    WhichDataType which(y_type);
    if (which.idx == TypeIndex::Int32) return AggregateFunctionPtr(createWithUIntegerOrTimeType<AggregateFunctionSparkbar, Int32>(name, x_type, args, params));
    if (which.idx == TypeIndex::Int64) return AggregateFunctionPtr(createWithUIntegerOrTimeType<AggregateFunctionSparkbar, Int64>(name, x_type, args, params));
    if (which.idx == TypeIndex::Int128) return AggregateFunctionPtr(createWithUIntegerOrTimeType<AggregateFunctionSparkbar, Int128>(name, x_type, args, params));
    if (which.idx == TypeIndex::Int256) return AggregateFunctionPtr(createWithUIntegerOrTimeType<AggregateFunctionSparkbar, Int256>(name, x_type, args, params));
    if (which.idx == TypeIndex::BFloat16) return AggregateFunctionPtr(createWithUIntegerOrTimeType<AggregateFunctionSparkbar, BFloat16>(name, x_type, args, params));
    if (which.idx == TypeIndex::Float32) return AggregateFunctionPtr(createWithUIntegerOrTimeType<AggregateFunctionSparkbar, Float32>(name, x_type, args, params));
    if (which.idx == TypeIndex::Float64) return AggregateFunctionPtr(createWithUIntegerOrTimeType<AggregateFunctionSparkbar, Float64>(name, x_type, args, params));
    return nullptr;
}

}
