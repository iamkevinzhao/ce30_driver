#ifndef INPUT_SOCKET_H
#define INPUT_SOCKET_H

#include <chrono>
#include <string>
#include <vector>
#include "packet.h"
#include "export.h"
#include <boost/any.hpp>

namespace ce30_driver {
enum class API Diagnose {
  socket_error = 0,
  bind_error = 1,
  non_block_error = 2,
  connect_successful = 3,
  poll_error = 4,
  poll_timeout = 5,
  device_error = 6,
  receive_fail = 7,
  receive_successful = 8,
  device_connected = 9,
  send_fail = 10,
  send_successful = 11,
  unequal_buffer_size = 12,
};

class API UDPSocket
{
public:
  UDPSocket(const std::string& ip = "192.168.1.80", uint16_t port = 2368);
  ~UDPSocket();
  Diagnose ConnectDevice();
  Diagnose Connect();
  Diagnose GetPacket(PacketBase& pkt, const double time_offset = 0.0);
  Diagnose SendPacket(const PacketBase& packet);

private:
  uint16_t port_;
  std::string ip_;
  boost::any properties_;
};
} // namespace ce30_driver

#endif // INPUT_SOCKET_H