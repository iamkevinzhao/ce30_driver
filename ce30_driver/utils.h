#ifndef UTILS_H
#define UTILS_H

/**
 * @file utils.h
 * @brief helper functions
 */

#include <vector>
#include "udp_socket.h"
#include "export.h"
#include <sstream>

namespace ce30_driver {
/**
* @brief retrieve device firmware version
* @param[out] version version string
* @param socket UDP socket object
* @return true if successfully retrieved the version; otherwise, false
*/
bool API GetVersion(std::string& version, UDPSocket& socket);

/**
 * @brief retrieve device ID [under development]
 * @param id device ID
 * @param socket UDP socket object
 * @return true if successfully retrieved device ID; otherwise, false
 */
bool API GetDeviceID(int& id, UDPSocket& socket);

/**
 * @brief set device ID [under development]
 * @param id device ID
 * @param socket UDP socket object
 * @return true if successfully set device ID; otherwise, false
 */
bool API SetDeviceID(const int& id, UDPSocket& socket);

/**
 * @brief start receiving measurement data packets
 * @param socket UDP socket object
 * @return true if started; otherwise, false
 */
bool API StartRunning(UDPSocket& socket);

/**
 * @brief stop receiving measurement data packets
 * @param socket UDP socket object
 * @return true if stopped; otherwise, false
 */
bool API StopRunning(UDPSocket& socket);

/**
 * @brief establish socket and connect device
 * @param socket UDP socket object
 * @return true if successful; otherwise, false
 */
bool API Connect(UDPSocket& socket);

/**
 * @brief get measurement data packet
 * @param[out] packet measurement data packet
 * @param socket UDP socket object
 * @param thread_safe if use thread safe mode
 * @return true if get packet; otherwise false
 */
bool API GetPacket(
    PacketBase& packet, UDPSocket& socket, const bool& thread_safe = false);

/// @cond DO_NO_DOCUMENT_THIS
bool API EnableFilter(UDPSocket& socket);

bool API DisableFilter(UDPSocket& socket);
/// @endcond

/// @cond DO_NOT_DOCUMENT_THIS
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
/// @endcond
}

#endif // UTILS_H
