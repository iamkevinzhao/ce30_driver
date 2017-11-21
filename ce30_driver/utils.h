#ifndef UTILS_H
#define UTILS_H

#include <vector>
#ifdef WIN32
  #include "win/udp_socket.h"
#else
  #include "unix/udp_socket.h"
#endif
#include "export.h"

namespace ce30_driver {
bool API GetVersion(std::string& version, UDPSocket& socket);
bool API GetDeviceID(int& id, UDPSocket& socket);
bool API SetDeviceID(const int& id, UDPSocket& socket);
}

#endif // UTILS_H
