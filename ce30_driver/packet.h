#ifndef PACKET_H
#define PACKET_H

#include <vector>
#include <chrono>
#include <memory>
#include <unordered_map>

namespace ce30_driver {
struct Channel {
  Channel();
  float distance;
  float amplitude;
  static float DistanceMax();
  static float DistanceMin();
};

struct Column {
  Column();
  float azimuth;
  std::vector<Channel> channels;
  static int ChannelNum();
};

struct ParsedPacket {
  ParsedPacket();
  double time_stamp;
  std::vector<Column> columns;
  static int ColumnNum();
};

class Scan {
public:
  Scan();
  void AddColumn(const Column& column);
  void AddColumnsFromPacket(const ParsedPacket& packet);
  bool Ready();
  void Reset();
  Channel at(const int& x, const int& y) const;
  static int Width();
  static int Height();
  static float DistanceMin();
  static float DistanceMax();
  static int ColumnNum();
  static std::vector<float> AzimuthMap();
  static float FoV();
  static float AzimuthDelta();
  static int WhichColumn(const float& azimuth);
private:
  std::unordered_map<int, Column> columns_;
};

struct PacketBase {
  virtual ~PacketBase();
  std::chrono::high_resolution_clock::time_point stamp;
  std::vector<unsigned char> data;
};

struct Packet : public PacketBase {
  Packet();

  std::unique_ptr<ParsedPacket> Parse();
  inline static int HeaderBytes();
  inline static int ColumnIdentifierBytes();
  inline static int AzimuthBytes();
  inline static int DistanceBytes();
  inline static int AmplitudeBytes();
  inline static int TimeStampBytes();
  inline static int FactoryBytes();
  inline static unsigned char ColumnIdentifierHigh();
  inline static unsigned char ColumnIdentifierLow();
private:
  float ParseAzimuth(const unsigned char& high, const unsigned char& low);
  float ParseDistance(const unsigned char& high, const unsigned char& low);
  float ParseAmplitude(const unsigned char& raw);
  double ParseTimeStamp(std::vector<unsigned char> stamp_raw);
};

struct RequestPacket : public PacketBase {
  RequestPacket();
  virtual ~RequestPacket();
  bool SetCmdString(const std::string& cmd);
  void ResetPacket();
};

struct VersionRequestPacket : public RequestPacket {
  VersionRequestPacket();
};

struct VersionResponsePacket : public PacketBase {
  VersionResponsePacket();
  std::string GetVersionString() const;
};

struct SetIDRequestPacket : public RequestPacket {
  SetIDRequestPacket(const int& id);
};

struct SetIDResponsePacket : public PacketBase {
  SetIDResponsePacket();
  bool Successful() const;
};

struct GetIDRequestPacket : public RequestPacket {
  GetIDRequestPacket();
};

struct GetIDResponsePacket : public PacketBase {
  GetIDResponsePacket();
  int32_t ID() const;
};

struct StampSyncRequestPacket : public RequestPacket {
  StampSyncRequestPacket(const uint32_t& microseconds);
};

// Identical Functionality
using StampSyncResponsePacket = SetIDResponsePacket;

struct StartRequestPacket : public RequestPacket {
  StartRequestPacket();
};
}

#endif // PACKET_H
