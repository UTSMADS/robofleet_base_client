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

class ClientNode : public QObject {
  Q_OBJECT

  struct TopicParams {
    double priority;
    double rate_limit;
    bool no_drop;
  };

  // In a complete client, we expect the following maps to be populated from configuration.
  // Map from topic names to more detailed params like priority, etc.
  std::unordered_map<std::string, TopicParams> topic_params;


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
    const TopicParams& params = topic_params[topic];
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

  void subscription_complete();

 public Q_SLOTS:
  /**
   * @brief Handle a received message here
   * @param data the Flatbuffer-encoded message data
   */
  void message_received(const QByteArray& data) {
    if (verbosity_ > 1) {
      std::cout << "Received message" << std::endl;
    }

    const auto* root =
          flatbuffers::GetRoot<typename flatbuffers_type_for<ElevatorCommand>::type>(
              data.data());
    const ElevatorCommand msg = decode<ElevatorCommand>(root);

    if (msg.floor_cmd > 0) {
      pressFloorButton(msg.floor_cmd);
    }

    if (msg.hold_door) {
      holdDoorOpen();
    }
  }

  /**
   * @brief Handle the connection of the websocket
   *
   * 
   */
  void connected() {
    if (verbosity_ > 1) {
      std::cout << "Websocket connection established." << std::endl;
    }

    RobofleetSubscription s;
    s.topic_regex = "/elevator/command";
    s.action = 1;

    encode_msg(s, "RobofleetSubscription", "subscriptions");
    Q_EMIT subscription_complete(); 
  }

  void emitStatus() {
    forever {
      ElevatorStatus status;
      updateElevatorStatus(status);
      printf("STATUS %d %d\n", status.floor, status.door);
      encode_msg(status, "amrl_msgs/ElevatorStatus", "/elevator/elevator_status");
      QThread::sleep(1);
    }
  }

 public:
  ClientNode(int verbosity) : verbosity_(verbosity)  {
    // run forever
    if (verbosity_ > 0) {
      std::cout << "Started Client Node" << std::endl;
    }
  }
};
