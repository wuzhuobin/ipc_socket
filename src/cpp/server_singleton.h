#ifndef __SERVER_SINGLETON_H__
#define __SERVER_SINGLETON_H__
// boost
#include <boost/smart_ptr.hpp>
#include <boost/asio.hpp>

class server_singleton: public boost::enable_shared_from_this<server_singleton>
{
public:
  static boost::shared_ptr<server_singleton> server_singleton_instance(boost::asio::io_service &io_service, short port);
  static boost::shared_ptr<server_singleton> server_singleton_instance();
  server_singleton();
  void run(boost::asio::io_service &io_service, short port);
private:
  void accepted(const boost::system::error_code &error_code, boost::asio::ip::tcp::socket& socket);
  static boost::shared_ptr<server_singleton> instance;
  boost::shared_ptr<boost::asio::ip::tcp::acceptor> acceptor;
};

#endif //!__SERVER_SINGLETON_H__