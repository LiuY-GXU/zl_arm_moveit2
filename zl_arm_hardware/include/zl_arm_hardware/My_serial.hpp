#ifndef __MY_SERIAL_HPP
#define __MY_SERIAL_HPP

#include <libserial/SerialPort.h>
#include <rclcpp/rclcpp.hpp>
#include "cstdint"
#include "cstdio"
#include "string"
#include <stdexcept>
#include <vector>
#include <unistd.h>
#include "iostream"
#include <fmt/core.h> 

constexpr uint8_t SERVO_BROADCAST_ID = 255;
constexpr uint8_t SERVO_MODE_MIN = 1;
constexpr uint8_t SERVO_MODE_MAX = 8;
constexpr uint16_t SERVO_POS_MIN = 520;
constexpr uint16_t SERVO_POS_MAX = 2480;
constexpr uint16_t TIME_MIN = 0;
constexpr uint16_t TIME_MAX = 9999;
constexpr uint16_t DEFAULT_TIME_COST = 1000;
constexpr uint8_t NUMBER_OF_SERVO = 6;

namespace zl_servo
{
  namespace detail
  {
    inline void send_cmd(LibSerial::SerialPort& port,const std::string& cmd)
    {
        if(!port.IsOpen())
        {
            RCLCPP_ERROR(rclcpp::get_logger("zl_servo"),"port is not open");
            return;
        }

        try
        {
            port.Write(cmd);
        }

        catch(const std::exception& e)
        {
            RCLCPP_ERROR(rclcpp::get_logger("zl_servo"),"error: %s",e.what());
            return;
        }
    }


