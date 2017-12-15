#ifndef UTILS_H
#define UTILS_H

#include <vector>
#include "udp_socket.h"
#include "export.h"
#include <sstream>

namespace ce30_driver {
bool API GetVersion(std::string& version, UDPSocket& socket);

bool API GetDeviceID(int& id, UDPSocket& socket);

bool API SetDeviceID(const int& id, UDPSocket& socket);

bool API StartRunning(UDPSocket& socket);

bool API StopRunning(UDPSocket& socket);

bool API Connect(UDPSocket& socket);

bool API GetPacket(PacketBase& packet, UDPSocket& socket);

bool API EnableFilter(UDPSocket& socket);

bool API DisableFilter(UDPSocket& socket);

template<typename T>
std::vector<std::string> B4ToByteHexStrs(const T& t) {
  std::vector<std::string> result(4, "");
  if (sizeof(t) != 4) {
    result.clear();
    return result;
  }

  T b3 = (t & 0xFF000000) >> (8 * 3);
  T b2 = (t & 0x00FF0000) >> (8 * 2);
  T b1 = (t & 0x0000FF00) >> (8 * 1);
  T b0 = (t & 0x000000FF) >> (8 * 0);

  std::stringstream ss;

  ss << std::hex << b0;
  result[0] = ss.str();
  ss.str("");

  ss << std::hex << b1;
  result[1] = ss.str();
  ss.str("");

  ss << std::hex << b2;
  result[2] = ss.str();
  ss.str("");

  ss << std::hex << b3;
  result[3] = ss.str();

  return result;
}
}

#endif // UTILS_H
