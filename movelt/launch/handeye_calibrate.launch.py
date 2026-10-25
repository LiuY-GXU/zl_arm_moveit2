import os
from launch import LaunchDescription
from launch.actions import IncludeLaunchDescription, DeclareLaunchArgument
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch.substitutions import PathJoinSubstitution, LaunchConfiguration
from launch_ros.substitutions import FindPackageShare
from launch_ros.actions import Node

def generate_launch_description():
    # 1. 启动 easy_handeye2 的标定界面
    handeye_calibration = IncludeLaunchDescription(
        PythonLaunchDescriptionSource([
            PathJoinSubstitution([
                FindPackageShare('easy_handeye2'),
                'launch/calibrate.launch.py'
            ])
        ]),
        launch_arguments={
            # --- 标定模式 ---
            'calibration_type': 'eye_in_hand',  # 眼在手上
            'name': 'my_eih_calib',             # 标定名称，可自定义
            
            # --- 机械臂坐标系（必须与你的URDF/TF完全一致） ---
            'robot_base_frame': 'base_link',    # 机械臂基座
            'robot_effector_frame': 'link5',    # 【必须修改】你的末端执行器link名称
            'publish_dummy_camera': 'false',
            
            # --- 视觉坐标系（必须与aruco_ros发布的一致） ---
            'tracking_base_frame': 'camera_color_optical_frame', # D435i的光学坐标系
            'tracking_marker_frame': 'aruco_marker_frame',       # ArUco节点发布的标记坐标系
            
            'freehand_robot_movement': 'true'  # 允许手动移动机械臂采样
        }.items()
    ) 
    
    return LaunchDescription([handeye_calibration])

