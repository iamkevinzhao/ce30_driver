#ifndef PACKET_H
#define PACKET_H

#include <vector>
#include <chrono>
#include <memory>
#include <unordered_map>
#include "export.h"
#include "data_types.h"

namespace ce30_driver {
/**
 * @brief contains the data collected by a photosensitive cell
 */
struct API Channel {
  /**
   * @brief constructor
   */
  Channel();
  /**
   * @brief distance in meters
   */
  float distance;

  unsigned short grey_value;
  /**
   * @brief amplitude
   */
  float amplitude;
  /**
    * @brief raw value of the amplitude byte
    */
  unsigned char amp_raw;

  enum class Type {
    normal = 0b00000001,
    signal_high = 0b00000010,
    signal_low = 0b00000100,
    amb_high = 0b00001000,
    kicked_out = 0b00010000,
    unknown = 0b00100000,
  };
  Type type() const;
  static int FullType();
  /**
   * @brief get 3D point
   * @return 3D point
   */
  Point point() const;
  /**
   * @brief maximum distance
   * @return distance in meters
   */
  static float DistanceMax();
  /**
   * @brief minimum distance
   * @return distance in meters
   */
  static float DistanceMin();

  static unsigned short GreyValueMax();

  static unsigned short GreyValueMin();
  /**
   * @brief horizontal azimuth in degrees
   */
  float h_azimuth;
  /**
   * @brief vertical azimuth in degrees
   */
  float v_azimuth;
};

/**
 * @brief a collection of channels which are vertically aligned
 */
struct API Column {
  /**
   * @brief constructor
   */
  Column();
  /**
   * @brief azimuth of this column
   */
  float azimuth;
  /**
   * @brief channels of this column
   */
  std::vector<Channel> channels;
  /**
   * @brief the number of channels in a column
   * @return the number of channels
   */
  static int ChannelNum();
};

/**
 * @brief contains data after being parsed from a packet
 */
struct API ParsedPacket {
  /**
   * @brief constructor
   */
  ParsedPacket();
  /**
   * @brief time stamp parsed from packet
   */
  double time_stamp;
  /**
   * @brief columns parsed from the packet
   */
  std::vector<Column> columns;
  /**
   * @brief the number of columns in a packet
   * @return the number of columns
   */
  static int ColumnNum();

