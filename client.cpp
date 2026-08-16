#include "client.h"
#include <boost/asio/io_context.hpp>


client::client(boost::asio::io_context& ioc, QObject *parent)
    : QObject{parent}, resolver_(ioc), socket_(ioc)
{}

client::client(boost::asio::io_context& ioc) : resolver_(ioc), socket_(ioc) {}

void client::run(const std::string &host, const unsigned short port, const std::string &target, int version)
{

}
