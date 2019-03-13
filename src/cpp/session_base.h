#ifndef __SESSION_BASE_H__
#define __SESSION_BASE_H__
// boost
#include <boost/smart_ptr.hpp>
#include <boost/asio.hpp>
#pragma once

class session_base: public boost::enable_shared_from_this<session_base>
{
public:
  virtual void start() = 0;
  session_base(boost::asio::ip::tcp::socket &socket) :
    io_service(socket.get_io_service()), 
    socket(boost::move(socket)) {}
  virtual ~session_base() {}
  class session_abstract_factory
  {
  public:
    virtual boost::shared_ptr<session_base> create_session(
      boost::asio::ip::tcp::socket &socket) = 0;
  };
  virtual void receive(const boost::system::error_code &error_code) {}
  virtual void received(const boost::system::error_code &error_code, std::size_t size) {}
  virtual void send(const boost::system::error_code &error_code) {}
  virtual void sent(const boost::system::error_code &error_code, std::size_t size) {}
  // virtual void read()
protected:
  boost::asio::io_service &io_service;
  boost::asio::ip::tcp::socket socket;
};


#endif //!__SESSION_BASE_H__