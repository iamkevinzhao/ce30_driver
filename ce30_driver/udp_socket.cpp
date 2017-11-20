#include "udp_socket.h"
#include <chrono>
#include <poll.h>
#include <arpa/inet.h>
#include <iostream>
#include <cstring>
#include <fcntl.h>
#include <unistd.h>

using namespace std::chrono;

namespace ce30_driver {
UDPSocket::UDPSocket(const std::string& ip, uint16_t port)
  : devip_str_(ip), port_(port)
{
}

Diagnose UDPSocket::ConnectDevice() {
  auto diagnose = Connect();
  if (diagnose != Diagnose::connect_successful) {
    return diagnose;
  }

  VersionRequestPacket version_request;
  diagnose = SendPacket(version_request);
  if (diagnose == Diagnose::send_successful) {
    VersionResponsePacket version_response;
    diagnose = GetPacket(version_response);
    if (diagnose == Diagnose::receive_successful) {
      std::cout
          << "Version: " << version_response.GetVersionString() << std::endl;
    } else {
      std::cerr << "'Get Version' not Responding\n";
    }
  } else {
    std::cerr << "Request 'Get Version' Failed\n";
  }

  SetIDRequestPacket set_id_request(10);
  diagnose = SendPacket(set_id_request);
  if (diagnose == Diagnose::send_successful) {
    SetIDResponsePacket set_id_response;
    diagnose = GetPacket(set_id_response);
    if (diagnose == Diagnose::receive_successful) {
      std::cout
          << "'Set ID' Response (1: Successful, 2: Failed): "
          << set_id_response.Successful() << std::endl;
    } else {
      std::cerr << "'Set ID' not Responding\n";
    }
  } else {
    std::cerr << "Request 'Set ID' Failed\n";
  }

  GetIDRequestPacket get_id_request;
  diagnose = SendPacket(get_id_request);
  if (diagnose == Diagnose::send_successful) {
    GetIDResponsePacket get_id_response;
    diagnose = GetPacket(get_id_response);
    if (diagnose == Diagnose::receive_successful) {
      std::cout << "ID: " << get_id_response.ID() << std::endl;
    } else {
      std::cerr << "'Get ID' not Responding\n";
    }
  } else {
    std::cerr << "Request 'Get ID' Failed\n";
  }

  StampSyncRequestPacket stamp_request(100000);
  diagnose = SendPacket(stamp_request);
  if (diagnose == Diagnose::send_successful) {
    StampSyncResponsePacket stamp_response;
    diagnose = GetPacket(stamp_response);
    if (diagnose == Diagnose::send_successful) {
      std::cout
          << "'Stamp Sync' Response (1: Successful, 2: Failed): "
          << stamp_response.Successful() << std::endl;
    } else {
      std::cerr << "'Stamp Sync' not Responding\n";
    }
  } else {
    std::cerr << "Request 'Stamp Sync' Failed\n";
  }

  return Diagnose::device_connected;
}

UDPSocket::~UDPSocket() {
  (void) close(sockfd_);
}

Diagnose UDPSocket::Connect() {
  sockfd_ = -1;

  if (!devip_str_.empty()) {
    inet_aton(devip_str_.c_str(), &devip_);
  }

  sockfd_ = socket(PF_INET, SOCK_DGRAM, 0);
  if (sockfd_ == -1) {
    return Diagnose::socket_error;
  }

  sockaddr_in my_addr;                     // my address information
  memset(&my_addr, 0, sizeof(my_addr));    // initialize to zeros
  my_addr.sin_family = AF_INET;            // host byte order
  my_addr.sin_port = htons(port_);          // port in network byte order
  my_addr.sin_addr.s_addr = INADDR_ANY;    // automatically fill in my IP

  if (bind(sockfd_, (sockaddr *)&my_addr, sizeof(sockaddr)) == -1) {
    return Diagnose::bind_error;
  }

  if (fcntl(sockfd_, F_SETFL, O_NONBLOCK | FASYNC) < 0) {
    return Diagnose::non_block_error;
  }
  return Diagnose::connect_successful;
}

Diagnose UDPSocket::GetPacket(PacketBase &pkt, const double time_offset) {
  pkt.stamp =
      high_resolution_clock::now() +
      std::chrono::milliseconds(long(time_offset * 1000));

  struct pollfd fds[1];
  fds[0].fd = sockfd_;
  fds[0].events = POLLIN;
  static const int POLL_TIMEOUT = 1000; // one second (in msec)

  sockaddr_in sender_address;
  socklen_t sender_address_len = sizeof(sender_address);

  while (true) {
    do {
      int retval = poll(fds, 1, POLL_TIMEOUT);
      if (retval < 0) {            // poll() error?
        if (errno != EINTR)
        return Diagnose::poll_error;
      }
      if (retval == 0) {          // poll() timeout?
        return Diagnose::poll_timeout;
      }
      if ((fds[0].revents & POLLERR)
          || (fds[0].revents & POLLHUP)
          || (fds[0].revents & POLLNVAL)) { // device error?
        return Diagnose::device_error;
      }
    } while ((fds[0].revents & POLLIN) == 0);

    // Receive packets that should now be available from the
    // socket using a blocking read.
    ssize_t nbytes =
        recvfrom(
            sockfd_, &pkt.data[0], pkt.data.size(), 0,
            (sockaddr*) &sender_address, &sender_address_len);

    if (nbytes < 0) {
      if (errno != EWOULDBLOCK) {
        perror("recvfail");
        return Diagnose::receive_fail;
      }
    } else if ((size_t) nbytes == pkt.data.size()) {
      // read successful,
      // if packet is not from the lidar scanner we selected by IP,
      // continue otherwise we are done
      if(devip_str_ != "" &&
         sender_address.sin_addr.s_addr != devip_.s_addr) {
        continue;
      } else {
        break; //done
      }
    }
  }
  return Diagnose::receive_successful;
}

Diagnose UDPSocket::SendPacket(const PacketBase& packet) {
  struct sockaddr_in myaddr;
  memset(&myaddr, 0, sizeof(myaddr));
  myaddr.sin_family = AF_INET;
  inet_pton(AF_INET, devip_str_.c_str(), &myaddr.sin_addr.s_addr);
  myaddr.sin_port=htons(port_);
  if (sendto(
        sockfd_,
        &packet.data[0], packet.data.size(), 0,
        (struct sockaddr *)&myaddr, sizeof(myaddr)) != packet.data.size()) {
    return Diagnose::send_fail;
  }
  return Diagnose::send_successful;
}
} // namespace ce30_driver
