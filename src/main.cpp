#include <uWS/uWS.h>
#include <iostream>
#include "json.hpp"
#include "PID.h"
#include <math.h>
#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
 
// for convenience
using json = nlohmann::json;

// For converting back and forth between radians and degrees.
constexpr double pi() { return M_PI; }
double deg2rad(double x) { return x * pi() / 180; }
double rad2deg(double x) { return x * 180 / pi(); }

// Checks if the SocketIO event has JSON data.
// If there is data the JSON object in string format will be returned,
// else the empty string "" will be returned.
std::string hasData(std::string s) {
  auto found_null = s.find("null");
  auto b1 = s.find_first_of("[");
  auto b2 = s.find_last_of("]");
  if (found_null != std::string::npos) {
    return "";
  }
  else if (b1 != std::string::npos && b2 != std::string::npos) {
    return s.substr(b1, b2 - b1 + 1);
  }
  return "";
}

int kbhit(void)
{
  struct termios oldt, newt;
  int ch;
  int oldf;
 
  tcgetattr(STDIN_FILENO, &oldt);
  newt = oldt;
  newt.c_lflag &= ~(ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &newt);
  oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
  fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);
 
  ch = getchar();
 
  tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
  fcntl(STDIN_FILENO, F_SETFL, oldf);
 
  if(ch != EOF)
  {
    ungetc(ch, stdin);
    return 1;
  }
 
  return 0;
}

int main()
{
  uWS::Hub h;

  PID pid;
  // TODO: Initialize the pid variable.
  pid.Init(0.075, 0.0001, 3.95);
  pid.throttle = 0.5;
  pid.step = 0.01;

  h.onMessage([&pid](uWS::WebSocket<uWS::SERVER> ws, char *data, size_t length, uWS::OpCode opCode) {
    // "42" at the start of the message means there's a websocket message event.
    // The 4 signifies a websocket message
    // The 2 signifies a websocket event
    if (length && length > 2 && data[0] == '4' && data[1] == '2')
    {
      auto s = hasData(std::string(data).substr(0, length));
      if (s != "") {
        auto j = json::parse(s);
        std::string event = j[0].get<std::string>();
        if (event == "telemetry") {
          // j[1] is the data JSON object
          double cte   = std::stod(j[1]["cte"].get<std::string>());
          //double speed = std::stod(j[1]["speed"].get<std::string>());
          //double angle = std::stod(j[1]["steering_angle"].get<std::string>());
          double steer_value;
          /*
          * TODO: Calcuate steering value here, remember the steering value is
          * [-1, 1].
          * NOTE: Feel free to play around with the throttle and speed. Maybe use
          * another PID controller to control the speed!
          */


          // keyboard input for parameter tuning
          char   key_code = 32; //space 
          
          if ( kbhit() )
          {
            key_code = getchar();

            switch (key_code)
            {
              case 'c':
                pid.step *= 10.0;
                break;
              case 'f':
                pid.step *= 0.1;
                break;
              case 'w':
                pid.throttle += 0.1;
                break;
              case 's':
                pid.throttle -= 0.1;
                break;
              case 'u': // t
                pid.Kp += pid.step;
                break;
              case 'j': // t
                pid.Kp -= pid.step;
                break;
              case 'i': // t
                pid.Ki += pid.step;
                break;
              case 'k': // t
                pid.Ki -= pid.step;
                break;
              case 'o': // t
                pid.Kd += pid.step;
                break;
              case 'l': // t
                pid.Kd -= pid.step;
                break;
              case 'r': // reset
                std::string msg = "42[\"reset\",{}]";
                ws.send(msg.data(), msg.length(), uWS::OpCode::TEXT);pid.p_error = 0.0;
                pid.p_error = 0.0;
                pid.i_error = 0.0;
                pid.d_error = 0.0;
            }
            

          }

          pid.UpdateError(cte);
          steer_value = pid.TotalError();
          // clamp to [-1, 1]
          steer_value = std::max(-1.0, std::min(steer_value, 1.0));
          // DEBUG
          std::cout << "CTE: " << cte << " Steering Value: " << steer_value
          << "\t Step: "<<pid.step<<"\t Throttle: "<<pid.throttle
          <<"\tKp: " << pid.Kp << " Ki: " << pid.Ki <<" Kd: " << pid.Kd <<std::endl;

          json msgJson;
          msgJson["steering_angle"] = steer_value;
          msgJson["throttle"] = pid.throttle;
          auto msg = "42[\"steer\"," + msgJson.dump() + "]";
          std::cout << msg << std::endl;
          ws.send(msg.data(), msg.length(), uWS::OpCode::TEXT);
        }
      } else {
        // Manual driving
        std::string msg = "42[\"manual\",{}]";
        ws.send(msg.data(), msg.length(), uWS::OpCode::TEXT);
      }
    }
  });

  // We don't need this since we're not using HTTP but if it's removed the program
  // doesn't compile :-(
  h.onHttpRequest([](uWS::HttpResponse *res, uWS::HttpRequest req, char *data, size_t, size_t) {
    const std::string s = "<h1>Hello world!</h1>";
    if (req.getUrl().valueLength == 1)
    {
      res->end(s.data(), s.length());
    }
    else
    {
      // i guess this should be done more gracefully?
      res->end(nullptr, 0);
    }
  });

  h.onConnection([&h](uWS::WebSocket<uWS::SERVER> ws, uWS::HttpRequest req) {
    std::cout << "Connected!!!" << std::endl;
  });

  h.onDisconnection([&h](uWS::WebSocket<uWS::SERVER> ws, int code, char *message, size_t length) {
    ws.close();
    std::cout << "Disconnected" << std::endl;
  });

  int port = 4567;
  if (h.listen(port))
  {
    std::cout << "Listening to port " << port << std::endl;
  }
  else
  {
    std::cerr << "Failed to listen to port" << std::endl;
    return -1;
  }
  h.run();
}
