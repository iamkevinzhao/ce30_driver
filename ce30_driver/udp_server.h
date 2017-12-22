#ifndef UDP_SERVER_H
#define UDP_SERVER_H

#include <memory>
#include "data_types.h"
#include "export.h"
#include <functional>
#include "udp_socket.h"

namespace ce30_driver {
class API UDPServer
{
public:
  UDPServer();
  virtual ~UDPServer();
  bool Start();
  void RegisterCallback(
      std::function<void(std::shared_ptr<PointCloud>)> callback);
  void SpinOnce(const int& millisec = 1000);
protected:
  virtual void OnScanReceived(std::shared_ptr<PointCloud> pointcloud);
private:
  void DataReceiveThread();
  static bool instance_running_;
  std::unique_ptr<std::function<void(std::shared_ptr<PointCloud>)>> callback_;
  std::unique_ptr<UDPSocket> socket_;
  std::unique_ptr<std::thread> thread_;
  std::mutex scan_mutex_;
  Scan scan_;
  std::mutex signal_mutex_;
  bool kill_signal_;
  std::condition_variable condition_;
};
}
#endif // UDP_SERVER_H
