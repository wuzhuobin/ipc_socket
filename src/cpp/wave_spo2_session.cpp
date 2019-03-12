// self
#include "wave_spo2_session.h"

// std
#include <vector>
#include <iostream>

// boost
#include <boost/bind.hpp>
static const std::vector<int> ONE_SECOND_DATA_ORIGIN{
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

wave_spo2_session::wave_spo2_session(boost::asio::ip::tcp::socket &socket) :
  session_base(socket), 
  timer(socket.get_io_service()), 
  milliseconds(50), 
  index(0),
  running(false)
{

}

void wave_spo2_session::start()
{
  this->running = true;
  this->timer.async_wait(boost::bind(
    &session_base::send, this->shared_from_this(), _1));
}

void wave_spo2_session::send(const boost::system::error_code &error_code)
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
    boost::bind(&session_base::sent, this->shared_from_this(), _1, _2));
  this->timer.expires_after(std::chrono::milliseconds(this->milliseconds));
  this->timer.async_wait(boost::bind(&session_base::send, this->shared_from_this(), _1));
  ++this->index;
  if(this->index == HZ) 
  {
    this->index = 0;
  }
}

void wave_spo2_session::sent(const boost::system::error_code &error_code, std::size_t size)
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
    this->running = false;
    return;
  }
}