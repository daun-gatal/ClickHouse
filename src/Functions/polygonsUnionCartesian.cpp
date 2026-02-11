#include <Functions/FunctionFactory.h>
#include <Functions/FunctionPolygonsUnion.h>

namespace DB
{

template <>
const char * FunctionPolygonsUnion<CartesianPoint>::name = "polygonsUnionCartesian";

REGISTER_FUNCTION(PolygonsUnionCartesian)
{
    FunctionDocumentation::Description description = R"(
Calculates the union of polygons.
    )";
    FunctionDocumentation::Syntax syntax = "polygonsUnionCartesian(polygon1, polygon2)";
    FunctionDocumentation::Arguments arguments = {
        {"polygon1", "The first Polygon.", {"Polygon"}},
        {"polygon2", "The second Polygon.", {"Polygon"}}
    };
    FunctionDocumentation::ReturnedValue returned_value = {"Returns the union of the polygons as a MultiPolygon.", {"MultiPolygon"}};
    FunctionDocumentation::Examples examples =
    {
    {
        "Usage example",
        R"(
SELECT wkt(polygonsUnionCartesian([[[(0., 0.), (0., 3.), (1., 2.9), (2., 2.6), (2.6, 2.), (2.9, 1), (3., 0.), (0., 0.)]]], [[[(1., 1.), (1., 4.), (4., 4.), (4., 1.), (1., 1.)]]]))
        )",
        R"(
MULTIPOLYGON(((1 2.9,1 4,4 4,4 1,2.9 1,3 0,0 0,0 3,1 2.9)))
        )"
    }
    };
    FunctionDocumentation::IntroducedIn introduced_in = {21, 4};
    FunctionDocumentation::Category category = FunctionDocumentation::Category::GeoPolygon;
    FunctionDocumentation function_documentation = {description, syntax, arguments, {}, returned_value, examples, introduced_in, category};

    factory.registerFunction<FunctionPolygonsUnion<CartesianPoint>>(function_documentation);
}

}
