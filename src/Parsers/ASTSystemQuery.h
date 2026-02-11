#pragma once

#include <Parsers/ASTQueryWithOnCluster.h>
#include <Parsers/IAST.h>
#include <Parsers/SyncReplicaMode.h>
#include <Server/ServerType.h>

#include "config.h"

#if USE_XRAY
#include <Interpreters/InstrumentationManager.h>
#include <variant>
#endif

namespace DB
{

class ASTSystemQuery : public IAST, public ASTQueryWithOnCluster
{
public:
    enum class Type : UInt64
    {
        UNKNOWN,
        SHUTDOWN,
        KILL,
        SUSPEND,
        CLEAR_DNS_CACHE,
        CLEAR_CONNECTIONS_CACHE,
        PREWARM_MARK_CACHE,
        PREWARM_PRIMARY_INDEX_CACHE,
        CLEAR_MARK_CACHE,
        CLEAR_PRIMARY_INDEX_CACHE,
        CLEAR_UNCOMPRESSED_CACHE,
        CLEAR_INDEX_MARK_CACHE,
        CLEAR_INDEX_UNCOMPRESSED_CACHE,
        CLEAR_VECTOR_SIMILARITY_INDEX_CACHE,
        CLEAR_TEXT_INDEX_DICTIONARY_CACHE,
        CLEAR_TEXT_INDEX_HEADER_CACHE,
        CLEAR_TEXT_INDEX_POSTINGS_CACHE,
        CLEAR_TEXT_INDEX_CACHES,
        CLEAR_MMAP_CACHE,
        CLEAR_QUERY_CONDITION_CACHE,
        CLEAR_QUERY_CACHE,
        CLEAR_COMPILED_EXPRESSION_CACHE,
        CLEAR_ICEBERG_METADATA_CACHE,
        CLEAR_PARQUET_METADATA_CACHE,
        CLEAR_FILESYSTEM_CACHE,
        CLEAR_DISTRIBUTED_CACHE,
        CLEAR_DISK_METADATA_CACHE,
        CLEAR_PAGE_CACHE,
        CLEAR_SCHEMA_CACHE,
        CLEAR_FORMAT_SCHEMA_CACHE,
        CLEAR_S3_CLIENT_CACHE,
        STOP_LISTEN,
        START_LISTEN,
        RESTART_REPLICAS,
        RESTART_REPLICA,
        RESTORE_REPLICA,
        RESTORE_DATABASE_REPLICA,
        WAIT_LOADING_PARTS,
        DROP_REPLICA,
        DROP_DATABASE_REPLICA,
        DROP_CATALOG_REPLICA,
        JEMALLOC_PURGE,
        JEMALLOC_ENABLE_PROFILE,
        JEMALLOC_DISABLE_PROFILE,
        JEMALLOC_FLUSH_PROFILE,
        SYNC_REPLICA,
        SYNC_DATABASE_REPLICA,
        SYNC_TRANSACTION_LOG,
        SYNC_FILE_CACHE,
        REPLICA_READY,
        REPLICA_UNREADY,
        RELOAD_DICTIONARY,
        RELOAD_DICTIONARIES,
        RELOAD_MODEL,
        RELOAD_MODELS,
        RELOAD_FUNCTION,
        RELOAD_FUNCTIONS,
        RELOAD_EMBEDDED_DICTIONARIES,
        RELOAD_CONFIG,
        RELOAD_USERS,
        RELOAD_ASYNCHRONOUS_METRICS,
        RESTART_DISK,
        STOP_MERGES,
        START_MERGES,
        STOP_TTL_MERGES,
        START_TTL_MERGES,
        STOP_FETCHES,
        START_FETCHES,
        STOP_MOVES,
        START_MOVES,
        STOP_REPLICATED_SENDS,
        START_REPLICATED_SENDS,
        STOP_REPLICATION_QUEUES,
        START_REPLICATION_QUEUES,
        STOP_REPLICATED_DDL_QUERIES,
        START_REPLICATED_DDL_QUERIES,
        FLUSH_LOGS,
        FLUSH_DISTRIBUTED,
        FLUSH_ASYNC_INSERT_QUEUE,
        STOP_DISTRIBUTED_SENDS,
        START_DISTRIBUTED_SENDS,
        START_THREAD_FUZZER,
        STOP_THREAD_FUZZER,
        UNFREEZE,
        ENABLE_FAILPOINT,
        DISABLE_FAILPOINT,
        WAIT_FAILPOINT,
        NOTIFY_FAILPOINT,
        SYNC_FILESYSTEM_CACHE,
        STOP_PULLING_REPLICATION_LOG,
        START_PULLING_REPLICATION_LOG,
        STOP_CLEANUP,
        START_CLEANUP,
        RESET_COVERAGE,
        REFRESH_VIEW,
        WAIT_VIEW,
        START_VIEW,
        START_VIEWS,
        START_REPLICATED_VIEW,
        STOP_VIEW,
        STOP_VIEWS,
        STOP_REPLICATED_VIEW,
        CANCEL_VIEW,
        TEST_VIEW,
        LOAD_PRIMARY_KEY,
        UNLOAD_PRIMARY_KEY,
        STOP_VIRTUAL_PARTS_UPDATE,
        START_VIRTUAL_PARTS_UPDATE,
        STOP_REDUCE_BLOCKING_PARTS,
        START_REDUCE_BLOCKING_PARTS,
        UNLOCK_SNAPSHOT,
        RECONNECT_ZOOKEEPER,
        INSTRUMENT_ADD,
        INSTRUMENT_REMOVE,
        RESET_DDL_WORKER,
        END
    };

