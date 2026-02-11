#include <AggregateFunctions/AggregateFunctionFactory.h>
#include <AggregateFunctions/FactoryHelpers.h>
#include <Common/FieldVisitorConvertToNumber.h>
#include <Common/FieldVisitorToString.h>

/// Include this last — see the reason inside
#include <AggregateFunctions/AggregateFunctionGroupNumericIndexedVector.h>

namespace DB
{

namespace
{
template <typename FirstType, template <typename, typename> class VectorImpl, typename... TArgs>
IAggregateFunction *
createBSIVectorWithTwoNumericTypesSecond(const IDataType & second_type, const DataTypes & types, const Array & params, TArgs &&... args)
{
    WhichDataType which(second_type);
#define DISPATCH(TYPE) \
    if (which.idx == TypeIndex::TYPE) \
        return new AggregateFunctionNumericIndexedVector<VectorImpl<FirstType, TYPE>, std::decay_t<TArgs>...>( \
            types, params, std::forward<TArgs>(args)...);
    FOR_BASIC_NUMERIC_TYPES(DISPATCH)
#undef DISPATCH
    return nullptr;
}
}

IAggregateFunction * createBSIVectorWithTwoNumericTypesFirstHalf2(
    const IDataType & first_type, const IDataType & second_type, const DataTypes & types, const Array & params, UInt32 integer_bit_num, UInt32 fraction_bit_num)
{
    WhichDataType which(first_type);
    /// Second half: Int types
    if (which.idx == TypeIndex::Int8)
        return createBSIVectorWithTwoNumericTypesSecond<Int8, BSINumericIndexedVector>(second_type, types, params, integer_bit_num, fraction_bit_num);
    if (which.idx == TypeIndex::Int16)
        return createBSIVectorWithTwoNumericTypesSecond<Int16, BSINumericIndexedVector>(second_type, types, params, integer_bit_num, fraction_bit_num);
    if (which.idx == TypeIndex::Int32)
        return createBSIVectorWithTwoNumericTypesSecond<Int32, BSINumericIndexedVector>(second_type, types, params, integer_bit_num, fraction_bit_num);
    if (which.idx == TypeIndex::Int64)
        return createBSIVectorWithTwoNumericTypesSecond<Int64, BSINumericIndexedVector>(second_type, types, params, integer_bit_num, fraction_bit_num);
    return nullptr;
}

}
