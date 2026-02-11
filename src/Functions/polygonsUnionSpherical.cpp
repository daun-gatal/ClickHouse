#include <Functions/FunctionFactory.h>
#include <Functions/FunctionPolygonsUnion.h>

namespace DB
{

template <>
const char * FunctionPolygonsUnion<SphericalPoint>::name = "polygonsUnionSpherical";

REGISTER_FUNCTION(PolygonsUnionSpherical)
{
    FunctionDocumentation::Description description = R"(
Calculates a union (OR) of polygons.
    )";
    FunctionDocumentation::Syntax syntax = "polygonsUnionSpherical(polygon1, polygon2)";
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
SELECT wkt(polygonsUnionSpherical([[[(4.3613577, 50.8651821), (4.349556, 50.8535879), (4.3602419, 50.8435626), (4.3830299, 50.8428851), (4.3904543, 50.8564867), (4.3613148, 50.8651279)]]], [[[(4.346693, 50.858306), (4.367945, 50.852455), (4.366227, 50.840809), (4.344961, 50.833264), (4.338074, 50.848677), (4.346693, 50.858306)]]]))
        )",
        R"(
MULTIPOLYGON(((4.36661 50.8434,4.36623 50.8408,4.34496 50.8333,4.33807 50.8487,4.34669 50.8583,4.35268 50.8567,4.36136 50.8652,4.36131 50.8651,4.39045 50.8565,4.38303 50.8429,4.36661 50.8434)))
        )"
    }
    };
    FunctionDocumentation::IntroducedIn introduced_in = {21, 4};
    FunctionDocumentation::Category category = FunctionDocumentation::Category::GeoPolygon;
    FunctionDocumentation function_documentation = {description, syntax, arguments, {}, returned_value, examples, introduced_in, category};

    factory.registerFunction<FunctionPolygonsUnion<SphericalPoint>>(function_documentation);
}

}
