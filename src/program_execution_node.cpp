#include "actionlib/client/simple_action_client.h"
#include "control_msgs/FollowJointTrajectoryAction.h"
#include "control_msgs/GripperCommandAction.h"
#include "rapid_pbd/action_executor.h"
#include "rapid_pbd/action_names.h"
#include "rapid_pbd/program_executor.h"
#include "rapid_pbd/robot_config.h"
#include "rapid_pbd/visualizer.h"
#include "ros/ros.h"
#include "std_msgs/Bool.h"
#include "tf/transform_listener.h"
#include "visualization_msgs/MarkerArray.h"

namespace pbd = rapid::pbd;

int main(int argc, char** argv) {
  ros::init(argc, argv, "program_executor");
  ros::NodeHandle nh;

  tf::TransformListener tf_listener;
  ros::Publisher is_running_pub =
      nh.advertise<std_msgs::Bool>("is_running", 5, true);

  std::string robot("");
  bool is_robot_specified = ros::param::get("robot", robot);
  if (!is_robot_specified) {
    ROS_ERROR("robot param must be specified.");
    return 1;
  }

  pbd::RobotConfig* robot_config;
  if (robot == "pr2") {
    robot_config = new pbd::Pr2RobotConfig();
  } else if (robot == "fetch") {
    robot_config = new pbd::FetchRobotConfig();
  } else {
    ROS_ERROR("Unsupported robot \"%s\"", robot.c_str());
    return 1;
  }

  pbd::ActionClients action_clients;
  while (!action_clients.head_client.waitForServer(ros::Duration(5)) &&
         ros::ok()) {
    ROS_WARN("Waiting for head server.");
  }
  if (robot_config->num_arms() == 1) {
    while (!action_clients.gripper_client.waitForServer(ros::Duration(5)) &&
           ros::ok()) {
      ROS_WARN("Waiting for gripper server.");
    }
    while (!action_clients.arm_joint_client.waitForServer(ros::Duration(5)) &&
           ros::ok()) {
      ROS_WARN("Waiting for arm joint server.");
    }
  } else if (robot_config->num_arms() == 2) {
    while (!action_clients.l_gripper_client.waitForServer(ros::Duration(5)) &&
           ros::ok()) {
      ROS_WARN("Waiting for left gripper server.");
    }
    while (!action_clients.r_gripper_client.waitForServer(ros::Duration(5)) &&
           ros::ok()) {
      ROS_WARN("Waiting for right gripper server.");
    }
    while (!action_clients.l_arm_joint_client.waitForServer(ros::Duration(5)) &&
           ros::ok()) {
      ROS_WARN("Waiting for left arm joint server.");
    }
    while (!action_clients.r_arm_joint_client.waitForServer(ros::Duration(5)) &&
           ros::ok()) {
      ROS_WARN("Waiting for right arm joint server.");
    }
  } else {
    ROS_ERROR("num_arms can only be 1 or 2.");
    return 1;
  }
  while (!action_clients.surface_segmentation_client.waitForServer(
             ros::Duration(5)) &&
         ros::ok()) {
    ROS_WARN("Waiting for surface segmentation server.");
  }
  while (!action_clients.moveit_client.waitForServer(ros::Duration(5)) &&
         ros::ok()) {
    ROS_WARN("Waiting for MoveIt action server.");
  }

  ros::Publisher box_pub =
      nh.advertise<visualization_msgs::MarkerArray>("runtime_segmentation", 10);
  pbd::RuntimeVisualizer runtime_viz(*robot_config, box_pub);

  rapid::pbd::ProgramExecutionServer server(
      rapid::pbd::kProgramActionName, is_running_pub, &action_clients,
      *robot_config, tf_listener, runtime_viz);
  server.Start();
  ROS_INFO("RapidPbD program executor ready.");
  ros::spin();
  if (robot_config) {
    delete robot_config;
  }
  return 0;
}
