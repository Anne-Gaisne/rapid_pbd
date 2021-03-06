//  Created by Ying Siu Liang on 28/07/2017.
//
//

#ifndef _RAPID_PBD_CONDITION_GENERATOR_H_
#define _RAPID_PBD_CONDITION_GENERATOR_H_
#include <string>
#include <vector>

#include "rapid_pbd_msgs/Condition.h"
#include "rapid_pbd_msgs/Landmark.h"

#include "rapid_pbd/world.h"

namespace msgs = rapid_pbd_msgs;
namespace rapid {
namespace pbd {
// ConditionGenerator takes landmarks and infers conditions that can be used for
// pre-/postconditions
class ConditionGenerator {
 public:
  ConditionGenerator(const RobotConfig& robot_config);

  void AssignConditions(World* world);
  void AssignLandmarkCondition(const World& world,
                               const std::string& landmark_name,
                               msgs::Condition* condition);
  void UpdateReferenceLandmark(const World& world, msgs::Condition* condition,
                               const msgs::Landmark& reference);

 private:
  World* world_;
  const RobotConfig& robot_config_;

  bool IsLandmarkName(const msgs::Landmark& landmark, const std::string& name);
  void GetPropertyConditions(const msgs::Landmark& landmark, const World& world,
                             msgs::Condition* condition,
                             const float& defaultVariance);
  void GetRelativeConditions(const msgs::Landmark& landmark, const World& world,
                             msgs::Condition* condition,
                             const float& defaultVariance);
  geometry_msgs::Vector3 QuaternionToRPY(const geometry_msgs::Quaternion& msg);
  void SetReferenceConditions(msgs::Condition* condition,
                              const msgs::Landmark& reference,
                              const msgs::Landmark& landmark,
                              const geometry_msgs::Vector3& defaultVarVector);
  bool ReferencedLandmark(const msgs::Landmark& landmark, const World& world,
                          const double squared_distance_cutoff,
                          msgs::Landmark* reference);
  void GetDisplacementVector(const msgs::Landmark& landmark,
                             const msgs::Landmark& reference,
                             msgs::Condition* condition);
  void GetSpatialRelation(msgs::Condition* condition);
  void GetRelativeAlignment(msgs::Condition* condition);
  void CheckRelevantProperties(const World& world, msgs::Condition* condition);
  // void GetPositionsAroundObject(
  //     msgs::Condition* condition, const geometry_msgs::Vector3& dimensions,
  //     const geometry_msgs::Vector3& obj_distance,
  //     std::vector<geometry_msgs::Pose>* positions);
};
}  // namespace pbd
}  // namespace rapid

#endif  // _RAPID_PBD_CONDITION_GENERATOR_H_
