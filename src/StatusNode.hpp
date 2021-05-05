#pragma once
#include <flatbuffers/flatbuffers.h>
#include <schema_generated.h>

#include <QObject>
#include <QThread>
#include <chrono>
#include <thread>
#include <unordered_map>
#include <iostream>
#include <chrono>
#include <thread>
#include <iostream>
 
#include "elevator_interface.h"

#include "decode_elevator.hpp"
#include "encode_elevator.hpp"


// Some sample structs are in this header file.
#include "message_structs.h"

class StatusNode : public QObject {
  Q_OBJECT
  const int verbosity_;

  /**
   * @brief Emit a message_encoded() signal given a message and metadata.
   *
   * @tparam T type of msg
   * @param msg message to encode
   * @param msg_type type of the message
   * @param topic remote topic to send to
   */
  template <typename T>
  void encode_msg(
      const T& msg, const std::string& msg_type, const std::string& topic) {
    // encode message
    flatbuffers::FlatBufferBuilder fbb;
    auto metadata = encode_metadata(fbb, msg_type, topic);
    auto root_offset = encode<T>(fbb, msg, metadata);
    fbb.Finish(flatbuffers::Offset<void>(root_offset));
    const QByteArray data{reinterpret_cast<const char*>(fbb.GetBufferPointer()),
                          static_cast<int>(fbb.GetSize())};

    Q_EMIT message_encoded(
        QString::fromStdString(topic),
        data,
        1.0,
        1.0,
        false);
  }

 Q_SIGNALS:
  void message_encoded(
      const QString& topic, const QByteArray& data, double priority, double rate_limit,
      bool no_drop);


 public Q_SLOTS:
  void emitStatus() {
    forever {
      ElevatorStatus elevator_status;
      updateElevatorStatus(elevator_status);
      encode_msg(elevator_status, "amrl_msgs/ElevatorStatus", "/elevator/elevator_status");
      RobofleetStatus status;
      updateRobofleetStatusFromElevatorStatus(elevator_status, status);
      encode_msg(status, "amrl_msgs/RobofleetStatus", "/elevator/status");
      QThread::sleep(1);
    }
  }


 public:
  StatusNode(int verbosity) : verbosity_(verbosity)  {
    if (verbosity_ > 0) {
      std::cout << "Started Status Node" << std::endl;
    }
  }
};
