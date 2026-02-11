#include <Server.h>

#include <Common/ErrorCodes.h>
#include <Common/ProfileEvents.h>
#include <Common/logger_useful.h>
#include <Common/makeSocketAddress.h>
#include <Compression/ICompressionCodec.h>
#include <Core/Settings.h>
#include <Core/ServerSettings.h>
#include <Interpreters/Context.h>

#include <Server/HTTPHandlerFactory.h>
#include <Server/TCPHandlerFactory.h>
#include <Server/TCPServer.h>
#include <Server/HTTP/HTTPServer.h>
#include <Server/HTTP/HTTPServerConnectionFactory.h>
#include <Server/MySQLHandlerFactory.h>
#include <Server/PostgreSQLHandlerFactory.h>
#include <Server/ProtocolServerAdapter.h>
#include <Server/ProxyV1HandlerFactory.h>
#include <Server/TLSHandlerFactory.h>
#include <Server/ArrowFlightHandler.h>

#include <unordered_set>

#include "config.h"

#if USE_SSL
#    include <Poco/Net/SecureServerSocket.h>
#    include <Server/SSH/SSHPtyHandlerFactory.h>
#endif

#if USE_GRPC
#   include <Server/GRPCServer.h>
#endif


namespace ProfileEvents
{
    extern const Event InterfaceHTTPReceiveBytes;
    extern const Event InterfaceHTTPSendBytes;
    extern const Event InterfaceNativeReceiveBytes;
    extern const Event InterfaceNativeSendBytes;
    extern const Event InterfaceMySQLReceiveBytes;
    extern const Event InterfaceMySQLSendBytes;
    extern const Event InterfacePostgreSQLReceiveBytes;
    extern const Event InterfacePostgreSQLSendBytes;
    extern const Event InterfaceInterserverReceiveBytes;
    extern const Event InterfaceInterserverSendBytes;
    extern const Event InterfacePrometheusReceiveBytes;
    extern const Event InterfacePrometheusSendBytes;
}

