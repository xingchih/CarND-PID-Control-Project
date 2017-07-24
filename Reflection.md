# CarND-Controls-PID
Self-Driving Car Engineer Nanodegree Program

---

## Reflection
### Describe the effect each of the P, I, D components had in your implementation.

P is the proportional term, which determines how "fast" the the car would steer when it deviates from its norminal course. A high gain will enable fast response but also tend to overshoot and lead to oscillation.  

I is the integral term. It eliminates the systematic bias in the system. A small I gain is sufficient to remove the steering bias while a large value can cause the car the "drift" off the track.

D is the derivative term, which reacts to the change rate of the error term. It restrains the system from reacting too fast or too agressively. Intuitively, this works like a "damper" that enables a smooth steering. 

### Describe how the final hyperparameters were chosen.
The hyperparameters are chosen manually. Keyboard inputs are used to allow user change the parameter Kp, Ki, Kd with adjustable steps as well as increase and decrease throttle with 0.1 steps.

Depends on the throttle or the speed of the car, the optimal PID tuning also varies. At a lower speed, a high gain controller could be used since the car have a higher crossover frequency. However, with a higher speed, Kp has to be detuned in order to keep the system stable with enough stability margin. For example, the car can drive around the track with a PID tuning of [Kp, Ki, Kd] = [0.15, 0.0001, 3.75] and throttle at 0.3. The avarage speed is about 35 MPH. However, if we increase the throttle to 0.7 or 0.8, the car can reach over 80 MPH and previous controller tuning will lead to large oscillation and eventually throw the car off the track. The final parameters are chosen as [Kp, Ki, Kd] = [0.075, 0.0001, 3.95], which allow the car stay on the track with throttle under 0.8 (speed < 80 MPH).

Then I added a second PID controller for throttle and the input is a weighted sum of CTE and steering angle, which is fabs(cte)-0.5 + 0.01*fabs(angle)-0.5, i.e., if we deviate from nominal steering or the steering angle is large, we need to reduce speed. Offsets are added for a relatively aggresive driving behavior. We also offset the pre-clamped throttle by 1.25 so that most of the time the car is just on full throttle. The steering PID has been slightly modified to [Kp, Ki, Kd] = [0.08, 0.0001, 3.95] and the throttle PID is also manually tuned as [Kp, Ki, Kd] = [0.15, 1e-7, 15.5].