    static const char * typeToString(Type type);

    Type type = Type::UNKNOWN;

    ASTPtr database;
    ASTPtr table;
    bool if_exists = false;
    ASTPtr query_settings;

    String getDatabase() const;
    String getTable() const;

    void setDatabase(const String & name);
    void setTable(const String & name);

    String target_model;
    String target_function;
    String replica;
    String shard;
    String replica_zk_path;
    bool is_drop_whole_replica{};
    bool with_tables{false};
    String storage_policy;
    String volume;
    String disk;
    UInt64 seconds{};

    std::optional<String> query_result_cache_tag;

    String filesystem_cache_name;
    String distributed_cache_server_id;
    bool distributed_cache_drop_connections = false;

    std::string key_to_drop;
    std::optional<size_t> offset_to_drop;

    String backup_name;
    ASTPtr backup_source; /// SYSTEM UNFREEZE SNAPSHOT `backup_name` FROM `backup_source`

    String schema_cache_storage;

    String schema_cache_format;

    String fail_point_name;

    enum class FailPointAction
    {
        UNSPECIFIED,
        PAUSE,
        RESUME
    };
    FailPointAction fail_point_action = FailPointAction::UNSPECIFIED;

    SyncReplicaMode sync_replica_mode = SyncReplicaMode::DEFAULT;

    std::vector<String> src_replicas;

    std::vector<std::pair<String, String>> tables;

    ServerType server_type;

#if USE_XRAY
    /// For SYSTEM INSTRUMENT ADD/REMOVE
    using InstrumentParameter = std::variant<String, Int64, Float64>;
    String instrumentation_function_name;
    String instrumentation_handler_name;
    Instrumentation::EntryType instrumentation_entry_type;
    std::optional<std::variant<UInt64, Instrumentation::All, String>> instrumentation_point;
    std::vector<InstrumentParameter> instrumentation_parameters;
    String instrumentation_subquery;
#endif

    /// For SYSTEM TEST VIEW <name> (SET FAKE TIME <time> | UNSET FAKE TIME).
    /// Unix time.
    std::optional<Int64> fake_time_for_view;

    String getID(char) const override { return "SYSTEM query"; }