namespace DB
{

namespace Setting
{
    extern const SettingsSeconds http_receive_timeout;
    extern const SettingsSeconds http_send_timeout;
    extern const SettingsSeconds receive_timeout;
    extern const SettingsSeconds send_timeout;
}

namespace ServerSetting
{
    extern const ServerSettingsSeconds keep_alive_timeout;
    extern const ServerSettingsUInt64 listen_backlog;
    extern const ServerSettingsUInt64 max_keep_alive_requests;
    extern const ServerSettingsBool mysql_require_secure_transport;
    extern const ServerSettingsBool postgresql_require_secure_transport;
    extern const ServerSettingsDouble os_cpu_busy_time_threshold;
    extern const ServerSettingsBool prometheus_keeper_metrics_only;
}

namespace ErrorCodes
{
    extern const int SUPPORT_IS_DISABLED;
    extern const int INVALID_CONFIG_PARAMETER;
}


static Poco::Net::TCPServerParams::Ptr makeServerParams(const ServerSettings & server_settings)
{
    Poco::Net::TCPServerParams::Ptr params = new Poco::Net::TCPServerParams();
    params->setMaxQueued(static_cast<int>(server_settings[ServerSetting::listen_backlog]));
    return params;
}


std::unique_ptr<TCPProtocolStackFactory> Server::buildProtocolStackFromConfig(
    const Poco::Util::AbstractConfiguration & config,
    const ServerSettings & server_settings,
    const std::string & protocol,
    Poco::Net::HTTPServerParams::Ptr http_params,
    AsynchronousMetrics & async_metrics,
    bool & is_secure)
{
    auto create_factory = [&](const std::string & type, const std::string & conf_name) -> TCPServerConnectionFactory::Ptr
    {
        if (type == "tcp")
            return TCPServerConnectionFactory::Ptr(new TCPHandlerFactory(*this, false, false, ProfileEvents::InterfaceNativeReceiveBytes, ProfileEvents::InterfaceNativeSendBytes));

        if (type == "tls")
#if USE_SSL
            return TCPServerConnectionFactory::Ptr(new TLSHandlerFactory(*this, conf_name));
#else
            throw Exception(ErrorCodes::SUPPORT_IS_DISABLED, "SSL support for TCP protocol is disabled because Poco library was built without NetSSL support.");
#endif

        if (type == "proxy1")
            return TCPServerConnectionFactory::Ptr(new ProxyV1HandlerFactory(*this, conf_name));
        if (type == "mysql")
            return TCPServerConnectionFactory::Ptr(new MySQLHandlerFactory(*this, server_settings[ServerSetting::mysql_require_secure_transport], ProfileEvents::InterfaceMySQLReceiveBytes, ProfileEvents::InterfaceMySQLSendBytes));
        if (type == "postgres")
#if USE_SSL
            return TCPServerConnectionFactory::Ptr(new PostgreSQLHandlerFactory(*this, server_settings[ServerSetting::postgresql_require_secure_transport], conf_name + ".", ProfileEvents::InterfacePostgreSQLReceiveBytes, ProfileEvents::InterfacePostgreSQLSendBytes));
#else
            return TCPServerConnectionFactory::Ptr(new PostgreSQLHandlerFactory(*this, server_settings[ServerSetting::postgresql_require_secure_transport], ProfileEvents::InterfacePostgreSQLReceiveBytes, ProfileEvents::InterfacePostgreSQLSendBytes));
#endif
        if (type == "http")
            return TCPServerConnectionFactory::Ptr(
                new HTTPServerConnectionFactory(httpContext(), http_params, createHandlerFactory(*this, config, async_metrics, "HTTPHandler-factory"), ProfileEvents::InterfaceHTTPReceiveBytes, ProfileEvents::InterfaceHTTPSendBytes)
            );
        if (type == "prometheus")
        {
            const std::string handler_name = server_settings[ServerSetting::prometheus_keeper_metrics_only] ? "KeeperPrometheusHandler-factory" : "PrometheusHandler-factory";
            return TCPServerConnectionFactory::Ptr(
                new HTTPServerConnectionFactory(httpContext(), http_params, createHandlerFactory(*this, config, async_metrics, handler_name), ProfileEvents::InterfacePrometheusReceiveBytes, ProfileEvents::InterfacePrometheusSendBytes)
            );
        }
        if (type == "interserver")
            return TCPServerConnectionFactory::Ptr(
                new HTTPServerConnectionFactory(httpContext(), http_params, createHandlerFactory(*this, config, async_metrics, "InterserverIOHTTPHandler-factory"), ProfileEvents::InterfaceInterserverReceiveBytes, ProfileEvents::InterfaceInterserverSendBytes)
            );

        throw Exception(ErrorCodes::INVALID_CONFIG_PARAMETER, "Protocol configuration error, unknown protocol name '{}'", type);
    };

    std::string conf_name = "protocols." + protocol;
    std::string prefix = conf_name + ".";
    std::unordered_set<std::string> pset {conf_name};

    auto stack = std::make_unique<TCPProtocolStackFactory>(*this, conf_name);

    while (true)
    {
        // if there is no "type" - it's a reference to another protocol and this is just an endpoint
        if (config.has(prefix + "type"))
        {
            std::string type = config.getString(prefix + "type");
            if (type == "tls")
            {
                if (is_secure)
                    throw Exception(ErrorCodes::INVALID_CONFIG_PARAMETER, "Protocol '{}' contains more than one TLS layer", protocol);
                is_secure = true;
            }

            stack->append(create_factory(type, conf_name));
        }

        if (!config.has(prefix + "impl"))
            break;

        conf_name = "protocols." + config.getString(prefix + "impl");
        prefix = conf_name + ".";

        if (!pset.insert(conf_name).second)
            throw Exception(ErrorCodes::INVALID_CONFIG_PARAMETER, "Protocol '{}' configuration contains a loop on '{}'", protocol, conf_name);
    }

    return stack;
}

void Server::createServers(
    Poco::Util::AbstractConfiguration & config,
    const ServerSettings & server_settings,
    const Strings & listen_hosts,
    bool listen_try,
    Poco::ThreadPool & server_pool,
    AsynchronousMetrics & async_metrics,
    std::vector<ProtocolServerAdapter> & servers,
    bool start_servers,
    const ServerType & server_type)
{
    const Settings & settings = global_context->getSettingsRef();

    Poco::Net::HTTPServerParams::Ptr http_params = new Poco::Net::HTTPServerParams;
    http_params->setTimeout(settings[Setting::http_receive_timeout]);
    http_params->setKeepAliveTimeout(global_context->getServerSettings()[ServerSetting::keep_alive_timeout]);
    http_params->setMaxKeepAliveRequests(static_cast<int>(global_context->getServerSettings()[ServerSetting::max_keep_alive_requests]));
    http_params->setMaxQueued(static_cast<int>(server_settings[ServerSetting::listen_backlog]));

    Poco::Util::AbstractConfiguration::Keys protocols;
    config.keys("protocols", protocols);

    const TCPServerConnectionFilter::Ptr & connection_filter = new TCPServerConnectionFilter{[&]()
    {
        return !ProfileEvents::checkCPUOverload(static_cast<Int64>(global_context->getServerSettings()[ServerSetting::os_cpu_busy_time_threshold]),
                global_context->getMinOSCPUWaitTimeRatioToDropConnection(),
                global_context->getMaxOSCPUWaitTimeRatioToDropConnection(),
                /*should_throw*/ false);
    }};

    for (const auto & protocol : protocols)
    {
        if (!server_type.shouldStart(ServerType::Type::CUSTOM, protocol))
            continue;

        std::string prefix = "protocols." + protocol + ".";
        std::string port_name = prefix + "port";
        std::string description {"<undefined> protocol"};
        if (config.has(prefix + "description"))
            description = config.getString(prefix + "description");

        if (!config.has(prefix + "port"))
            continue;

        std::vector<std::string> hosts;
        if (config.has(prefix + "host"))
            hosts.push_back(config.getString(prefix + "host"));
        else
            hosts = listen_hosts;

        for (const auto & host : hosts)
        {
            bool is_secure = false;
            auto stack = buildProtocolStackFromConfig(config, server_settings, protocol, http_params, async_metrics, is_secure);

            if (stack->empty())
                throw Exception(ErrorCodes::INVALID_CONFIG_PARAMETER, "Protocol '{}' stack empty", protocol);

            createServer(config, host, port_name.c_str(), listen_try, start_servers, servers, [&](UInt16 port) -> ProtocolServerAdapter
            {
                Poco::Net::ServerSocket socket;
                auto address = socketBindListen(server_settings, socket, host, port, is_secure);
                socket.setReceiveTimeout(settings[Setting::receive_timeout]);
                socket.setSendTimeout(settings[Setting::send_timeout]);

                return ProtocolServerAdapter(
                    host,
                    port_name.c_str(),
                    description + ": " + address.toString(),
                    std::make_unique<TCPServer>(
                        stack.release(),
                        server_pool,
                        socket,
                        makeServerParams(server_settings),
                        connection_filter));
            });
        }
    }

    for (const auto & listen_host : listen_hosts)
    {
        const char * port_name;

        if (server_type.shouldStart(ServerType::Type::HTTP))
        {
            /// HTTP
            port_name = "http_port";
            createServer(config, listen_host, port_name, listen_try, start_servers, servers, [&](UInt16 port) -> ProtocolServerAdapter
            {
                Poco::Net::ServerSocket socket;
                auto address = socketBindListen(server_settings, socket, listen_host, port);
                socket.setReceiveTimeout(settings[Setting::http_receive_timeout]);
                socket.setSendTimeout(settings[Setting::http_send_timeout]);

                return ProtocolServerAdapter(
                    listen_host,
                    port_name,
                    "http://" + address.toString(),
                    std::make_unique<HTTPServer>(
                        httpContext(), createHandlerFactory(*this, config, async_metrics, "HTTPHandler-factory"), server_pool, socket, http_params, connection_filter, ProfileEvents::InterfaceHTTPReceiveBytes, ProfileEvents::InterfaceHTTPSendBytes));
            });
        }

        if (server_type.shouldStart(ServerType::Type::HTTPS))
        {
            /// HTTPS
            port_name = "https_port";
            createServer(config, listen_host, port_name, listen_try, start_servers, servers, [&](UInt16 port) -> ProtocolServerAdapter
            {
#if USE_SSL
                Poco::Net::SecureServerSocket socket;
                auto address = socketBindListen(server_settings, socket, listen_host, port, /* secure = */ true);
                socket.setReceiveTimeout(settings[Setting::http_receive_timeout]);
                socket.setSendTimeout(settings[Setting::http_send_timeout]);
                return ProtocolServerAdapter(
                    listen_host,
                    port_name,
                    "https://" + address.toString(),
                    std::make_unique<HTTPServer>(
                        httpContext(), createHandlerFactory(*this, config, async_metrics, "HTTPSHandler-factory"), server_pool, socket, http_params, connection_filter, ProfileEvents::InterfaceHTTPReceiveBytes, ProfileEvents::InterfaceHTTPSendBytes));
#else
                UNUSED(port);
                throw Exception(ErrorCodes::SUPPORT_IS_DISABLED, "HTTPS protocol is disabled because Poco library was built without NetSSL support.");
#endif
            });
        }

        if (server_type.shouldStart(ServerType::Type::TCP))
        {
            /// TCP
            port_name = "tcp_port";
            createServer(config, listen_host, port_name, listen_try, start_servers, servers, [&](UInt16 port) -> ProtocolServerAdapter
            {
                Poco::Net::ServerSocket socket;
                auto address = socketBindListen(server_settings, socket, listen_host, port);
                socket.setReceiveTimeout(settings[Setting::receive_timeout]);
                socket.setSendTimeout(settings[Setting::send_timeout]);
                return ProtocolServerAdapter(
                    listen_host,
                    port_name,
                    "native protocol (tcp): " + address.toString(),
                    std::make_unique<TCPServer>(
                        new TCPHandlerFactory(*this, /* secure */ false, /* proxy protocol */ false, ProfileEvents::InterfaceNativeReceiveBytes, ProfileEvents::InterfaceNativeSendBytes),
                        server_pool,
                        socket,
                        makeServerParams(server_settings),
                        connection_filter));
            });
        }

        if (server_type.shouldStart(ServerType::Type::TCP_WITH_PROXY))
        {
            /// TCP with PROXY protocol, see https://github.com/wolfeidau/proxyv2/blob/master/docs/proxy-protocol.txt
            port_name = "tcp_with_proxy_port";
            createServer(config, listen_host, port_name, listen_try, start_servers, servers, [&](UInt16 port) -> ProtocolServerAdapter
            {
                Poco::Net::ServerSocket socket;
                auto address = socketBindListen(server_settings, socket, listen_host, port);
                socket.setReceiveTimeout(settings[Setting::receive_timeout]);
                socket.setSendTimeout(settings[Setting::send_timeout]);
                return ProtocolServerAdapter(
                    listen_host,
                    port_name,
                    "native protocol (tcp) with PROXY: " + address.toString(),
                    std::make_unique<TCPServer>(
                        new TCPHandlerFactory(*this, /* secure */ false, /* proxy protocol */ true, ProfileEvents::InterfaceNativeReceiveBytes, ProfileEvents::InterfaceNativeSendBytes),
                        server_pool,
                        socket,
                        makeServerParams(server_settings),
                        connection_filter));
            });
        }

#if USE_ARROWFLIGHT
        if (server_type.shouldStart(ServerType::Type::ARROW_FLIGHT))
        {
            port_name = "arrowflight_port";
            createServer(config, listen_host, port_name, listen_try, start_servers, servers, [&](UInt16 port) -> ProtocolServerAdapter
            {
                Poco::Net::ServerSocket socket;
                auto address = socketBindListen(server_settings, socket, listen_host, port, /* secure = */ true);
                socket.setReceiveTimeout(Poco::Timespan());
                socket.setSendTimeout(settings[Setting::send_timeout]);
                return ProtocolServerAdapter(
                    listen_host,
                    port_name,
                    "Arrow Flight compatibility protocol: " + address.toString(),
                    std::unique_ptr<IGRPCServer>(new ArrowFlightHandler(*this, makeSocketAddress(listen_host, port, &logger()))),
                    true);
            });
        }
#endif

        if (server_type.shouldStart(ServerType::Type::TCP_SECURE))
        {
            /// TCP with SSL
            port_name = "tcp_port_secure";
            createServer(config, listen_host, port_name, listen_try, start_servers, servers, [&](UInt16 port) -> ProtocolServerAdapter
            {
    #if USE_SSL
                Poco::Net::SecureServerSocket socket;
                auto address = socketBindListen(server_settings, socket, listen_host, port, /* secure = */ true);
                socket.setReceiveTimeout(settings[Setting::receive_timeout]);
                socket.setSendTimeout(settings[Setting::send_timeout]);
                return ProtocolServerAdapter(
                    listen_host,
                    port_name,
                    "secure native protocol (tcp_secure): " + address.toString(),
                    std::make_unique<TCPServer>(
                        new TCPHandlerFactory(*this, /* secure */ true, /* proxy protocol */ false, ProfileEvents::InterfaceNativeReceiveBytes, ProfileEvents::InterfaceNativeSendBytes),
                        server_pool,
                        socket,
                        makeServerParams(server_settings),
                        connection_filter));
    #else
                UNUSED(port);
                throw Exception(ErrorCodes::SUPPORT_IS_DISABLED, "SSL support for TCP protocol is disabled because Poco library was built without NetSSL support.");
    #endif
            });
        }

        if (server_type.shouldStart(ServerType::Type::TCP_SSH))
        {
            port_name = "tcp_ssh_port";
            createServer(
                config,
                listen_host,
                port_name,
                listen_try,
                start_servers,
                servers,
                [&](UInt16 port) -> ProtocolServerAdapter
                {
#if USE_SSH && defined(OS_LINUX)
                    Poco::Net::ServerSocket socket;
                    auto address = socketBindListen(server_settings, socket, listen_host, port, /* secure = */ false);
                    return ProtocolServerAdapter(
                        listen_host,
                        port_name,
                        "SSH PTY: " + address.toString(),
                        std::make_unique<TCPServer>(
                            new SSHPtyHandlerFactory(*this, config),
                            server_pool,
                            socket,
                            makeServerParams(server_settings),
                            connection_filter));
#else
                UNUSED(port);
                throw Exception(ErrorCodes::SUPPORT_IS_DISABLED, "SSH protocol is disabled for ClickHouse, as it has been either built without libssh or not for Linux");
#endif
                });
        }

        if (server_type.shouldStart(ServerType::Type::MYSQL))
        {
            port_name = "mysql_port";
            createServer(config, listen_host, port_name, listen_try, start_servers, servers, [&](UInt16 port) -> ProtocolServerAdapter
            {
                Poco::Net::ServerSocket socket;
                auto address = socketBindListen(server_settings, socket, listen_host, port, /* secure = */ true);
                socket.setReceiveTimeout(Poco::Timespan());
                socket.setSendTimeout(settings[Setting::send_timeout]);
                bool secure_required = server_settings[ServerSetting::mysql_require_secure_transport];
                return ProtocolServerAdapter(
                    listen_host,
                    port_name,
                    "MySQL compatibility protocol: " + address.toString(),
                    std::make_unique<TCPServer>(
                         new MySQLHandlerFactory(*this, secure_required, ProfileEvents::InterfaceMySQLReceiveBytes, ProfileEvents::InterfaceMySQLSendBytes),
                         server_pool,
                         socket,
                         makeServerParams(server_settings),
                         connection_filter));
            });
        }

        if (server_type.shouldStart(ServerType::Type::POSTGRESQL))
        {
            port_name = "postgresql_port";
            createServer(config, listen_host, port_name, listen_try, start_servers, servers, [&](UInt16 port) -> ProtocolServerAdapter
            {
                Poco::Net::ServerSocket socket;
                auto address = socketBindListen(server_settings, socket, listen_host, port, /* secure = */ true);
                socket.setReceiveTimeout(Poco::Timespan());
                socket.setSendTimeout(settings[Setting::send_timeout]);
                bool secure_required = server_settings[ServerSetting::postgresql_require_secure_transport];
                return ProtocolServerAdapter(
                    listen_host,
                    port_name,
                    "PostgreSQL compatibility protocol: " + address.toString(),
#if USE_SSL
                    std::make_unique<TCPServer>(
                         new PostgreSQLHandlerFactory(*this, secure_required, Poco::Net::SSLManager::CFG_SERVER_PREFIX, ProfileEvents::InterfacePostgreSQLReceiveBytes, ProfileEvents::InterfacePostgreSQLSendBytes),
                         server_pool,
                         socket,
                         makeServerParams(server_settings),
                         connection_filter));
#else
                    std::make_unique<TCPServer>(
                         new PostgreSQLHandlerFactory(*this, secure_required, ProfileEvents::InterfacePostgreSQLReceiveBytes, ProfileEvents::InterfacePostgreSQLSendBytes),
                         server_pool,
                         socket,
                         makeServerParams(server_settings),
                         connection_filter));
#endif
            });
        }

#if USE_GRPC
        if (server_type.shouldStart(ServerType::Type::GRPC))
        {
            port_name = "grpc_port";
            createServer(config, listen_host, port_name, listen_try, start_servers, servers, [&](UInt16 port) -> ProtocolServerAdapter
            {
                Poco::Net::SocketAddress server_address(listen_host, port);
                return ProtocolServerAdapter(
                    listen_host,
                    port_name,
                    "gRPC protocol: " + server_address.toString(),
                    std::make_unique<GRPCServer>(*this, makeSocketAddress(listen_host, port, &logger())));
            });
        }
#endif
        if (server_type.shouldStart(ServerType::Type::PROMETHEUS))
        {
            /// Prometheus (if defined and not setup yet with http_port)
            port_name = "prometheus.port";

            const char * handler_name = server_settings[ServerSetting::prometheus_keeper_metrics_only] ? "KeeperPrometheusHandler-factory" : "PrometheusHandler-factory";

            createServer(config, listen_host, port_name, listen_try, start_servers, servers, [&](UInt16 port) -> ProtocolServerAdapter
            {
                Poco::Net::ServerSocket socket;
                auto address = socketBindListen(server_settings, socket, listen_host, port);
                socket.setReceiveTimeout(settings[Setting::http_receive_timeout]);
                socket.setSendTimeout(settings[Setting::http_send_timeout]);
                return ProtocolServerAdapter(
                    listen_host,
                    port_name,
                    "Prometheus: http://" + address.toString(),
                    std::make_unique<HTTPServer>(
                        httpContext(), createHandlerFactory(*this, config, async_metrics, handler_name), server_pool, socket, http_params, nullptr, ProfileEvents::InterfacePrometheusReceiveBytes, ProfileEvents::InterfacePrometheusSendBytes));
            });
        }
    }
}

