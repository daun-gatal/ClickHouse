#include <AggregateFunctions/AggregateFunctionAvgWeighted.h>


namespace DB
{

/// Second half of first-level dispatch for avgWeighted.
/// Handles UInt64, UInt128, UInt256, Int128, Int256, Float32, Float64.
IAggregateFunction * createAvgWeightedSecondHalf(const IDataType & first_type, const IDataType & second_type, const DataTypes & argument_types)
{
    const WhichDataType which(first_type);

#define LINE(Type) \
    case TypeIndex::Type: return createAvgWeightedSecondType<Type>(second_type, argument_types)
    switch (which.idx)
    {
        LINE(UInt64); LINE(UInt128); LINE(UInt256);
        LINE(Int128); LINE(Int256);
        LINE(Float32); LINE(Float64);
        default: return nullptr;
    }
#undef LINE
}

}
