#include <Functions/FunctionsNullSafeCmp.h>
#include <Functions/FunctionsComparison.h>
#include <Functions/FunctionsLogical.h>
#include <Functions/FunctionHelpers.h>
#include <Functions/IFunctionAdaptors.h>
#include <Columns/ColumnTuple.h>

#include <Functions/isNotDistinctFrom.h>
#include <Functions/isDistinctFrom.h>

/// Suppress FunctionComparison instantiation here — equals.cpp and notEquals.cpp provide the definitions.
extern template class DB::FunctionComparison<DB::EqualsOp, DB::NameEquals>;
extern template class DB::FunctionComparison<DB::NotEqualsOp, DB::NameNotEquals>;

namespace DB
{

namespace ErrorCodes
{
    extern const int BAD_ARGUMENTS;
    extern const int NOT_IMPLEMENTED;
}

template <typename Name, NullSafeCmpMode cmp_mode, template <typename, typename> class CompareOp, typename CompareName>
ColumnPtr FunctionsNullSafeCmp<Name, cmp_mode, CompareOp, CompareName>::executeImpl(
    const ColumnsWithTypeAndName & arguments, const DataTypePtr & result_type, size_t input_rows_count) const
{
    ColumnPtr left_col = arguments[0].column;
    ColumnPtr right_col = arguments[1].column;
    const ColumnWithTypeAndName & type_and_name_left_col = arguments[0];
    const ColumnWithTypeAndName & type_and_name_right_col = arguments[1];
    if (!left_col || !right_col)
    {
        throw Exception(ErrorCodes::BAD_ARGUMENTS,
                        "Function {} received null column: left_col={} right_col={}. "
                        "Please check the input columns.",
                        backQuote(name),
                        left_col ? "NOT NULL" : "NULL",
                        right_col ? "NOT NULL" : "NULL");
    }

    /// Self null-safe comparison.
    if (type_and_name_left_col.name == type_and_name_right_col.name
        && type_and_name_left_col.type->equals(*type_and_name_right_col.type)
        && !isTuple(type_and_name_left_col.type)
        && left_col.get() == right_col.get())
    {
        return is_equal_mode ? result_type->createColumnConst(input_rows_count, UInt8(1)) :
                                result_type->createColumnConst(input_rows_count, UInt8(0));
    }

    /// Map vs null or Array vs null: always 0.
    if (((isMap(type_and_name_left_col.type) || isArray(type_and_name_left_col.type))
            && type_and_name_right_col.type->onlyNull())
        || ((isMap(type_and_name_right_col.type) || isArray(type_and_name_right_col.type))
            && type_and_name_left_col.type->onlyNull()))
    {
        return result_type->createColumnConst(input_rows_count, UInt8(0));
    }

    /// Variant vs null or Dynamic vs null.
    if (((isVariant(type_and_name_left_col.type) || isDynamic(type_and_name_left_col.type))
            && type_and_name_right_col.type->onlyNull())
        || ((isVariant(type_and_name_right_col.type) || isDynamic(type_and_name_right_col.type))
            && type_and_name_left_col.type->onlyNull()))
    {
        return executeForVariantOrDynamicAndNull(
            isVariant(type_and_name_left_col.type) || isDynamic(type_and_name_left_col.type)
                ? type_and_name_left_col
                : type_and_name_right_col);
    }

    /// Tuple comparison: compare element-wise using self (null-safe) then convolve with AND/OR.
    /// Must be handled here rather than delegating to FunctionComparison, because
    /// tuple elements may be Nullable and need null-safe semantics.
    const auto * left_tuple_type = checkAndGetDataType<DataTypeTuple>(type_and_name_left_col.type.get());
    const auto * right_tuple_type = checkAndGetDataType<DataTypeTuple>(type_and_name_right_col.type.get());
    if (left_tuple_type && right_tuple_type)
    {
        const size_t tuple_size = left_tuple_type->getElements().size();

        if (0 == tuple_size)
            throw Exception(ErrorCodes::NOT_IMPLEMENTED, "Comparison of zero-sized tuples is not implemented.");

        if (tuple_size != right_tuple_type->getElements().size())
            throw Exception(ErrorCodes::BAD_ARGUMENTS, "Cannot compare tuples of different sizes.");

        if (result_type->onlyNull())
            return result_type->createColumnConstWithDefaultValue(input_rows_count);

        /// Decompose tuple columns into element columns.
        const auto * x_const = checkAndGetColumnConst<ColumnTuple>(type_and_name_left_col.column.get());
        const auto * y_const = checkAndGetColumnConst<ColumnTuple>(type_and_name_right_col.column.get());

        Columns x_columns = x_const
            ? convertConstTupleToConstantElements(*x_const)
            : assert_cast<const ColumnTuple &>(*type_and_name_left_col.column).getColumnsCopy();

        Columns y_columns = y_const
            ? convertConstTupleToConstantElements(*y_const)
            : assert_cast<const ColumnTuple &>(*type_and_name_right_col.column).getColumnsCopy();

        ColumnsWithTypeAndName x(tuple_size);
        ColumnsWithTypeAndName y(tuple_size);
        for (size_t i = 0; i < tuple_size; ++i)
        {
            x[i].type = left_tuple_type->getElements()[i];
            y[i].type = right_tuple_type->getElements()[i];
            x[i].column = x_columns[i];
            y[i].column = y_columns[i];
        }

        /// Element-wise comparison using this null-safe function.
        auto func_self = std::make_unique<FunctionToOverloadResolverAdaptor>(
            std::make_shared<FunctionsNullSafeCmp<Name, cmp_mode, CompareOp, CompareName>>(params));

        ColumnsWithTypeAndName convolution_columns(tuple_size);
        ColumnsWithTypeAndName tmp_columns(2);

        for (size_t i = 0; i < tuple_size; ++i)
        {
            tmp_columns[0] = x[i];
            tmp_columns[1] = y[i];

            auto impl = func_self->build(tmp_columns);
            convolution_columns[i].type = impl->getResultType();
            convolution_columns[i].column = impl->execute(tmp_columns, impl->getResultType(), input_rows_count, /* dry_run = */ false);
        }

        if (tuple_size == 1)
            return convolution_columns[0].column;

        /// Logical convolution: AND for equal, OR for not-equal.
        FunctionOverloadResolverPtr func_logic;
        if constexpr (is_equal_mode)
            func_logic = std::make_unique<FunctionToOverloadResolverAdaptor>(std::make_shared<FunctionAnd>());
        else
            func_logic = std::make_unique<FunctionToOverloadResolverAdaptor>(std::make_shared<FunctionOr>());

        auto impl = func_logic->build(convolution_columns);
        return impl->execute(convolution_columns, impl->getResultType(), input_rows_count, /* dry_run = */ false);
    }

    /// Get common type for null-safe comparison.
    DataTypePtr common_type = getLeastSupertype(DataTypes{arguments[0].type, arguments[1].type});

    ColumnPtr c0_converted = castColumn(arguments[0], common_type);
    ColumnPtr c1_converted = castColumn(arguments[1], common_type);

    /// Nullable vs Nullable: use compareAt which treats NULL=NULL as equal.
    if (c0_converted->isNullable() && c1_converted->isNullable())
    {
        auto c_res = ColumnUInt8::create();
        ColumnUInt8::Container & vec_res = c_res->getData();
        vec_res.resize(arguments[0].column->size());
        c0_converted = c0_converted->convertToFullColumnIfConst();
        c1_converted = c1_converted->convertToFullColumnIfConst();

        for (size_t i = 0; i < input_rows_count; i++)
            vec_res[i] = c0_converted->compareAt(i, i, *c1_converted, 1) == 0 ? is_equal_mode : !is_equal_mode;

        return c_res;
    }

    /// Non-nullable, non-tuple: delegate to regular FunctionComparison.
    /// At this point neither argument is nullable, so null-safe mode is irrelevant.
    ColumnPtr res;
    FunctionOverloadResolverPtr comparator
        = std::make_unique<FunctionToOverloadResolverAdaptor>(std::make_shared<FunctionComparison<CompareOp, CompareName>>(params));

    auto executable_func = comparator->build(arguments);
    auto data_type = executable_func->getResultType();
    res = executable_func->execute(arguments, data_type, input_rows_count, /* dry_run = */ false);

    return res;
}

/// Explicit instantiations for the two null-safe comparison variants.
template class FunctionsNullSafeCmp<NameFunctionIsNotDistinctFrom, NullSafeCmpMode::NullSafeEqual, EqualsOp, NameEquals>;
template class FunctionsNullSafeCmp<NameFunctionIsDistinctFrom, NullSafeCmpMode::NullSafeNotEqual, NotEqualsOp, NameNotEquals>;

}
