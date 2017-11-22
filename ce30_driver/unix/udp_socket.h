#ifndef INPUT_SOCKET_H
#define INPUT_SOCKET_H

#include <chrono>
#include <netinet/in.h>
#include <string>
#include <vector>
#include "ce30_driver/packet.h"

namespace ce30_driver {
enum class Diagnose {
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
};

class UDPSocket
{
public:
  UDPSocket(const std::string& ip = "192.168.1.80", uint16_t port = 2368);
  ~UDPSocket();
  Diagnose ConnectDevice();
  Diagnose Connect();
  Diagnose GetPacket(PacketBase& pkt, const double time_offset = 0.0);
  Diagnose SendPacket(const PacketBase& packet);

private:
  int sockfd_;
  in_addr devip_;
  uint16_t port_;
  std::string devip_str_;
};
} // namespace ce30_driver

#endif // INPUT_SOCKET_H