    ASTPtr clone() const override
    {
        auto res = make_intrusive<ASTSystemQuery>(*this);
        res->children.clear();

        if (database) { res->database = database->clone(); res->children.push_back(res->database); }
        if (table) { res->table = table->clone(); res->children.push_back(res->table); }
        if (query_settings) { res->query_settings = query_settings->clone(); res->children.push_back(res->query_settings); }
        if (backup_source) { res->backup_source = backup_source->clone(); res->children.push_back(res->backup_source); }

        return res;
    }

    ASTPtr getRewrittenASTWithoutOnCluster(const WithoutOnClusterASTRewriteParams & params) const override
    {
        return removeOnCluster<ASTSystemQuery>(clone(), params.default_database);
    }

    QueryKind getQueryKind() const override { return QueryKind::System; }

protected:
    void formatImpl(WriteBuffer & ostr, const FormatSettings & settings, FormatState & state, FormatStateStacked frame) const override;
};

inline std::string_view enumToString(ASTSystemQuery::Type type)
{
    switch (type)
    {
        case ASTSystemQuery::Type::UNKNOWN:                              return "UNKNOWN";
        case ASTSystemQuery::Type::SHUTDOWN:                             return "SHUTDOWN";
        case ASTSystemQuery::Type::KILL:                                 return "KILL";
        case ASTSystemQuery::Type::SUSPEND:                              return "SUSPEND";
        case ASTSystemQuery::Type::CLEAR_DNS_CACHE:                      return "CLEAR_DNS_CACHE";
        case ASTSystemQuery::Type::CLEAR_CONNECTIONS_CACHE:              return "CLEAR_CONNECTIONS_CACHE";
        case ASTSystemQuery::Type::PREWARM_MARK_CACHE:                   return "PREWARM_MARK_CACHE";
        case ASTSystemQuery::Type::PREWARM_PRIMARY_INDEX_CACHE:          return "PREWARM_PRIMARY_INDEX_CACHE";
        case ASTSystemQuery::Type::CLEAR_MARK_CACHE:                     return "CLEAR_MARK_CACHE";
        case ASTSystemQuery::Type::CLEAR_PRIMARY_INDEX_CACHE:            return "CLEAR_PRIMARY_INDEX_CACHE";
        case ASTSystemQuery::Type::CLEAR_UNCOMPRESSED_CACHE:             return "CLEAR_UNCOMPRESSED_CACHE";
        case ASTSystemQuery::Type::CLEAR_INDEX_MARK_CACHE:               return "CLEAR_INDEX_MARK_CACHE";
        case ASTSystemQuery::Type::CLEAR_INDEX_UNCOMPRESSED_CACHE:       return "CLEAR_INDEX_UNCOMPRESSED_CACHE";
        case ASTSystemQuery::Type::CLEAR_VECTOR_SIMILARITY_INDEX_CACHE:  return "CLEAR_VECTOR_SIMILARITY_INDEX_CACHE";
        case ASTSystemQuery::Type::CLEAR_TEXT_INDEX_DICTIONARY_CACHE:    return "CLEAR_TEXT_INDEX_DICTIONARY_CACHE";
        case ASTSystemQuery::Type::CLEAR_TEXT_INDEX_HEADER_CACHE:        return "CLEAR_TEXT_INDEX_HEADER_CACHE";
        case ASTSystemQuery::Type::CLEAR_TEXT_INDEX_POSTINGS_CACHE:      return "CLEAR_TEXT_INDEX_POSTINGS_CACHE";
        case ASTSystemQuery::Type::CLEAR_TEXT_INDEX_CACHES:              return "CLEAR_TEXT_INDEX_CACHES";
        case ASTSystemQuery::Type::CLEAR_MMAP_CACHE:                     return "CLEAR_MMAP_CACHE";
        case ASTSystemQuery::Type::CLEAR_QUERY_CONDITION_CACHE:          return "CLEAR_QUERY_CONDITION_CACHE";
        case ASTSystemQuery::Type::CLEAR_QUERY_CACHE:                    return "CLEAR_QUERY_CACHE";
        case ASTSystemQuery::Type::CLEAR_COMPILED_EXPRESSION_CACHE:      return "CLEAR_COMPILED_EXPRESSION_CACHE";
        case ASTSystemQuery::Type::CLEAR_ICEBERG_METADATA_CACHE:         return "CLEAR_ICEBERG_METADATA_CACHE";
        case ASTSystemQuery::Type::CLEAR_PARQUET_METADATA_CACHE:         return "CLEAR_PARQUET_METADATA_CACHE";
        case ASTSystemQuery::Type::CLEAR_FILESYSTEM_CACHE:               return "CLEAR_FILESYSTEM_CACHE";
        case ASTSystemQuery::Type::CLEAR_DISTRIBUTED_CACHE:              return "CLEAR_DISTRIBUTED_CACHE";
        case ASTSystemQuery::Type::CLEAR_DISK_METADATA_CACHE:            return "CLEAR_DISK_METADATA_CACHE";
        case ASTSystemQuery::Type::CLEAR_PAGE_CACHE:                     return "CLEAR_PAGE_CACHE";
        case ASTSystemQuery::Type::CLEAR_SCHEMA_CACHE:                   return "CLEAR_SCHEMA_CACHE";
        case ASTSystemQuery::Type::CLEAR_FORMAT_SCHEMA_CACHE:            return "CLEAR_FORMAT_SCHEMA_CACHE";
        case ASTSystemQuery::Type::CLEAR_S3_CLIENT_CACHE:                return "CLEAR_S3_CLIENT_CACHE";
        case ASTSystemQuery::Type::STOP_LISTEN:                          return "STOP_LISTEN";
        case ASTSystemQuery::Type::START_LISTEN:                         return "START_LISTEN";
        case ASTSystemQuery::Type::RESTART_REPLICAS:                     return "RESTART_REPLICAS";
        case ASTSystemQuery::Type::RESTART_REPLICA:                      return "RESTART_REPLICA";
        case ASTSystemQuery::Type::RESTORE_REPLICA:                      return "RESTORE_REPLICA";
        case ASTSystemQuery::Type::RESTORE_DATABASE_REPLICA:             return "RESTORE_DATABASE_REPLICA";
        case ASTSystemQuery::Type::WAIT_LOADING_PARTS:                   return "WAIT_LOADING_PARTS";
        case ASTSystemQuery::Type::DROP_REPLICA:                         return "DROP_REPLICA";
        case ASTSystemQuery::Type::DROP_DATABASE_REPLICA:                return "DROP_DATABASE_REPLICA";
        case ASTSystemQuery::Type::DROP_CATALOG_REPLICA:                 return "DROP_CATALOG_REPLICA";
        case ASTSystemQuery::Type::JEMALLOC_PURGE:                       return "JEMALLOC_PURGE";
        case ASTSystemQuery::Type::JEMALLOC_ENABLE_PROFILE:              return "JEMALLOC_ENABLE_PROFILE";
        case ASTSystemQuery::Type::JEMALLOC_DISABLE_PROFILE:             return "JEMALLOC_DISABLE_PROFILE";
        case ASTSystemQuery::Type::JEMALLOC_FLUSH_PROFILE:               return "JEMALLOC_FLUSH_PROFILE";
        case ASTSystemQuery::Type::SYNC_REPLICA:                         return "SYNC_REPLICA";
        case ASTSystemQuery::Type::SYNC_DATABASE_REPLICA:                return "SYNC_DATABASE_REPLICA";
        case ASTSystemQuery::Type::SYNC_TRANSACTION_LOG:                 return "SYNC_TRANSACTION_LOG";
        case ASTSystemQuery::Type::SYNC_FILE_CACHE:                      return "SYNC_FILE_CACHE";
        case ASTSystemQuery::Type::REPLICA_READY:                        return "REPLICA_READY";
        case ASTSystemQuery::Type::REPLICA_UNREADY:                      return "REPLICA_UNREADY";
        case ASTSystemQuery::Type::RELOAD_DICTIONARY:                    return "RELOAD_DICTIONARY";
        case ASTSystemQuery::Type::RELOAD_DICTIONARIES:                  return "RELOAD_DICTIONARIES";
        case ASTSystemQuery::Type::RELOAD_MODEL:                         return "RELOAD_MODEL";
        case ASTSystemQuery::Type::RELOAD_MODELS:                        return "RELOAD_MODELS";
        case ASTSystemQuery::Type::RELOAD_FUNCTION:                      return "RELOAD_FUNCTION";
        case ASTSystemQuery::Type::RELOAD_FUNCTIONS:                     return "RELOAD_FUNCTIONS";
        case ASTSystemQuery::Type::RELOAD_EMBEDDED_DICTIONARIES:         return "RELOAD_EMBEDDED_DICTIONARIES";
        case ASTSystemQuery::Type::RELOAD_CONFIG:                        return "RELOAD_CONFIG";
        case ASTSystemQuery::Type::RELOAD_USERS:                         return "RELOAD_USERS";
        case ASTSystemQuery::Type::RELOAD_ASYNCHRONOUS_METRICS:          return "RELOAD_ASYNCHRONOUS_METRICS";
        case ASTSystemQuery::Type::RESTART_DISK:                         return "RESTART_DISK";
        case ASTSystemQuery::Type::STOP_MERGES:                          return "STOP_MERGES";
        case ASTSystemQuery::Type::START_MERGES:                         return "START_MERGES";
        case ASTSystemQuery::Type::STOP_TTL_MERGES:                      return "STOP_TTL_MERGES";
        case ASTSystemQuery::Type::START_TTL_MERGES:                     return "START_TTL_MERGES";
        case ASTSystemQuery::Type::STOP_FETCHES:                         return "STOP_FETCHES";
        case ASTSystemQuery::Type::START_FETCHES:                        return "START_FETCHES";
        case ASTSystemQuery::Type::STOP_MOVES:                           return "STOP_MOVES";
        case ASTSystemQuery::Type::START_MOVES:                          return "START_MOVES";
        case ASTSystemQuery::Type::STOP_REPLICATED_SENDS:                return "STOP_REPLICATED_SENDS";
        case ASTSystemQuery::Type::START_REPLICATED_SENDS:               return "START_REPLICATED_SENDS";
        case ASTSystemQuery::Type::STOP_REPLICATION_QUEUES:              return "STOP_REPLICATION_QUEUES";
        case ASTSystemQuery::Type::START_REPLICATION_QUEUES:             return "START_REPLICATION_QUEUES";
        case ASTSystemQuery::Type::STOP_REPLICATED_DDL_QUERIES:          return "STOP_REPLICATED_DDL_QUERIES";
        case ASTSystemQuery::Type::START_REPLICATED_DDL_QUERIES:         return "START_REPLICATED_DDL_QUERIES";
        case ASTSystemQuery::Type::FLUSH_LOGS:                           return "FLUSH_LOGS";
        case ASTSystemQuery::Type::FLUSH_DISTRIBUTED:                    return "FLUSH_DISTRIBUTED";
        case ASTSystemQuery::Type::FLUSH_ASYNC_INSERT_QUEUE:             return "FLUSH_ASYNC_INSERT_QUEUE";
        case ASTSystemQuery::Type::STOP_DISTRIBUTED_SENDS:               return "STOP_DISTRIBUTED_SENDS";
        case ASTSystemQuery::Type::START_DISTRIBUTED_SENDS:              return "START_DISTRIBUTED_SENDS";
        case ASTSystemQuery::Type::START_THREAD_FUZZER:                  return "START_THREAD_FUZZER";
        case ASTSystemQuery::Type::STOP_THREAD_FUZZER:                   return "STOP_THREAD_FUZZER";
        case ASTSystemQuery::Type::UNFREEZE:                             return "UNFREEZE";
        case ASTSystemQuery::Type::ENABLE_FAILPOINT:                     return "ENABLE_FAILPOINT";
        case ASTSystemQuery::Type::DISABLE_FAILPOINT:                    return "DISABLE_FAILPOINT";
        case ASTSystemQuery::Type::WAIT_FAILPOINT:                       return "WAIT_FAILPOINT";
        case ASTSystemQuery::Type::NOTIFY_FAILPOINT:                     return "NOTIFY_FAILPOINT";
        case ASTSystemQuery::Type::SYNC_FILESYSTEM_CACHE:                return "SYNC_FILESYSTEM_CACHE";
        case ASTSystemQuery::Type::STOP_PULLING_REPLICATION_LOG:         return "STOP_PULLING_REPLICATION_LOG";
        case ASTSystemQuery::Type::START_PULLING_REPLICATION_LOG:        return "START_PULLING_REPLICATION_LOG";
        case ASTSystemQuery::Type::STOP_CLEANUP:                         return "STOP_CLEANUP";
        case ASTSystemQuery::Type::START_CLEANUP:                        return "START_CLEANUP";
        case ASTSystemQuery::Type::RESET_COVERAGE:                       return "RESET_COVERAGE";
        case ASTSystemQuery::Type::REFRESH_VIEW:                         return "REFRESH_VIEW";
        case ASTSystemQuery::Type::WAIT_VIEW:                            return "WAIT_VIEW";
        case ASTSystemQuery::Type::START_VIEW:                           return "START_VIEW";
        case ASTSystemQuery::Type::START_VIEWS:                          return "START_VIEWS";
        case ASTSystemQuery::Type::START_REPLICATED_VIEW:                return "START_REPLICATED_VIEW";
        case ASTSystemQuery::Type::STOP_VIEW:                            return "STOP_VIEW";
        case ASTSystemQuery::Type::STOP_VIEWS:                           return "STOP_VIEWS";
        case ASTSystemQuery::Type::STOP_REPLICATED_VIEW:                 return "STOP_REPLICATED_VIEW";
        case ASTSystemQuery::Type::CANCEL_VIEW:                          return "CANCEL_VIEW";
        case ASTSystemQuery::Type::TEST_VIEW:                            return "TEST_VIEW";
        case ASTSystemQuery::Type::LOAD_PRIMARY_KEY:                     return "LOAD_PRIMARY_KEY";
        case ASTSystemQuery::Type::UNLOAD_PRIMARY_KEY:                   return "UNLOAD_PRIMARY_KEY";
        case ASTSystemQuery::Type::STOP_VIRTUAL_PARTS_UPDATE:            return "STOP_VIRTUAL_PARTS_UPDATE";
        case ASTSystemQuery::Type::START_VIRTUAL_PARTS_UPDATE:           return "START_VIRTUAL_PARTS_UPDATE";
        case ASTSystemQuery::Type::STOP_REDUCE_BLOCKING_PARTS:           return "STOP_REDUCE_BLOCKING_PARTS";
        case ASTSystemQuery::Type::START_REDUCE_BLOCKING_PARTS:          return "START_REDUCE_BLOCKING_PARTS";
        case ASTSystemQuery::Type::UNLOCK_SNAPSHOT:                      return "UNLOCK_SNAPSHOT";
        case ASTSystemQuery::Type::RECONNECT_ZOOKEEPER:                  return "RECONNECT_ZOOKEEPER";
        case ASTSystemQuery::Type::INSTRUMENT_ADD:                       return "INSTRUMENT_ADD";
        case ASTSystemQuery::Type::INSTRUMENT_REMOVE:                    return "INSTRUMENT_REMOVE";
        case ASTSystemQuery::Type::RESET_DDL_WORKER:                     return "RESET_DDL_WORKER";
        case ASTSystemQuery::Type::END:                                  return "END";
    }
    throw std::runtime_error("Unknown ASTSystemQuery::Type");
}

}
