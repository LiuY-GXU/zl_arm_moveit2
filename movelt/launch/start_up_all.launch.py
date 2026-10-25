
import os
from launch import LaunchDescription
from launch.actions import IncludeLaunchDescription, TimerAction
from launch.launch_description_sources import PythonLaunchDescriptionSource
from ament_index_python.packages import get_package_share_directory

def generate_launch_description():
    # 获取各个功能包的路径
    realsense_dir = get_package_share_directory('realsense2_camera')
    movelt_dir = get_package_share_directory('movelt')
    zlarm_descriptionc= get_package_share_directory('zlarm_description')
    aruco_dir = get_package_share_directory('aruco_ros')

    # 1. 深度相机 (RealSense)
    camera_launch = IncludeLaunchDescription(
        PythonLaunchDescriptionSource(
            os.path.join(realsense_dir, 'launch', 'rs_launch.py')
        ),
        launch_arguments={
            'enable_color': 'true',
            'enable_depth': 'true',
            'align_depth.enable': 'true'
        }.items()
    )

    # 2. MoveIt (机械臂控制)
    moveit_launch = IncludeLaunchDescription(
        PythonLaunchDescriptionSource(
            os.path.join(movelt_dir, 'launch', 'demo.launch.py')
        )
    )

    # 3. ArUco 识别
    aruco_launch = IncludeLaunchDescription(
        PythonLaunchDescriptionSource(
            os.path.join(aruco_dir, 'launch', 'single.launch.py')
        )
    )

    # 4. EasyHandEye2 标定界面
    handeye_launch = IncludeLaunchDescription(
        PythonLaunchDescriptionSource(
            os.path.join(movelt_dir, 'launch', 'handeye_calibrate.launch.py')
        )
    )

    return LaunchDescription([
        # 0秒：启动相机（最底层，需最先准备好数据流）
        camera_launch,
        
        # 延迟 1 秒：启动 MoveIt（需要时间加载 URDF 和建立 TF）
        TimerAction(
            period=1.0,
            actions=[moveit_launch]
        ),
        
        # 延迟 2 秒：启动 ArUco 识别（需要相机图像和 MoveIt 的 TF 就绪）
        TimerAction(
            period=4.0,
            actions=[aruco_launch]
        ),
        
        # 延迟 3 秒：启动标定程序（需要以上所有节点都跑起来）
        TimerAction(
            period=5.0,
            actions=[handeye_launch]
        )
    ])
