#pragma once

#include <map>
#include <optional>
#include <string>
#include <vector>

#include <Poco/Util/AbstractConfiguration.h>

#include <Core/Field.h>
#include <Core/TypeId.h>
#include <IO/ReadBufferFromString.h>
#include <DataTypes/IDataType.h>
#include <Interpreters/IExternalLoadable.h>


namespace DB
{
using TypeIndexUnderlying = std::underlying_type_t<TypeIndex>;

// We need to be able to map TypeIndex -> AttributeUnderlyingType and AttributeUnderlyingType -> real type
// The first can be done by defining AttributeUnderlyingType enum values to TypeIndex values and then performing
// a enum_cast.
// The second can be achieved by using TypeIndexToType
#define map_item(__T) __T = static_cast<TypeIndexUnderlying>(TypeIndex::__T)

enum class AttributeUnderlyingType : TypeIndexUnderlying
{
    map_item(Int8), map_item(Int16), map_item(Int32), map_item(Int64), map_item(Int128), map_item(Int256),
    map_item(UInt8), map_item(UInt16), map_item(UInt32), map_item(UInt64), map_item(UInt128), map_item(UInt256),
    map_item(Float32), map_item(Float64),
    map_item(Decimal32), map_item(Decimal64), map_item(Decimal128), map_item(Decimal256),
    map_item(DateTime64),

    map_item(UUID), map_item(String), map_item(Array),

