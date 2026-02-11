#include <Functions/bitHammingDistance.h>

namespace DB
{

template class FunctionBinaryArithmetic<BitHammingDistanceImpl, NameBitHammingDistance>;
template class FunctionBinaryArithmeticWithConstants<BitHammingDistanceImpl, NameBitHammingDistance>;
template class BinaryArithmeticOverloadResolver<BitHammingDistanceImpl, NameBitHammingDistance>;

}
