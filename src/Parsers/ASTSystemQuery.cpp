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

namespace
{
    std::vector<std::string> getTypeIndexToTypeName()
    {
        constexpr std::size_t types_size = static_cast<std::size_t>(ASTSystemQuery::Type::END) + 1;

        std::vector<std::string> type_index_to_type_name;
        type_index_to_type_name.resize(types_size);

        static constexpr std::pair<ASTSystemQuery::Type, std::string_view> entries[] = {
            {ASTSystemQuery::Type::UNKNOWN, "UNKNOWN"},
            {ASTSystemQuery::Type::SHUTDOWN, "SHUTDOWN"},
            {ASTSystemQuery::Type::KILL, "KILL"},
            {ASTSystemQuery::Type::SUSPEND, "SUSPEND"},
            {ASTSystemQuery::Type::CLEAR_DNS_CACHE, "CLEAR_DNS_CACHE"},
            {ASTSystemQuery::Type::CLEAR_CONNECTIONS_CACHE, "CLEAR_CONNECTIONS_CACHE"},
            {ASTSystemQuery::Type::PREWARM_MARK_CACHE, "PREWARM_MARK_CACHE"},
            {ASTSystemQuery::Type::PREWARM_PRIMARY_INDEX_CACHE, "PREWARM_PRIMARY_INDEX_CACHE"},
            {ASTSystemQuery::Type::CLEAR_MARK_CACHE, "CLEAR_MARK_CACHE"},
            {ASTSystemQuery::Type::CLEAR_PRIMARY_INDEX_CACHE, "CLEAR_PRIMARY_INDEX_CACHE"},
            {ASTSystemQuery::Type::CLEAR_UNCOMPRESSED_CACHE, "CLEAR_UNCOMPRESSED_CACHE"},
            {ASTSystemQuery::Type::CLEAR_INDEX_MARK_CACHE, "CLEAR_INDEX_MARK_CACHE"},
            {ASTSystemQuery::Type::CLEAR_INDEX_UNCOMPRESSED_CACHE, "CLEAR_INDEX_UNCOMPRESSED_CACHE"},
            {ASTSystemQuery::Type::CLEAR_VECTOR_SIMILARITY_INDEX_CACHE, "CLEAR_VECTOR_SIMILARITY_INDEX_CACHE"},
            {ASTSystemQuery::Type::CLEAR_TEXT_INDEX_DICTIONARY_CACHE, "CLEAR_TEXT_INDEX_DICTIONARY_CACHE"},
            {ASTSystemQuery::Type::CLEAR_TEXT_INDEX_HEADER_CACHE, "CLEAR_TEXT_INDEX_HEADER_CACHE"},
            {ASTSystemQuery::Type::CLEAR_TEXT_INDEX_POSTINGS_CACHE, "CLEAR_TEXT_INDEX_POSTINGS_CACHE"},
            {ASTSystemQuery::Type::CLEAR_TEXT_INDEX_CACHES, "CLEAR_TEXT_INDEX_CACHES"},
            {ASTSystemQuery::Type::CLEAR_MMAP_CACHE, "CLEAR_MMAP_CACHE"},
            {ASTSystemQuery::Type::CLEAR_QUERY_CONDITION_CACHE, "CLEAR_QUERY_CONDITION_CACHE"},
            {ASTSystemQuery::Type::CLEAR_QUERY_CACHE, "CLEAR_QUERY_CACHE"},
            {ASTSystemQuery::Type::CLEAR_COMPILED_EXPRESSION_CACHE, "CLEAR_COMPILED_EXPRESSION_CACHE"},
            {ASTSystemQuery::Type::CLEAR_ICEBERG_METADATA_CACHE, "CLEAR_ICEBERG_METADATA_CACHE"},
            {ASTSystemQuery::Type::CLEAR_PARQUET_METADATA_CACHE, "CLEAR_PARQUET_METADATA_CACHE"},
            {ASTSystemQuery::Type::CLEAR_FILESYSTEM_CACHE, "CLEAR_FILESYSTEM_CACHE"},
            {ASTSystemQuery::Type::CLEAR_DISTRIBUTED_CACHE, "CLEAR_DISTRIBUTED_CACHE"},
            {ASTSystemQuery::Type::CLEAR_DISK_METADATA_CACHE, "CLEAR_DISK_METADATA_CACHE"},
            {ASTSystemQuery::Type::CLEAR_PAGE_CACHE, "CLEAR_PAGE_CACHE"},
            {ASTSystemQuery::Type::CLEAR_SCHEMA_CACHE, "CLEAR_SCHEMA_CACHE"},
            {ASTSystemQuery::Type::CLEAR_FORMAT_SCHEMA_CACHE, "CLEAR_FORMAT_SCHEMA_CACHE"},
            {ASTSystemQuery::Type::CLEAR_S3_CLIENT_CACHE, "CLEAR_S3_CLIENT_CACHE"},
            {ASTSystemQuery::Type::STOP_LISTEN, "STOP_LISTEN"},
            {ASTSystemQuery::Type::START_LISTEN, "START_LISTEN"},
            {ASTSystemQuery::Type::RESTART_REPLICAS, "RESTART_REPLICAS"},
            {ASTSystemQuery::Type::RESTART_REPLICA, "RESTART_REPLICA"},
            {ASTSystemQuery::Type::RESTORE_REPLICA, "RESTORE_REPLICA"},
            {ASTSystemQuery::Type::RESTORE_DATABASE_REPLICA, "RESTORE_DATABASE_REPLICA"},
            {ASTSystemQuery::Type::WAIT_LOADING_PARTS, "WAIT_LOADING_PARTS"},
            {ASTSystemQuery::Type::DROP_REPLICA, "DROP_REPLICA"},
            {ASTSystemQuery::Type::DROP_DATABASE_REPLICA, "DROP_DATABASE_REPLICA"},
            {ASTSystemQuery::Type::DROP_CATALOG_REPLICA, "DROP_CATALOG_REPLICA"},
            {ASTSystemQuery::Type::JEMALLOC_PURGE, "JEMALLOC_PURGE"},
            {ASTSystemQuery::Type::JEMALLOC_ENABLE_PROFILE, "JEMALLOC_ENABLE_PROFILE"},
            {ASTSystemQuery::Type::JEMALLOC_DISABLE_PROFILE, "JEMALLOC_DISABLE_PROFILE"},
            {ASTSystemQuery::Type::JEMALLOC_FLUSH_PROFILE, "JEMALLOC_FLUSH_PROFILE"},
            {ASTSystemQuery::Type::SYNC_REPLICA, "SYNC_REPLICA"},
            {ASTSystemQuery::Type::SYNC_DATABASE_REPLICA, "SYNC_DATABASE_REPLICA"},
            {ASTSystemQuery::Type::SYNC_TRANSACTION_LOG, "SYNC_TRANSACTION_LOG"},
            {ASTSystemQuery::Type::SYNC_FILE_CACHE, "SYNC_FILE_CACHE"},
            {ASTSystemQuery::Type::REPLICA_READY, "REPLICA_READY"},
            {ASTSystemQuery::Type::REPLICA_UNREADY, "REPLICA_UNREADY"},
            {ASTSystemQuery::Type::RELOAD_DICTIONARY, "RELOAD_DICTIONARY"},
            {ASTSystemQuery::Type::RELOAD_DICTIONARIES, "RELOAD_DICTIONARIES"},
            {ASTSystemQuery::Type::RELOAD_MODEL, "RELOAD_MODEL"},
            {ASTSystemQuery::Type::RELOAD_MODELS, "RELOAD_MODELS"},
            {ASTSystemQuery::Type::RELOAD_FUNCTION, "RELOAD_FUNCTION"},
            {ASTSystemQuery::Type::RELOAD_FUNCTIONS, "RELOAD_FUNCTIONS"},
            {ASTSystemQuery::Type::RELOAD_EMBEDDED_DICTIONARIES, "RELOAD_EMBEDDED_DICTIONARIES"},
            {ASTSystemQuery::Type::RELOAD_CONFIG, "RELOAD_CONFIG"},
            {ASTSystemQuery::Type::RELOAD_USERS, "RELOAD_USERS"},
            {ASTSystemQuery::Type::RELOAD_ASYNCHRONOUS_METRICS, "RELOAD_ASYNCHRONOUS_METRICS"},
            {ASTSystemQuery::Type::RESTART_DISK, "RESTART_DISK"},
            {ASTSystemQuery::Type::STOP_MERGES, "STOP_MERGES"},
            {ASTSystemQuery::Type::START_MERGES, "START_MERGES"},
            {ASTSystemQuery::Type::STOP_TTL_MERGES, "STOP_TTL_MERGES"},
            {ASTSystemQuery::Type::START_TTL_MERGES, "START_TTL_MERGES"},
            {ASTSystemQuery::Type::STOP_FETCHES, "STOP_FETCHES"},
            {ASTSystemQuery::Type::START_FETCHES, "START_FETCHES"},
            {ASTSystemQuery::Type::STOP_MOVES, "STOP_MOVES"},
            {ASTSystemQuery::Type::START_MOVES, "START_MOVES"},
            {ASTSystemQuery::Type::STOP_REPLICATED_SENDS, "STOP_REPLICATED_SENDS"},
            {ASTSystemQuery::Type::START_REPLICATED_SENDS, "START_REPLICATED_SENDS"},
            {ASTSystemQuery::Type::STOP_REPLICATION_QUEUES, "STOP_REPLICATION_QUEUES"},
            {ASTSystemQuery::Type::START_REPLICATION_QUEUES, "START_REPLICATION_QUEUES"},
            {ASTSystemQuery::Type::STOP_REPLICATED_DDL_QUERIES, "STOP_REPLICATED_DDL_QUERIES"},
            {ASTSystemQuery::Type::START_REPLICATED_DDL_QUERIES, "START_REPLICATED_DDL_QUERIES"},
            {ASTSystemQuery::Type::FLUSH_LOGS, "FLUSH_LOGS"},
            {ASTSystemQuery::Type::FLUSH_DISTRIBUTED, "FLUSH_DISTRIBUTED"},
            {ASTSystemQuery::Type::FLUSH_ASYNC_INSERT_QUEUE, "FLUSH_ASYNC_INSERT_QUEUE"},
            {ASTSystemQuery::Type::STOP_DISTRIBUTED_SENDS, "STOP_DISTRIBUTED_SENDS"},
            {ASTSystemQuery::Type::START_DISTRIBUTED_SENDS, "START_DISTRIBUTED_SENDS"},
            {ASTSystemQuery::Type::START_THREAD_FUZZER, "START_THREAD_FUZZER"},
            {ASTSystemQuery::Type::STOP_THREAD_FUZZER, "STOP_THREAD_FUZZER"},
            {ASTSystemQuery::Type::UNFREEZE, "UNFREEZE"},
            {ASTSystemQuery::Type::ENABLE_FAILPOINT, "ENABLE_FAILPOINT"},
            {ASTSystemQuery::Type::DISABLE_FAILPOINT, "DISABLE_FAILPOINT"},
            {ASTSystemQuery::Type::WAIT_FAILPOINT, "WAIT_FAILPOINT"},
            {ASTSystemQuery::Type::NOTIFY_FAILPOINT, "NOTIFY_FAILPOINT"},
            {ASTSystemQuery::Type::SYNC_FILESYSTEM_CACHE, "SYNC_FILESYSTEM_CACHE"},
            {ASTSystemQuery::Type::STOP_PULLING_REPLICATION_LOG, "STOP_PULLING_REPLICATION_LOG"},
            {ASTSystemQuery::Type::START_PULLING_REPLICATION_LOG, "START_PULLING_REPLICATION_LOG"},
            {ASTSystemQuery::Type::STOP_CLEANUP, "STOP_CLEANUP"},
            {ASTSystemQuery::Type::START_CLEANUP, "START_CLEANUP"},
            {ASTSystemQuery::Type::RESET_COVERAGE, "RESET_COVERAGE"},
            {ASTSystemQuery::Type::REFRESH_VIEW, "REFRESH_VIEW"},
            {ASTSystemQuery::Type::WAIT_VIEW, "WAIT_VIEW"},
            {ASTSystemQuery::Type::START_VIEW, "START_VIEW"},
            {ASTSystemQuery::Type::START_VIEWS, "START_VIEWS"},
            {ASTSystemQuery::Type::START_REPLICATED_VIEW, "START_REPLICATED_VIEW"},
            {ASTSystemQuery::Type::STOP_VIEW, "STOP_VIEW"},
            {ASTSystemQuery::Type::STOP_VIEWS, "STOP_VIEWS"},
            {ASTSystemQuery::Type::STOP_REPLICATED_VIEW, "STOP_REPLICATED_VIEW"},
            {ASTSystemQuery::Type::CANCEL_VIEW, "CANCEL_VIEW"},
            {ASTSystemQuery::Type::TEST_VIEW, "TEST_VIEW"},
            {ASTSystemQuery::Type::LOAD_PRIMARY_KEY, "LOAD_PRIMARY_KEY"},
            {ASTSystemQuery::Type::UNLOAD_PRIMARY_KEY, "UNLOAD_PRIMARY_KEY"},
            {ASTSystemQuery::Type::STOP_VIRTUAL_PARTS_UPDATE, "STOP_VIRTUAL_PARTS_UPDATE"},
            {ASTSystemQuery::Type::START_VIRTUAL_PARTS_UPDATE, "START_VIRTUAL_PARTS_UPDATE"},
            {ASTSystemQuery::Type::STOP_REDUCE_BLOCKING_PARTS, "STOP_REDUCE_BLOCKING_PARTS"},
            {ASTSystemQuery::Type::START_REDUCE_BLOCKING_PARTS, "START_REDUCE_BLOCKING_PARTS"},
            {ASTSystemQuery::Type::UNLOCK_SNAPSHOT, "UNLOCK_SNAPSHOT"},
            {ASTSystemQuery::Type::RECONNECT_ZOOKEEPER, "RECONNECT_ZOOKEEPER"},
            {ASTSystemQuery::Type::INSTRUMENT_ADD, "INSTRUMENT_ADD"},
            {ASTSystemQuery::Type::INSTRUMENT_REMOVE, "INSTRUMENT_REMOVE"},
            {ASTSystemQuery::Type::RESET_DDL_WORKER, "RESET_DDL_WORKER"},
            {ASTSystemQuery::Type::END, "END"},
        };

        for (const auto & [entry, str] : entries)
        {
            auto str_copy = String(str);
            std::replace(str_copy.begin(), str_copy.end(), '_', ' ');
            type_index_to_type_name[static_cast<UInt64>(entry)] = std::move(str_copy);
        }

        return type_index_to_type_name;
    }
}

const char * ASTSystemQuery::typeToString(Type type)
{
    /** During parsing if SystemQuery is not parsed properly it is added to Expected variants as description check IParser.h.
      * Description string must be statically allocated.
      */
    static std::vector<std::string> type_index_to_type_name = getTypeIndexToTypeName();
    const auto & type_name = type_index_to_type_name[static_cast<UInt64>(type)];
    return type_name.data();
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
                print_keyword(enumToString(sync_replica_mode));

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
                print_keyword(enumToString(sync_replica_mode));
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
