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
#include <boost/smart_ptr.hpp>
// #include <boost/geometry.hpp>
// std 
#include <vector>
#if __cplusplus <= 199711L
#define constexpr const
#endif

std::ostream &operator<<(std::ostream &os, const std::vector<int> &data)
{
  for(int v: data)
  {
    os << v << ' ';
  }
  return os;
}
// template <size_t N>
// struct array_
// {
//   static const int INDEX = 252 / HZ * N;
//   static const int V = ONE_SECOND_DATA_ORIGIN[INDEX];
//   static const int PREVIOUS_V = array_<N - 1>::V;
// };
// template <>
// struct array_<0>
// {
//   static const int INDEX = 0;
//   static const int V = ONE_SECOND_DATA_ORIGIN[0];
//   static const int PREVIOUS_V = -1;
// };

class server
{
public:
  server(boost::asio::io_service& io_service, short port)
    : io_service(io_service),
      acceptor(io_service, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port))
  {
    this->acceptor.async_accept(boost::bind(&server::accepted, this, _1, _2));
  }

private:
  void accepted(boost::system::error_code error_code, boost::asio::ip::tcp::socket& socket)
  {
    if(error_code)
    {
      std::cerr << "Failed to accept.\n";
      std::cerr << error_code.message() << ' ' << error_code << '\n';
    }
    else
    {
      // boost::make_shared<session>(this->io_service, boost::move(socket))->start();
      // boost::make_shared<spo2_session>(this->io_service, boost::move(socket), 500)->start();
      boost::make_shared<wave_spo2_session>(this->io_service, boost::move(socket), 50)->start();
    }
    this->acceptor.async_accept(boost::bind(&server::accepted, this, _1, _2));
  }

  boost::asio::io_service& io_service;
  boost::asio::ip::tcp::acceptor acceptor;
  class session: public boost::enable_shared_from_this<session>
  {
    public:
      session(boost::asio::io_service& io_service, boost::asio::ip::tcp::socket &socket):
        socket(boost::move(socket)),
        timer(io_service)
      {
      }
      void start()
      {
        this->timer.async_wait(boost::bind(&session::send, this->shared_from_this(), _1));
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
        data = std::to_string(a_hundred(this->rng));
        // std::cerr << "send\n";
        this->socket.async_send(boost::asio::buffer(data, sizeof(data)), 0, [](const boost::system::error_code &, std::size_t){}); 
        this->timer.expires_after(std::chrono::milliseconds(500));
        this->timer.async_wait(boost::bind(&session::send, this->shared_from_this(), _1));
      }

      boost::asio::ip::tcp::socket socket;
      boost::asio::steady_timer timer;
      boost::random::mt19937 rng;
      std::string data;
  };

  class spo2_session: public boost::enable_shared_from_this<spo2_session>
  {
    public:
      spo2_session(boost::asio::io_service& io_service, boost::asio::ip::tcp::socket &socket, const int &milliseconds):
        socket(boost::move(socket)),
        timer(io_service), 
        milliseconds(milliseconds)
      {

      }
      void start()
      {
        this->timer.async_wait(boost::bind(&spo2_session::send, this->shared_from_this(), _1));
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
        boost::random::uniform_int_distribution<> a_hundred(30, 129);
        this->data = std::to_string(a_hundred(this->rng));
        this->socket.async_send(boost::asio::buffer(this->data, sizeof(this->data)), 0, 
          boost::bind(&spo2_session::sent, this->shared_from_this(), _1, _2)); 
        this->timer.expires_after(std::chrono::milliseconds(this->milliseconds));
        this->timer.async_wait(boost::bind(&spo2_session::send, this->shared_from_this(), _1));
      }
      void sent(const boost::system::error_code &error_code, std::size_t size)
      {
        (void)(size);
        if(error_code)
        {
          std::cerr << "Failed to send.\n";
          std::cerr << error_code.message() << '\n';
        }
      }
      boost::asio::ip::tcp::socket socket;
      boost::asio::steady_timer timer;
      boost::random::mt19937 rng;
      std::string data;
      int milliseconds;
  };

  class wave_spo2_session: public boost::enable_shared_from_this<wave_spo2_session>
  {
    public:
      wave_spo2_session(
        boost::asio::io_service &io_service, 
        boost::asio::ip::tcp::socket &socket, 
        const int &milliseconds) :
        socket(boost::move(socket)),
        timer(io_service), 
        milliseconds(milliseconds),
        index(0),
        running(false)
      {
        // this->one_second_data = create_data(this->milliseconds);
        // this->cit = this->one_second_data.cbegin();
      }
      void start()
      {
        this->running = true;
        this->timer.async_wait(boost::bind(&wave_spo2_session::send, this->shared_from_this(), _1));
      }
    private:
      static const std::vector<int> create_data(const int &interval)
      {
        std::vector<int> one_second;
        const int HZ = 1000 / interval;
        one_second.resize(HZ);
        const size_t num = ONE_SECOND_DATA_ORIGIN.size() / sizeof(int);
        for(size_t i = 0; i < HZ; ++i)
        {
          one_second[i] = ONE_SECOND_DATA_ORIGIN[i * num / HZ];
        }
        return boost::move(one_second);
      }

      void send(const boost::system::error_code &error_code)
      {
        if(error_code)
        {
          std::cerr << "Fail to send. \n";
          std::cerr << error_code.message() << ' ' << error_code << '\n';
          return;
        }
        if(!this->running)
        {
          std::cerr << "Fail to send and stop. \n ";
          return;
        }
        // this->data = std::to_string(*this->cit);
        const int HZ = 1000 / this->milliseconds;

        std::vector<int>::const_iterator begin = 
          ONE_SECOND_DATA_ORIGIN.cbegin() + this->index * ONE_SECOND_DATA_ORIGIN.size() / HZ;
        std::vector<int>::const_iterator end = 
          (this->index == HZ - 1)?  ONE_SECOND_DATA_ORIGIN.cend() : ONE_SECOND_DATA_ORIGIN.cbegin() + (this->index + 1 ) * ONE_SECOND_DATA_ORIGIN.size() / HZ;
        this->frame = std::vector<int>(begin, end);
        this->socket.async_send(boost::asio::buffer(this->frame), 0, 
          boost::bind(&wave_spo2_session::sent, this->shared_from_this(), _1, _2));
        this->timer.expires_after(std::chrono::milliseconds(this->milliseconds));
        this->timer.async_wait(boost::bind(&wave_spo2_session::send, this->shared_from_this(), _1));
        ++this->index;
        if(this->index == HZ) 
        {
          this->index = 0;
        }
      }

      void sent(const boost::system::error_code &error_code, std::size_t size)
      {
        (void*)(size);
        if(error_code)
        {
          std::cerr << "Fail to send. \n";
          std::cerr << error_code.message() << ' ' << error_code << '\n';
          // if(error_code)
          // {
          //   std::cerr << "Fail to send. \n";
          //   std::cerr << error_code.message() << ' ' << error_code << '\n';
          // }
          return;
        }
      }

      boost::asio::ip::tcp::socket socket;
      boost::asio::steady_timer timer;
      int milliseconds;
      std::vector<int> frame;
      size_t index;
      static const std::vector<int> ONE_SECOND_DATA_ORIGIN;
      bool running;
  };
};

