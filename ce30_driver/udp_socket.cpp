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


//#define NOMINMAX
#include <WinSock2.h>
#define BUFLEN 1024

namespace ce30_driver {

SOCKET s;
struct sockaddr_in server, si_other, sensor;
int slen, recv_len;
char buf[BUFLEN];
WSADATA wsa;

UDPSocket::UDPSocket(const std::string& ip, uint16_t port)
  : ip_(ip), port_(port)
{
  slen = sizeof(si_other);
}

UDPSocket::~UDPSocket() {
  closesocket(s);
  WSACleanup();
}

Diagnose UDPSocket::Connect() {
//  try {
//    timed_socket_.reset(new TimedUDPSocket(udp::endpoint(address::from_string(ip_), port_)));
//  } catch (const std::exception& e) {
//    std::cerr << "UDPSocket Connection Error: " << e.what() << std::endl;
//    return Diagnose::connect_failed;
//  }
//  return Diagnose::connect_successful;
  if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
    return Diagnose::connect_failed;
  }
  if ((s = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET) {
    return Diagnose::connect_failed;
  }
//  u_long mode = 1;
//  if (ioctlsocket(s, FIONBIO, &mode) != NO_ERROR) {
//    return Diagnose::connect_failed;
//  }
  int timeout = 1000;
  setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout));
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = INADDR_ANY;
  server.sin_port = htons(port_);
  sensor.sin_family = AF_INET;
  sensor.sin_addr.s_addr = inet_addr("192.168.1.80");
  sensor.sin_port = htons(port_);
  if (bind(s, (struct sockaddr*)&server, sizeof(server)) == SOCKET_ERROR) {
    return Diagnose::connect_failed;
  }
  std::cout << __LINE__ << std::endl;
  return Diagnose::connect_successful;
}

Diagnose UDPSocket::GetPacket(PacketBase &pkt, const double time_offset) {
//  if (!timed_socket_) {
//    return Diagnose::no_prior_connection;
//  }
//  boost::system::error_code ec;
//  auto size_received =
//      timed_socket_->Receive(
//          boost::asio::buffer(&pkt.data[0], pkt.data.size()),
//          boost::posix_time::seconds(1), ec);
//  if (size_received != pkt.data.size()) {
//    return Diagnose::unexcepted_packet_size;
//  }
//  if (ec) {
//    std::cerr << "UDPSocket Receive Error: " << ec.message() << std::endl;
//    return Diagnose::receive_error;
//  }
//  return Diagnose::receive_successful;
    std::cout << __LINE__ << std::endl;
//  auto clock = system_clock::now();
//  while ((system_clock::now() - clock).duration.count() < 1000) {
//    if (
//        (recv_len =
//            recvfrom(s, buf, BUFLEN, 0, (struct sockaddr*)&si_other, &slen))
//        == SOCKET_ERROR) {
//      continue;
//    } else {
//      break;
//    }
//  }
  if (
      (recv_len =
          recvfrom(s, buf, BUFLEN, 0, (struct sockaddr*)&si_other, &slen))
      == SOCKET_ERROR) {
    return Diagnose::receive_error;
  }
  if (recv_len <= 0) {
    return Diagnose::receive_error;
  }
    std::cout << __LINE__ << std::endl;
  memcpy(pkt.data.data(), buf, pkt.data.size());
    std::cout << __LINE__ << std::endl;
  return Diagnose::receive_successful;
}

Diagnose UDPSocket::SendPacket(const PacketBase& packet) {
//  timed_socket_->GetSocket().send_to(
//        boost::asio::buffer(&packet.data[0], packet.data.size()),
//        timed_socket_->GetEndpoint());
//  return Diagnose::send_successful;
  memcpy(buf, packet.data.data(), packet.data.size());
  if (
      sendto(
          s, buf, packet.data.size(), 0,
          (struct sockaddr*)&sensor, slen) == SOCKET_ERROR) {
    return Diagnose::send_successful;
  }
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
