// self
#include "mute_alarm_setting_session.h"

// boost
#include <boost/bind.hpp>

// std
#include <iostream>

std::ostream & operator<<(std::ostream &os, MuteAlarmSetting data)
{
  os << "stx: " << data.stx << '\n';
  os << "type: " << data.type << ' ' << "len: " << data.len << '\n';
  os << "mute: " << data.mute  << ' ' << "duration: " << data.duration << '\n'; 
  os << "etx: " << data.etx;
}

void mute_alarm_setting_session::start()
{
  this->running = true;
  this->socket.async_receive(this->buffer, boost::bind(
    &session_base::received, this->shared_from_this(), _1, _2));
}

mute_alarm_setting_session::mute_alarm_setting_session(boost::asio::ip::tcp::socket &socket):
   session_base(socket), 
   buffer(boost::asio::buffer(this->data)),
   running(false)
 {
    std::cerr << "MuteAlarmSetting: " << sizeof(MuteAlarmSetting) << '\n';
    std::cerr << this->data[0] << '\n';
 }

void mute_alarm_setting_session::receive(const boost::system::error_code &error_code)
{

}

void mute_alarm_setting_session::received(const boost::system::error_code &error_code, std::size_t size)
{
  if(error_code)
  {
    std::cerr << "Fail to received. \n";
    std::cerr << error_code.message() << ' ' << error_code << '\n';
    return;
  }
  std::cout << "size: " << size << '\n';
  std::cout << this->data[0] << '\n';
  if(this->running)
  {
    this->socket.async_receive(this->buffer, boost::bind(
      &session_base::received, this->shared_from_this(), _1, _2));
  }
}