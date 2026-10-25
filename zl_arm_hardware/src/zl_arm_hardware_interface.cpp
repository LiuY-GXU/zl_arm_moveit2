#include "zl_arm_hardware/zl_arm_hardware_interface.hpp"

namespace arm_hardware{
    hardware_interface::CallbackReturn ArmHardwareInterface::on_init
    (const hardware_interface::HardwareInfo &info)
    {
        if(hardware_interface::SystemInterface::on_init(info) != 
        hardware_interface::CallbackReturn::SUCCESS)
        {
            return hardware_interface::CallbackReturn::ERROR;
        }

        info_ = info;
        
        for(uint8_t i =0 ; i<NUMBER_OF_SERVO;i++)
        {
            
            if(i<5)
            {
                servo_ID_and_POSITION[i].first = std::stoi
                                (info_.hardware_parameters[fmt::format("joint{}_motor_id",i+1)]);
            }

            else{
                servo_ID_and_POSITION[i].first = std::stoi
                    (info_.hardware_parameters["figger_joint_id"]);
            }
            
            
        }


        port_ = info_.hardware_parameters["port_name"];
        release_effort_ = info_.hardware_parameters["servo_release"];


        serial_ = std::make_unique<LibSerial::SerialPort>();

        return CallbackReturn::SUCCESS;

    }


    hardware_interface::CallbackReturn ArmHardwareInterface::on_configure
    (const rclcpp_lifecycle::State & previous_state)
    {
        (void) previous_state;

        RCLCPP_INFO(rclcpp::get_logger("ArmHardwareInterface"),"all_servo is configuring...");

        try{
            serial_->Open(port_);
            serial_->SetBaudRate(LibSerial::BaudRate::BAUD_115200);
            serial_->SetCharacterSize(LibSerial::CharacterSize::CHAR_SIZE_8);
            serial_->SetStopBits(LibSerial::StopBits::STOP_BITS_1);
            serial_->SetParity(LibSerial::Parity::PARITY_NONE);


            zl_servo::detail::remove_default_response(*serial_);
            
            if(release_effort_ == "true" || release_effort_ == "True")
            zl_servo::extend_all::all_Servo_realse_effort(*serial_);

        }
        catch(...) {
            RCLCPP_WARN(rclcpp::get_logger("ArmHardwareInterface"),"on_configuring failed");

            return CallbackReturn::FAILURE;
        }

        return CallbackReturn::SUCCESS;

    }

    hardware_interface::CallbackReturn ArmHardwareInterface::on_activate
    (const rclcpp_lifecycle::State& previous_state)
    {
        (void) previous_state;

        try{
            RCLCPP_INFO(rclcpp::get_logger("ArmHardwareInterface"),"on_activateing ...");

           auto read_function = [this](std::vector<std::pair<uint8_t,uint16_t>>* pair_vec,
                        std::vector<double> * double_vec ,
                            std::vector<double> *command_position) -> void {
            while (running_state.load())
            {
            
                zl_servo::extend_all::get_multiply_servo_position(*serial_,*pair_vec,NUMBER_OF_SERVO);

                // RCLCPP_INFO(rclcpp::get_logger("pwm:"),"%d",(*pair_vec)[5].second);

                std::lock_guard<std::mutex> lock(mux1);

                
                for (size_t i = 0; i < NUMBER_OF_SERVO; ++i) {

                    uint16_t pwm = (*pair_vec)[i].second;
                    
                    if(i < 5) {
                        (*double_vec)[i] = (pwm-1500.0)/1000.0 * (M_PI/2);
                    } 
                    
                    else {
                        (*double_vec)[i] = (1500.0-pwm)/500.0 *(0.0125);
                    }
                }

            if(release_effort_ == "false" || release_effort_ == "False")
             {
                for (size_t i = 0; i < NUMBER_OF_SERVO; ++i)
                {
                    if (i == 5) 
                    {
                        (*pair_vec)[i].second = static_cast<uint16_t>
                                    ((-(*command_position)[i] / 0.02 ) * 1000 + 1500);
                    } 
                    
                    else 
                    {
                        (*pair_vec)[i].second = static_cast<uint16_t>
                                    ((*command_position)[i] / (M_PI / 2) * 1000.0 + 1500.0);
                    }
                }

            
                zl_servo::extend_all::set_multiple_servo(*serial_, *pair_vec, 5);

            
                // std::this_thread::sleep_for(std::chrono::milliseconds(20)); 
             }  
            }
        };


            thread_read_ = std::thread(read_function,&servo_ID_and_POSITION,
                        &servo_position_,&command_position_);
    
            return hardware_interface::CallbackReturn::SUCCESS;

        }

        catch(...)
        {
            RCLCPP_WARN(rclcpp::get_logger("ArmHardwareInterface"),"on_activating failed");

            return hardware_interface::CallbackReturn::FAILURE;
        }


    }

    hardware_interface::return_type ArmHardwareInterface::read
        (const rclcpp::Time & /*time*/ , const rclcpp::Duration & /*period*/)
    {
        
        try{
            
             

            return hardware_interface::return_type::OK;
        }

        catch(...)
        {
            RCLCPP_WARN(rclcpp::get_logger("ArmHardwareInterface"),"read falied...");
            return hardware_interface::return_type::ERROR;
        }

    }

    hardware_interface::return_type ArmHardwareInterface::write
        (const rclcpp::Time & time, const rclcpp::Duration & period)
    {
        (void) time;
        (void) period;

        return hardware_interface::return_type::OK;
    }


    hardware_interface::CallbackReturn ArmHardwareInterface::on_deactivate(
    const rclcpp_lifecycle::State& /*previous_state*/)
    {
        running_state = false;

        if (thread_read_.joinable()) {
        thread_read_.join();}
        
        
        serial_->Close();

        RCLCPP_INFO(rclcpp::get_logger("ArmHardwareInterface"), "on_deactivated");
        return CallbackReturn::SUCCESS;
    }


}

#include "pluginlib/class_list_macros.hpp"

PLUGINLIB_EXPORT_CLASS(arm_hardware::ArmHardwareInterface, hardware_interface::SystemInterface)

