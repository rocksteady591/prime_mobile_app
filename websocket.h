#pragma once
#include <QObject>
#include <QQmlEngine>
#include <boost/asio.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/beast.hpp>
#include <boost/beast/core/error.hpp>
#include <boost/beast/core/flat_buffer.hpp>
#include <boost/beast/core/tcp_stream.hpp>
#include <boost/beast/websocket/stream.hpp>
#include <boost/beast/websocket/ssl.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/ssl.hpp>
#include <string>
#include <memory>

namespace net = boost::asio;
namespace beast = boost::beast;
namespace ssl = net::ssl;
namespace http = beast::http;
using tcp = net::ip::tcp;

class Websocket : public QObject, public std::enable_shared_from_this<Websocket>{
    Q_OBJECT
    QML_ELEMENT
public:
    explicit Websocket(net::io_context& ioc, ssl::context& ctx, QObject *parent = nullptr);
    Q_INVOKABLE void run(const QString& host, const QString& port);
    void on_resolve(const beast::error_code& ec, tcp::resolver::results_type results);
    void on_connect(const beast::error_code& ec, tcp::resolver::results_type::endpoint_type ep);
    void on_ssl_handshake(const beast::error_code& ec);
    void on_handhsake(const beast::error_code& ec);
    //Q_INVOKABLE QString get_token() const;
    //Q_INVOKABLE void send_message(const QString& message);
    //Q_INVOKABLE void set_token(const QString& token);
private:
    tcp::resolver resolver_;
    beast::websocket::stream<ssl::stream<beast::tcp_stream>> ws_;
    beast::flat_buffer buffer_;
    std::string host_;
    std::string text_;
    QString token_;
};
