#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QQmlEngine>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ssl/context.hpp>
#include <boost/asio/ssl/stream.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/core/flat_buffer.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/http/empty_body.hpp>
#include <boost/beast/http/message_fwd.hpp>
#include <boost/beast/http/string_body_fwd.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/system/detail/error_code.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/json.hpp>
#include <memory>

using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>
namespace http = boost::beast::http;    // from <boost/beast/http.hpp>

class Client : public QObject, public std::enable_shared_from_this<Client>
{
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(QString errorMessage READ errorMessage NOTIFY errorMessageChanged)
public:
    explicit Client(boost::asio::io_context& ioc, QObject *parent = nullptr);
    Q_INVOKABLE void login(const QString& login, const QString& password);
    Q_INVOKABLE void registerUser(const QString& login, const QString& password, const QString& passwordConfirm);
private:
    tcp::resolver resolver_;
    boost::asio::ssl::context ctx_;
    boost::asio::ssl::stream<tcp::socket> stream_;
    boost::beast::flat_buffer buffer_;
    http::request<http::string_body> request_;
    http::response<http::string_body> response_;
    QString m_errorMessage;

public:
    void run(const std::string& host, const unsigned short port, const std::string& target, int version, boost::json::object body);
    void on_resolve(const boost::system::error_code& ec, tcp::resolver::results_type results_type);
    void on_connect(const boost::system::error_code& ec, const tcp::endpoint& endpoint);
    void on_handshake(const boost::system::error_code& ec);
    void on_write(const boost::system::error_code& ec, std::size_t bytes_transfered);
    void on_read(const boost::system::error_code& ec, std::size_t bytes_transfered);
    QString errorMessage() const { return m_errorMessage; }
signals:
    void errorMessageChanged();
    void requestFailed(const QString& msg);
    void registerSuccess();
    void loginSuccess(const QString& token);
    //void requestSuccess(const QString& token);
private slots:
    void handleFailed(const QString& msg);
    //void handleSuccess(const QString& token);
private:
    void saveToken(const QString& token);
    void setError(const QString& msg);
};

#endif // CLIENT_H
