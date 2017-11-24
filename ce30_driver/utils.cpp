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
      cerr << "'Get Version' not Responding" << endl;
    }
  } else {
    cerr << "Request 'Get Version' Failed" << endl;
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
      cerr << "'Get ID' not Responding" << endl;
    }
  } else {
    cerr << "Request 'Get ID' Failed" << endl;
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
      if (set_id_response.Successful()) {
        return true;
      } else {
        cerr << "'Set ID' Failed" << endl;
      }
    } else {
      cerr << "'Set ID' not Responding" << endl;
    }
  } else {
    cerr << "Request 'Set ID' Failed" << endl;
  }
  return false;
}

bool StartRunning(UDPSocket& socket) {
  StartRequestPacket start_request;
  auto diagnose = socket.SendPacket(start_request);
  if (diagnose == Diagnose::send_successful) {
    return true;
  } else {
    cerr << "Request 'Start' Failed" << endl;
  }
  return false;
}

bool StopRunning(UDPSocket& socket) {
  StopRequestPacket stop_request;
  auto diagnose = socket.SendPacket(stop_request);
  if (diagnose == Diagnose::send_successful) {
    return true;
  } else {
    cerr << "Request 'Stop' Failed" << endl;
  }
  return false;
}

bool Connect(UDPSocket& socket) {
  return socket.Connect() == Diagnose::connect_successful;
}

bool GetPacket(PacketBase& packet, UDPSocket& socket) {
  return socket.GetPacket(packet) == Diagnose::receive_successful;
}
}
