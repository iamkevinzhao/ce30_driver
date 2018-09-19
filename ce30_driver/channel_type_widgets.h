#ifndef CHANNEL_TYPE_WIDGETS_H
#define CHANNEL_TYPE_WIDGETS_H

#include <vector>
#include <map>
#include <set>
#include <QCheckBox>
#include "packet.h"
#include "export.h"

namespace ce30_driver {

class API ChannelTypeWidgets
{
public:
  ChannelTypeWidgets(QWidget* parent);
  ~ChannelTypeWidgets();
  std::vector<QWidget*> GetAllWidgets();
  bool IsChannelTypeChecked(const Channel& channel);
  void Update();
private:
  std::map<Channel::Type, QCheckBox*> check_boxes_;
  QWidget* parent_;
  std::set<Channel::Type> channel_type_checked_;
};
} // namespace ce30_driver

#endif // CHANNEL_TYPE_WIDGETS_H
