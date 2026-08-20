#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QtQml>
#include <boost/asio.hpp>
#include <boost/asio/executor_work_guard.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/beast.hpp>
#include "client.h"
#include <memory>
#include <thread>

namespace net = boost::asio;
using tcp = net::ip::tcp;

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;

    net::io_context ioc;

    auto work_guard = net::make_work_guard(ioc);
    std::thread ioc_thread([&ioc]() {
        ioc.run();
    });
    auto apiClient = std::make_shared<Client>(ioc);
    qmlRegisterSingletonInstance("android_prime", 1, 0, "ApiClient", apiClient.get());

    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    engine.loadFromModule("android_prime", "Main");
    int result = QGuiApplication::exec();
    work_guard.reset();
    ioc.stop();
    if (ioc_thread.joinable()) {
        ioc_thread.join();
    }
    return result;
}
