// self
#include "server_singleton.h"
//boost
#include <boost/bind.hpp>
// std
#include <iostream>
boost::shared_ptr<server_singleton> server_singleton::instance;
boost::shared_ptr<server_singleton> server_singleton::server_singleton_instance()
{
  if(!instance)
  {
    instance = boost::make_shared<server_singleton>();
  }
  return instance;
}

server_singleton::server_singleton()
{
  this->initialization();
}

void server_singleton::run(boost::asio::io_service &io_service, short port)
{
//   this->acceptor = boost::shared_ptr<boost::asio::ip::tcp::acceptor>(new boost::asio::ip::tcp::acceptor(io_service, 
//     boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)));
  this->acceptor = boost::make_shared<boost::asio::ip::tcp::acceptor>(
      io_service, 
      boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port));
  this->socket = boost::make_shared<boost::asio::ip::tcp::socket>(io_service);
  this->acceptor->async_accept(*this->socket, boost::bind(&server_singleton::accepted, this->shared_from_this(), _1));
}

void server_singleton::accepted(const boost::system::error_code &error_code)
{
  if(error_code)
  {
    std::cerr << "Failed to accept.\n";
    std::cerr << error_code.message() << ' ' << error_code << '\n';
  }
  else
  {
    for(auto factory: factories) 
    {
      
      factory->create_session(*this->socket)->start();
    }
  }
  this->acceptor->async_accept(*this->socket, boost::bind(&server_singleton::accepted, this->shared_from_this(), _1));
}