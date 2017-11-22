#include "udp_socket.h"
#include <chrono>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/array.hpp>
#include <boost/bind.hpp>

using namespace std::chrono;
using namespace boost::asio::ip;

namespace ce30_driver {
UDPSocket::UDPSocket(const std::string& ip, uint16_t port)
  : ip_(ip), port_(port)
{
}

UDPSocket::~UDPSocket() {
}

Diagnose UDPSocket::Connect() {
  try {
    udp::endpoint listen_endpoint(address::from_string(ip_), port_);
    timed_socket_.reset(new TimedUDPSocket(listen_endpoint));
  } catch (const std::exception& e) {
    std::cerr << "UDPSocket Connection Error: " << e.what() << std::endl;
    return Diagnose::connect_failed;
  }
  return Diagnose::connect_successful;
}

Diagnose UDPSocket::GetPacket(PacketBase &pkt, const double time_offset) {
  if (!timed_socket_) {
    return Diagnose::no_prior_connection;
  }
  boost::system::error_code ec;
  if (timed_socket_->Receive(
          boost::asio::buffer(&pkt.data[0], pkt.data.size()),
          boost::posix_time::seconds(1), ec) != pkt.data.size()) {
    return Diagnose::unexcepted_packet_size;
  }
  if (ec) {
    std::cerr << "UDPSocket Receive Error: " << ec.message() << std::endl;
    return Diagnose::receive_error;
  }
  return Diagnose::receive_successful;
}

Diagnose UDPSocket::SendPacket(const PacketBase& packet) {
  return Diagnose::send_successful;
}
} // namespace ce30_driver
