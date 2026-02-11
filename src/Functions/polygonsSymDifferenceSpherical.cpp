#include <Functions/FunctionFactory.h>
#include <Functions/FunctionPolygonsSymDifference.h>

namespace DB
{

template <>
const char * FunctionPolygonsSymDifference<SphericalPoint>::name = "polygonsSymDifferenceSpherical";

REGISTER_FUNCTION(PolygonsSymDifferenceSpherical)
{
    FunctionDocumentation::Description description = R"(
Calculates the spatial set theoretic symmetric difference (XOR) between two polygons
    )";
    FunctionDocumentation::Syntax syntax = "polygonsSymDifferenceSpherical(polygon1, polygon2)";
    FunctionDocumentation::Arguments arguments = {
        {"polygon1", "The first Polygon.", {"Polygon"}},
        {"polygon2", "The second Polygon", {"Polygon"}}
    };
    FunctionDocumentation::ReturnedValue returned_value = {"Returns the symmetric difference of the polygons as a MultiPolygon.", {"MultiPolygon"}};
    FunctionDocumentation::Examples examples =
    {
    {
        "Usage example",
        R"(
SELECT wkt(arraySort(polygonsSymDifferenceSpherical([[(50., 50.), (50., -50.), (-50., -50.), (-50., 50.), (50., 50.)], [(10., 10.), (10., 40.), (40., 40.), (40., 10.), (10., 10.)], [(-10., -10.), (-10., -40.), (-40., -40.), (-40., -10.), (-10., -10.)]], [[(-20., -20.), (-20., 20.), (20., 20.), (20., -20.), (-20., -20.)]])));
        )",
        R"(
MULTIPOLYGON(((-20 -10.3067,-10 -10,-10 -20.8791,-20 -20,-20 -10.3067)),((10 20.8791,20 20,20 10.3067,10 10,10 20.8791)),((50 50,50 -50,-50 -50,-50 50,50 50),(20 10.3067,40 10,40 40,10 40,10 20.8791,-20 20,-20 -10.3067,-40 -10,-40 -40,-10 -40,-10 -20.8791,20 -20,20 10.3067)))
        )"
    }
    };
    FunctionDocumentation::IntroducedIn introduced_in = {21, 4};
    FunctionDocumentation::Category category = FunctionDocumentation::Category::GeoPolygon;
    FunctionDocumentation function_documentation = {description, syntax, arguments, {}, returned_value, examples, introduced_in, category};

    factory.registerFunction<FunctionPolygonsSymDifference<SphericalPoint>>(function_documentation);
}

}
