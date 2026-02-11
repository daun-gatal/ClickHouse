#include <Functions/FunctionFactory.h>
#include <Functions/FunctionPolygonsIntersection.h>

namespace DB
{

template <>
const char * FunctionPolygonsIntersection<SphericalPoint>::name = "polygonsIntersectionSpherical";

REGISTER_FUNCTION(PolygonsIntersectionSpherical)
{
    FunctionDocumentation::Description description = R"(
Calculates the intersection (AND) between polygons, coordinates are spherical.
    )";
    FunctionDocumentation::Syntax syntax = "polygonsIntersectionSpherical(polygon1, polygon2)";
    FunctionDocumentation::Arguments arguments = {
        {"polygon1", "First Polygon with spherical coordinates.", {"Polygon"}},
        {"polygon2", "Second Polygon with spherical coordinates.", {"Polygon"}}
    };
    FunctionDocumentation::ReturnedValue returned_value = {"Returns the intersection of the polygons as a MultiPolygon.", {"MultiPolygon"}};
    FunctionDocumentation::Examples examples =
    {
    {
        "Spherical intersection example",
        R"(
SELECT wkt(arrayMap(a -> arrayMap(b -> arrayMap(c -> (round(c.1, 6), round(c.2, 6)), b), a), polygonsIntersectionSpherical([[[(4.3613577, 50.8651821), (4.349556, 50.8535879), (4.3602419, 50.8435626), (4.3830299, 50.8428851), (4.3904543, 50.8564867), (4.3613148, 50.8651279)]]], [[[(4.346693, 50.858306), (4.367945, 50.852455), (4.366227, 50.840809), (4.344961, 50.833264), (4.338074, 50.848677), (4.346693, 50.858306)]]]])))
        )",
        R"(
MULTIPOLYGON(((4.3666 50.8434,4.36024 50.8436,4.34956 50.8536,4.35268 50.8567,4.36794 50.8525,4.3666 50.8434)))
        )"
    }
    };
    FunctionDocumentation::IntroducedIn introduced_in = {21, 4};
    FunctionDocumentation::Category category = FunctionDocumentation::Category::GeoPolygon;
    FunctionDocumentation function_documentation = {description, syntax, arguments, {}, returned_value, examples, introduced_in, category};

    factory.registerFunction<FunctionPolygonsIntersection<SphericalPoint>>(function_documentation);
}

}
