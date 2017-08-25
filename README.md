# PID Controller for Vehicle Control

In this project a PID controller is used to calculate steering inputs for a vehicle driving on a simulated test track.
The error metric used for the PID controller is the cross track error (cte) which is defined to be the shortest distance between the test track center line and the vehicles current position (referring to the mid point of the vehicles rear axle).

Describe the effect each of the P, I, D components had in your implementation.

Student describes the effect of the P, I, D component of the PID algorithm in their implementation. Is it what you expected?
Visual aids are encouraged, i.e. record of a small video of the car in the simulator and describe what each component is set to.

## PID Parameter Tuning

In order to safely guide the vehicle along the test track, appropriate values vor the P-, I- and D-gain values have to be chosen.

### Influence of I-Term

The integral (I) term of the PID controller is mainly used to counteract permanent deviations from the desired target control point.

As the PID controller is tested in a simulated environment, such .. are barely present.

### PD Tuning