    inline void remove_default_response(LibSerial::SerialPort& port)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(2000));
        port.FlushInputBuffer();
    }

  }

  


  namespace test
  {
    inline void set_single_servo_position(LibSerial::SerialPort& port,uint8_t servo_ID,uint16_t position,uint16_t time_cost = DEFAULT_TIME_COST)
    {
        if (servo_ID == SERVO_BROADCAST_ID)
            {
                RCLCPP_ERROR(rclcpp::get_logger("zl_servo"), "Use broadcast_set_pos instead");
                return;
            }

        if (position< SERVO_POS_MIN || position > SERVO_POS_MAX)
        {
            RCLCPP_ERROR(rclcpp::get_logger("zl_servo"),"postion out of range 500-2500");
            return;
        }

        if(time_cost == 0)
        {
            time_cost = 1000;
        }

        if(time_cost >9999)
        {
            RCLCPP_ERROR(rclcpp::get_logger("zl_servo"),"time_cost out of range 0-9999,reset time_cost=1000");
            time_cost = 1000;
        }

        try{
            char buf[64] = {0};
            std::sprintf(buf,"#%03dP%04dT%04d!",servo_ID,position,time_cost);
            std::this_thread::sleep_for(std::chrono::milliseconds(30));
            detail::send_cmd(port,buf);
            std::this_thread::sleep_for(std::chrono::milliseconds(15));
            port.FlushInputBuffer();
        }

        catch(const std::exception& e)
        {
            RCLCPP_ERROR(rclcpp::get_logger("zl_servo"),"error: %s",e.what());
            return;
        }

        try{
            port.FlushInputBuffer();
        }

        catch(...)
        {
            RCLCPP_ERROR(rclcpp::get_logger("zl_servo"),"set_single_servo_position 2th catch");
        }
        

    }

    inline void reset_one_servo_center_angle(LibSerial::SerialPort& port,uint8_t servo_ID)
    {
          if (servo_ID == SERVO_BROADCAST_ID)
            {
                RCLCPP_ERROR(rclcpp::get_logger("zl_servo"), "Use broadcast_set_pos instead");
            }

        char buf[64] = {0};
        std::sprintf(buf,"#%03dPSCK!",servo_ID);

        try
        {
            detail::send_cmd(port,buf);
        }

        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        port.FlushInputBuffer();
        

    }

    inline void reset_single_servo_id(LibSerial::SerialPort& port,uint8_t orignal_ID,uint8_t new_ID)
    {
        if (orignal_ID == SERVO_BROADCAST_ID)
            {
                RCLCPP_ERROR(rclcpp::get_logger("zl_servo"), "Use broadcast_set_pos instead");
                return ;
            }

        char buf[64];
        std::sprintf(buf,"#%03dPID%03d!",orignal_ID,new_ID);
        detail::send_cmd(port,buf);
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
        port.FlushInputBuffer();

        
    }

    inline  std::string get_single_servo_position(LibSerial::SerialPort& port,uint8_t servo_ID)
    {
        if (servo_ID == SERVO_BROADCAST_ID)
            {
                RCLCPP_ERROR(rclcpp::get_logger("zl_servo"), "Use broadcast_set_pos instead");
                return "error";
            }
        
        char buf[64] = {0};
        std::string back_position;
        std::sprintf(buf,"#%03dPRAD!",servo_ID);
        
       
        try{
            detail::send_cmd(port,buf);
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            port.Read(back_position,19,1000);
            port.FlushInputBuffer();
        }
        
        catch(...)
        {
            back_position = "out of time";
            
        }
       
        try{
            port.FlushInputBuffer();
        }

        catch(...)
        {
            back_position = "get_single_servo_position 2th catch";
        }

        // RCLCPP_INFO(rclcpp::get_logger("zl_servo"),back_position.c_str());
        return back_position;
        
    }

    inline void realse_single_servo_effort(LibSerial::SerialPort& port,uint8_t servo_ID)
    {
        if (servo_ID == SERVO_BROADCAST_ID)
            {
                RCLCPP_ERROR(rclcpp::get_logger("zl_servo"), "Use broadcast_set_pos instead");
                return ;
            }
        
        char buf[64];
        std::sprintf(buf,"#%03dPULK!",servo_ID);
        detail::send_cmd(port,buf);
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
        port.FlushInputBuffer();
    }

    inline void reset_single_servo_effort(LibSerial::SerialPort& port,uint8_t servo_ID)
    {
        if (servo_ID == SERVO_BROADCAST_ID)
            {
                RCLCPP_ERROR(rclcpp::get_logger("zl_servo"), "Use broadcast_set_pos instead");
                return;
            }
        
        char buf[64];
        std::sprintf(buf,"#%03dPULR!",servo_ID);
        detail::send_cmd(port,buf);
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
        port.FlushInputBuffer();
    }

    inline void stop_servo(LibSerial::SerialPort& port,uint8_t servo_ID)
    {
        if (servo_ID == SERVO_BROADCAST_ID)
            {
                RCLCPP_ERROR(rclcpp::get_logger("zl_servo"), "Use broadcast_set_pos instead");
                return;
            }
        
        char buf[64];
        std::sprintf(buf,"#%03dPDPT!",servo_ID);
        detail::send_cmd(port,buf);
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
        port.FlushInputBuffer();
    }


    inline void start_servo(LibSerial::SerialPort& port,uint8_t servo_ID)
    {
        if (servo_ID == SERVO_BROADCAST_ID)
            {
                RCLCPP_ERROR(rclcpp::get_logger("zl_servo"), "Use broadcast_set_pos instead");
                return;
            }
        
        char buf[64];
        std::sprintf(buf,"#%03dPDCT!",servo_ID);
        detail::send_cmd(port,buf);
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
        port.FlushInputBuffer();
    }


    inline void stop_servo_and_not_restart(LibSerial::SerialPort& port,uint8_t servo_ID)
    {
         if (servo_ID == SERVO_BROADCAST_ID)
            {
                RCLCPP_ERROR(rclcpp::get_logger("zl_servo"), "Use broadcast_set_pos instead");
                return;
            }
        
        char buf[64];
        std::sprintf(buf,"#%03dPDST!",servo_ID);
        detail::send_cmd(port,buf);
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
        port.FlushInputBuffer();
    }

   
    inline void single_servo_mode_change(LibSerial::SerialPort& port,uint8_t mode,uint8_t servo_ID)
    {
        if (servo_ID == SERVO_BROADCAST_ID)
            {
                RCLCPP_ERROR(rclcpp::get_logger("zl_servo"), "Use broadcast_set_pos instead");
                return;
            }
        
        if(mode>= 9)
        {
            RCLCPP_ERROR(rclcpp::get_logger("zl_servo"),"mode out of range,default mode = 1");
            mode = 1;
        }

        char buf[64];
        std::sprintf(buf,"#%03dPMOD%01d!",servo_ID,mode);
        detail::send_cmd(port,buf);
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
        port.FlushInputBuffer();
    }

    inline void single_servo_get_mode(LibSerial::SerialPort& port,uint8_t servo_ID)
    {
        if (servo_ID == SERVO_BROADCAST_ID)
            {
                RCLCPP_ERROR(rclcpp::get_logger("zl_servo"), "Use broadcast_set_pos instead");
                return;
            }
        
        char buf[64];
        std::sprintf(buf,"#%03dPMOD!",servo_ID);
        detail::send_cmd(port,buf);
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
        std::string response;
        port.ReadLine(response,'!');
        RCLCPP_INFO(rclcpp::get_logger("ZL_servo"),response.c_str());
        port.FlushInputBuffer();

    }

    inline void write_and_read(LibSerial::SerialPort& port,uint8_t servo_ID)
    {
        if (servo_ID == SERVO_BROADCAST_ID)
            {
                RCLCPP_ERROR(rclcpp::get_logger("zl_servo"), "Use broadcast_set_pos instead");
                return;
            }

        char buf[64];
        std::sprintf(buf,"#%03dPMOD3!",servo_ID);
        detail::send_cmd(port,buf);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        port.FlushInputBuffer();

        uint16_t a = 500;

        while(1)
        {
            a+=20;
            if(a > 2480)
            {
                a = 500;
            }

            set_single_servo_position(port,servo_ID,a,50);
            get_single_servo_position(port,servo_ID);
        }
    }


  }

  namespace extend_all
  {
    inline void all_servo_init(LibSerial::SerialPort& port,uint8_t mode,const uint8_t num = NUMBER_OF_SERVO)
    { 
        detail::remove_default_response(port);
        
        
        for (uint8_t i;i<num;i++)
        {
            char buf[64] = {0};
            std::sprintf(buf,"#%03dPMOD%01d!",i,mode);
            detail::send_cmd(port,buf);
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }

       
    }

    inline void all_Servo_realse_effort(LibSerial::SerialPort& port, uint8_t num = NUMBER_OF_SERVO)
    {
        for (uint8_t i;i<num;i++)
        {
            test::realse_single_servo_effort(port,i);
        }
    }

    inline void all_Servo_reset_effort(LibSerial::SerialPort& port, uint8_t num = NUMBER_OF_SERVO)
    {
        for (uint8_t i;i<num;i++)
        {
            test::reset_single_servo_effort(port,i);
        }
    }

    inline void stop_all_servo(LibSerial::SerialPort& port, uint8_t num = NUMBER_OF_SERVO)
    {
        for(uint8_t i=0;i<num;i++)
        {
            test::stop_servo_and_not_restart(port,i);
        }

        port.FlushInputBuffer();

    }

    inline void reset_all_center_angle(LibSerial::SerialPort& port,uint8_t num = NUMBER_OF_SERVO)
    {
        for(uint8_t i =0;i<num;i++)
        {
            test::reset_one_servo_center_angle(port,i);
        }

        port.FlushInputBuffer();
    }

    inline void set_multiple_servo(LibSerial::SerialPort& port,
       const std::vector<std::pair<uint8_t,uint16_t>>& ID_and_position,
        const uint16_t time_cost = DEFAULT_TIME_COST)
    {
        std::string temp;
        std::string servo_ID_and_POSITIONs ;
        for (uint8_t i=0 ; i<ID_and_position.size(); i++)
        {
            // if(i == 1) temp = fmt::format("#001P{:04d}T{:04d}!",
            //     ID_and_position[1].second,time_cost);
            if(i == 5)
            {
                temp = fmt::format("#{:03d}P{:04d}T{:04d}!",
                ID_and_position[i].first,ID_and_position[i].second,100);
            }

            else temp = fmt::format("#{:03d}P{:04d}T{:04d}!",
                ID_and_position[i].first,ID_and_position[i].second,time_cost);

            servo_ID_and_POSITIONs += temp;
        }

        servo_ID_and_POSITIONs= '{' + servo_ID_and_POSITIONs + '}';

        detail::send_cmd(port,servo_ID_and_POSITIONs);

        // std::this_thread::sleep_for(std::chrono::milliseconds(30));
        
        // std::cout<<servo_ID_and_POSITION<<std::endl;
        RCLCPP_INFO(rclcpp::get_logger("zl_servo"),servo_ID_and_POSITIONs.c_str());

        port.FlushInputBuffer();


    }

inline void get_multiply_servo_position(
    LibSerial::SerialPort& port,
    std::vector<std::pair<uint8_t,uint16_t>> & s, 
    uint8_t num = NUMBER_OF_SERVO
    )
        {
            std::string temp;
            size_t index;

            for (uint8_t i = 0; i < num; i++)
            {
                try{
                   
                    temp = test::get_single_servo_position(port, s[i].first);
                    index = temp.find('!');
                    
                   
                    if (index != std::string::npos && temp.length() >= index + 10) {
                        s[i].second = atoi(temp.substr(index+6,4).c_str());
                    }
                }
                catch(...)
                {
                    RCLCPP_WARN(rclcpp::get_logger("zl_servo"),"get_multiply_servo_position function error");
                }
            }
            port.FlushInputBuffer();
        }
  }
  
    
}

#endif
