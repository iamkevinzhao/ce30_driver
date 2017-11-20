#include "ce30_driver/udp_socket.h"
#include <chrono>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/array.hpp>

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
  boost::asio::io_service io_service;
  boost::shared_ptr<udp::socket> socket(
      new udp::socket(io_service, udp::endpoint(udp::v4(), port_)));
  properties_ = socket;
  return Diagnose::connect_successful;
}

Diagnose UDPSocket::GetPacket(PacketBase &pkt, const double time_offset) {
  boost::shared_ptr<udp::socket> socket =
      boost::any_cast<boost::shared_ptr<udp::socket>>(properties_);
  boost::array<unsigned char, 1000> recv_buf;
  udp::endpoint remote_endpoint;
  boost::system::error_code error;
  if (socket->receive_from(
        boost::asio::buffer(recv_buf), remote_endpoint, 0, error) !=
      pkt.data.size()) {
    return Diagnose::unequal_buffer_size;
  }
  memcpy(&pkt.data[0], &recv_buf[0], pkt.data.size());
  return Diagnose::receive_successful;
}

Diagnose UDPSocket::SendPacket(const PacketBase& packet) {
  udp::endpoint endpoint;
  try {
    std::cout << port_ << std::endl;
    // udp::endpoint endp(address::from_string(ip_), port_);
    udp::endpoint endp(address)
    std::cout << __LINE__ << std::endl;
    endpoint = endp;
    std::cout << __LINE__ << std::endl;
  } catch (std::exception& e) {
    std::cout << __LINE__ << std::endl;
    std::cerr << e.what() << std::endl;
    std::cout << __LINE__ << std::endl;
  }
  boost::shared_ptr<udp::socket> socket =
      boost::any_cast<boost::shared_ptr<udp::socket>>(properties_);
  boost::array<unsigned char, 100> send_buf;
  memcpy(&send_buf[0], &packet.data[0], packet.data.size());
  socket->send_to(boost::asio::buffer(send_buf), endpoint);
  return Diagnose::send_successful;
}
} // namespace ce30_driver
