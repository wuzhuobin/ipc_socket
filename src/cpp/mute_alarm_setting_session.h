#ifndef __MUTE_ALARM_SETTING_SESSION_H__
#define __MUTE_ALARM_SETTING_SESSION_H__
#pragma once
// self
#include "session_base.h"
#pragma pack(1)
typedef struct MuteAlarmSetting
{
  struct
  {
    unsigned char stx;
  };
  struct
  {
    unsigned char type;
    unsigned short len;
  };
  struct
  {
    bool mute;
    unsigned char duration;
  };
  struct
  {
    unsigned char etx;
  };
} MuteAlarmSetting;
#pragma pack()

class mute_alarm_setting_session: public session_base
{
public:
  virtual void start() override;
  class mute_alarm_setting_session_factory: public session_abstract_factory
  {
  public:
    virtual boost::shared_ptr<session_base> create_session(
      boost::asio::ip::tcp::socket &socket) {
        return boost::shared_ptr<session_base>(
          new mute_alarm_setting_session(socket));
    }
  };
private:
  mute_alarm_setting_session(boost::asio::ip::tcp::socket &socket);
  virtual void receive(const boost::system::error_code &error_code) override;
  virtual void received(const boost::system::error_code &error_code, std::size_t size) override;
  boost::asio::mutable_buffer buffer;
  MuteAlarmSetting data[1];
  bool running;
};

#endif//!__MUTE_ALARM_SETTING_SESSION_H__