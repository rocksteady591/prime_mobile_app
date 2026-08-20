#include "client.h"
#include <boost/asio/connect.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ssl/context.hpp>
#include <boost/asio/write.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/http/field.hpp>
#include <boost/beast/http/verb.hpp>
#include <boost/beast/http/write.hpp>
#include <boost/beast/version.hpp>
#include <boost/json/impl/serialize.hpp>
#include <boost/json/object.hpp>
#include <boost/json/parse.hpp>
#include <boost/system/detail/error_code.hpp>
#include <boost/beast/core/bind_handler.hpp>
#include <boost/asio/ip/basic_resolver.hpp>
#include <boost/hash2/sha2.hpp>
#include <boost/hash2/digest.hpp>
#include <qt6keychain/keychain.h>
#include <QSettings>
#include <cstdlib>
#include <exception>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <qt6keychain/keychain.h>


namespace json = boost::json;


void fail(const boost::system::error_code& ec, char const* what){
    std::cerr << what << ": " << ec.message() << std::endl;
}

Client::Client(boost::asio::io_context& ioc, QObject *parent)
    : QObject{parent}, resolver_(ioc), ctx_(boost::asio::ssl::context::tlsv12_client), stream_(ioc, ctx_)
{
    connect(this, &Client::requestFailed, this, &Client::handleFailed);
    //connect(this, &Client::requestSuccess, this, &Client::handleSuccess);
    //для самоподписных сертификатов отключает проверки
    ctx_.set_verify_mode(boost::asio::ssl::verify_none);
}

Q_INVOKABLE void Client::login(const QString& login, const QString& password){
    constexpr std::string target = "/api/login";
    boost::hash2::sha2_256 h;
    std::string std_pass = password.toStdString();
    h.update(std_pass.data(), std_pass.size());
    auto diget = h.result();
    boost::json::object body;
    body["login"] = login.toStdString();
    body["password_hash"] = boost::hash2::to_string(diget);
    run("127.0.0.1", 8081, target, 11, body);
}
Q_INVOKABLE void Client::registerUser(const QString& login, const QString& password, const QString& passwordConfirm){
    if(password != passwordConfirm){
        return;
    }
    constexpr std::string target = "/api/register";
    boost::json::object body;
    boost::hash2::sha2_256 h;
    std::string std_pass = password.toStdString();
    h.update(std_pass.data(), std_pass.size());
    auto diget = h.result();
    body["login"] = login.toStdString();
    body["password_hash"] = boost::hash2::to_string(diget);
    run("127.0.0.1", 8081, target, 11, body);
}

void Client::run(const std::string &host, const unsigned short port, const std::string &target, int version, boost::json::object body)
{
    request_.version(version);
    request_.method(http::verb::post);
    request_.target(target);
    request_.set(http::field::host, host);
    request_.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
    request_.body() = boost::json::serialize(body);
    request_.prepare_payload();
    resolver_.async_resolve(
        host,
        std::to_string(port),
        boost::beast::bind_front_handler(&Client::on_resolve, shared_from_this())
    );
}

void Client::on_resolve(const boost::system::error_code& ec, tcp::resolver::results_type results_type){
    if(ec){
        fail(ec, "resolve");
    }
    boost::asio::async_connect(stream_.next_layer(),
        results_type,
        boost::beast::bind_front_handler(&Client::on_connect, shared_from_this()));
}

void Client::on_connect(const boost::system::error_code& ec, const tcp::endpoint& endpoint){
    if(ec){
        fail(ec, "on connect");
    }
    stream_.async_handshake(
        boost::asio::ssl::stream_base::client,
        boost::beast::bind_front_handler(&Client::on_handshake, shared_from_this())
    );
}


void Client::on_handshake(const boost::system::error_code& ec){
    if(ec){
        fail(ec, "handshake");
    }
    http::async_write(
        stream_,
        request_,
        boost::beast::bind_front_handler(&Client::on_write, shared_from_this())
    );
}

void Client::on_write(const boost::system::error_code& ec, std::size_t bytes_transfered){
    if(ec){
        fail(ec, "on write");
    }

    http::async_read(
        stream_,
        buffer_,
        response_,
        boost::beast::bind_front_handler(&Client::on_read, shared_from_this())
    );
}

void Client::on_read(const boost::system::error_code& ec, std::size_t bytes_transfered){
    if(ec){
        fail(ec, "on read");
        emit requestFailed(QString("Ошибка чтения: %1").arg(QString::fromStdString(ec.message())));
        return;
    }
    std::string token;
    std::string id;
    int user_id = 0;
    try{
        json::value body_val = json::parse(response_.body());
        json::object body_obj = body_val.as_object();
        token = body_obj["token"].as_string();
        id = body_obj["user_id"].as_string();
        user_id = std::stoi(id);
    }catch(const std::exception& e){
        std::cerr << "on read parsing error: " << e.what() << std::endl;
        emit requestFailed(QString("Ошибка парсинга: %1").arg(e.what()));
        return;
    }
    if(request_.target() == "/api/register"){
        emit registerSuccess();
    }else if(request_.target() == "/api/login"){
        setError("");
        saveToken(QString::fromStdString(token));
        emit loginSuccess(QString::fromStdString(token));
    }
}

void Client::handleFailed(const QString& msg){
    setError(msg);
}

void Client::saveToken(const QString& token){
    auto job = new QKeychain::WritePasswordJob("PrimalRussTechnologies", this);
    job->setAutoDelete(true);
    job->setKey("authToken");
    job->setTextData(token);

    connect(job, &QKeychain::Job::finished, this, [](QKeychain::Job *baseJob) {
        if (baseJob->error()) {
            std::cerr << "Keychain write error: " << baseJob->errorString().toStdString() << std::endl;
        } else {
            std::cout << "Token safely stored!" << std::endl;
        }
    });

    job->start();
}

void Client::setError(const QString& msg){
    if(m_errorMessage != msg){
        m_errorMessage = msg;
        emit errorMessageChanged();
    }
}
