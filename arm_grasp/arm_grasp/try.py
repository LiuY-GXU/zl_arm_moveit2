import rclpy
from rclpy.node import Node
from geometry_msgs.msg import PoseStamped
import tf2_ros
import tf2_geometry_msgs
from rclpy.duration import Duration

class TargetTFNode(Node):
    def __init__(self):
        super().__init__('target_tf_node')
        
        
        self.tf_buffer = tf2_ros.Buffer()
        self.tf_listener = tf2_ros.TransformListener(self.tf_buffer, self)

        
        self.sub = self.create_subscription(
            PoseStamped,
            '/aruco_single/pose',
            self.pose_callback,
            10
        )
        self.get_logger().info("已启动！等待识别到 ArUco 码...")

    def pose_callback(self, msg):
        cam_x = msg.pose.position.x
        cam_y = msg.pose.position.y
        cam_z = msg.pose.position.z
        
        self.get_logger().info(f"[相机下坐标] 中心点: X={cam_x:.3f}, Y={cam_y:.3f}, Z={cam_z:.3f} 米")

        try:
          
            msg.header.stamp = rclpy.time.Time().to_msg()
            target_in_base = self.tf_buffer.transform(msg, 'base_link', timeout=Duration(seconds=0.5))
            
            base_x = target_in_base.pose.position.x
            base_y = target_in_base.pose.position.y
            base_z = target_in_base.pose.position.z 
            
            self.get_logger().info(f"底座下坐标中心点: X={base_x:.3f}, Y={base_y:.3f}, Z={base_z:.3f} 米\n")
            
        except Exception as e:
            self.get_logger().warn(f"TF 转换失败，原因: {e}")

def main(args=None):
    rclpy.init(args=args)
    node = TargetTFNode()
    rclpy.spin(node)
    node.destroy_node()
    rclpy.shutdown()

if __name__ == '__main__':
    main()