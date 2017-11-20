#include "udp_socket.h"

namespace ce30_driver {

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
} // namespace ce30_driver
