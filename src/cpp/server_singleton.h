#ifndef __SERVER_SINGLETON_H__
#define __SERVER_SINGLETON_H__
#pragma once
// self
#include "session_base.h"

// boost
#include <boost/smart_ptr.hpp>
#include <boost/asio.hpp>

// std
#include <vector>

class server_singleton: public boost::enable_shared_from_this<server_singleton>
{
public:
  static boost::shared_ptr<server_singleton> server_singleton_instance();
  server_singleton();
  void run(boost::asio::io_service &io_service, short port);
private:
  void accepted(const boost::system::error_code &error_code);
  void initialization();
  static boost::shared_ptr<server_singleton> instance;
  boost::shared_ptr<boost::asio::ip::tcp::acceptor> acceptor;
  boost::shared_ptr<boost::asio::ip::tcp::socket> socket;
  static std::vector<boost::shared_ptr<session_base::session_abstract_factory>> factories;
};

#endif //!__SERVER_SINGLETON_H__