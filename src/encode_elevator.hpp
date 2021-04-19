#pragma once
#ifndef _ENCODE_ELEVATOR_HPP
#define _ENCODE_ELEVATOR_HPP
#include "encode.hpp"

// amrl_msgs/ElevatorStatus
template <>
flatbuffers::uoffset_t encode(
    FBB& fbb, const ElevatorStatus& msg, const MetadataOffset& metadata) {
  return fb::amrl_msgs::CreateElevatorStatus(
             fbb, metadata, msg.floor, msg.door)
      .o;
}

#endif