const std::vector<int> server::wave_spo2_session::ONE_SECOND_DATA_ORIGIN{
  81, 74, 76, 79, 81, 74, 66, 58, 51, 43, 30, 28, 23, 17, 10,
  5, 2, 2, 2, 7, 30, 66, 110, 156, 189, 204, 199, 179, 151,
  122, 97, 81, 74, 76, 79,
  81, 74, 66, 58, 51, 43, 30, 28, 23, 17, 10, 5, 2, 2, 2, 7,
  30, 66, 110, 156, 189, 204, 199, 179, 151, 122, 97, 81, 74, 76, 79,
  81, 74, 66, 58, 51, 43, 30, 28, 23, 17, 10, 5, 2, 2, 2, 7,
  30, 66, 110, 156, 189, 204, 199, 179, 151, 122, 97, 81, 74, 76, 79,
  81, 74, 66, 58, 51, 43, 30, 28, 23, 17, 10,
  5, 2, 2, 2, 7, 30, 66, 110, 156, 189, 204, 199, 179, 151,
  122, 97, 81, 74, 76, 79, 81, 74, 66, 58, 51, 43, 30, 28,
  23, 17, 10, 5, 2, 2, 2, 7, 30, 66, 110, 156, 189, 204, 199,
  179, 151, 122, 97, 81, 74, 76, 79, 81, 74, 66, 58, 51, 43,
  30, 28, 23, 17, 10, 5, 2, 2, 2, 7, 30, 66, 110, 156, 189,
  204, 199, 179, 151, 122, 97, 81, 74, 76, 79, 81, 74, 66, 58,
  51, 43, 30, 28, 23, 17, 10, 5, 2, 2, 2, 7, 30, 66, 110, 156,
  189, 204, 199, 179, 151, 122, 97, 81, 74, 76, 79, 81, 74, 66,
  58, 51, 43, 30, 28, 23, 17, 10, 5, 2, 2, 2, 7, 30, 66, 110,
  156, 189, 204, 199, 179, 151, 122, 97, 81, 74, 76, 79};
int main(int argc, char* argv[]) try
{
  if (argc != 2)
  {
    std::cerr << "Usage: ipc_socket <port>\n";
    return EXIT_FAILURE;
  }
  boost::asio::io_service io_service;
  server s(io_service, atoi(argv[1]));
  io_service.run();
  return EXIT_SUCCESS;
}
catch (std::exception& e)
{
  std::cerr << "Exception: " << e.what() << "\n";
}