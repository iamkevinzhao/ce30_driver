#ifndef TIMED_UDP_SOCKET_H
#define TIMED_UDP_SOCKET_H

#include <boost/asio.hpp>

namespace ce30_driver {
class TimedUDPSocket
{
public:
  TimedUDPSocket(const boost::asio::ip::udp::endpoint& listen_endpoint);
  std::size_t Receive(
      const boost::asio::mutable_buffer& buffer,
      boost::posix_time::time_duration timeout,
      boost::system::error_code& ec);
  static void HandleReceive(
      const boost::system::error_code& ec,
      std::size_t length,
      boost::system::error_code* out_ec,
      std::size_t* out_length);
  boost::asio::ip::udp::socket& GetSocket();
  boost::asio::ip::udp::endpoint GetEndpoint();
private:
  void CheckDeadline();
  boost::asio::io_service io_service_;
  boost::asio::ip::udp::socket socket_;
  boost::asio::deadline_timer deadline_;
  boost::asio::ip::udp::endpoint listen_endpoint_;
};
}

#endif // TIMED_UDP_SOCKET_H
