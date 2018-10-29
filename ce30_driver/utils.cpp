#include "utils.h"
#include <iostream>
#include <thread>
#include <fstream>
#include <iomanip>

#ifdef QT5_WIDGETS_EXISTS
#include <QSettings>
#endif

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
    CommonResponsePacket set_id_response;
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
    Packet packet;
    diagnose = socket.GetPacket(packet);
    if (diagnose == Diagnose::receive_successful) {
      // cerr << "Still Receving Packets though Stop Packet was Sent" << endl;
      return false;
    } else {
      return true;
    }
  } else {
    cerr << "Request 'Stop' Failed" << endl;
  }
  return false;
}

/// @cond DO_NOT_DOCUMENT_THIS
bool EnableFilter(UDPSocket& socket) {
  EnableFilterRequestPacket request_packet;
  auto diagnose = socket.SendPacket(request_packet);
  if (diagnose == Diagnose::send_successful) {
    EnableFilterResponsePacket response_packet;
    diagnose = socket.GetPacket(response_packet);
    if (diagnose == Diagnose::receive_successful) {
      if (response_packet.Successful()) {
        return true;
      } else {
        cerr << "'Enable Filter' Failed" << endl;
      }
    } else {
      cerr << "'Enable Filter' not Responding" << endl;
    }
  } else {
    cerr << "Request 'Enable Filter' Failed" << endl;
  }
  return false;
}
/// @endcond

/// @cond DO_NOT_DOCUMENT_THIS
bool DisableFilter(UDPSocket& socket) {
  DisableFilterRequestPacket request_packet;
  auto diagnose = socket.SendPacket(request_packet);
  if (diagnose == Diagnose::send_successful) {
    DisableFilterResponsePacket response_packet;
    diagnose = socket.GetPacket(response_packet);
    if (diagnose == Diagnose::receive_successful) {
      if (response_packet.Successful()) {
        return true;
      } else {
        cerr << "'Disable Filter' Failed" << endl;
      }
    } else {
      cerr << "'Disable Filter' not Responding" << endl;
    }
  } else {
    cerr << "Request 'Disable Filter' Failed" << endl;
  }
  return false;
}
/// @endcond

bool EnableGrayOutput(UDPSocket &socket) {
  EnableGrayOutputRequestPacket request_packet;
  auto diagnose = socket.SendPacket(request_packet);
  if (diagnose == Diagnose::send_successful) {
    EnableGrayOutputResponsePacket response_packet;
    diagnose = socket.GetPacket(response_packet);
    if (diagnose == Diagnose::receive_successful) {
      if (response_packet.Successful()) {
        return true;
      } else {
        cerr << "'Enable Gray Output' Failed" << endl;
      }
    } else {
      cerr << "'Enable Gray Output' not Responding" << endl;
    }
  } else {
    cerr << "Request 'Enable Gray Output' Failed" << endl;
  }
  return false;
}

bool DisableGrayOutput(UDPSocket &socket) {
  DisableGrayOutputRequestPacket request_packet;
  auto diagnose = socket.SendPacket(request_packet);
  if (diagnose == Diagnose::send_successful) {
    DisableGrayOutputResponsePacket response_packet;
    diagnose = socket.GetPacket(response_packet);
    if (diagnose == Diagnose::receive_successful) {
      if (response_packet.Successful()) {
        return true;
      } else {
        cerr << "'Disable Gray Output' Failed" << endl;
      }
    } else {
      cerr << "'Disable Gray Output' not Responding" << endl;
    }
  } else {
    cerr << "Request 'Disable Gray Output' Failed" << endl;
  }
  return false;
}

bool EnableSafetyMode(UDPSocket& socket) {
  EnableSafetyModeRequestPacket request_packet;
  auto diagnose = socket.SendPacket(request_packet);
  if (diagnose == Diagnose::send_successful) {
    EnableSafetyModeResponsePacket response_packet;
    diagnose = socket.GetPacket(response_packet);
    if (diagnose == Diagnose::receive_successful) {
      if (response_packet.Successful()) {
        return true;
      } else {
        cerr << "'Enable Safety Mode' Failed" << endl;
      }
    } else {
      cerr << "'Enable Safety Mode' not Responding" << endl;
    }
  } else {
    cerr << "Request 'Enable Safety Mode' Failed" << endl;
  }
  return false;
}

