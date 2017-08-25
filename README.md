# PID Controller for Vehicle Control

In this project a PID controller is used to calculate steering inputs for a virtual vehicle driving on a simulated test track.
The error metric used for the PID controller is the cross track error (cte) which is defined as the shortest distance between the test track center line and the vehicles current position (referring to the mid point of the vehicles rear axle).


## PID Parameter Tuning

In order to safely guide the vehicle along the test track, appropriate values vor the P-, I- and D-gain values have to be chosen.

### Influence of Integral Term

In principal choosing a large I-gain will often lead to oscillations around the target control point, while an appropriately chosen I-gain will help to counteract permanent deviations from the desired target point (resulting from e.g. from changing environmental factors).
As in this project the PID controller is tested in a simulated environment, there are no factors present which might lead to such a permanent deviation.
The I-gain is therefore chosen to be zero.

### PD Tuning

Using only a P-controller either leads to overshooting the target point and subsequent oscillations or to never reaching the target point at all.
Using only a D-controller does not make sense at all, as this controller would only react on changes in the error signal but never care for the actual value of the error signal. 

The combination of P- and D-




