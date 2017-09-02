#ifndef PID_H
#define PID_H


#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <algorithm>   


class PID {
public:
  /*
  * Errors
  */
  double p_error_;
  double i_error_;
  double d_error_;

  /*
  * Coefficients
  */ 
  double Kp_;
  double Ki_;
  double Kd_;


  bool controller_active_;
  double last_p_error_;

  std::ofstream data_file_;

  /*
  * Constructor
  */
  PID();

  double get_control_command();

  /*
  * Destructor.
  */
  virtual ~PID();

  /*
  * Initialize PID.
  */
  void Init();

  void Reset();

  void set_parameters(double Kp, double Ki, double Kd);

  double Symmetric_clip(double n, double bound);

  /*
  * Update the PID error variables given cross track error.
  */
  void UpdateError(double cte);

};

#endif /* PID_H */
