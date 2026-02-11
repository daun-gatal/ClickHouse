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

AggregateFunctionPtr createAggregateFunctionUniqExactSerial(const std::string & name, const DataTypes & argument_types, const Array & params, const Settings * settings)
{
    return createAggregateFunctionUniq<true, AggregateFunctionUniqExactData, AggregateFunctionUniqExactDataForVariadic, false /* is_able_to_parallelize_merge */>(name, argument_types, params, settings);
}

}
