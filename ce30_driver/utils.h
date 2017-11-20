#ifndef UTILS_H
#define UTILS_H

#include <vector>
#include "udp_socket.h"

namespace ce30_driver {
bool GetVersion(std::string& version, UDPSocket& socket);
bool GetDeviceID(int& id, UDPSocket& socket);
bool SetDeviceID(const int& id, UDPSocket& socket);
}

#endif // UTILS_H
