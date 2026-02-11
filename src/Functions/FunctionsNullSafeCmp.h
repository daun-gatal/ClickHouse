#pragma once
#include <DataTypes/DataTypeTuple.h>
#include <DataTypes/IDataType.h>
#include <Functions/IFunction.h>
#include <Functions/FunctionsComparison.h>
#include <Columns/ColumnNullable.h>
#include <Common/quoteString.h>
#include <Columns/ColumnVariant.h>
#include <Columns/ColumnDynamic.h>
namespace DB
{

namespace ErrorCodes
{
    extern const int NUMBER_OF_ARGUMENTS_DOESNT_MATCH;
    extern const int BAD_ARGUMENTS;
    extern const int ILLEGAL_TYPE_OF_ARGUMENT;
}

enum class NullSafeCmpMode : uint8_t
{
    NullSafeEqual,
    NullSafeNotEqual
};

template <
    typename Name,                                              // Function Name
    NullSafeCmpMode cmp_mode,                                   // Null-safe mode (Equal or NotEqual)
    template <typename, typename > class CompareOp,             // EqualsOp / NotEqualsOp
    typename CompareName>                                       // NameEquals / NameNotEquals
class FunctionsNullSafeCmp : public IFunction
{
private:
    const ComparisonParams params;

    static bool containsNothing(const DataTypePtr & type)
    {
        if (isNothing(type))
            return true;

        if (const auto * tuple_type = typeid_cast<const DataTypeTuple *>(type.get()))
        {
            for (const auto & elem : tuple_type->getElements())
            {
                if (containsNothing(elem))
                    return true;
            }
        }
        return false;
    }

public:
    explicit FunctionsNullSafeCmp(ComparisonParams params_) : params(std::move(params_)) {}

    static constexpr auto name = Name::name;
    static constexpr bool is_equal_mode = (cmp_mode == NullSafeCmpMode::NullSafeEqual);


    static FunctionPtr create(ContextPtr context)
    {
        return std::make_shared<FunctionsNullSafeCmp>(context ? ComparisonParams(context) : ComparisonParams());
    }

    String getName() const override { return name; }

    bool isVariadic() const override { return false; }

    size_t getNumberOfArguments() const override { return 2; }

    bool isSuitableForShortCircuitArgumentsExecution(const DataTypesWithConstInfo & /*arguments*/) const override { return false; }

    bool useDefaultImplementationForNulls() const override { return false; }

    bool useDefaultImplementationForNothing() const override { return true; }
    bool useDefaultImplementationForConstants() const override { return true; }
    bool useDefaultImplementationForLowCardinalityColumns() const override { return true; }

    DataTypePtr getReturnTypeImpl(const DataTypes & arguments) const override
    {
        if (arguments.size() != 2)
            throw Exception(ErrorCodes::NUMBER_OF_ARGUMENTS_DOESNT_MATCH,
                            "Function {} expects exactly 2 arguments, got {}",
                            backQuote(name),
                            arguments.size());

        const DataTypePtr & left_ele_type = arguments[0];
        const DataTypePtr & right_ele_type = arguments[1];

        if (containsNothing(left_ele_type) || containsNothing(right_ele_type))
            return std::make_shared<DataTypeNothing>();

        if ((isMap(left_ele_type) && right_ele_type->onlyNull())
                || (left_ele_type->onlyNull() && isMap(right_ele_type))
                || (isArray(left_ele_type) && right_ele_type->onlyNull())
                || (left_ele_type->onlyNull() && isArray(right_ele_type)))
        {
            return std::make_shared<DataTypeUInt8>();
        }

        if (!tryGetLeastSupertype(arguments))
            throw Exception(ErrorCodes::ILLEGAL_TYPE_OF_ARGUMENT, "Illegal types of arguments ({}, {})"
                " of function {}", backQuote(arguments[0]->getName()), backQuote(arguments[1]->getName()), backQuote(getName()));

        return std::make_shared<DataTypeUInt8>();
    }

    ColumnPtr ALWAYS_INLINE executeForVariantOrDynamicAndNull(const ColumnWithTypeAndName & variant_or_dynamic_col) const
    {
        const auto & column_variant_or_dynamic =
            isVariant(variant_or_dynamic_col.type) ?
                checkAndGetColumn<ColumnVariant>(*variant_or_dynamic_col.column) :
                checkAndGetColumn<ColumnDynamic>(*variant_or_dynamic_col.column).getVariantColumn();
        auto res = DataTypeUInt8().createColumn();
        auto & data = typeid_cast<ColumnUInt8 &>(*res).getData();
        data.resize(column_variant_or_dynamic.size());
        for (size_t i = 0; i < column_variant_or_dynamic.size(); ++i)
        {
            bool ele_is_null = column_variant_or_dynamic.isNullAt(i);
            data[i] = is_equal_mode ? ele_is_null && true : ele_is_null && false;
        }
        return res;
    }

    /// Defined in FunctionsNullSafeCmp.cpp to avoid heavy FunctionComparison template instantiation
    /// in every TU that uses this class.
    ColumnPtr executeImpl(const ColumnsWithTypeAndName & arguments, const DataTypePtr & result_type, size_t input_rows_count) const override;
};
}
