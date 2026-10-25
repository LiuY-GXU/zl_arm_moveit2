from launch_ros.parameter_descriptions import ParameterValue
import launch
import launch_ros
from ament_index_python.packages import get_package_share_directory
import os

def generate_launch_description():
    urdf_package_path = get_package_share_directory("zlarm_description")
    xacro_path =  os.path.join(urdf_package_path,"urdf","zl_arm.xacro")
    controller_yaml = os.path.join(urdf_package_path,"config","ros2_controllers.yaml")

    action_declare_arg_model_path = launch.actions.DeclareLaunchArgument(
        name="model",default_value=xacro_path,description="加载的model路径"
    )

    # 【新增】1. 声明一个 launch 参数，默认值为 true，可以在命令行覆盖
    action_declare_arg_servo_release = launch.actions.DeclareLaunchArgument(
        name="servo_release", 
        default_value="true", 
        description="是否在启动时释放舵机力矩"
    )

    # 【修改】2. 在 xacro 命令后面加上参数传入
    command_result = launch.substitutions.Command([
        'xacro ', 
        launch.substitutions.LaunchConfiguration("model"),
        ' servo_release:=', launch.substitutions.LaunchConfiguration("servo_release") # <--- 核心在这
    ]) 
    
    value = ParameterValue(command_result,value_type=str)

    action_robot_state_pulisher = launch_ros.actions.Node(
        package="robot_state_publisher",
        executable="robot_state_publisher",
        parameters=[{"robot_description":value}]
    )

    controller_node = launch_ros.actions.Node(
        package="controller_manager",
        executable="ros2_control_node",
        parameters=[controller_yaml],
        remappings=[
            ("/controller_manager/robot_description", "/robot_description")
        ]
    )

    action_joint_state_broadcaster = launch_ros.actions.Node(
        package="controller_manager",
        executable="spawner",
        arguments=["joint_state_broadcaster"],
        output="screen"
    )
    
    action_arm_controller = launch_ros.actions.Node(
        package="controller_manager",
        executable="spawner",
        arguments=["arm_controller"],
        output="screen"
    )
    action_gripper_drive_controller = launch_ros.actions.Node(
        package="controller_manager",
        executable="spawner",
        arguments=["gripper_controller"],
        output="screen"
    )
    
    action_rviz_node = launch_ros.actions.Node(
        package="rviz2",
        executable="rviz2")

    return launch.LaunchDescription(
        [
            action_declare_arg_model_path,
            action_declare_arg_servo_release, # 【新增】把参数声明加进来
            action_robot_state_pulisher,
            controller_node,
            action_joint_state_broadcaster,
            action_arm_controller,
            action_gripper_drive_controller,
            action_rviz_node,
        ]
    )