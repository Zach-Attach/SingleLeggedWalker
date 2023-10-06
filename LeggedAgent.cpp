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

double cx; // position along x axis?
double cy; // position along y axis?
double vx; // velocity along x axis

TLeg Leg;

double LeggedAgent::GetJointX() {return Leg.JointX;};
double LeggedAgent::GetJointY() {return Leg.JointY;};
double LeggedAgent::GetFootX() {return Leg.FootX;};
double LeggedAgent::GetFootY() {return Leg.FootY;};
bool LeggedAgent::GetFootState() {return Leg.FootState;};

double LeggedAgent::PositionX() {return cx;};
void LeggedAgent::SetPositionX(double newx) {cx = newx;};

// *******
// Control
// *******

double LeggedAgent::calculateTotalOutput(vector<int> neurons){
	double backwardOutput = 0.0;
	for (int i = 0; i < neurons.size(); i++) {
		backwardOutput += NervousSystem.NeuronOutput(neurons[i]);
	}
	return backwardOutput * MaxLegForce;
};

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
void resetHelper(double ix, double iy, int randomize){
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

// void LeggedAgent::Step(double StepSize, int forwardNeurons[], int forwardNeuronsSize, int backwardNeurons[], int backwardNeuronsSize)
void LeggedAgent::Step(double StepSize, vector<int> forwardNeurons, vector<int> backwardNeurons)
{	
	// Update the nervous system
	NervousSystem.EulerStep(StepSize);
	// Update the leg effectors
	if (NervousSystem.NeuronOutput(1) > 0.5) {
		Leg.FootState = 1; 
		Leg.Omega = 0;
	} else {
		Leg.FootState = 0;
	}

	Leg.ForwardForce = calculateTotalOutput(forwardNeurons);
	Leg.BackwardForce = calculateTotalOutput(backwardNeurons);

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


// Step the LeggedAgent using a 2-neuron CTRNN CPG

void LeggedAgent::Step2(double StepSize)
{
	double force = 0.0;
	
	// Update the nervous system
	NervousSystem.EulerStep(StepSize);
	// Update the leg effectors
	if (NervousSystem.NeuronOutput(1) > 0.5) {Leg.FootState = 1; Leg.Omega = 0;}
	else Leg.FootState = 0;
	Leg.ForwardForce = NervousSystem.NeuronOutput(1) * MaxLegForce; //1
	Leg.BackwardForce = NervousSystem.NeuronOutput(2) * MaxLegForce; //2
  double f = Leg.ForwardForce - Leg.BackwardForce;
	if (Leg.FootState == 1)
		if ((Leg.Angle >= BackwardAngleLimit && Leg.Angle <= ForwardAngleLimit) || 
		    (Leg.Angle < BackwardAngleLimit && f < 0) || 
		    (Leg.Angle > ForwardAngleLimit && f > 0))
		force = f;
	// Update the position of the body
	vx = vx + StepSize * force;
	if (vx < -MaxVelocity) vx = -MaxVelocity;
	if (vx > MaxVelocity) vx = MaxVelocity;
	cx = cx + StepSize * vx;
	// Update the leg geometry
	Leg.JointX = Leg.JointX + StepSize * vx;
	if (Leg.FootState == 1) {
		double angle = atan2(Leg.FootX - Leg.JointX,Leg.FootY - Leg.JointY);
		Leg.Omega = (angle - Leg.Angle)/StepSize;
		Leg.Angle = angle;
	}
	else {
		vx = 0.0;
		Leg.Omega	= Leg.Omega + StepSize * MaxTorque * (Leg.BackwardForce - Leg.ForwardForce);
		if (Leg.Omega < -MaxOmega) Leg.Omega = -MaxOmega;
		if (Leg.Omega > MaxOmega) Leg.Omega = MaxOmega;
		Leg.Angle = Leg.Angle + StepSize * Leg.Omega;
		if (Leg.Angle < BackwardAngleLimit) {Leg.Angle = BackwardAngleLimit; Leg.Omega = 0;}
		if (Leg.Angle > ForwardAngleLimit) {Leg.Angle = ForwardAngleLimit; Leg.Omega = 0;}
		UpdateFoot();
	}
	// If the foot is too far back, the body becomes "unstable" and forward motion ceases
	if (cx - Leg.FootX > 20) vx = 0.0;
}

void LeggedAgent::Step2RPG(double StepSize)
{
	double force = 0.0;

	// Update the sensory input
	for (int i = 1; i <= NervousSystem.CircuitSize(); i++){
		NervousSystem.SetNeuronExternalInput(i, Leg.Angle * 5.0/ForwardAngleLimit); // NOTE: Why 5?
	}
	
	// Update the nervous system
	NervousSystem.EulerStep(StepSize);
	// Update the leg effectors
	if (NervousSystem.NeuronOutput(1) > 0.5) {Leg.FootState = 1; Leg.Omega = 0;}
	else Leg.FootState = 0;
	Leg.ForwardForce = NervousSystem.NeuronOutput(1) * MaxLegForce;
	Leg.BackwardForce = NervousSystem.NeuronOutput(2) * MaxLegForce;
    double f = Leg.ForwardForce - Leg.BackwardForce;
	if (Leg.FootState == 1)
		if ((Leg.Angle >= BackwardAngleLimit && Leg.Angle <= ForwardAngleLimit) || 
		    (Leg.Angle < BackwardAngleLimit && f < 0) || 
		    (Leg.Angle > ForwardAngleLimit && f > 0))
		force = f;
	// Update the position of the body
	vx = vx + StepSize * force;
	if (vx < -MaxVelocity) vx = -MaxVelocity;
	if (vx > MaxVelocity) vx = MaxVelocity;
	cx = cx + StepSize * vx;
	// Update the leg geometry
	Leg.JointX = Leg.JointX + StepSize * vx;
	if (Leg.FootState == 1) {
		double angle = atan2(Leg.FootX - Leg.JointX,Leg.FootY - Leg.JointY);
		Leg.Omega = (angle - Leg.Angle)/StepSize;
		Leg.Angle = angle;
	}
	else {
		vx = 0.0;
		Leg.Omega	= Leg.Omega + StepSize * MaxTorque * (Leg.BackwardForce - Leg.ForwardForce);
		if (Leg.Omega < -MaxOmega) Leg.Omega = -MaxOmega;
		if (Leg.Omega > MaxOmega) Leg.Omega = MaxOmega;
		Leg.Angle = Leg.Angle + StepSize * Leg.Omega;
		if (Leg.Angle < BackwardAngleLimit) {Leg.Angle = BackwardAngleLimit; Leg.Omega = 0;}
		if (Leg.Angle > ForwardAngleLimit) {Leg.Angle = ForwardAngleLimit; Leg.Omega = 0;}
		UpdateFoot();
	}
	// If the foot is too far back, the body becomes "unstable" and forward motion ceases
	if (cx - Leg.FootX > 20) vx = 0.0;
}


// Step the LeggedAgent using a 1-neuron CTRNN CPG

void LeggedAgent::Step1(double StepSize)
{
	double force = 0.0;
	
	// Update the sensory input
	NervousSystem.SetNeuronExternalInput(1,Leg.Angle * 5.0/ForwardAngleLimit);
	// Update the nervous system
	NervousSystem.EulerStep(StepSize);
	double o = NervousSystem.NeuronOutput(1);
	// Update the leg effectors
	if (o > 0.5) {
		Leg.FootState = 1; 
		Leg.Omega = 0;
		Leg.ForwardForce = 2 * (o - 0.5) * MaxLegForce;
	}
	else {
		Leg.FootState = 0;
		Leg.BackwardForce = 2 * (0.5 - o) * MaxLegForce;
	}
	// Compute the force applied to the body (*** USING THE "NEW" MODEL ***)
	double f = Leg.ForwardForce - Leg.BackwardForce;
	if (Leg.FootState == 1)
		if ((Leg.Angle >= BackwardAngleLimit && Leg.Angle <= ForwardAngleLimit) || 
		    (Leg.Angle < BackwardAngleLimit && f < 0) || 
		    (Leg.Angle > ForwardAngleLimit && f > 0))
		force = f;
	// Update the position of the body
	vx = vx + StepSize * force;
	if (vx < -MaxVelocity) vx = -MaxVelocity;
	if (vx > MaxVelocity) vx = MaxVelocity;
	cx = cx + StepSize * vx;
	// Update the leg geometry
	Leg.JointX = Leg.JointX + StepSize * vx;
	if (Leg.FootState == 1) {
		double angle = atan2(Leg.FootX - Leg.JointX,Leg.FootY - Leg.JointY);
		Leg.Omega = (angle - Leg.Angle)/StepSize;
		Leg.Angle = angle;
	}
	else {
		vx = 0.0;
		Leg.Omega	= Leg.Omega + StepSize * MaxTorque * (Leg.BackwardForce - Leg.ForwardForce);
		if (Leg.Omega < -MaxOmega) Leg.Omega = -MaxOmega;
		if (Leg.Omega > MaxOmega) Leg.Omega = MaxOmega;
		Leg.Angle = Leg.Angle + StepSize * Leg.Omega;
		if (Leg.Angle < BackwardAngleLimit) {Leg.Angle = BackwardAngleLimit; Leg.Omega = 0;}
		if (Leg.Angle > ForwardAngleLimit) {Leg.Angle = ForwardAngleLimit; Leg.Omega = 0;}
		UpdateFoot();
	}
	// If the foot is too far back, the body becomes "unstable" and forward motion ceases
	if (cx - Leg.FootX > 20) vx = 0.0;
}


// Step the LeggedAgent using the optimal pattern generator

void LeggedAgent::PerfectStep(double StepSize)
{
	double force = 0.0;

	// Update the leg effectors
	if (Leg.FootState == 0 && Leg.Angle >= ForwardAngleLimit) {Leg.FootState = 1; Leg.Omega = 0;}
	else if (Leg.FootState == 1 && (cx - Leg.FootX > 20)) Leg.FootState = 0;
	// Compute the force applied to the body
	if (Leg.FootState == 1 && Leg.Angle >= BackwardAngleLimit && Leg.Angle <= ForwardAngleLimit)
		force = MaxLegForce;
	// Update the position of the body
	vx = vx + StepSize * force;
	if (vx < -MaxVelocity) vx = -MaxVelocity;
	if (vx > MaxVelocity) vx = MaxVelocity;
	cx = cx + StepSize * vx;
	// Update the leg geometry
	Leg.JointX = Leg.JointX + StepSize * vx;
	if (Leg.FootState == 1) {
		double angle = atan2(Leg.FootX - Leg.JointX,Leg.FootY - Leg.JointY);
		Leg.Omega = (angle - Leg.Angle)/StepSize;
		Leg.Angle = angle;
	}
	else {
		vx = 0.0;
		Leg.Omega	= Leg.Omega + StepSize * MaxTorque * MaxLegForce;
		if (Leg.Omega < -MaxOmega) Leg.Omega = -MaxOmega;
		if (Leg.Omega > MaxOmega) Leg.Omega = MaxOmega;
		Leg.Angle = Leg.Angle + StepSize * Leg.Omega;
		if (Leg.Angle < BackwardAngleLimit) {Leg.Angle = BackwardAngleLimit; Leg.Omega = 0;}
		if (Leg.Angle > ForwardAngleLimit) {Leg.Angle = ForwardAngleLimit; Leg.Omega = 0;}
		UpdateFoot();
	}
	// If the foot is too far back, the body becomes "unstable" and forward motion ceases
	if (cx - Leg.FootX > 20) vx = 0.0;
}
