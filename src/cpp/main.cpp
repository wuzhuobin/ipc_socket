// boost
#include <boost/asio.hpp>
#include <boost/smart_ptr.hpp>

// std 
#include <cstdlib>
#include <iostream>
#include <vector>

// self
#include "server_singleton.h"

int main(int argc, char* argv[]) try
{
  if (argc != 2)
  {
    std::cerr << "Usage: ipc_socket <port>\n";
    return EXIT_FAILURE;
  }
  boost::asio::io_service io_service;
  boost::shared_ptr<server_singleton> server = server_singleton::server_singleton_instance();
  server->run(io_service, atoi(argv[1]));
  io_service.run();
  return EXIT_SUCCESS;
}
catch (std::exception& e)
{
  std::cerr << "Exception: " << e.what() << "\n";
}