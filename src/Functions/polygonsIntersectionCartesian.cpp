#include <Functions/FunctionFactory.h>
#include <Functions/FunctionPolygonsIntersection.h>

namespace DB
{

template <>
const char * FunctionPolygonsIntersection<CartesianPoint>::name = "polygonsIntersectionCartesian";

REGISTER_FUNCTION(PolygonsIntersectionCartesian)
{
    FunctionDocumentation::Description description = R"(
Calculates the intersection of polygons.
    )";
    FunctionDocumentation::Syntax syntax = "polygonsIntersectionCartesian(polygon1, polygon2)";
    FunctionDocumentation::Arguments arguments = {
        {"polygon1", "The first Polygon.", {"Polygon"}},
        {"polygon2", "The second Polygon.", {"Polygon"}}
    };
    FunctionDocumentation::ReturnedValue returned_value = {"Returns the intersection of the polygons as a MultiPolygon.", {"MultiPolygon"}};
    FunctionDocumentation::Examples examples =
    {
    {
        "Intersection example",
        R"(
SELECT wkt(polygonsIntersectionCartesian([[[(0., 0.), (0., 3.), (1., 2.9), (2., 2.6), (2.6, 2.), (2.9, 1.), (3., 0.), (0., 0.)]]], [[[(1., 1.), (1., 4.), (4., 4.), (4., 1.), (1., 1.)]]]))
        )",
        R"(
MULTIPOLYGON(((1 2.9,2 2.6,2.6 2,2.9 1,1 1,1 2.9)))
        )"
    }
    };
    FunctionDocumentation::IntroducedIn introduced_in = {21, 4};
    FunctionDocumentation::Category category = FunctionDocumentation::Category::GeoPolygon;
    FunctionDocumentation function_documentation = {description, syntax, arguments, {}, returned_value, examples, introduced_in, category};

    factory.registerFunction<FunctionPolygonsIntersection<CartesianPoint>>(function_documentation);
}

}
