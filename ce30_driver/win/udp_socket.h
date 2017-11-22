#ifndef INPUT_SOCKET_H
#define INPUT_SOCKET_H

#include <chrono>
#include <string>
#include <vector>
#include "ce30_driver/packet.h"
#include "ce30_driver/export.h"
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <mutex>
#include <condition_variable>
#include <thread>
#include "ce30_driver/timed_udp_socket.h"

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
  insufficient_sent_size = 13,
  connect_failed = 14,
  no_prior_connection = 15,
  unexcepted_packet_size = 16,
  receive_error = 17,
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

  std::shared_ptr<TimedUDPSocket> timed_socket_;
};
} // namespace ce30_driver

#endif // INPUT_SOCKET_H
