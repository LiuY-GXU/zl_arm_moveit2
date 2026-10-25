#include <zl_arm_hardware/My_serial.hpp>
#include "stdint.h"

std::string port_ = "/dev/ttyUSB0";

std::unique_ptr<LibSerial::SerialPort> serial_ 
        = std::make_unique<LibSerial::SerialPort>();

uint16_t cnt = 2480;



int main()
{
    serial_->Open(port_);
    serial_->SetBaudRate(LibSerial::BaudRate::BAUD_115200);
    serial_->SetCharacterSize(LibSerial::CharacterSize::CHAR_SIZE_8);
    serial_->SetStopBits(LibSerial::StopBits::STOP_BITS_1);
    serial_->SetParity(LibSerial::Parity::PARITY_NONE);

    // while (1)
    // {
    //     if(cnt <= 520) break;
    //     zl_servo::test::set_single_servo_position(*serial_,5,cnt,5);
    //     cnt-= 20;
    //     RCLCPP_INFO(rclcpp::get_logger("zl_servo"), zl_servo::test::get_single_servo_position(*serial_,5).c_str());
    // }

    uint16_t cnt  = 1500;

    while(1)
    {
        

        if (cnt >= 2400) cnt =  1000 ;

        std::vector<std::pair<uint8_t,uint16_t>> id_and_position = {{0,cnt},{1,cnt},
                    {2,cnt},{3,cnt},{4,cnt},{5,cnt}};

        zl_servo::extend_all::set_multiple_servo(*serial_,id_and_position,500);

        for(uint8_t i = 0 ; i<=5;i++)
        {
            RCLCPP_INFO(rclcpp::get_logger("zl_servo"), 
                    zl_servo::test::get_single_servo_position(*serial_,i).c_str());
        }

        cnt += 5;
        

    }

    serial_->Close();

    return 0;
}

