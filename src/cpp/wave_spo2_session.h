#ifndef __WAVE_SPO2_SESSION_H__
#define __WAVE_SPO2_SESSION_H__
#pragma once
// self
#include "session_base.h"

// std
#include <vector>

class wave_spo2_session: public session_base
{
public:
  virtual void start() override;
  class wave_spo2_session_factory: public session_abstract_factory
  {
  public:
    virtual boost::shared_ptr<session_base> create_session(
      boost::asio::ip::tcp::socket &socket) override
    {
      return boost::shared_ptr<session_base>(
        new wave_spo2_session(socket));
    }
  };
private:
  wave_spo2_session(boost::asio::ip::tcp::socket &socket);
  virtual void send(const boost::system::error_code &error_code) override;
  virtual void sent(const boost::system::error_code &error_code, std::size_t size) override;
  boost::asio::steady_timer timer;
  int milliseconds;
  std::vector<int> frame;
  size_t index;
  // static const std::vector<int> ONE_SECOND_DATA_ORIGIN;
  bool running;
};

#endif //!__WAVE_SPO2_SESSION_H__