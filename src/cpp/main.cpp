//
// async_tcp_echo_server.cpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2016 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <cstdlib>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/random.hpp>

using boost::asio::ip::tcp;

class session
{
public:
  session(boost::asio::io_service& io_service):
    socket_(io_service),
    timer(io_service)
  {
  }

  tcp::socket& socket()
  {
    return socket_;
  }

  void start()
  {
    // socket_.async_read_some(boost::asio::buffer(data_, max_length),
    //     boost::bind(&session::handle_read, this,
    //       boost::asio::placeholders::error,
    //       boost::asio::placeholders::bytes_transferred));
    this->timer.expires_after(std::chrono::nanoseconds(1000000000));
    this->timer.async_wait(boost::bind(&session::send, this, _1));
  }

private:
  void send(const boost::system::error_code &error_code)
  {
    if(error_code)
    {
      std::cerr << "Fail to send. \n";
      std::cerr << error_code.message() << ' ' << error_code << '\n';
      return;
    }
    boost::random::uniform_int_distribution<> a_hundred(0, 99);
    std::string data = std::to_string(a_hundred(this->rng));
    // std::cerr << "send\n";
    this->socket_.async_send(boost::asio::buffer(data, sizeof(data)), 0, [](const boost::system::error_code &, std::size_t){}); 
    // this->timer.expires_after(std::chrono::seconds(1));
    this->timer.expires_after(std::chrono::nanoseconds(1000000000));
    this->timer.async_wait(boost::bind(&session::send, this, _1));
  }

  tcp::socket socket_;
  boost::asio::steady_timer timer;
  boost::random::mt19937 rng;
};

class server
{
public:
  server(boost::asio::io_service& io_service, short port)
    : io_service_(io_service),
      acceptor_(io_service, tcp::endpoint(tcp::v4(), port))
  {
    start_accept();
  }

private:
  void start_accept()
  {
    session* new_session = new session(io_service_);
    acceptor_.async_accept(new_session->socket(),
        boost::bind(&server::handle_accept, this, new_session,
          boost::asio::placeholders::error));
  }

  void handle_accept(session* new_session,
      const boost::system::error_code& error)
  {
    if (!error)
    {
      new_session->start();
    }
    else
    {
      delete new_session;
    }

    start_accept();
  }

  boost::asio::io_service& io_service_;
  tcp::acceptor acceptor_;
};

int main(int argc, char* argv[])
{
  try
  {
    if (argc != 2)
    {
      std::cerr << "Usage: async_tcp_echo_server <port>\n";
      return 1;
    }

    boost::asio::io_service io_service;

    using namespace std; // For atoi.
    server s(io_service, atoi(argv[1]));

    io_service.run();
  }
  catch (std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}