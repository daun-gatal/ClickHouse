#include <Parsers/ASTIdentifier.h>
#include <Parsers/IAST.h>
#include <Parsers/IAST_erase.h>
#include <Parsers/ASTSystemQuery.h>
#include <Poco/String.h>
#include <Common/quoteString.h>
#include <Interpreters/InstrumentationManager.h>
#include <IO/WriteBuffer.h>
#include <IO/Operators.h>

namespace DB
{

namespace ErrorCodes
{
    extern const int LOGICAL_ERROR;
}

const char * ASTSystemQuery::typeToString(Type type)
{
    /** During parsing if SystemQuery is not parsed properly it is added to Expected variants as description check IParser.h.
      * Description string must be statically allocated.
      */
    switch (type)
    {
        case Type::UNKNOWN:                      return "UNKNOWN";
        case Type::SHUTDOWN:                     return "SHUTDOWN";
        case Type::KILL:                         return "KILL";
        case Type::SUSPEND:                      return "SUSPEND";
        case Type::CLEAR_DNS_CACHE:              return "CLEAR DNS CACHE";
        case Type::CLEAR_CONNECTIONS_CACHE:      return "CLEAR CONNECTIONS CACHE";
        case Type::PREWARM_MARK_CACHE:           return "PREWARM MARK CACHE";
        case Type::PREWARM_PRIMARY_INDEX_CACHE:  return "PREWARM PRIMARY INDEX CACHE";
        case Type::CLEAR_MARK_CACHE:             return "CLEAR MARK CACHE";
        case Type::CLEAR_PRIMARY_INDEX_CACHE:    return "CLEAR PRIMARY INDEX CACHE";
        case Type::CLEAR_UNCOMPRESSED_CACHE:     return "CLEAR UNCOMPRESSED CACHE";
        case Type::CLEAR_INDEX_MARK_CACHE:       return "CLEAR INDEX MARK CACHE";
        case Type::CLEAR_INDEX_UNCOMPRESSED_CACHE: return "CLEAR INDEX UNCOMPRESSED CACHE";
        case Type::CLEAR_VECTOR_SIMILARITY_INDEX_CACHE: return "CLEAR VECTOR SIMILARITY INDEX CACHE";
        case Type::CLEAR_TEXT_INDEX_DICTIONARY_CACHE: return "CLEAR TEXT INDEX DICTIONARY CACHE";
        case Type::CLEAR_TEXT_INDEX_HEADER_CACHE: return "CLEAR TEXT INDEX HEADER CACHE";
        case Type::CLEAR_TEXT_INDEX_POSTINGS_CACHE: return "CLEAR TEXT INDEX POSTINGS CACHE";
        case Type::CLEAR_TEXT_INDEX_CACHES:      return "CLEAR TEXT INDEX CACHES";
        case Type::CLEAR_MMAP_CACHE:             return "CLEAR MMAP CACHE";
        case Type::CLEAR_QUERY_CONDITION_CACHE:  return "CLEAR QUERY CONDITION CACHE";
        case Type::CLEAR_QUERY_CACHE:            return "CLEAR QUERY CACHE";
        case Type::CLEAR_COMPILED_EXPRESSION_CACHE: return "CLEAR COMPILED EXPRESSION CACHE";
        case Type::CLEAR_ICEBERG_METADATA_CACHE: return "CLEAR ICEBERG METADATA CACHE";
        case Type::CLEAR_PARQUET_METADATA_CACHE: return "CLEAR PARQUET METADATA CACHE";
        case Type::CLEAR_FILESYSTEM_CACHE:       return "CLEAR FILESYSTEM CACHE";
        case Type::CLEAR_DISTRIBUTED_CACHE:      return "CLEAR DISTRIBUTED CACHE";
        case Type::CLEAR_DISK_METADATA_CACHE:    return "CLEAR DISK METADATA CACHE";
        case Type::CLEAR_PAGE_CACHE:             return "CLEAR PAGE CACHE";
        case Type::CLEAR_SCHEMA_CACHE:           return "CLEAR SCHEMA CACHE";
        case Type::CLEAR_FORMAT_SCHEMA_CACHE:    return "CLEAR FORMAT SCHEMA CACHE";
        case Type::CLEAR_S3_CLIENT_CACHE:        return "CLEAR S3 CLIENT CACHE";
        case Type::STOP_LISTEN:                  return "STOP LISTEN";
        case Type::START_LISTEN:                 return "START LISTEN";
        case Type::RESTART_REPLICAS:             return "RESTART REPLICAS";
        case Type::RESTART_REPLICA:              return "RESTART REPLICA";
        case Type::RESTORE_REPLICA:              return "RESTORE REPLICA";
        case Type::RESTORE_DATABASE_REPLICA:     return "RESTORE DATABASE REPLICA";
        case Type::WAIT_LOADING_PARTS:           return "WAIT LOADING PARTS";
        case Type::DROP_REPLICA:                 return "DROP REPLICA";
        case Type::DROP_DATABASE_REPLICA:        return "DROP DATABASE REPLICA";
        case Type::DROP_CATALOG_REPLICA:         return "DROP CATALOG REPLICA";
        case Type::JEMALLOC_PURGE:               return "JEMALLOC PURGE";
        case Type::JEMALLOC_ENABLE_PROFILE:      return "JEMALLOC ENABLE PROFILE";
        case Type::JEMALLOC_DISABLE_PROFILE:     return "JEMALLOC DISABLE PROFILE";
        case Type::JEMALLOC_FLUSH_PROFILE:       return "JEMALLOC FLUSH PROFILE";
        case Type::SYNC_REPLICA:                 return "SYNC REPLICA";
        case Type::SYNC_DATABASE_REPLICA:        return "SYNC DATABASE REPLICA";
        case Type::SYNC_TRANSACTION_LOG:         return "SYNC TRANSACTION LOG";
        case Type::SYNC_FILE_CACHE:              return "SYNC FILE CACHE";
        case Type::REPLICA_READY:                return "REPLICA READY";
        case Type::REPLICA_UNREADY:              return "REPLICA UNREADY";
        case Type::RELOAD_DICTIONARY:            return "RELOAD DICTIONARY";
        case Type::RELOAD_DICTIONARIES:          return "RELOAD DICTIONARIES";
        case Type::RELOAD_MODEL:                 return "RELOAD MODEL";
        case Type::RELOAD_MODELS:                return "RELOAD MODELS";
        case Type::RELOAD_FUNCTION:              return "RELOAD FUNCTION";
        case Type::RELOAD_FUNCTIONS:             return "RELOAD FUNCTIONS";
        case Type::RELOAD_EMBEDDED_DICTIONARIES: return "RELOAD EMBEDDED DICTIONARIES";
        case Type::RELOAD_CONFIG:                return "RELOAD CONFIG";
        case Type::RELOAD_USERS:                 return "RELOAD USERS";
        case Type::RELOAD_ASYNCHRONOUS_METRICS:  return "RELOAD ASYNCHRONOUS METRICS";
        case Type::RESTART_DISK:                 return "RESTART DISK";
        case Type::STOP_MERGES:                  return "STOP MERGES";
        case Type::START_MERGES:                 return "START MERGES";
        case Type::STOP_TTL_MERGES:              return "STOP TTL MERGES";
        case Type::START_TTL_MERGES:             return "START TTL MERGES";
        case Type::STOP_FETCHES:                 return "STOP FETCHES";
        case Type::START_FETCHES:                return "START FETCHES";
        case Type::STOP_MOVES:                   return "STOP MOVES";
        case Type::START_MOVES:                  return "START MOVES";
        case Type::STOP_REPLICATED_SENDS:        return "STOP REPLICATED SENDS";
        case Type::START_REPLICATED_SENDS:       return "START REPLICATED SENDS";
        case Type::STOP_REPLICATION_QUEUES:      return "STOP REPLICATION QUEUES";
        case Type::START_REPLICATION_QUEUES:     return "START REPLICATION QUEUES";
        case Type::STOP_REPLICATED_DDL_QUERIES:  return "STOP REPLICATED DDL QUERIES";
        case Type::START_REPLICATED_DDL_QUERIES: return "START REPLICATED DDL QUERIES";
        case Type::FLUSH_LOGS:                   return "FLUSH LOGS";
        case Type::FLUSH_DISTRIBUTED:            return "FLUSH DISTRIBUTED";
        case Type::FLUSH_ASYNC_INSERT_QUEUE:     return "FLUSH ASYNC INSERT QUEUE";
        case Type::STOP_DISTRIBUTED_SENDS:       return "STOP DISTRIBUTED SENDS";
        case Type::START_DISTRIBUTED_SENDS:      return "START DISTRIBUTED SENDS";
        case Type::START_THREAD_FUZZER:          return "START THREAD FUZZER";
        case Type::STOP_THREAD_FUZZER:           return "STOP THREAD FUZZER";
        case Type::UNFREEZE:                     return "UNFREEZE";
        case Type::ENABLE_FAILPOINT:             return "ENABLE FAILPOINT";
        case Type::DISABLE_FAILPOINT:            return "DISABLE FAILPOINT";
        case Type::WAIT_FAILPOINT:               return "WAIT FAILPOINT";
        case Type::NOTIFY_FAILPOINT:             return "NOTIFY FAILPOINT";
        case Type::SYNC_FILESYSTEM_CACHE:        return "SYNC FILESYSTEM CACHE";
        case Type::STOP_PULLING_REPLICATION_LOG: return "STOP PULLING REPLICATION LOG";
        case Type::START_PULLING_REPLICATION_LOG: return "START PULLING REPLICATION LOG";
        case Type::STOP_CLEANUP:                 return "STOP CLEANUP";
        case Type::START_CLEANUP:                return "START CLEANUP";
        case Type::RESET_COVERAGE:               return "RESET COVERAGE";
        case Type::REFRESH_VIEW:                 return "REFRESH VIEW";
        case Type::WAIT_VIEW:                    return "WAIT VIEW";
        case Type::START_VIEW:                   return "START VIEW";
        case Type::START_VIEWS:                  return "START VIEWS";
        case Type::START_REPLICATED_VIEW:        return "START REPLICATED VIEW";
        case Type::STOP_VIEW:                    return "STOP VIEW";
        case Type::STOP_VIEWS:                   return "STOP VIEWS";
        case Type::STOP_REPLICATED_VIEW:         return "STOP REPLICATED VIEW";
        case Type::CANCEL_VIEW:                  return "CANCEL VIEW";
        case Type::TEST_VIEW:                    return "TEST VIEW";
        case Type::LOAD_PRIMARY_KEY:             return "LOAD PRIMARY KEY";
        case Type::UNLOAD_PRIMARY_KEY:           return "UNLOAD PRIMARY KEY";
        case Type::STOP_VIRTUAL_PARTS_UPDATE:    return "STOP VIRTUAL PARTS UPDATE";
        case Type::START_VIRTUAL_PARTS_UPDATE:   return "START VIRTUAL PARTS UPDATE";
        case Type::STOP_REDUCE_BLOCKING_PARTS:   return "STOP REDUCE BLOCKING PARTS";
        case Type::START_REDUCE_BLOCKING_PARTS:  return "START REDUCE BLOCKING PARTS";
        case Type::UNLOCK_SNAPSHOT:              return "UNLOCK SNAPSHOT";
        case Type::RECONNECT_ZOOKEEPER:          return "RECONNECT ZOOKEEPER";
        case Type::INSTRUMENT_ADD:               return "INSTRUMENT ADD";
        case Type::INSTRUMENT_REMOVE:            return "INSTRUMENT REMOVE";
        case Type::RESET_DDL_WORKER:             return "RESET DDL WORKER";
        case Type::END:                          return "END";
    }
    return "UNKNOWN";
}

String ASTSystemQuery::getDatabase() const
{
    String name;
    tryGetIdentifierNameInto(database, name);
    return name;
}

String ASTSystemQuery::getTable() const
{
    String name;
    tryGetIdentifierNameInto(table, name);
    return name;
}

void ASTSystemQuery::setDatabase(const String & name)
{
    if (database)
    {
        std::erase(children, database);
        database.reset();
    }

    if (!name.empty())
    {
        database = make_intrusive<ASTIdentifier>(name);
        children.push_back(database);
    }
}

void ASTSystemQuery::setTable(const String & name)
{
    if (table)
    {
        std::erase(children, table);
        table.reset();
    }

    if (!name.empty())
    {
        table = make_intrusive<ASTIdentifier>(name);
        children.push_back(table);
    }
}

void ASTSystemQuery::formatImpl(WriteBuffer & ostr, const FormatSettings & settings, FormatState & state, FormatStateStacked frame) const
{
    auto print_identifier = [&](const String & identifier) -> WriteBuffer &
    {
        ostr << backQuoteIfNeed(identifier)
                     ;
        return ostr;
    };

    auto print_keyword = [&](const auto & keyword) -> WriteBuffer &
    {
        ostr << keyword;
        return ostr;
    };

    auto print_database_table = [&]() -> WriteBuffer &
    {
        if (database)
        {
            database->format(ostr, settings, state, frame);
            ostr << '.';
        }

        chassert(table);
        table->format(ostr, settings, state, frame);

        if (if_exists)
            print_keyword(" IF EXISTS");

        return ostr;
    };

    auto print_restore_database_replica = [&]() -> WriteBuffer &
    {
        chassert(database);

        ostr << " ";
        print_identifier(getDatabase());

        return ostr;
    };

    auto print_drop_replica = [&]
    {
        ostr << " " << quoteString(replica);
        if (!shard.empty())
            print_keyword(" FROM SHARD ") << quoteString(shard);

        if (table)
        {
            print_keyword(" FROM TABLE ");
            print_database_table();
        }
        else if (!replica_zk_path.empty())
        {
            print_keyword(" FROM ZKPATH ") << quoteString(replica_zk_path);
        }
        else if (database)
        {
            print_keyword(" FROM DATABASE ");
            print_identifier(getDatabase());
        }
    };

    auto print_on_volume = [&]
    {
        print_keyword(" ON VOLUME ");
        print_identifier(storage_policy) << ".";
        print_identifier(volume);
    };

    print_keyword("SYSTEM") << " ";
    print_keyword(typeToString(type));

    std::unordered_set<Type> queries_with_on_cluster_at_end = {
        Type::CLEAR_FILESYSTEM_CACHE,
        Type::SYNC_FILESYSTEM_CACHE,
        Type::CLEAR_QUERY_CACHE,
    };

    if (!queries_with_on_cluster_at_end.contains(type) && !cluster.empty())
        formatOnCluster(ostr, settings);

    switch (type)
    {
        case Type::STOP_MERGES:
        case Type::START_MERGES:
        case Type::STOP_TTL_MERGES:
        case Type::START_TTL_MERGES:
        case Type::STOP_MOVES:
        case Type::START_MOVES:
        case Type::STOP_FETCHES:
        case Type::START_FETCHES:
        case Type::STOP_REPLICATED_SENDS:
        case Type::START_REPLICATED_SENDS:
        case Type::STOP_REPLICATION_QUEUES:
        case Type::START_REPLICATION_QUEUES:
        case Type::STOP_DISTRIBUTED_SENDS:
        case Type::START_DISTRIBUTED_SENDS:
        case Type::STOP_PULLING_REPLICATION_LOG:
        case Type::START_PULLING_REPLICATION_LOG:
        case Type::STOP_CLEANUP:
        case Type::START_CLEANUP:
        case Type::LOAD_PRIMARY_KEY:
        case Type::UNLOAD_PRIMARY_KEY:
        case Type::STOP_VIRTUAL_PARTS_UPDATE:
        case Type::START_VIRTUAL_PARTS_UPDATE:
        case Type::STOP_REDUCE_BLOCKING_PARTS:
        case Type::START_REDUCE_BLOCKING_PARTS:
        {
            if (table)
            {
                ostr << ' ';
                print_database_table();
            }
            else if (!volume.empty())
            {
                print_on_volume();
            }
            break;
        }
        case Type::RESTART_REPLICA:
        case Type::RESTORE_REPLICA:
        case Type::SYNC_REPLICA:
        case Type::WAIT_LOADING_PARTS:
        case Type::FLUSH_DISTRIBUTED:
        case Type::PREWARM_MARK_CACHE:
        case Type::PREWARM_PRIMARY_INDEX_CACHE:
        {
            if (table)
            {
                ostr << ' ';
                print_database_table();
            }

            if (sync_replica_mode != SyncReplicaMode::DEFAULT)
            {
                ostr << ' ';
                print_keyword(toString(sync_replica_mode));

                // If the mode is LIGHTWEIGHT and specific source replicas are specified
                if (sync_replica_mode == SyncReplicaMode::LIGHTWEIGHT && !src_replicas.empty())
                {
                    ostr << ' ';
                    print_keyword("FROM");
                    ostr << ' ';

                    bool first = true;
                    for (const auto & src : src_replicas)
                    {
                        if (!first)
                            ostr << ", ";
                        first = false;
                        ostr << quoteString(src);
                    }
                }
            }

            if (query_settings)
            {
                ostr << settings.nl_or_ws << "SETTINGS ";
                query_settings->format(ostr, settings, state, frame);
            }

            break;
        }
        case Type::RELOAD_DICTIONARY:
        case Type::RELOAD_MODEL:
        case Type::RELOAD_FUNCTION:
        case Type::RESTART_DISK:
        case Type::CLEAR_DISK_METADATA_CACHE:
        {
            if (table)
            {
                ostr << ' ';
                print_database_table();
            }
            else if (!target_model.empty())
            {
                ostr << ' ';
                print_identifier(target_model);
            }
            else if (!target_function.empty())
            {
                ostr << ' ';
                print_identifier(target_function);
            }
            else if (!disk.empty())
            {
                ostr << ' ';
                print_identifier(disk);
            }

            break;
        }
        case Type::SYNC_DATABASE_REPLICA:
        {
            ostr << ' ';
            database->format(ostr, settings, state, frame);
            if (sync_replica_mode != SyncReplicaMode::DEFAULT)
            {
                ostr << ' ';
                print_keyword(toString(sync_replica_mode));
            }
            break;
        }
        case Type::DROP_REPLICA:
        case Type::DROP_DATABASE_REPLICA:
        case Type::DROP_CATALOG_REPLICA:
        {
            print_drop_replica();
            break;
        }
        case Type::RESTORE_DATABASE_REPLICA:
        {
            if (database)
            {
                print_restore_database_replica();
            }
            break;
        }
        case Type::SUSPEND:
        {
            print_keyword(" FOR ") << seconds;
            print_keyword(" SECOND");
            break;
        }
        case Type::CLEAR_FORMAT_SCHEMA_CACHE:
        {
            if (!schema_cache_format.empty())
            {
                print_keyword(" FOR ");
                print_identifier(schema_cache_format);
            }
            break;
        }
        case Type::CLEAR_FILESYSTEM_CACHE:
        {
            if (!filesystem_cache_name.empty())
            {
                ostr << ' ' << quoteString(filesystem_cache_name);
                if (!key_to_drop.empty())
                {
                    print_keyword(" KEY ");
                    print_identifier(key_to_drop);
                    if (offset_to_drop.has_value())
                    {
                        print_keyword(" OFFSET ");
                        ostr << offset_to_drop.value();
                    }
                }
            }
            break;
        }
        case Type::CLEAR_SCHEMA_CACHE:
        {
            if (!schema_cache_storage.empty())
            {
                print_keyword(" FOR ");
                print_identifier(schema_cache_storage);
            }
            break;
        }
        case Type::CLEAR_DISTRIBUTED_CACHE:
        {
            if (distributed_cache_drop_connections)
                print_keyword(" CONNECTIONS");
            else if (!distributed_cache_server_id.empty())
                ostr << " " << distributed_cache_server_id;
            break;
        }
        case Type::CLEAR_QUERY_CACHE:
        {
            if (query_result_cache_tag.has_value())
            {
                print_keyword(" TAG ");
                ostr << quoteString(*query_result_cache_tag);
            }
            break;
        }
        case Type::UNFREEZE:
        {
            print_keyword(" WITH NAME ");
            ostr << quoteString(backup_name);
            break;
        }
        case Type::UNLOCK_SNAPSHOT:
        {
            ostr << quoteString(backup_name);
            if (backup_source)
            {
                print_keyword(" FROM ");
                backup_source->format(ostr, settings);
            }
            break;
        }
        case Type::START_LISTEN:
        case Type::STOP_LISTEN:
        {
            ostr << ' ';
            print_keyword(ServerType::serverTypeToString(server_type.type));

            if (server_type.type == ServerType::Type::CUSTOM)
                ostr << ' ' << quoteString(server_type.custom_name);

            bool comma = false;

            if (!server_type.exclude_types.empty())
            {
                print_keyword(" EXCEPT");

                for (auto cur_type : server_type.exclude_types)
                {
                    if (cur_type == ServerType::Type::CUSTOM)
                        continue;

                    if (comma)
                        ostr << ',';
                    else
                        comma = true;

                    ostr << ' ';
                    print_keyword(ServerType::serverTypeToString(cur_type));
                }

                if (server_type.exclude_types.contains(ServerType::Type::CUSTOM))
                {
                    for (const auto & cur_name : server_type.exclude_custom_names)
                    {
                        if (comma)
                            ostr << ',';
                        else
                            comma = true;

                        ostr << ' ';
                        print_keyword(ServerType::serverTypeToString(ServerType::Type::CUSTOM));
                        ostr << " " << quoteString(cur_name);
                    }
                }
            }
            break;
        }
        case Type::ENABLE_FAILPOINT:
        case Type::DISABLE_FAILPOINT:
        case Type::NOTIFY_FAILPOINT:
        {
            ostr << ' ';
            print_identifier(fail_point_name);
            break;
        }
        case Type::WAIT_FAILPOINT:
        {
            ostr << ' ';
            print_identifier(fail_point_name);
            if (fail_point_action == FailPointAction::PAUSE)
            {
                ostr << ' ';
                print_keyword("PAUSE");
            }
            else if (fail_point_action == FailPointAction::RESUME)
            {
                ostr << ' ';
                print_keyword("RESUME");
            }
            break;
        }
        case Type::REFRESH_VIEW:
        case Type::START_VIEW:
        case Type::START_REPLICATED_VIEW:
        case Type::STOP_VIEW:
        case Type::STOP_REPLICATED_VIEW:
        case Type::CANCEL_VIEW:
        case Type::WAIT_VIEW:
        {
            ostr << ' ';
            print_database_table();
            break;
        }
        case Type::TEST_VIEW:
        {
            ostr << ' ';
            print_database_table();

            if (!fake_time_for_view)
            {
                ostr << ' ';
                print_keyword("UNSET FAKE TIME");
            }
            else
            {
                ostr << ' ';
                print_keyword("SET FAKE TIME");
                ostr << " '" << LocalDateTime(*fake_time_for_view) << "'";
            }
            break;
        }
        case Type::FLUSH_ASYNC_INSERT_QUEUE:
        case Type::FLUSH_LOGS:
        {
            bool comma = false;
            for (const auto & cur_log : tables)
            {
                if (comma)
                    ostr << ',';
                else
                    comma = true;
                ostr << ' ';

                if (!cur_log.first.empty())
                    print_identifier(cur_log.first) << ".";
                print_identifier(cur_log.second);
            }
            break;
        }

#if USE_XRAY
        case Type::INSTRUMENT_ADD:
        {
            if (!instrumentation_function_name.empty())
                ostr << ' ' << quoteString(instrumentation_function_name);

            if (!instrumentation_handler_name.empty())
            {
                ostr << ' ';
                print_identifier(Poco::toUpper(instrumentation_handler_name));
            }

            switch (instrumentation_entry_type)
            {
                case Instrumentation::EntryType::ENTRY:
                    ostr << " ENTRY"; break;
                case Instrumentation::EntryType::EXIT:
                    ostr << " EXIT"; break;
                case Instrumentation::EntryType::ENTRY_AND_EXIT:
                    break;
            }

            bool whitespace = false;
            for (const auto & param : instrumentation_parameters)
            {
                if (!whitespace)
                    ostr << ' ';
                else
                    whitespace = true;
                std::visit([&](const auto & value)
                {
                    using T = std::decay_t<decltype(value)>;
                    if constexpr (std::is_same_v<T, String>)
                        ostr << ' ' << quoteString(value);
                    else
                        ostr << ' ' << value;
                }, param);
            }
            break;
        }
        case Type::INSTRUMENT_REMOVE:
        {
            if (!instrumentation_subquery.empty())
                ostr << " (" << instrumentation_subquery << ')';
            else if (instrumentation_point)
            {
                if (std::holds_alternative<Instrumentation::All>(instrumentation_point.value()))
                    ostr << " ALL";
                else if (std::holds_alternative<String>(instrumentation_point.value()))
                    ostr << ' ' << quoteString(std::get<String>(instrumentation_point.value()));
                else
                    ostr << ' ' << std::get<UInt64>(instrumentation_point.value());
            }
            break;
        }
#else
        case Type::INSTRUMENT_ADD:
        case Type::INSTRUMENT_REMOVE:
#endif

        case Type::KILL:
        case Type::SHUTDOWN:
        case Type::CLEAR_DNS_CACHE:
        case Type::CLEAR_CONNECTIONS_CACHE:
        case Type::CLEAR_MMAP_CACHE:
        case Type::CLEAR_QUERY_CONDITION_CACHE:
        case Type::CLEAR_MARK_CACHE:
        case Type::CLEAR_PRIMARY_INDEX_CACHE:
        case Type::CLEAR_INDEX_MARK_CACHE:
        case Type::CLEAR_UNCOMPRESSED_CACHE:
        case Type::CLEAR_INDEX_UNCOMPRESSED_CACHE:
        case Type::CLEAR_VECTOR_SIMILARITY_INDEX_CACHE:
        case Type::CLEAR_TEXT_INDEX_DICTIONARY_CACHE:
        case Type::CLEAR_TEXT_INDEX_HEADER_CACHE:
        case Type::CLEAR_TEXT_INDEX_POSTINGS_CACHE:
        case Type::CLEAR_TEXT_INDEX_CACHES:
        case Type::CLEAR_COMPILED_EXPRESSION_CACHE:
        case Type::CLEAR_S3_CLIENT_CACHE:
        case Type::CLEAR_ICEBERG_METADATA_CACHE:
        case Type::CLEAR_PARQUET_METADATA_CACHE:
        case Type::RESET_COVERAGE:
        case Type::RESTART_REPLICAS:
        case Type::JEMALLOC_PURGE:
        case Type::JEMALLOC_FLUSH_PROFILE:
        case Type::JEMALLOC_ENABLE_PROFILE:
        case Type::JEMALLOC_DISABLE_PROFILE:
        case Type::SYNC_TRANSACTION_LOG:
        case Type::SYNC_FILE_CACHE:
        case Type::SYNC_FILESYSTEM_CACHE:
        case Type::REPLICA_READY:   /// Obsolete
        case Type::REPLICA_UNREADY: /// Obsolete
        case Type::RELOAD_DICTIONARIES:
        case Type::RELOAD_EMBEDDED_DICTIONARIES:
        case Type::RELOAD_MODELS:
        case Type::RELOAD_FUNCTIONS:
        case Type::RELOAD_CONFIG:
        case Type::RELOAD_USERS:
        case Type::RELOAD_ASYNCHRONOUS_METRICS:
        case Type::START_THREAD_FUZZER:
        case Type::STOP_THREAD_FUZZER:
        case Type::START_VIEWS:
        case Type::STOP_VIEWS:
        case Type::CLEAR_PAGE_CACHE:
        case Type::STOP_REPLICATED_DDL_QUERIES:
        case Type::START_REPLICATED_DDL_QUERIES:
        case Type::RECONNECT_ZOOKEEPER:
        case Type::RESET_DDL_WORKER:
            break;
        case Type::UNKNOWN:
        case Type::END:
            throw Exception(ErrorCodes::LOGICAL_ERROR, "Unknown SYSTEM command");
    }

    if (queries_with_on_cluster_at_end.contains(type) && !cluster.empty())
        formatOnCluster(ostr, settings);
}


}
