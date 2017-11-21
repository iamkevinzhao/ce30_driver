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
  : ip_(ip), port_(port), socket_(io_service_, udp::endpoint(udp::v4(), port))
{
}

UDPSocket::~UDPSocket() {
}

Diagnose UDPSocket::Connect() {
//  StartReceive();
//  thread_ = std::thread(&UDPSocket::RunIOService, this);
  std::cout << ip_ << " " << port_ << std::endl;
  udp::endpoint listen_endpoint(address::from_string(ip_), port_);
      std::cout << __LINE__ << std::endl;
  try {
        std::cout << __LINE__ << std::endl;
    timed_socket_.reset(new TimedUDPSocket(listen_endpoint));
  } catch (const std::exception& e) {
    std::cout << __LINE__ << std::endl;
    std::cout << e.what() << std::endl;
  }
  std::cout << __LINE__ << std::endl;
  return Diagnose::connect_successful;
}

Diagnose UDPSocket::GetPacket(PacketBase &pkt, const double time_offset) {
//  std::unique_lock<std::mutex> lock(receive_mutex_);
//  if (receive_condition_.wait_for(lock, std::chrono::seconds(1)) ==
//      std::cv_status::timeout) {
//    return Diagnose::poll_timeout;
//  }
  if (!timed_socket_) {
    return Diagnose::bind_error;
  }
  boost::system::error_code ec;
  std::size_t n = timed_socket_->Receive(boost::asio::buffer(&pkt.data[0], pkt.data.size()), boost::posix_time::seconds(1), ec);
  if (ec) {
    std::cout << "Receive Error: " << ec.message() << std::endl;
  }
  return Diagnose::receive_successful;
}

Diagnose UDPSocket::SendPacket(const PacketBase& packet) {
  return Diagnose::send_successful;
}

void UDPSocket::HandleReceive(const boost::system::error_code &error, size_t) {
  receive_condition_.notify_all();
  Connect();
}

void UDPSocket::StartReceive() {
  socket_.async_receive_from(
      boost::asio::buffer(buffer_),
      endpoint_,
      boost::bind(
          &UDPSocket::HandleReceive, this,
          boost::asio::placeholders::error,
          boost::asio::placeholders::bytes_transferred));
}
} // namespace ce30_driver
