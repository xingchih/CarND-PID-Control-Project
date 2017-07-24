#include "PID.h"

using namespace std;

/*
* TODO: Complete the PID class.
*/

PID::PID() {}

PID::~PID() {}

void PID::Init(double Kp, double Ki, double Kd) {
    this->Kp = Kp;
    this->Ki = Ki;
    this->Kd = Kd;

    this->p_error = 0;
    this->i_error = 0;
    this->d_error = 0;
}

void PID::UpdateError(double cte) {
    
    this->d_error = cte - this->p_error;
    this->p_error = cte;
    this->i_error += cte;
}

double PID::TotalError() {
    double output;
    output = -(Kp*p_error + Kd*d_error + Ki*i_error);
    return output;
}