bool DisableSafetyMode(UDPSocket& socket) {
  DisableSafetyModeRequestPacket request_packet;
  auto diagnose = socket.SendPacket(request_packet);
  if (diagnose == Diagnose::send_successful) {
    DisableSafetyModeResponsePacket response_packet;
    diagnose = socket.GetPacket(response_packet);
    if (diagnose == Diagnose::receive_successful) {
      if (response_packet.Successful()) {
        return true;
      } else {
        cerr << "'Disable Safety Mode' Failed" << endl;
      }
    } else {
      cerr << "'Disable Safety Mode' not Responding" << endl;
    }
  } else {
    cerr << "Request 'Disable Safety Mode' Failed" << endl;
  }
  return false;
}

bool Connect(UDPSocket& socket) {
  return socket.Connect() == Diagnose::connect_successful;
}

bool GetPacket(
    PacketBase& packet, UDPSocket& socket, const bool& thread_safe) {
  Diagnose diagnose;
  if (thread_safe) {
    diagnose = socket.GetPacketThreadSafe(packet);
  } else {
    diagnose = socket.GetPacket(packet);
  }
  return diagnose == Diagnose::receive_successful;
}

bool SendPacket(
    const PacketBase &packet, UDPSocket &socket, const bool &thread_safe) {
  Diagnose diagnose;
  if (thread_safe) {
    diagnose = socket.SendPacketThreadSafe(packet);
  } else {
    diagnose = socket.SendPacket(packet);
  }
  return diagnose == Diagnose::send_successful;
}

bool SaveImages(const string &file, const Scan &scan) {
  ofstream os(file);
  if (!os.is_open()) {
    return false;
  }
  os << std::fixed << std::showpoint << std::setprecision(2);

  os << "dist:\n";
  float distance;
  for (int row = 0; row < scan.Height(); ++row) {
    for (int col = 0; col < scan.Width(); ++col) {
      distance = scan.at(row, col).distance;
      if (distance > Channel::DistanceMax() ||
          distance < Channel::DistanceMin()) {
        distance = 0.0f;
      }
      os << distance << " ";
    }
    os << "\n";
  }
  os << "\n";

  os << "amp:\n";
  float amp;
  for (int row = 0; row < scan.Height(); ++row) {
    for (int col = 0; col < scan.Width(); ++col) {
      amp = scan.at(row, col).amplitude;
      if (amp > Channel::AmplitudeMax() || amp < Channel::AmplitudeMin()) {
        amp = 0.0f;
      }
      os << amp << " ";
    }
    os << "\n";
  }
  os << "\n";
  os.close();
  return true;
}

std::vector<int> VersionParser(string version) {
  std::vector<int> result;
  std::string tmp;
  while (!version.empty()) {
    if (version[0] == '.') {
      int bit;
      try {
        bit = stoi(tmp);
      } catch (...) {
        result.clear();
        return result;
      }
      result.push_back(bit);
      tmp.clear();
    } else {
      tmp += *version.begin();
    }
    version.erase(version.begin());
  }
  if (!tmp.empty()) {
    int bit;
    try {
      bit = stoi(tmp);
    } catch (...) {
      result.clear();
      return result;
    }
    result.push_back(bit);
  }
  return result;
}

bool VersionGreater(const string &version, const string &minimal) {
  if (version.empty() || minimal.empty()) {
    return false;
  }
  if (version[0] != minimal[0]) {
    return false;
  }
  std::string v_high(version.begin(), version.end());
  std::string v_low(minimal.begin(), minimal.end());
  auto high = VersionParser(v_high);
  auto low = VersionParser(v_low);
  if ((high.size() != 3) || (low.size() != 3)) {
    return false;
  }
  return high[2] >= low[2];
}

#ifdef QT5_WIDGETS_EXISTS
std::string DeviceConfig::kSafetyModeKey = "safety-mode";
bool DeviceConfig::ConfigureDevice(UDPSocket& socket, const QString &ini_file) {
  std::string version;
  if (!GetVersion(version, socket)) {
    return false;
  }
  QSettings settings(ini_file, QSettings::IniFormat);
  if (VersionGreater(version, "d9.4.8")) {
    auto value = settings.value(kSafetyModeKey, false).toBool();
  }
}
#endif
}
