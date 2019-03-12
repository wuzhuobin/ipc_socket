// self
#include "server_singleton.h"
#include "session_base.h"
#include "wave_spo2_session.h"

// std
#include <vector>
std::vector<boost::shared_ptr<session_base::session_abstract_factory>> 
  server_singleton::factories;
void server_singleton::initialization()
{
  factories.push_back(boost::shared_ptr<session_base::session_abstract_factory>(new wave_spo2_session::wave_spo2_session_factory));
}