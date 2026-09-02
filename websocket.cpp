#include "websocket.h"
#include <boost/asio/io_context.hpp>
#include <boost/asio/ssl/context.hpp>
#include <boost/asio/ssl/host_name_verification.hpp>
#include <boost/asio/strand.hpp>
#include <boost/beast/core/bind_handler.hpp>
#include <boost/beast/core/error.hpp>
#include <boost/beast/core/stream_traits.hpp>
#include <QEventLoop>
#include <boost/beast/websocket/rfc6455.hpp>
#include <boost/beast/websocket/stream_base.hpp>
#include <openssl/tls1.h>
#include <chrono>
#include <iostream>
#include <string>

Websocket::Websocket(net::io_context& ioc, ssl::context& ctx, QObject* parent) : resolver_(net::make_strand(ioc)), ws_(net::make_strand(ioc), ctx) {}

void Websocket::run(const QString& host, const QString& port){
    if(!SSL_set_tlsext_host_name(ws_.next_layer().native_handle(), host.data())){
        beast::error_code ec{
                        static_cast<int>(::ERR_get_error()),
                        net::error::get_ssl_category()};
                    std::cerr << ec.message() << "\n";
                    return;
    }
    ws_.next_layer().set_verify_callback(ssl::host_name_verification(host.toStdString()));
    host_ = host.toStdString();
    resolver_.async_resolve(host_, port.toStdString(), beast::bind_front_handler(&Websocket::on_resolve, shared_from_this()));
}

/*void Websocket::set_token(const QString& token){
    token_ = token;
    std::cout << "Token set token: " << token_.toStdString() << std::endl;
}*/

void Websocket::on_resolve(const beast::error_code& ec, tcp::resolver::results_type results){
    if(ec){
        std::cerr << "on resolve: " << ec.message() << std::endl;
        return;
    }
    beast::get_lowest_layer(ws_).expires_after(std::chrono::seconds(30));

    beast::get_lowest_layer(ws_).async_connect(
        results,
        beast::bind_front_handler(&Websocket::on_connect, shared_from_this())
    );
}

void Websocket::on_connect(const beast::error_code& ec, tcp::resolver::results_type::endpoint_type ep){
    if(ec){
        std::cerr << "on connect: " << ec.message() << std::endl;
        return;
    }
    beast::get_lowest_layer(ws_).expires_after(std::chrono::seconds(30));

    host_ += ":" + std::to_string(ep.port());
    ws_.next_layer().async_handshake(
        ssl::stream_base::client,
        beast::bind_front_handler(&Websocket::on_ssl_handshake, shared_from_this())
    );
}

/*QString Websocket::get_token() const {
    QKeychain::ReadPasswordJob job("PrimalRussTechnologies");
        job.setKey("authToken");

        QString tokenResult = "";
        QEventLoop loop;

        // Как только джоб закончит работу, выходим из локального цикла
        connect(&job, &QKeychain::Job::finished, &loop, [&job, &tokenResult, &loop](QKeychain::Job *baseJob) {
            if (!baseJob->error()) {
                auto readJob = qobject_cast<QKeychain::ReadPasswordJob*>(baseJob);
                tokenResult = readJob->textData();
            }
            loop.quit();
        });

        job.start();
        loop.exec(); // Блокируем выполнение на микросекунды до получения данных из хранилища

        return tokenResult;
}*/

void Websocket::on_ssl_handshake(const beast::error_code& ec){
    if(ec){
        std::cerr << "on ssl handshake: " << ec.message() << std::endl;
    }

    beast::get_lowest_layer(ws_).expires_never();
    ws_.set_option(
                beast::websocket::stream_base::timeout::suggested(
                    beast::role_type::client));

    if(token_.isEmpty()){
        std::cerr << "on ssl handshake: " << "token is empty" << std::endl;
        return;
    }

    ws_.set_option(beast::websocket::stream_base::decorator(
        [this](beast::websocket::request_type& req){
            req.set(http::field::authorization, "Bearer " + token_.toStdString());
        }
    ));
    ws_.async_handshake(
        host_,
        "/",
        beast::bind_front_handler(&Websocket::on_handhsake, shared_from_this())
    );
}

void Websocket::on_handhsake(const beast::error_code& ec){
    if(ec){
        std::cerr << "on handshake: " << ec.message() << std::endl;
        return;
    }
}
