#include "packet.h"
#include <algorithm>
#include <iostream>
#include <string.h>
#include <sstream>
#include "utils.h"

#define _USE_MATH_DEFINES
#include <math.h>
// #define ToRad(x) x * M_PI / 180

inline static float ToRad(const float& x) {
  return x * M_PI / 180;
}

using namespace std;

namespace ce30_driver {
Channel::Channel()
  : distance(0.0f), amplitude(0.0f), amp_raw(0), grey_value(0) {

}

Channel::Type Channel::type() const {
  switch (amp_raw) {
  case 0x00: return Type::normal;
  case 0x01: return Type::signal_high;
  case 0x02: return Type::signal_low;
  case 0x03: return Type::amb_high;
  case 0x04: return Type::kicked_out;
  default: return Type::unknown;
  }
}

int Channel::FullType() {
  return
      (int)Type::normal |
      (int)Type::signal_high |
      (int)Type::signal_low |
      (int)Type::amb_high |
      (int)Type::unknown;
}

Point Channel::point() const {
  return
      Point(
          distance * sin(ToRad(90.0f - v_azimuth)) * cos(ToRad(h_azimuth)),
          distance * sin(ToRad(90.0f - v_azimuth)) * sin(ToRad(h_azimuth)),
          distance * cos(ToRad(90.0f - v_azimuth)));
}

float Channel::DistanceMax() {
  return 30.0f;
}

float Channel::DistanceMin() {
  return 0.0f;
}

unsigned short Channel::GreyValueMax() {
  return 51200;
}

unsigned short Channel::GreyValueMin() {
  return 16;
}

Column::Column() {
  azimuth = 0.0f;
  channels.resize(ChannelNum());
}

int Column::ChannelNum() {
  return 20;
}

ParsedPacket::ParsedPacket() : grey_image(false) {
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

float Scan::LookUpVerticalAzimuth(const int& i) {
  return 1.9f - i * 0.2f;
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

/// @cond DO_NOT_DOCUMENT_THIS
int Packet::HeaderBytes() {
  return 42;
}
/// @endcond

/// @cond DO_NOT_DOCUMENT_THIS
int Packet::ColumnIdentifierBytes() {
  return 2;
}
/// @endcond

/// @cond DO_NOT_DOCUMENT_THIS
int Packet::AzimuthBytes() {
  return 2;
}
/// @endcond

/// @cond DO_NOT_DOCUMENT_THIS
int Packet::DistanceBytes() {
  return 2;
}
/// @endcond

/// @cond DO_NOT_DOCUMENT_THIS
int Packet::AmplitudeBytes() {
  return 1;
}
/// @endcond

/// @cond DO_NOT_DOCUMENT_THIS
int Packet::TimeStampBytes() {
  return 4;
}
/// @endcond

/// @cond DO_NOT_DOCUMENT_THIS
int Packet::FactoryBytes() {
  return 2;
}
/// @endcond

/// @cond DO_NOT_DOCUMENT_THIS
unsigned char Packet::ColumnIdentifierHigh() {
  return 0xFF;
}
/// @endcond

/// @cond DO_NOT_DOCUMENT_THIS
unsigned char Packet::ColumnIdentifierLow() {
  return 0xEE;
}
/// @endcond

int Packet::GreyImageStatusIndex() {
  return 16;
}

bool Packet::IsGreyImage(const char &grey_image_byte) {
  return grey_image_byte & 0x02;
}

std::unique_ptr<ParsedPacket> Packet::Parse() {
  std::unique_ptr<ParsedPacket> null_packet;
  std::unique_ptr<ParsedPacket> packet(new ParsedPacket);
  packet->grey_image = IsGreyImage(data[GreyImageStatusIndex()]);
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
    int chn_index = 0;
    for (auto& chn : col.channels) {
      auto dist_low = data[index++];
      auto dist_high = data[index++];
      auto amp = data[index++];

//      cout << hex << (short)dist_low << " " << (short)dist_high << endl;

      if (packet->grey_image) {
        chn.grey_value = ParseGreyValue(dist_high, dist_low);
      } else {
        chn.distance = ParseDistance(dist_high, dist_low);
      }
      chn.amplitude = ParseAmplitude(amp);
      chn.amp_raw = amp;

//      cout << chn.distance << endl;

      chn.h_azimuth = col.azimuth - Scan::FoV() / 2.0f;
      chn.v_azimuth = Scan::LookUpVerticalAzimuth(chn_index++);

//      cout << chn.h_azimuth << " " << chn.v_azimuth << endl;
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

unsigned short Packet::ParseGreyValue(
    const unsigned char &high, const unsigned char &low) {
//  unsigned char value_uc[2];
//  value_uc[0] = high;
//  value_uc[1] = low;
//  unsigned short value;
//  memcpy(&value, value_uc, 2);
  unsigned short value = high;
  value = (value << 8);
  value |= low;
  return value;
}

float Packet::ParseAmplitude(const unsigned char &raw) {
  return raw;
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

CommonResponsePacket::CommonResponsePacket() {
  data.resize(4, 0);
}

bool CommonResponsePacket::Successful() const {
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

/// @cond DO_NOT_DOCUMENT_THIS
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
/// @endcond DO_NOT_DOCUMENT_THIS

StartRequestPacket::StartRequestPacket() : RequestPacket() {
  SetCmdString("getDistanceAndAmplitudeSorted");
}

StopRequestPacket::StopRequestPacket() : RequestPacket() {
  SetCmdString("join");
}

/// @cond DO_NOT_DOCUMENT_THIS
EnableFilterRequestPacket::EnableFilterRequestPacket() : RequestPacket() {
  SetCmdString("enableFeatures 104876");
}
/// @endcond

/// @cond DO_NOT_DOCUMENT_THIS
DisableFilterRequestPacket::DisableFilterRequestPacket() : RequestPacket() {
  SetCmdString("disableFeatures 104876");
}
/// @endcond

EnableGrayOutputRequestPacket::EnableGrayOutputRequestPacket()
  : RequestPacket() {
  SetCmdString("enableGrayOutput 1");
}

DisableGrayOutputRequestPacket::DisableGrayOutputRequestPacket()
  : RequestPacket() {
  SetCmdString("enableGrayOutput 0");
}
}
