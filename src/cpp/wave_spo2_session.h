#ifndef __WAVE_SPO2_SESSION_H__
#define __WAVE_SPO2_SESSION_H__
// self
#include "session_base.h"

class wave_spo2_session: public session_base
{
public:
  virtual void start() override {}
  class wave_spo2_session_factory: public session_abstract_factory
  {
  public:
    virtual boost::shared_ptr<session_base> create_session()
    {
      return boost::shared_ptr<session_base>(new wave_spo2_session);
    }
  };
private:
  // wave_spo2_session()
};

#endif //!__WAVE_SPO2_SESSION_H__