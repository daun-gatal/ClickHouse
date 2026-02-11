#include <Common/ExternalLoaderStatus.h>

namespace DB
{

std::vector<std::pair<String, Int8>> getExternalLoaderStatusEnumAllPossibleValues()
{
    std::vector<std::pair<String, Int8>> out;
    out.reserve(ExternalLoaderStatusCount);

    static constexpr ExternalLoaderStatus statuses[] = {
        ExternalLoaderStatus::NOT_LOADED,
        ExternalLoaderStatus::LOADED,
        ExternalLoaderStatus::FAILED,
        ExternalLoaderStatus::LOADING,
        ExternalLoaderStatus::FAILED_AND_RELOADING,
        ExternalLoaderStatus::LOADED_AND_RELOADING,
        ExternalLoaderStatus::NOT_EXIST,
    };

    for (auto status : statuses)
        out.emplace_back(std::string{enumToString(status)}, static_cast<Int8>(status));

    return out;
}

}
