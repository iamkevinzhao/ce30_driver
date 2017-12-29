## Using UDP Packets

Now let's go deeper to see how to use the UDP socket and different types of packet to achieve the communication. Let's look at the following code snippet.

```c++
#include <iostream>
#include <ce30_driver/ce30_driver.h>

using namespace std;
using namespace ce30_driver;

int main() {
  UDPSocket socket;
  if (!Connect(socket)) {
    return -1;
  }
  VersionRequestPacket version_request;
  if (!SendPacket(version_request, socket)) {
    return -1;
  }
  VersionResponsePacket version_response;
  if (!GetPacket(version_response, socket)) {
    return -1;
  }
  cout << "CE30-D Version: " << version_response.GetVersionString() << endl;
  StartRequestPacket start_request;
  if (!SendPacket(start_request, socket)) {
    return -1;
  }
  // Now it's ready to receive measurement data
  Packet packet;
  while (true) {
    if (!GetPacket(packet, socket)) {
      continue;
    }
    unique_ptr<ParsedPacket> parsed = packet.Parse();
    if (parsed) {
      for (Column& column : parsed->columns) {
        for (Channel& channel : column.channels) {
          // Print "[distance, amplitude] (x, y, z)"
          cout << 
              "(" << channel.distance << ", " << channel.amplitude << ") "
              "[" << 
                  channel.point().x << ", " << 
                  channel.point().y << ", " << 
                  channel.point().z << "]" << endl;
        }
      }
    }
  }
  StopRequestPacket stop_request;
  SendPacket(stop_request, socket);
}
```

This program basically achieved the same function that the previous tutorial has achieved. Beside, it also provides the API to get the measured distances and amplitudes, which will be discussed in the next tutorial. Here, we focus on how to manipulate UDP socket and packets in order to acquire such functionalities.

```c++
  UDPSocket socket;
  if (!Connect(socket)) {
    return -1;
  }
```

First, we instantiate a `UDPSocket` object and then create a connection with the device. We should handle the case where `Connect` returned `false`, because it will cause further operation failed.

```c++
  VersionRequestPacket version_request;
  if (!SendPacket(version_request, socket)) {
    return -1;
  }
  VersionResponsePacket version_response;
  if (!GetPacket(version_response, socket)) {
    return -1;
  }
```

Next, we send a request packet to get device version and immediately listen to the response. We should not proceed the program until the version string is acquired. When we get the device version, it indicates that we are able to interact with the device.

```c++
  StartRequestPacket start_request;
  if (!SendPacket(start_request, socket)) {
    return -1;
  }
```

So far, CE30-D has not been working yet. We need to send "start" command to make it perform measuring. We achieve this by sending a `StartRequestPacket` and expect it to work. 

```c++
  Packet packet;
  while (true) {
    if (!GetPacket(packet, socket)) {
      continue;
    }
```

Now it's time to receive measurement data packets. In this tutorial, we are not going to talk about how to parse the packet. We will do it in the next tutorials.

```c++
  StopRequestPacket stop_request;
  SendPacket(stop_request, socket);
```

When we want CE30-D to stop measuring, we send a `StopRequestPacket`.



[Previous Tutorial: Write A Simple Server](write_a_simple_server.md)

[Next Tutorial: Channel, Column and Scan](channel_column_and_scan.md)