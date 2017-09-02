#pragma once

#include <vector>
#include <iostream>
#include <limits>
#include <algorithm>  
#include <math.h>


class TwiddleOptimizer {
public:
  int num_params_, num_sim_steps_, num_skip_steps_;
  bool request_sim_reset_;
  std::vector<double> parameters_;

  TwiddleOptimizer();

  void init(std::vector<double> parameters, std::vector<double> dp, int num_sim_steps, int num_skip_steps);
  void update_sim_error(double error);  

private:
  std::vector<double> sim_errors_, dp_;
  double best_error_;
  int sim_step_count_;
  int p_index_;
  bool increased_param_value;

  double calculate_error_metric();
  void update_parameters();

};