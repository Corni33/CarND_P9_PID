#include <uWS/uWS.h>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <chrono>
#include <ctime>
#include "json.hpp"
#include "PID.h"
#include "TwiddleOptimizer.h"
#include <math.h>

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

int main(int argc, char* argv[])
{

  enum Mode {record, fit, tune, control};
  std::ofstream drive_data_file;
  auto last_time = std::chrono::duration_cast< std::chrono::milliseconds >(
    std::chrono::system_clock::now().time_since_epoch()).count();

  int mode = Mode::control;
 
  size_t optind;
  for (optind = 1; optind < argc && argv[optind][0] == '-'; optind++) 
  {
    switch (argv[optind][1]) 
    {
      //case 'r': mode = Mode::record; break;
      //case 'f': mode = Mode::fit; break;
      case 't': mode = Mode::tune; break;
      default:
        fprintf(stderr, "Usage: %s [-rtf] [file...]\n", argv[0]);
        exit(EXIT_FAILURE);
    }
  }  

  PID pid;   
  TwiddleOptimizer twiddle; 

  if (mode == Mode::tune)  
  {    
    std::vector<double> twiddle_parameters{ 0.15, 3.0 };
    std::vector<double> twiddle_dp{ 0.03,  0.2 };

    twiddle.init(twiddle_parameters, twiddle_dp, 3000, 800);

    pid.set_parameters(twiddle_parameters[0], 0.0, twiddle_parameters[1]);
  }
  else
  {        
    pid.set_parameters(0.15, 0.0, 3.0); // manual tuning
    //pid.set_parameters(0.24, 0.0, 3.2); // after long twiddle session
  }

  uWS::Hub h;

  h.onMessage([&pid,&mode,&drive_data_file,&last_time,&twiddle](uWS::WebSocket<uWS::SERVER> ws, char *data, size_t length, uWS::OpCode opCode) {
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
          double cte = std::stod(j[1]["cte"].get<std::string>());
          double speed = std::stod(j[1]["speed"].get<std::string>());
          double angle = std::stod(j[1]["steering_angle"].get<std::string>());
          double steer_value;

          pid.UpdateError(cte);

          steer_value = pid.get_control_command();

          switch (mode) {
            case Mode::tune:
            {
              twiddle.update_sim_error(cte);

              if (twiddle.request_sim_reset_)
              {
                pid.set_parameters(twiddle.parameters_[0], 0.0, twiddle.parameters_[1]);

                // send reset message to simulator
                std::string msg = "42[\"reset\",{}]";
                ws.send(msg.data(), msg.length(), uWS::OpCode::TEXT);
              }
            }

            case Mode::control:
            {
              json msgJson;
              msgJson["steering_angle"] = steer_value;
              msgJson["throttle"] = 0.4;
              auto msg = "42[\"steer\"," + msgJson.dump() + "]";
              //std::cout << msg << std::endl;
              ws.send(msg.data(), msg.length(), uWS::OpCode::TEXT);
            }
          }

          // DEBUG
          //std::cout << "CTE: " << cte << " Steering Value: " << steer_value << std::endl;

        }
      } else {
        // Manual driving
        pid.Reset();

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