void Server::createInterserverServers(
    Poco::Util::AbstractConfiguration & config,
    const ServerSettings & server_settings,
    const Strings & interserver_listen_hosts,
    bool listen_try,
    Poco::ThreadPool & server_pool,
    AsynchronousMetrics & async_metrics,
    std::vector<ProtocolServerAdapter> & servers,
    bool start_servers,
    const ServerType & server_type)
{
    const Settings & settings = global_context->getSettingsRef();

    Poco::Net::HTTPServerParams::Ptr http_params = new Poco::Net::HTTPServerParams;
    http_params->setTimeout(settings[Setting::http_receive_timeout]);
    http_params->setKeepAliveTimeout(global_context->getServerSettings()[ServerSetting::keep_alive_timeout]);
    http_params->setMaxQueued(static_cast<int>(server_settings[ServerSetting::listen_backlog]));

    /// Now iterate over interserver_listen_hosts
    for (const auto & interserver_listen_host : interserver_listen_hosts)
    {
        const char * port_name;

        if (server_type.shouldStart(ServerType::Type::INTERSERVER_HTTP))
        {
            /// Interserver IO HTTP
            port_name = "interserver_http_port";
            createServer(config, interserver_listen_host, port_name, listen_try, start_servers, servers, [&](UInt16 port) -> ProtocolServerAdapter
            {
                Poco::Net::ServerSocket socket;
                auto address = socketBindListen(server_settings, socket, interserver_listen_host, port);
                socket.setReceiveTimeout(settings[Setting::http_receive_timeout]);
                socket.setSendTimeout(settings[Setting::http_send_timeout]);
                return ProtocolServerAdapter(
                    interserver_listen_host,
                    port_name,
                    "replica communication (interserver): http://" + address.toString(),
                    std::make_unique<HTTPServer>(
                        httpContext(),
                        createHandlerFactory(*this, config, async_metrics, "InterserverIOHTTPHandler-factory"),
                        server_pool,
                        socket,
                        http_params,
                        nullptr,
                        ProfileEvents::InterfaceInterserverReceiveBytes,
                        ProfileEvents::InterfaceInterserverSendBytes));
            });
        }

        if (server_type.shouldStart(ServerType::Type::INTERSERVER_HTTPS))
        {
            port_name = "interserver_https_port";
            createServer(config, interserver_listen_host, port_name, listen_try, start_servers, servers, [&](UInt16 port) -> ProtocolServerAdapter
            {
#if USE_SSL
                Poco::Net::SecureServerSocket socket;
                auto address = socketBindListen(server_settings, socket, interserver_listen_host, port, /* secure = */ true);
                socket.setReceiveTimeout(settings[Setting::http_receive_timeout]);
                socket.setSendTimeout(settings[Setting::http_send_timeout]);
                return ProtocolServerAdapter(
                    interserver_listen_host,
                    port_name,
                    "secure replica communication (interserver): https://" + address.toString(),
                    std::make_unique<HTTPServer>(
                        httpContext(),
                        createHandlerFactory(*this, config, async_metrics, "InterserverIOHTTPSHandler-factory"),
                        server_pool,
                        socket,
                        http_params,
                        nullptr,
                        ProfileEvents::InterfaceInterserverReceiveBytes,
                        ProfileEvents::InterfaceInterserverSendBytes));
#else
                UNUSED(port);
                throw Exception(ErrorCodes::SUPPORT_IS_DISABLED, "SSL support for TCP protocol is disabled because Poco library was built without NetSSL support.");
#endif
            });
        }
    }
}

}
