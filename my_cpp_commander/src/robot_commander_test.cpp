#include <rclcpp/rclcpp.hpp>
#include <moveit/move_group_interface/move_group_interface.h>
#include <iostream>
#include <string>
#include <chrono>
// #include <robot_interfaces/msg/gripper_status.hpp>

class commander{
    private :
        std::shared_ptr<rclcpp::Node> node_;
        std::shared_ptr<moveit::planning_interface::MoveGroupInterface> arm_;
        std::shared_ptr<moveit::planning_interface::MoveGroupInterface> gripper_;

        void PlanAndExecute(const std::shared_ptr<moveit::planning_interface::MoveGroupInterface> &interface) {
                moveit::planning_interface::MoveGroupInterface::Plan pose_plan;
                bool flag = (interface->plan(pose_plan) == moveit::core::MoveItErrorCode::SUCCESS) ;
                if (flag) {
                    interface->execute(pose_plan);
                }

                else {
                    RCLCPP_WARN(node_->get_logger(),"规划失败");
        }
    }

    public:
        commander(std::shared_ptr<rclcpp::Node> node){
            node_ = node;
            arm_ = std::make_shared<moveit::planning_interface::MoveGroupInterface> (node_,"arm");
            arm_ ->setMaxVelocityScalingFactor(1.0);
            arm_ ->setMaxAccelerationScalingFactor(0.7);
            gripper_ = std::make_shared<moveit::planning_interface::MoveGroupInterface>(node_,"gripper");

        }

        void Ik_Goto_Pose (const std::vector<double> &p) {
            if (p.size() != 6 && p.size() !=7) {
                RCLCPP_WARN(node_->get_logger(),"请输入x,y,z,rpy or x,y,z,Quaternion");
                return ;
            }
            
            geometry_msgs::msg::Pose pose_p;
            
            pose_p.position.x = p[0];
            pose_p.position.y = p[1];
            pose_p.position.z = p[2];

            if(p.size() == 6)
            { tf2::Quaternion q;
            q.setRPY(p[3],p[4],p[5]);
            q.normalize();
            pose_p.orientation.x = q.getX();
            pose_p.orientation.y = q.getY();
            pose_p.orientation.z = q.getZ();
            pose_p.orientation.w = q.getW();
            }

            else if(p.size() ==7)
            {
                pose_p.orientation.x = p[3];
                pose_p.orientation.y = p[4];
                pose_p.orientation.z = p[5];
                pose_p.orientation.w = p[6];

            }
            
           

            arm_->setStartStateToCurrentState();
            arm_->setPoseTarget(pose_p);

            PlanAndExecute(arm_);
    }

    void Goto_joint_pose (const std::vector<double> &joints) {
        arm_->setStartStateToCurrentState();
        arm_->setJointValueTarget(joints);
        PlanAndExecute(arm_);
    }
};

int main(int argc,char ** argv)
{
    rclcpp::init(argc,argv);
    auto node = std::make_shared<rclcpp::Node>("moviet_commander");
    std::shared_ptr<commander>
         commander1  = std::make_shared<commander>(node);

    std::vector<double> ready = {-0.010, 0.016, 0.307,-0.071, 0.434, 0.831, -0.339};
        commander1->Ik_Goto_Pose(ready);

    // std::vector<double> v1 = {0.189,-0.105,0.195,0.477,1.400,-0.026};
    //      commander1->Ik_Goto_Pose(v1);

    // std::vector<double> joint_value = {0.9, 0.4, 0.5, 0.5, 0.1};
    // commander1->Goto_joint_pose(joint_value);

    rclcpp::spin(node);
    rclcpp::shutdown();

}

