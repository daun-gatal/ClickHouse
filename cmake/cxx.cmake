# Enable libcxx hardening, see https://libcxx.llvm.org/Hardening.html
if (CMAKE_BUILD_TYPE_UC STREQUAL "DEBUG")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_LIBCPP_HARDENING_MODE=_LIBCPP_HARDENING_MODE_EXTENSIVE")
else ()
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_LIBCPP_HARDENING_MODE=_LIBCPP_HARDENING_MODE_FAST")
endif ()

# Prevent <vector> and <ostream> from pulling in expensive <format> headers in C++23 mode.
# ClickHouse uses fmt::format, not std::format, so std::formatter<vector<bool>> and std::print
# on ostreams are not needed. This eliminates ~3000s of format header parsing across ~6000 TUs.
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_LIBCPP_NO_TRANSITIVE_FORMAT_INCLUDE")

disable_dummy_launchers_if_needed()
add_subdirectory(contrib/libcxxabi-cmake)
add_subdirectory(contrib/libcxx-cmake)
enable_dummy_launchers_if_needed()

# Exception handling library is embedded into libcxxabi.

target_link_libraries(global-libs INTERFACE cxx cxxabi)
