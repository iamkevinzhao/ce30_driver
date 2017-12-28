## Write a Simple Server

This tutorial shows you how to retrieve a scan of point cloud from CE30-D. Without further ado, let's dive into some codes.

```c++
#include <iostream>
#include <ce30_driver/ce30_driver.h>

using namespace std;
using namespace ce30_driver;

void DataReceiveCB(shared_ptr<PointCloud> cloud) {
  for (Point& point : cloud->points) {
    cout << point.x << " " << point.y << " " << point.z << endl;
  }
}

int main() {
  UDPServer server;
  server.RegisterCallback(DataReceiveCB);
  if (!server.Start()) {
    return -1;
  }
  while (true) {
   	server.SpinOnce();
  }
}
```

Easy and simple, right? Now let's crack each line.

```c++
#include <ce30_driver/ce30_driver.h>
```

Include this header file and you are able to invoke all `ce30_driver` components.

```c++
  UDPServer server;
```

Instantiate the `UDPServer` class. Note that `UDPServer` should be instantiated only once, or a `std::exception` will be thrown.

```c++
  server.RegisterCallback(DataReceiveCB);
```

Register a data receiver function. Every time there is a scan of point cloud data ready, the receiver function will get called.

```c++
  if (!server.Start()) {
    return -1;
  }
```

Start the server. Under the hood the server establishes a UDP connection between the host machine and the sensor device and sends a start command to the sensor. If everything goes smoothly, the function returns true; if any error that will disable further steps occurs during this process, the function returns false.

```c++
  while (true) {
   	server.SpinOnce();
  }
```

Spin the server so that it can seize and parse incoming data packets. When a scan of point cloud data gets ready, your receiver function will be invoked inside the `SpinOnce` routine. By default, `SpinOnce` blocks the thread for minimal one second (subject to change).

```c++
void DataReceiveCB(shared_ptr<PointCloud> cloud) {
  for (Point& point : cloud->points) {
    cout << point.x << " " << point.y << " " << point.z << endl;
  }
}
```

When the scan of point cloud is ready, it is passed through `std::shared_ptr` to your receiver function. An elementary member of `PointCloud` is `points`, an vector of `Point`. Note that `PointCloud` and `Point` are both defined by `ce30_driver` other than by any other third-party library. `Point` contains `x`, `y` and `z`, the Cartesian coordinates of the point in meters. Please refer to the [API documentation](../api_doc/html/index.html) for complete definitions of above-mentioned classes.