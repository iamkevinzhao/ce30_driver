#ifdef UDP_SOCKET_USE_WINSOCK2_API
#include "udp_socket.h"
#include <iostream>
#include <WinSock2.h>

namespace ce30_driver {

SOCKET gWinUDPSocket;
struct sockaddr_in gDeviceSocketAddress;
int gSockAddrInLength = sizeof(sockaddr_in);
constexpr int gReadBufferLength = 1024;
char gUDPSocketReadBuffer[gReadBufferLength];
WSADATA gWSA;

UDPSocket::UDPSocket(const std::string& ip, uint16_t port)
  : ip_(ip), port_(port)
{}

UDPSocket::~UDPSocket() {
  closesocket(gWinUDPSocket);
  WSACleanup();
}

Diagnose UDPSocket::Connect() {
  if (WSAStartup(MAKEWORD(2, 2), &gWSA) != 0) {
    return Diagnose::connect_failed;
  }
  if ((gWinUDPSocket = socket(AF_INET, SOCK_DGRAM, 0)) ==
      INVALID_SOCKET) {
    return Diagnose::connect_failed;
  }
  int timeout = 1000;
  setsockopt(
      gWinUDPSocket, SOL_SOCKET, SO_RCVTIMEO,
      (const char*)&timeout, sizeof(timeout));
  struct sockaddr_in address;
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(port_);
  gDeviceSocketAddress.sin_family = AF_INET;
  gDeviceSocketAddress.sin_addr.s_addr = inet_addr(ip_.c_str());
  gDeviceSocketAddress.sin_port = htons(port_);
  if (bind(gWinUDPSocket, (struct sockaddr*)&address, sizeof(address)) ==
      SOCKET_ERROR) {
    return Diagnose::connect_failed;
  }
  return Diagnose::connect_successful;
}

Diagnose UDPSocket::GetPacket(PacketBase &pkt, const double time_offset) {
  int length;
  struct sockaddr_in address;
  if ((length =
          recvfrom(
              gWinUDPSocket, gUDPSocketReadBuffer, gReadBufferLength, 0,
              (struct sockaddr*)&address, &gSockAddrInLength)) ==
      SOCKET_ERROR) {
    return Diagnose::receive_error;
  }
  if (length < pkt.data.size()) {
    return Diagnose::receive_error;
  }
  memcpy(pkt.data.data(), gUDPSocketReadBuffer, pkt.data.size());
  return Diagnose::receive_successful;
}

Diagnose UDPSocket::SendPacket(const PacketBase& packet) {
  memcpy(gUDPSocketReadBuffer, packet.data.data(), packet.data.size());
  if (sendto(
          gWinUDPSocket, gUDPSocketReadBuffer, packet.data.size(), 0,
          (struct sockaddr*)&gDeviceSocketAddress, gSockAddrInLength) ==
      SOCKET_ERROR) {
    return Diagnose::send_fail;
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

#endif
