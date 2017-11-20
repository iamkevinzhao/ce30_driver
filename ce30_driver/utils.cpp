#include "utils.h"
#include <iostream>

using namespace std;

namespace ce30_driver {
bool GetVersion(std::string& version, UDPSocket& socket) {
  VersionRequestPacket version_request;
  auto diagnose = socket.SendPacket(version_request);
  if (diagnose == Diagnose::send_successful) {
    VersionResponsePacket version_response;
    diagnose = socket.GetPacket(version_response);
    if (diagnose == Diagnose::receive_successful) {
      version = version_response.GetVersionString();
      return true;
    } else {
      std::cerr << "'Get Version' not Responding\n";
    }
  } else {
    std::cerr << "Request 'Get Version' Failed\n";
  }
  return false;
}

bool GetDeviceID(int& id, UDPSocket& socket) {
  GetIDRequestPacket get_id_request;
  auto diagnose = socket.SendPacket(get_id_request);
  if (diagnose == Diagnose::send_successful) {
    GetIDResponsePacket get_id_response;
    diagnose = socket.GetPacket(get_id_response);
    if (diagnose == Diagnose::receive_successful) {
      id = get_id_response.ID();
      return true;
    } else {
      std::cerr << "'Get ID' not Responding\n";
    }
  } else {
    std::cerr << "Request 'Get ID' Failed\n";
  }
  return false;
}

bool SetDeviceID(const int& id, UDPSocket& socket) {
  SetIDRequestPacket set_id_request(id);
  auto diagnose = socket.SendPacket(set_id_request);
  if (diagnose == Diagnose::send_successful) {
    SetIDResponsePacket set_id_response;
    diagnose = socket.GetPacket(set_id_response);
    if (diagnose == Diagnose::receive_successful) {
      return true;
    } else {
      std::cerr << "'Set ID' not Responding\n";
    }
  } else {
    std::cerr << "Request 'Set ID' Failed\n";
  }
  return false;
}
}
