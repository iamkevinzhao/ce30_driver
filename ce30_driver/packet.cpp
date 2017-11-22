#include "packet.h"
#include <algorithm>
#include <iostream>
#include <string.h>
#include <sstream>
#include "utils.h"

using namespace std;

namespace ce30_driver {
Channel::Channel() : distance(0.0f), amplitude(0.0f) {

}

float Channel::DistanceMax() {
  return 30.0f;
}

float Channel::DistanceMin() {
  return 0.0f;
}

Column::Column() {
  azimuth = 0.0f;
  channels.resize(ChannelNum());
}

int Column::ChannelNum() {
  return 20;
}

ParsedPacket::ParsedPacket() {
  time_stamp = 0.0;
  columns.resize(ColumnNum());
}

int ParsedPacket::ColumnNum() {
  return 12;
}

Scan::Scan() {

}

void Scan::AddColumn(const Column& column) {
  auto column_n = WhichColumn(column.azimuth);
  if (column_n >= 0) {
    columns_[column_n] = column;
  }
}

void Scan::AddColumnsFromPacket(const ParsedPacket& packet) {
  for (auto& i : packet.columns) {
    AddColumn(i);
  }
}

bool Scan::Ready() {
  return columns_.size() >= ColumnNum();
}

void Scan::Reset() {
  columns_.clear();
}

Channel Scan::at(const int &x, const int &y) const {
  return columns_.at(x).channels[y];
}

int Scan::Width() {
  return ColumnNum();
}

int Scan::Height() {
  return Column::ChannelNum();
}

float Scan::DistanceMax() {
  return Channel::DistanceMax();
}

float Scan::DistanceMin() {
  return Channel::DistanceMin();
}

int Scan::ColumnNum() {
  return 320;
}

float Scan::FoV() {
  return 60.0f;
}

float Scan::AzimuthDelta() {
  return FoV() / ColumnNum();
}

vector<float> Scan::AzimuthMap() {
  vector<float> map(ColumnNum(), 0.0f);
  for (int i = 0; i < ColumnNum(); ++i) {
    map[i] = AzimuthDelta() * i;
  }
  return map;
}

int Scan::WhichColumn(const float& azimuth) {
  static float e = AzimuthDelta() * 0.1f;
  static vector<float> azimuth_map = AzimuthMap();
  for (int i = 0; i < azimuth_map.size(); ++i) {
    if (azimuth < (azimuth_map[i] + e) && azimuth > (azimuth_map[i] - e)) {
      return i;
    }
  }
  return -1;
}

PacketBase::~PacketBase() {}

Packet::Packet() {
  data.resize(
      HeaderBytes() +
      ParsedPacket::ColumnNum() * (
          ColumnIdentifierBytes() +
          AzimuthBytes() +
          Column::ChannelNum() * (
              DistanceBytes() +
              AmplitudeBytes())) +
          TimeStampBytes() +
          FactoryBytes(),
      0);
}

int Packet::HeaderBytes() {
  return 42;
}

int Packet::ColumnIdentifierBytes() {
  return 2;
}

int Packet::AzimuthBytes() {
  return 2;
}

int Packet::DistanceBytes() {
  return 2;
}

int Packet::AmplitudeBytes() {
  return 1;
}

int Packet::TimeStampBytes() {
  return 4;
}

int Packet::FactoryBytes() {
  return 2;
}

unsigned char Packet::ColumnIdentifierHigh() {
  return 0xFF;
}

unsigned char Packet::ColumnIdentifierLow() {
  return 0xEE;
}

std::unique_ptr<ParsedPacket> Packet::Parse() {
  std::unique_ptr<ParsedPacket> null_packet;
  std::unique_ptr<ParsedPacket> packet(new ParsedPacket);
  int index = HeaderBytes();
  for (auto& col : packet->columns) {
    if (data[index++] != ColumnIdentifierHigh()) {
      return null_packet;
    }
    if (data[index++] != ColumnIdentifierLow()) {
      return null_packet;
    }
    auto azimuth_low = data[index++];
    auto azimuth_high = data[index++];
    col.azimuth = ParseAzimuth(azimuth_high, azimuth_low);
    for (auto& chn : col.channels) {
      auto dist_low = data[index++];
      auto dist_high = data[index++];
      auto amp = data[index++];

      chn.distance = ParseDistance(dist_high, dist_low);
      chn.amplitude = ParseAmplitude(amp);
    }
  }
  vector<unsigned char> stamp_raw(4, 0);
  for (auto& i : stamp_raw) {
    i = data[index++];
  }
  std::reverse(stamp_raw.begin(), stamp_raw.end());
  packet->time_stamp = ParseTimeStamp(stamp_raw);
  return packet;
}

float Packet::ParseAzimuth(
    const unsigned char &high, const unsigned char &low) {
  int i = high;
  i = (i << 8);
  i |= low;
  return i / 100.0f;
}

float Packet::ParseDistance(
    const unsigned char& high, const unsigned char& low) {
  int i = high;
  i = (i << 8);
  i |= low;
  double d = i * 2.0;
  return
      max(
          Channel::DistanceMin(),
          min(float(d / 1000.0), Channel::DistanceMax()));
}

float Packet::ParseAmplitude(const unsigned char &raw) {
  return 0.0f;
}

double Packet::ParseTimeStamp(std::vector<unsigned char> stamp_raw) {
  std::reverse(stamp_raw.begin(), stamp_raw.end());
  int integer;
  for (int i = 0; i < stamp_raw.size(); ++i) {
    int tmp = stamp_raw[i];
    tmp = (tmp << (stamp_raw.size() - i) * 8);
    integer |= tmp;
  }
  return integer / 1000000.0;
}

RequestPacket::RequestPacket() {
  ResetPacket();
}

RequestPacket::~RequestPacket() {}

void RequestPacket::ResetPacket() {
  data.resize(50, 0);
}

bool RequestPacket::SetCmdString(const string &cmd) {
  if (cmd.size() > data.size()) {
    return false;
  }
  ResetPacket();
  copy(cmd.begin(), cmd.end(), data.begin());
  return true;
}

VersionRequestPacket::VersionRequestPacket() : RequestPacket() {
  SetCmdString("version");
}

VersionResponsePacket::VersionResponsePacket() {
  data.resize(6, 0);
}

string VersionResponsePacket::GetVersionString() const {
  std::string result(data.size(), 0);
  copy(data.begin(), data.end(), result.begin());
  return result;
}

SetIDRequestPacket::SetIDRequestPacket(const int32_t& id) : RequestPacket() {
  auto b4 = B4ToByteHexStrs<int32_t>(id);
  if (b4.empty()) {
    return;
  }
  string b4s;
  for (auto& i : b4) {
    b4s += i + " ";
  }
  b4s.erase(b4s.end() - 1);
  SetCmdString("setLidarID " + b4s);
}

SetIDResponsePacket::SetIDResponsePacket() {
  data.resize(4, 0);
}

bool SetIDResponsePacket::Successful() const {
  int32_t result;
  memcpy(&result, &data[0], data.size());
  return result == 0;
}

GetIDRequestPacket::GetIDRequestPacket() : RequestPacket() {
  SetCmdString("getLidarID");
}

GetIDResponsePacket::GetIDResponsePacket() {
  data.resize(4, 0);
  int32_t default_id = -1;
  memcpy(&data[0], &default_id, data.size());
}

int32_t GetIDResponsePacket::ID() const {
  int32_t id;
  memcpy(&id, &data[0], data.size());
  return id;
}

StampSyncRequestPacket::StampSyncRequestPacket(const uint32_t& microseconds)
  : RequestPacket() {
  uint32_t b3 = (microseconds & 0xFF000000) >> (8 * 3);
  uint32_t b2 = (microseconds & 0x00FF0000) >> (8 * 2);
  uint32_t b1 = (microseconds & 0x0000FF00) >> (8 * 1);
  uint32_t b0 = (microseconds & 0x000000FF) >> (8 * 0);

  string cmd = "updateTimeStamp ";
  stringstream ss;

  ss << hex << b0;
  cmd += ss.str() + " ";
  ss.str("");

  ss << hex << b1;
  cmd += ss.str() + " ";
  ss.str("");

  ss << hex << b2;
  cmd += ss.str() + " ";
  ss.str("");

  ss << hex << b3;
  cmd += ss.str();

  SetCmdString(cmd);
}

StartRequestPacket::StartRequestPacket() : RequestPacket() {
  SetCmdString("getDistanceAndAmplitudeSorted");
}

StopRequestPacket::StopRequestPacket() : RequestPacket() {
  SetCmdString("join");
}
}
