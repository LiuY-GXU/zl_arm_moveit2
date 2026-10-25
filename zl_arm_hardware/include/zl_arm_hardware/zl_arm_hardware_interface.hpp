#ifndef ARM_HARDWARE_INTERFACE
#define ARM_HARDWARE_INTERFACE

#include "hardware_interface/system_interface.hpp"
#include "libserial/SerialPort.h"
#include "memory"
#include "string"
#include "My_serial.hpp"
#include "cmath"
#include "thread"
#include "mutex"
#include "atomic"
#include <algorithm> 



namespace arm_hardware{

class ArmHardwareInterface : public hardware_interface::SystemInterface
{
    public:
        hardware_interface::CallbackReturn 
            on_init(const hardware_interface::HardwareInfo &info) override;

        hardware_interface::CallbackReturn
            on_configure(const rclcpp_lifecycle::State & previous_state) override;

        hardware_interface::CallbackReturn 
            on_activate(const rclcpp_lifecycle::State & previous_state) override;

        hardware_interface::return_type
            read(const rclcpp::Time & time,const rclcpp::Duration & period ) override;

        hardware_interface::return_type
            write(const rclcpp::Time & time,const rclcpp::Duration & period ) override;

        hardware_interface::CallbackReturn
        on_deactivate(const rclcpp_lifecycle::State & previous_state) override;

        std::vector<hardware_interface::StateInterface> export_state_interfaces() override
        {
            std::vector<hardware_interface::StateInterface> states;
            
            for(uint8_t i = 0;i<NUMBER_OF_SERVO;i++)
            {

                if(i<5)
                {
                    states.emplace_back(fmt::format("joint{}",i+1),
                    "position",
                    &servo_position_[i]);
                }

                else if (i == 5)
                {
                    states.emplace_back("figger_joint1",
                        "position",
                        &servo_position_[i]);
                }

            }

            return states;
        }

        std::vector<hardware_interface::CommandInterface> export_command_interfaces() override
        {
            std::vector<hardware_interface::CommandInterface> commands;

            for(uint8_t j = 0;j<NUMBER_OF_SERVO;j++)
            {
                if(j<5)
                {
                    commands.emplace_back(fmt::format("joint{}",j+1),"position",&command_position_[j]);
                }
                
                else if (j == 5)
                {
                    commands.emplace_back("figger_joint1","position",&command_position_[j]);
                }
            }
            

            return commands;
        }


        
            
    private:
        std::unique_ptr<LibSerial::SerialPort> serial_;
        std::string port_;
        std::vector<std::pair<uint8_t,uint16_t>> servo_ID_and_POSITION{NUMBER_OF_SERVO,{0,1500}};

        std::vector<double> servo_position_
                    = std::vector<double>(6,M_PI/2);

        std::vector<double> command_position_
                    = std::vector<double>(6,0.0);

        std::string release_effort_ = "false";

        std::thread thread_read_;
        std::mutex mux1 ;
        std::atomic<bool> running_state = std::atomic<bool>(true);
};
}

#endif