    map_item(IPv4), map_item(IPv6)
};

#undef map_item

/// Min and max lifetimes for a dictionary or its entry
using DictionaryLifetime = ExternalLoadableLifetime;

/** Holds the description of a single dictionary attribute:
*    - name, used for lookup into dictionary and source;
*    - type, used in conjunction with DataTypeFactory and getAttributeUnderlyingTypeByname;
*    - nested_type, contains nested type of complex type like Nullable, Array
*    - null_value, used as a default value for non-existent entries in the dictionary,
*        decimal representation for numeric attributes;
*    - hierarchical, whether this attribute defines a hierarchy;
*    - injective, whether the mapping to parent is injective (can be used for optimization of GROUP BY?);
*    - is_object_id, used in mongo dictionary, converts string key to objectid;
*    - is_nullable, is attribute nullable;
*/
struct DictionaryAttribute final
{
    const std::string name;
    const AttributeUnderlyingType underlying_type;
    const DataTypePtr type;
    const SerializationPtr type_serialization;
    const std::string expression;
    const Field null_value;
    const bool hierarchical;
    const bool bidirectional;
    const bool injective;
    const bool is_object_id;
    const bool is_nullable;
};

template <AttributeUnderlyingType type>
struct DictionaryAttributeType
{
    /// Converts @c type to it underlying type e.g. AttributeUnderlyingType::UInt8 -> UInt8
    using AttributeType = TypeIndexToType<
        static_cast<TypeIndex>(
            static_cast<TypeIndexUnderlying>(type))>;
};

template <typename F>
constexpr void callOnDictionaryAttributeType(AttributeUnderlyingType type, F && func)
{
    switch (type)
    {
        case AttributeUnderlyingType::Int8: func(DictionaryAttributeType<AttributeUnderlyingType::Int8>{}); break;
        case AttributeUnderlyingType::Int16: func(DictionaryAttributeType<AttributeUnderlyingType::Int16>{}); break;
        case AttributeUnderlyingType::Int32: func(DictionaryAttributeType<AttributeUnderlyingType::Int32>{}); break;
        case AttributeUnderlyingType::Int64: func(DictionaryAttributeType<AttributeUnderlyingType::Int64>{}); break;
        case AttributeUnderlyingType::Int128: func(DictionaryAttributeType<AttributeUnderlyingType::Int128>{}); break;
        case AttributeUnderlyingType::Int256: func(DictionaryAttributeType<AttributeUnderlyingType::Int256>{}); break;
        case AttributeUnderlyingType::UInt8: func(DictionaryAttributeType<AttributeUnderlyingType::UInt8>{}); break;
        case AttributeUnderlyingType::UInt16: func(DictionaryAttributeType<AttributeUnderlyingType::UInt16>{}); break;
        case AttributeUnderlyingType::UInt32: func(DictionaryAttributeType<AttributeUnderlyingType::UInt32>{}); break;
        case AttributeUnderlyingType::UInt64: func(DictionaryAttributeType<AttributeUnderlyingType::UInt64>{}); break;
        case AttributeUnderlyingType::UInt128: func(DictionaryAttributeType<AttributeUnderlyingType::UInt128>{}); break;
        case AttributeUnderlyingType::UInt256: func(DictionaryAttributeType<AttributeUnderlyingType::UInt256>{}); break;
        case AttributeUnderlyingType::Float32: func(DictionaryAttributeType<AttributeUnderlyingType::Float32>{}); break;
        case AttributeUnderlyingType::Float64: func(DictionaryAttributeType<AttributeUnderlyingType::Float64>{}); break;
        case AttributeUnderlyingType::Decimal32: func(DictionaryAttributeType<AttributeUnderlyingType::Decimal32>{}); break;
        case AttributeUnderlyingType::Decimal64: func(DictionaryAttributeType<AttributeUnderlyingType::Decimal64>{}); break;
        case AttributeUnderlyingType::Decimal128: func(DictionaryAttributeType<AttributeUnderlyingType::Decimal128>{}); break;
        case AttributeUnderlyingType::Decimal256: func(DictionaryAttributeType<AttributeUnderlyingType::Decimal256>{}); break;
        case AttributeUnderlyingType::DateTime64: func(DictionaryAttributeType<AttributeUnderlyingType::DateTime64>{}); break;
        case AttributeUnderlyingType::UUID: func(DictionaryAttributeType<AttributeUnderlyingType::UUID>{}); break;
        case AttributeUnderlyingType::String: func(DictionaryAttributeType<AttributeUnderlyingType::String>{}); break;
        case AttributeUnderlyingType::Array: func(DictionaryAttributeType<AttributeUnderlyingType::Array>{}); break;
        case AttributeUnderlyingType::IPv4: func(DictionaryAttributeType<AttributeUnderlyingType::IPv4>{}); break;
        case AttributeUnderlyingType::IPv6: func(DictionaryAttributeType<AttributeUnderlyingType::IPv6>{}); break;
    }
}

struct DictionaryTypedSpecialAttribute final
{
    const std::string name;
    const std::string expression;
    const DataTypePtr type;
};


/// Name of identifier plus list of attributes
struct DictionaryStructure final
{
    std::optional<DictionaryTypedSpecialAttribute> id;
    std::optional<std::vector<DictionaryAttribute>> key;
    std::vector<DictionaryAttribute> attributes;
    std::unordered_map<std::string, size_t> attribute_name_to_index;
    std::optional<DictionaryTypedSpecialAttribute> range_min;
    std::optional<DictionaryTypedSpecialAttribute> range_max;
    std::optional<size_t> hierarchical_attribute_index;

    bool has_expressions = false;
    bool access_to_key_from_attributes = false;

    DictionaryStructure(const Poco::Util::AbstractConfiguration & config, const std::string & config_prefix);

    DataTypes getKeyTypes() const;
    void validateKeyTypes(const DataTypes & key_types) const;

    bool hasAttribute(const std::string & attribute_name) const;
    const DictionaryAttribute & getAttribute(const std::string & attribute_name) const;
    const DictionaryAttribute & getAttribute(const std::string & attribute_name, const DataTypePtr & type) const;

    Strings getKeysNames() const;
    size_t getKeysSize() const;

    std::string getKeyDescription() const;

private:
    /// range_min and range_max have to be parsed before this function call
    std::vector<DictionaryAttribute> getAttributes(
        const Poco::Util::AbstractConfiguration & config,
        const std::string & config_prefix,
        bool complex_key_attributes);

    /// parse range_min and range_max
    void parseRangeConfiguration(const Poco::Util::AbstractConfiguration & config, const std::string & structure_prefix);

};

}
