#ifndef UDP_SERVER_H
#define UDP_SERVER_H

#include <memory>
#include "data_types.h"
#include "export.h"
#include <functional>
#include "udp_socket.h"

namespace ce30_driver {
/**
 * @brief device communication framework
 */
class API UDPServer
{
public:
  /**
   * @brief constructor
   */
  UDPServer();
  virtual ~UDPServer();
  /**
   * @brief start server
   * @return true is successful; otherwise, false
   */
  bool Start();
  /**
   * @brief register measurement data receiver function
   * @param callback the receiver function
   */
  void RegisterCallback(
      std::function<void(std::shared_ptr<PointCloud>)> callback);
  /**
   * @brief spin server once
   * @param millisec blocking time in milliseconds
   */
  void SpinOnce(const int& millisec = 1000);
protected:
  /**
   * @brief Default data receiver function
   * @param pointcloud pointer to the incoming point cloud
   * @remark the function won't be called after #RegisterCallback is called
   */
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
