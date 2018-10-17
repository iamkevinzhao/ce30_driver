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