  bool grey_image;
};

/**
 * @brief a scan of channels which contains data from all channels
 */
class API Scan {
public:
  /**
   * @brief constructor
   */
  Scan();
  /**
   * @brief add a column to this scan
   * @param column the column to add
   */
  void AddColumn(const Column& column);
  /**
   * @brief add columns from a parsed packet
   * @param packet the parsed packet to add
   */
  void AddColumnsFromPacket(const ParsedPacket& packet);
  /**
   * @brief check whether a scan is ready
   * @return true if the scan is ready; otherwise, false
   */
  bool Ready();
  /**
   * @brief clear all columns in this scan
   */
  void Reset();
  /**
   * @brief get the channel by giving its 2D indices in the scan
   * @param x horizontal index
   * @param y vertical index
   * @return the channel
   */
  Channel at(const int& x, const int& y) const;
  /**
   * @brief the width of the scan
   * @return the number of channels in horizontal
   */
  static int Width();
  /**
   * @brief the height of the scan
   * @return the number of channels in vertical
   */
  static int Height();
  /**
   * @brief refer to \ref Channel::DistanceMin()
   */
  static float DistanceMin();
  /**
   * @brief refer to \ref Channel::DistanceMax()
   */
  static float DistanceMax();
  /**
   * @brief the number of columns in a scan
   * @return the number of columns
   */
  static int ColumnNum();
  /**
   * @brief enumerate all azimuth values
   * @return the vector of azimuths in degrees
   */
  static std::vector<float> AzimuthMap();
  /**
   * @brief field of view
   * @return FoV in degree
   */
  static float FoV();
  static float AzimuthDelta();
  static int WhichColumn(const float& azimuth);
  /**
   * @brief given vertical index, find the vertical azimuth
   * @param i the vertical index
   * @return the vertical azimuth in degrees
   */
  inline static float LookUpVerticalAzimuth(const int& i);
private:
  std::unordered_map<int, Column> columns_;
};

/**
 * @brief base class of all UDP packets
 */
struct API PacketBase {
  virtual ~PacketBase();
  /**
   * @brief time stamp when the packet is received
   */
  std::chrono::high_resolution_clock::time_point stamp;
  /**
   * @brief raw packet message in ASCIIs
   */
  std::vector<unsigned char> data;
};

/**
 * @brief device measurement data packet
 */
struct API Packet : public PacketBase {
  /**
   * @brief constructor
   */
  Packet();
  /**
   * @brief parse this packet
   * @return the pointer to the parsed packet
   */
  std::unique_ptr<ParsedPacket> Parse();
  /// @cond DO_NOT_DOCUMENT_THIS
  inline static int HeaderBytes();
  /// @endcond
  /// @cond DO_NOT_DOCUMENT_THIS
  inline static int ColumnIdentifierBytes();
  /// @endcond
  /// @cond DO_NOT_DOCUMENT_THIS
  inline static int AzimuthBytes();
  /// @endcond
  /// @cond DO_NOT_DOCUMENT_THIS
  inline static int DistanceBytes();
  /// @endcond
  /// @cond DO_NOT_DOCUMENT_THIS
  inline static int AmplitudeBytes();
  /// @endcond
  /// @cond DO_NOT_DOCUMENT_THIS
  inline static int TimeStampBytes();
  /// @endcond
  /// @cond DO_NOT_DOCUMENT_THIS
  inline static int FactoryBytes();
  /// @endcond
  /// @cond DO_NOT_DOCUMENT_THIS
  inline static unsigned char ColumnIdentifierHigh();
  /// @endcond
  /// @cond DO_NOT_DOCUMENT_THIS
  inline static unsigned char ColumnIdentifierLow();
  /// @endcond
private:
  inline static int GreyImageStatusIndex();
  static bool IsGreyImage(const char& grey_image_byte);
  float ParseAzimuth(const unsigned char& high, const unsigned char& low);
  float ParseDistance(const unsigned char& high, const unsigned char& low);
  unsigned short ParseGreyValue(const unsigned char& high, const unsigned char& low);
  float ParseAmplitude(const unsigned char& raw);
  double ParseTimeStamp(std::vector<unsigned char> stamp_raw);
};

/**
 * @brief the base class of all request types
 */
struct API RequestPacket : public PacketBase {
  /**
   * @brief constructor
   */
  RequestPacket();
  virtual ~RequestPacket();
  /**
   * @brief set command string
   * @param cmd the command string
   * @return true if the command string is valid; otherwise, false
   */
  bool SetCmdString(const std::string& cmd);
  /**
   * @brief reset this packet
   */
  void ResetPacket();
};

/**
 * @brief packet of command for requesting device version
 */
struct API VersionRequestPacket : public RequestPacket {
  /**
   * @brief constructor
   */
  VersionRequestPacket();
};

/**
 * @brief packet of version information sent by the device
 */
struct API VersionResponsePacket : public PacketBase {
  VersionResponsePacket();
  /**
   * @brief parse the packet for the version string
   * @return the version string
   */
  std::string GetVersionString() const;
};

/**
 * @brief packet of command for setting device ID
 */
struct API SetIDRequestPacket : public RequestPacket {
  /**
   * @brief constructor
   * @param id the ID to set
   */
  SetIDRequestPacket(const int32_t& id);
};

/**
 * @brief meta class of common response packets
 */
struct API CommonResponsePacket : public PacketBase {
  /**
   * @brief constructor
   */
  CommonResponsePacket();
  /**
   * @brief if the device gave a positive response to the request
   * @return true if positive; otherwise, false
   */
  bool Successful() const;
};

using SetIDResponsePacket = CommonResponsePacket;

/**
 * @brief packet of command for acquiring device ID
 */
struct API GetIDRequestPacket : public RequestPacket {
  /**
   * @brief constructor
   */
  GetIDRequestPacket();
};

/**
 * @brief response packet to the ID requeset
 */
struct API GetIDResponsePacket : public PacketBase {
  /**
   * @brief constructor
   */
  GetIDResponsePacket();
  /**
   * @brief parse the packet for ID
   * @return the ID
   */
  int32_t ID() const;
};

/// @cond DO_NOT_DOCUMENT_THIS
struct API StampSyncRequestPacket : public RequestPacket {
  StampSyncRequestPacket(const uint32_t& microseconds);
};
/// @endcond

// Identical Functionality
using StampSyncResponsePacket = CommonResponsePacket;

/**
 * @brief packet of command for starting measuring
 */
struct API StartRequestPacket : public RequestPacket {
  /**
   * @brief constructor
   */
  StartRequestPacket();
};

/**
 * @brief packet of command for stopping measuring
 */
struct API StopRequestPacket : public RequestPacket {
  /**
   * @brief constructor
   */
  StopRequestPacket();
};

/// @cond DO_NOT_DOCUMENT_THIS
struct API EnableFilterRequestPacket : public RequestPacket {
  EnableFilterRequestPacket();
};
/// @endcond

using EnableFilterResponsePacket = CommonResponsePacket;

/// @cond DO_NOT_DOCUMENT_THIS
struct API DisableFilterRequestPacket : public RequestPacket {
  DisableFilterRequestPacket();
};
/// @endcond

using DisableFilterResponsePacket = CommonResponsePacket;

struct API EnableGrayOutputRequestPacket : public RequestPacket {
  EnableGrayOutputRequestPacket();
};

using EnableGrayOutputResponsePacket = CommonResponsePacket;

struct API DisableGrayOutputRequestPacket : public RequestPacket {
  DisableGrayOutputRequestPacket();
};

using DisableGrayOutputResponsePacket = CommonResponsePacket;
}

#endif // PACKET_H
