#pragma once

#include <cstdint>
#include <optional>
#include <stdexcept>
#include <string_view>

namespace DB
{

enum class LocalFSReadMethod : uint8_t
{
    /**
     * Simple synchronous reads with 'read'.
     * Can use direct IO after specified size.
     * Can use prefetch by asking OS to perform readahead.
     */
    read,

    /**
     * Simple synchronous reads with 'pread'.
     * In contrast to 'read', shares single file descriptor from multiple threads.
     * Can use direct IO after specified size.
     * Can use prefetch by asking OS to perform readahead.
     */
    pread,

    /**
     * Use mmap after specified size or simple synchronous reads with 'pread'.
     * Can use prefetch by asking OS to perform readahead.
     */
    mmap,

    /**
     * Use the io_uring Linux subsystem for asynchronous reads.
     * Can use direct IO after specified size.
     * Can do prefetch with double buffering.
     */
    io_uring,

    /**
     * Checks if data is in page cache with 'preadv2' on modern Linux kernels.
     * If data is in page cache, read from the same thread.
     * If not, offload IO to separate threadpool.
     * Can do prefetch with double buffering.
     * Can use specified priorities and limit the number of concurrent reads.
     */
    pread_threadpool,

    /// Use asynchronous reader with fake backend that in fact synchronous.
    /// @attention Use only for testing purposes.
    pread_fake_async
};

inline std::string_view enumToString(LocalFSReadMethod method)
{
    switch (method)
    {
        case LocalFSReadMethod::read: return "read";
        case LocalFSReadMethod::pread: return "pread";
        case LocalFSReadMethod::mmap: return "mmap";
        case LocalFSReadMethod::io_uring: return "io_uring";
        case LocalFSReadMethod::pread_threadpool: return "pread_threadpool";
        case LocalFSReadMethod::pread_fake_async: return "pread_fake_async";
    }
    throw std::logic_error("Unknown LocalFSReadMethod");
}

inline std::optional<LocalFSReadMethod> localFSReadMethodFromString(std::string_view str)
{
    if (str == "read") return LocalFSReadMethod::read;
    if (str == "pread") return LocalFSReadMethod::pread;
    if (str == "mmap") return LocalFSReadMethod::mmap;
    if (str == "io_uring") return LocalFSReadMethod::io_uring;
    if (str == "pread_threadpool") return LocalFSReadMethod::pread_threadpool;
    if (str == "pread_fake_async") return LocalFSReadMethod::pread_fake_async;
    return std::nullopt;
}

enum class RemoteFSReadMethod : uint8_t
{
    read,
    threadpool,
};

inline std::string_view enumToString(RemoteFSReadMethod method)
{
    switch (method)
    {
        case RemoteFSReadMethod::read: return "read";
        case RemoteFSReadMethod::threadpool: return "threadpool";
    }
    throw std::logic_error("Unknown RemoteFSReadMethod");
}

inline std::optional<RemoteFSReadMethod> remoteFSReadMethodFromString(std::string_view str)
{
    if (str == "read") return RemoteFSReadMethod::read;
    if (str == "threadpool") return RemoteFSReadMethod::threadpool;
    return std::nullopt;
}

}
