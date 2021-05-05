#pragma once
#ifndef _ENCODE_ELEVATOR_HPP
#define _ENCODE_ELEVATOR_HPP
#include "elevator_interface.h"
#include "encode.hpp"

// amrl_msgs/ElevatorStatus
template <>
flatbuffers::uoffset_t encode(
    FBB& fbb, const ElevatorStatus& msg, const MetadataOffset& metadata) {
  return fb::amrl_msgs::CreateElevatorStatus(
             fbb, metadata, msg.floor, msg.door)
      .o;
}

// amrl_msgs/RobofleetStatus
template <>
flatbuffers::uoffset_t encode(
    FBB& fbb, const RobofleetStatus& msg, const MetadataOffset& metadata) {
  return fb::amrl_msgs::CreateRobofleetStatusDirect(
            fbb, metadata, msg.status.c_str(), msg.is_ok, msg.battery_level, msg.location.c_str())
      .o;
}


#endif