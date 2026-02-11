#include <Common/ExternalLoaderStatus.h>

#include <array>

namespace DB
{

static constexpr std::array<std::pair<ExternalLoaderStatus, std::string_view>, 7> external_loader_status_entries =
{{
    {ExternalLoaderStatus::NOT_LOADED, "NOT_LOADED"},
    {ExternalLoaderStatus::LOADED, "LOADED"},
    {ExternalLoaderStatus::FAILED, "FAILED"},
    {ExternalLoaderStatus::LOADING, "LOADING"},
    {ExternalLoaderStatus::FAILED_AND_RELOADING, "FAILED_AND_RELOADING"},
    {ExternalLoaderStatus::LOADED_AND_RELOADING, "LOADED_AND_RELOADING"},
    {ExternalLoaderStatus::NOT_EXIST, "NOT_EXIST"},
}};

std::vector<std::pair<String, Int8>> getExternalLoaderStatusEnumAllPossibleValues()
{
    std::vector<std::pair<String, Int8>> out;
    out.reserve(external_loader_status_entries.size());

    for (const auto & [value, str] : external_loader_status_entries)
        out.emplace_back(std::string{str}, static_cast<Int8>(value));

    return out;
}

}
