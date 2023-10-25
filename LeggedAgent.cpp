// ***********************
// Methods for LeggedAgent
//
// RDB 2/16/96
// ***********************

#include "LeggedAgent.h"
#include "random.h"

// Constants
static const double Pi = 3.1415926;

static const int    LegLength = 15;
static const double MaxLegForce = 0.05;
static const double ForwardAngleLimit = Pi/6;
static const double BackwardAngleLimit = -Pi/6;
static const double MaxVelocity = 6.0;
static const double MaxTorque = 0.5;
static const double MaxOmega = 1.0;

vector<int> FootMotorNeurons = {};
vector<int> ForwardMotorNeurons = {};
vector<int> BackwardMotorNeurons = {};
vector<int> FootSensorNeurons = {};
vector<int> AngleSensorNeurons = {};

TLeg Leg;

double LeggedAgent::GetJointX() {return Leg.JointX;};
double LeggedAgent::GetJointY() {return Leg.JointY;};
double LeggedAgent::GetFootX() {return Leg.FootX;};
double LeggedAgent::GetFootY() {return Leg.FootY;};
bool LeggedAgent::GetFootState() {return Leg.FootState;};

// *******
// Control
// *******

double LeggedAgent::SumOutput(vector<int> neurons){
	double output = 0.0;
	for (int i = 0; i < neurons.size(); i++) {
		if (neurons[i] > 0) {
			output += NervousSystem.NeuronOutput(neurons[i]);
		} else if (neurons[i] < 0) {
			output -= NervousSystem.NeuronOutput(-neurons[i]);
		}
	}
	return output;
};

void LeggedAgent::SensoryUpdate(vector<int> neurons, double stimulus){
	for (int i = 0; i < neurons.size(); i++) {
		if (neurons[i] > 0) 
			NervousSystem.SetNeuronExternalInput(neurons[i], stimulus);
		else if (neurons[i] < 0)
			NervousSystem.SetNeuronExternalInput(-neurons[i], -stimulus);
	}
}

void UpdateFoot(){
	Leg.FootX = Leg.JointX + LegLength * sin(Leg.Angle);
	Leg.FootY = Leg.JointY + LegLength * cos(Leg.Angle);
};

double cap(double value, double min, double max){
	if (value < min) return min;
	else if (value > max) return max;
	else return value;
}

// Reset the state of the agent
void LeggedAgent::resetHelper(double ix, double iy, int randomize){
	cx = ix;
	cy = iy;
	vx = 0.0;
	Leg.FootState = 0;
	Leg.Angle = randomize ? UniformRandom(BackwardAngleLimit,ForwardAngleLimit) : ForwardAngleLimit;

	Leg.Omega = Leg.ForwardForce = Leg.BackwardForce = 0.0;
	Leg.JointX = cx;
	Leg.JointY = cy + 12.5;
	UpdateFoot();
}

bool safeMove(){
	return ((Leg.Angle >= BackwardAngleLimit && Leg.Angle <= ForwardAngleLimit) || 
		(Leg.Angle < BackwardAngleLimit && Leg.ForwardForce < Leg.BackwardForce) || 
		(Leg.Angle > ForwardAngleLimit && Leg.ForwardForce > Leg.BackwardForce));
}

void LeggedAgent::SetMotorCouplings(vector<int> foot, vector<int> forward, vector<int> backward){ // TOD: does this need to be in header?
	FootMotorNeurons = foot;
	ForwardMotorNeurons = forward;
	BackwardMotorNeurons = backward;
}

void LeggedAgent::SetSensorCouplings(vector<int> foot, vector<int> angle){ // TODO: does this need to be in header?
	FootSensorNeurons = foot;
	AngleSensorNeurons = angle;
}

void LeggedAgent::Reset(double ix, double iy, int randomize)
{
	resetHelper(ix, iy, randomize);
	if (randomize) NervousSystem.RandomizeCircuitState(-0.1,0.1);
	else NervousSystem.RandomizeCircuitState(0.0,0.0);	
}

void LeggedAgent::Reset(double ix, double iy, int randomize, RandomState &rs)
{
	resetHelper(ix, iy, randomize);
	if (randomize) NervousSystem.RandomizeCircuitState(-0.1,0.1,rs);
	else NervousSystem.RandomizeCircuitState(0.0,0.0,rs);	
}

// Step the insect using a general CTRNN CPG
void LeggedAgent::Step(double StepSize)
{
	SensoryUpdate(AngleSensorNeurons, Leg.Angle * 5.0/ForwardAngleLimit);
	SensoryUpdate(FootSensorNeurons, 5.0*Leg.FootState); // TODO: Figure out if it should be scaled by 5 like the other or what
	// Update the nervous system
	NervousSystem.EulerStep(StepSize);
	// Update the leg effectors
	if (SumOutput(FootMotorNeurons) > 0.5) {
		Leg.FootState = 1; 
		Leg.Omega = 0;
	} else {
		Leg.FootState = 0;
	}

	Leg.ForwardForce = MaxLegForce * SumOutput(ForwardMotorNeurons);
	Leg.BackwardForce = MaxLegForce * SumOutput(BackwardMotorNeurons);

		// Compute the force applied to the body
	double force = (Leg.FootState == 1 && safeMove()) ? Leg.ForwardForce - Leg.BackwardForce : 0.0;

	// Update the position of the body
	vx = cap(vx + StepSize * force, -MaxVelocity, MaxVelocity);
	cx += StepSize * vx;

	// Update the leg geometry
	Leg.JointX = cx;
	if (Leg.FootState == 1) {
		double angle = atan2(Leg.FootX - Leg.JointX, Leg.FootY - Leg.JointY);
		Leg.Omega = (angle - Leg.Angle)/StepSize;
		Leg.Angle = angle;
	} else {
		vx = 0.0;
		Leg.Omega	+= StepSize * MaxTorque * (Leg.BackwardForce - Leg.ForwardForce);
		Leg.Omega = cap(Leg.Omega, -MaxOmega, MaxOmega);
		Leg.Angle += StepSize * Leg.Omega;
		if (Leg.Angle < BackwardAngleLimit) {
			Leg.Angle = BackwardAngleLimit; 
			Leg.Omega = 0;
		} else if (Leg.Angle > ForwardAngleLimit) {
			Leg.Angle = ForwardAngleLimit; 
			Leg.Omega = 0;
		}
		UpdateFoot();
	}

	// If the foot is too far back, the body becomes "unstable" and forward motion ceases
	if (cx - Leg.FootX > 20) vx = 0.0;
	}