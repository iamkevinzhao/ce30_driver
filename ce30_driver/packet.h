#ifndef PACKET_H
#define PACKET_H

#include <vector>
#include <chrono>
#include <memory>
#include <unordered_map>
#include "export.h"

namespace ce30_driver {
struct API Channel {
  Channel();
  float distance;
  float amplitude;
  static float DistanceMax();
  static float DistanceMin();
};

struct API Column {
  Column();
  float azimuth;
  std::vector<Channel> channels;
  static int ChannelNum();
};

struct API ParsedPacket {
  ParsedPacket();
  double time_stamp;
  std::vector<Column> columns;
  static int ColumnNum();
};

class API Scan {
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

struct API PacketBase {
  virtual ~PacketBase();
  std::chrono::high_resolution_clock::time_point stamp;
  std::vector<unsigned char> data;
};

struct API Packet : public PacketBase {
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

struct API RequestPacket : public PacketBase {
  RequestPacket();
  virtual ~RequestPacket();
  bool SetCmdString(const std::string& cmd);
  void ResetPacket();
};

struct API VersionRequestPacket : public RequestPacket {
  VersionRequestPacket();
};

struct API VersionResponsePacket : public PacketBase {
  VersionResponsePacket();
  std::string GetVersionString() const;
};

struct API SetIDRequestPacket : public RequestPacket {
  SetIDRequestPacket(const int32_t& id);
};

struct API SetIDResponsePacket : public PacketBase {
  SetIDResponsePacket();
  bool Successful() const;
};

struct API GetIDRequestPacket : public RequestPacket {
  GetIDRequestPacket();
};

struct API GetIDResponsePacket : public PacketBase {
  GetIDResponsePacket();
  int32_t ID() const;
};

struct API StampSyncRequestPacket : public RequestPacket {
  StampSyncRequestPacket(const uint32_t& microseconds);
};

// Identical Functionality
using StampSyncResponsePacket = SetIDResponsePacket;

struct API StartRequestPacket : public RequestPacket {
  StartRequestPacket();
};

struct API StopRequestPacket : public RequestPacket {
  StopRequestPacket();
};
}

#endif // PACKET_H
