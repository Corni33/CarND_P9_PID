#include "TwiddleOptimizer.h"


TwiddleOptimizer::TwiddleOptimizer() {}

void TwiddleOptimizer::init(std::vector<double> parameters, std::vector<double> dp, int num_sim_steps, int num_skip_steps)
{
  parameters_ = parameters;
  dp_ = dp;
  num_sim_steps_ = num_sim_steps;
  num_skip_steps_ = num_skip_steps;  

  sim_errors_.assign(num_sim_steps, 0.0);

  best_error_ = std::numeric_limits<double>::max();
  request_sim_reset_ = false;
  sim_step_count_ = 0;
  p_index_ = -1;
  increased_param_value = true;
} 

double TwiddleOptimizer::calculate_error_metric()
{
  double rmse = 0.0;

  for (int i = num_skip_steps_; i < num_sim_steps_ - 1; ++i)
  {
    rmse += sim_errors_[i] * sim_errors_[i];
  }    
 
  rmse = sqrt(rmse / (num_sim_steps_ - num_skip_steps_));
 
  return rmse;

}

void TwiddleOptimizer::update_sim_error(double error)
{
  ++sim_step_count_;
  request_sim_reset_ = false;

  //std::cout << sim_step_count_ << std::endl;

  if (sim_step_count_ < num_sim_steps_)
  {
    sim_errors_[sim_step_count_] = error;
  }
  else //current simulation run has finished
  {
    update_parameters();    
  }

}

void TwiddleOptimizer::update_parameters()
{
  double sim_run_error = calculate_error_metric();

  std::cout << "twiddle parameters: (updating parameter " << p_index_ << ", error = " << sim_run_error << ") \n";
  for (int i = 0; i < parameters_.size(); ++i)
  {
    std::cout << parameters_[i] << " (" << dp_[i] << ") \n";
  }

  if (p_index_ == -1) // first simulation run
  {
    best_error_ = sim_run_error;
    p_index_ = 0;
    parameters_[0] += dp_[0];
    increased_param_value = true; // could also be called: outer_loop = true/false

    // --> run simulation
  }
  else 
  {
    if (sim_run_error < best_error_)
    {
      best_error_ = sim_run_error;
      dp_[p_index_] *= 1.1;
            
      p_index_ = (p_index_ + 1) % parameters_.size();
      parameters_[p_index_] += dp_[p_index_];  
      increased_param_value = true;

      // --> run simulation           
    }
    else
    {
      if (increased_param_value)
      {
        parameters_[p_index_] -= 2 * dp_[p_index_];
        increased_param_value = false;

        // --> run simulation
      }
      else
      {
        parameters_[p_index_] += dp_[p_index_];
        dp_[p_index_] *= 0.8;

        p_index_ = (p_index_ + 1) % parameters_.size();
        parameters_[p_index_] += dp_[p_index_];
        increased_param_value = true;

        // --> run simulation
      }
    }
  }  

  request_sim_reset_ = true;
  sim_step_count_ = 0;
}