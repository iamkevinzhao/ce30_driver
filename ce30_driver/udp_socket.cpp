#ifdef UDP_SOCKET_USE_BOOST_API

#include "udp_socket.h"
#include <chrono>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/array.hpp>
#include <boost/bind.hpp>
#include "ce30_driver/timed_udp_socket.h"

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
    timed_socket_.reset(new TimedUDPSocket(udp::endpoint(address::from_string(ip_), port_)));
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
  auto size_received =
      timed_socket_->Receive(
          boost::asio::buffer(&pkt.data[0], pkt.data.size()),
          boost::posix_time::seconds(1), ec);
  if (size_received != pkt.data.size()) {
    return Diagnose::unexcepted_packet_size;
  }
  if (ec) {
    std::cerr << "UDPSocket Receive Error: " << ec.message() << std::endl;
    return Diagnose::receive_error;
  }
  return Diagnose::receive_successful;
}

Diagnose UDPSocket::SendPacket(const PacketBase& packet) {
  timed_socket_->GetSocket().send_to(
        boost::asio::buffer(&packet.data[0], packet.data.size()),
        timed_socket_->GetEndpoint());
  return Diagnose::send_successful;
}

Diagnose UDPSocket::SendPacketThreadSafe(const PacketBase &packet) {
  io_mutex_.lock();
  auto diagnose = SendPacket(packet);
  io_mutex_.unlock();
  return diagnose;
}

Diagnose UDPSocket::GetPacketThreadSafe(
    PacketBase &pkt, const double time_offset) {
  io_mutex_.lock();
  auto diagnose = GetPacket(pkt, time_offset);
  io_mutex_.unlock();
  return diagnose;
}
} // namespace ce30_driver

#endif
