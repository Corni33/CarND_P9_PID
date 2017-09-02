#include "PID.h"

using namespace std;


PID::PID() {
    Init();
}

PID::~PID() {}

void PID::Init() 
{
  Reset();

  auto t = std::time(nullptr);
  auto tm = *std::localtime(&t);

  std::ostringstream oss;
  oss << std::put_time(&tm, "%Y%m%d_%H-%M-%S");
  auto str = oss.str();

  data_file_.open(str + ".csv");
}

void PID::set_parameters(double Kp, double Ki, double Kd)
{
  this->Kp_ = Kp; 
  this->Ki_ = Ki;
  this->Kd_ = Kd; 
}

void PID::Reset() 
{  
  this->i_error_ = 0.0;
  this->controller_active_ = false;
}

double PID::Symmetric_clip(double n, double bound) {
  return std::max(-bound, std::min(n, bound));
}

void PID::UpdateError(double cte) 
{

  p_error_ = -cte;

  i_error_ += p_error_;
  i_error_ = Symmetric_clip(i_error_, 1.0); //prevent accumulation of too big i_error values

  if (controller_active_)
  {    
    d_error_ = p_error_ - last_p_error_;    
  }
  else
  {
    d_error_ = 0.0;    
    controller_active_ = true;
  }

  last_p_error_ = p_error_;
}

double PID::get_control_command() 
{ 
  double control_command = Kp_*p_error_ + Ki_*i_error_ + Kd_*d_error_;

  data_file_ << p_error_ << "," << i_error_ << "," << d_error_ << "," << control_command << "\n";
  data_file_.flush();

  return control_command;
}


