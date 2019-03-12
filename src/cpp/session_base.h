#ifndef __SESSION_BASE_H__
#define __SESSION_BASE_H__
// boost
#include <boost/smart_ptr.hpp>

class session_base: public boost::enable_shared_from_this<session_base>
{
public:
  virtual void start() = 0;
  virtual ~session_base() {}
  class session_abstract_factory
  {
    virtual boost::shared_ptr<session_base> create_session() = 0;
  };
};


#endif //!__SESSION_BASE_H__