#ifndef INPUT_SOCKET_H
#define INPUT_SOCKET_H

#include <chrono>
#include <string>
#include <vector>
#include "ce30_driver/packet.h"
#include "ce30_driver/export.h"
#include <mutex>

namespace ce30_driver {
/**
 * @brief The Diagnose enum
 */
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

class TimedUDPSocket;

/**
 * @brief UDP communication interface
 */
class API UDPSocket
{
public:
  /**
   * @brief constructor
   * @param ip device IP
   * @param port device port
   */
  UDPSocket(const std::string& ip = "192.168.1.80", uint16_t port = 2368);
  ~UDPSocket();
  /**
   * @brief establish UDP socket
   * @return socket diagnose
   */
  Diagnose Connect();
  /**
   * @brief packet receiving interface
   * @param pkt packet object
   * @param time_offset time stamp offset [under development]
   * @return socket diagnose
   */
  Diagnose GetPacket(PacketBase& pkt, const double time_offset = 0.0);
  /**
   * @brief packet sending interface
   * @param packet packet object
   * @return socket diagnose
   */
  Diagnose SendPacket(const PacketBase& packet);
  /**
   * @brief packet sending interface (thread safe)
   * @param packet packet object
   * @return socket diagnose
   */
  Diagnose SendPacketThreadSafe(const PacketBase& packet);
  /**
   * @brief packet receiving interface (thread safe)
   * @param pkt packet object
   * @param time_offset time stamp offset [under development]
   * @return socket diagnose
   */
  Diagnose GetPacketThreadSafe(PacketBase& pkt, const double time_offset = 0.0);
private:
  uint16_t port_;
  std::string ip_;

  std::shared_ptr<TimedUDPSocket> timed_socket_;

  std::mutex io_mutex_;
};
} // namespace ce30_driver

#endif // INPUT_SOCKET_H
