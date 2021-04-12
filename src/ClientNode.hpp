#pragma once

#include <flatbuffers/flatbuffers.h>
#include <schema_generated.h>

#include <QObject>
#include <chrono>
#include <thread>
#include <unordered_map>
#include <iostream>
#include <chrono>
#include <thread>
#include <iostream>
 
#include "decode.hpp"
#include "encode.hpp"

#include "elevator_interface.hpp"

#include "decode_elevator.hpp"
#include "encode_elevator.hpp"

// Some sample structs are in this header file.
#include "message_structs.h"

class ClientNode : public QObject {
  Q_OBJECT

  struct TopicParams {
    double priority;
    bool no_drop;
  };

  // In a complete client, we expect the following maps to be populated from configuration.
  // Map from topic name to pair of <last publication time, publication
  // interval?(sec)>
  std::unordered_map<
      std::string,
      std::pair<
          std::chrono::time_point<std::chrono::high_resolution_clock>, double>>
      rate_limits;
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
    // check rate limits
    if (rate_limits.count(topic)) {
      auto& rate_info = rate_limits[topic];
      const auto curr_time = std::chrono::high_resolution_clock::now();
      const auto duration =
          std::chrono::duration_cast<std::chrono::milliseconds>(
              curr_time - rate_info.first);
      const double elapsed_sec = duration.count() / 1000.0;

      if (elapsed_sec > rate_info.second) {
        rate_info.first = curr_time;
      } else {
        return;
      }
    }

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
        params.priority,
        params.no_drop);
  }

 Q_SIGNALS:
  void message_encoded(
      const QString& topic, const QByteArray& data, double priority,
      bool no_drop);

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
    s.topic_regex = "elevator/command";
    s.action = 1;

    encode_msg(s, "RobofleetSubscription", "subscriptions");
    std::chrono::seconds interval( 1 );
    while(true) {
      ElevatorStatus status;
      updateElevatorStatus(status);
      encode_msg(status, "ElevatorStatus", "elevator/status");
      std::this_thread::sleep_for( interval );
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
