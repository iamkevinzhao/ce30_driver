#include "channel_type_widgets.h"

namespace ce30_driver {
ChannelTypeWidgets::ChannelTypeWidgets(QWidget* parent)
  : parent_(parent)
{
  QCheckBox* box;

  box = new QCheckBox(parent);
  box->setText("Normal Channels");
  box->setChecked(true);
  check_boxes_[Channel::Type::normal] = box;

  box = new QCheckBox(parent);
  box->setText("High Signal Channels");
  box->setChecked(true);
  check_boxes_[Channel::Type::signal_high] = box;

  box = new QCheckBox(parent);
  box->setText("Low Signal Channels");
  box->setChecked(true);
  check_boxes_[Channel::Type::signal_low] = box;

  box = new QCheckBox(parent);
  box->setText("High Ambient Channels");
  box->setChecked(true);
  check_boxes_[Channel::Type::amb_high] = box;
}

ChannelTypeWidgets::~ChannelTypeWidgets() {

}

std::vector<QWidget*> ChannelTypeWidgets::GetAllWidgets() {
  std::vector<QWidget*> result;
  for (auto& box : check_boxes_) {
    result.push_back(box.second);
  }
  return result;
}

bool ChannelTypeWidgets::IsChannelTypeChecked(const Channel &channel) {
  return channel_type_checked_.count(channel.type());
}

void ChannelTypeWidgets::Update() {
  channel_type_checked_.clear();
  for (auto& widget : check_boxes_) {
    if (widget.second->isChecked()) {
      channel_type_checked_.insert(widget.first);
    }
  }
}
}

