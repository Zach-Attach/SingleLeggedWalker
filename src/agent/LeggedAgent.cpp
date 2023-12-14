// ***********************
// Methods for LeggedAgent
//
// RDB 2/16/96
// ***********************

#include "LeggedAgent.h"
#include "random.h"
#include<vector>

// Constants

const int    LegLength = 15;
const double MaxLegForce = 0.05;
const double ForwardAngleLimit = Pi/6;
const double BackwardAngleLimit = -Pi/6;
const double MaxVelocity = 6.0;
const double MaxTorque = 0.5;
const double MaxOmega = 1.0;

std::vector<int> FootMotorNeurons = {};
std::vector<int> ForwardMotorNeurons = {};
std::vector<int> BackwardMotorNeurons = {};
std::vector<int> FootSensorNeurons = {};
std::vector<int> AngleSensorNeurons = {};


TLeg Leg;

double LeggedAgent::GetJointX() {return Leg.JointX;};
double LeggedAgent::GetJointY() {return Leg.JointY;};
double LeggedAgent::GetFootX() {return Leg.FootX;};
double LeggedAgent::GetFootY() {return Leg.FootY;};
bool LeggedAgent::GetFootState() {return Leg.FootState;};

// *******
// Control
// *******

double cap(double value, double min, double max){
	if (value < min) return min;
	else if (value > max) return max;
	else return value;
}

bool safeMove(){
	return ((Leg.Angle >= BackwardAngleLimit && Leg.Angle <= ForwardAngleLimit) || 
		(Leg.Angle < BackwardAngleLimit && Leg.ForwardForce < Leg.BackwardForce) || 
		(Leg.Angle > ForwardAngleLimit && Leg.ForwardForce > Leg.BackwardForce));
}

void LeggedAgent::SetMotorCouplings(std::vector<int> foot, std::vector<int> forward, std::vector<int> backward){ // TOD: does this need to be in header?
	FootMotorNeurons = foot;
	ForwardMotorNeurons = forward;
	BackwardMotorNeurons = backward;
}

void LeggedAgent::SetSensorCouplings(std::vector<int> foot, std::vector<int> angle){ // TODO: does this need to be in header?
	FootSensorNeurons = foot;
	AngleSensorNeurons = angle;
}

void LeggedAgent::Reset(double ix, double iy, int randomize)
{
	cx = ix; cy = iy; vx = 0.0;
	Leg.FootState = 0;
	if (randomize) Leg.Angle = UniformRandom(BackwardAngleLimit,ForwardAngleLimit);
	else Leg.Angle = ForwardAngleLimit;
	Leg.Omega = Leg.ForwardForce = Leg.BackwardForce = 0.0;
	Leg.JointX = cx; Leg.JointY = cy + 12.5;
	Leg.FootX = Leg.JointX + LegLength * sin(Leg.Angle);
	Leg.FootY = Leg.JointY + LegLength * cos(Leg.Angle);
	if (randomize) NervousSystem.RandomizeCircuitState(-0.1,0.1);
	else NervousSystem.RandomizeCircuitState(0.0,0.0);	
}

void LeggedAgent::Reset(double ix, double iy, int randomize, RandomState &rs)
{
	cx = ix; cy = iy; vx = 0.0;
	Leg.FootState = 0;
	if (randomize) Leg.Angle = rs.UniformRandom(BackwardAngleLimit,ForwardAngleLimit);
	else Leg.Angle = ForwardAngleLimit;
	Leg.Omega = Leg.ForwardForce = Leg.BackwardForce = 0.0;
	Leg.JointX = cx; Leg.JointY = cy + 12.5;
	Leg.FootX = Leg.JointX + LegLength * sin(Leg.Angle);
	Leg.FootY = Leg.JointY + LegLength * cos(Leg.Angle);
	if (randomize) NervousSystem.RandomizeCircuitState(-0.1,0.1,rs);
	else NervousSystem.RandomizeCircuitState(0.0,0.0,rs);	
}

// Step the insect using a general CTRNN CPG
void LeggedAgent::Step(double StepSize)
{
	cout << cx << endl;
	// cout << Leg.FootState << endl;
	// cout << Leg.Angle << " " << Leg.Omega << endl;
	// std::vector<int> inputs;
	TVector<double> inputs;
	inputs.SetBounds(1, NervousSystem.CircuitSize());
	// inputs.assign(NervousSystem.CircuitSize(), 0);

	if (!AngleSensorNeurons.empty()){
		double stimA = Leg.Angle * angleWeight/ForwardAngleLimit;

		for (int i = 0; i < AngleSensorNeurons.size(); i++) {
			if (AngleSensorNeurons[i] > 0) 
				inputs(AngleSensorNeurons[i]) = stimA;
			else if (AngleSensorNeurons[i] < 0)
				inputs(-AngleSensorNeurons[i]) = -stimA;
		}
	}

	if (!FootSensorNeurons.empty()){
		double stimF = footWeight*Leg.FootState;

		for (int i = 0; i < FootSensorNeurons.size(); i++) {
			if (FootSensorNeurons[i] > 0) 
				inputs(FootSensorNeurons[i]) += stimF;
			else if (FootSensorNeurons[i] < 0)
				inputs(-FootSensorNeurons[i]) -= stimF;
		}
	}

	for (int i = 1; i <= inputs.Size(); i++) {
		// cout << inputs(i) << " ";
		NervousSystem.SetNeuronExternalInput(i, inputs(i));
	}
	// cout << endl;


	// Update the nervous system
	NervousSystem.EulerStep(StepSize);

	double footOutput = 0.0;
	if (!FootMotorNeurons.empty()){
		for (int i = 0; i < FootMotorNeurons.size(); i++) {
			if (FootMotorNeurons[i] > 0) {
				footOutput += NervousSystem.NeuronOutput(FootMotorNeurons[i]);
			} else if (FootMotorNeurons[i] < 0) {
				footOutput -= NervousSystem.NeuronOutput(-FootMotorNeurons[i]);
			}
		}
	}

	// Update the leg effectors
	if (footOutput > 0.5) {
		Leg.FootState = 1; 
		Leg.Omega = 0;
	} else {
		Leg.FootState = 0;
	}

	double forwardOutput = 0.0;
	
	if (!ForwardMotorNeurons.empty()){
		for (int i = 0; i < ForwardMotorNeurons.size(); i++) {
			if (ForwardMotorNeurons[i] > 0) {
				forwardOutput += NervousSystem.NeuronOutput(ForwardMotorNeurons[i]);
			} else if (ForwardMotorNeurons[i] < 0) {
				forwardOutput -= NervousSystem.NeuronOutput(-ForwardMotorNeurons[i]);
			}
		}
	}

	double backwardOutput = 0.0;
	if (!BackwardMotorNeurons.empty()){
		for (int i = 0; i < BackwardMotorNeurons.size(); i++) {
			if (BackwardMotorNeurons[i] > 0) {
				backwardOutput += NervousSystem.NeuronOutput(BackwardMotorNeurons[i]);
			} else if (BackwardMotorNeurons[i] < 0) {
				backwardOutput -= NervousSystem.NeuronOutput(-BackwardMotorNeurons[i]);
			}
		}
	}

	Leg.ForwardForce = MaxLegForce * forwardOutput;
	Leg.BackwardForce = MaxLegForce * backwardOutput;

		// Compute the force applied to the body
	double force = (Leg.FootState == 1 && ((Leg.Angle >= BackwardAngleLimit && Leg.Angle <= ForwardAngleLimit) || 
		(Leg.Angle < BackwardAngleLimit && Leg.ForwardForce < Leg.BackwardForce) || 
		(Leg.Angle > ForwardAngleLimit && Leg.ForwardForce > Leg.BackwardForce))) ? Leg.ForwardForce - Leg.BackwardForce : 0.0;

	// Update the position of the body
	double value = vx + StepSize * force;
	double min = -MaxVelocity;
	double max = MaxVelocity;
	if (value < min) vx = min;
	else if (value > max) vx = max;
	else vx = value;
	cx += StepSize * vx;

	// Update the leg geometry
	Leg.JointX = Leg.JointX + StepSize * vx;
	if (Leg.FootState == 1.0) {
		double angle = atan2(Leg.FootX - Leg.JointX,Leg.FootY - Leg.JointY);
		Leg.Omega = (angle - Leg.Angle)/StepSize;
		Leg.Angle = angle;
	} else {
		vx = 0.0;
		Leg.Omega	+= StepSize * MaxTorque * (Leg.BackwardForce - Leg.ForwardForce);
		value = Leg.Omega;
		min = -MaxOmega;
		max = MaxOmega;
		if (value < min) Leg.Omega = min;
		else if (value > max) Leg.Omega = max;
		else Leg.Omega = value;
		Leg.Angle += StepSize * Leg.Omega;
		if (Leg.Angle < BackwardAngleLimit) {
			Leg.Angle = BackwardAngleLimit; 
			Leg.Omega = 0;
		} else if (Leg.Angle > ForwardAngleLimit) {
			Leg.Angle = ForwardAngleLimit; 
			Leg.Omega = 0;
		}
		Leg.FootX = Leg.JointX + LegLength * sin(Leg.Angle);
		Leg.FootY = Leg.JointY + LegLength * cos(Leg.Angle);
	}

	// If the foot is too far back, the body becomes "unstable" and forward motion ceases
	if (cx - Leg.FootX > 20) vx = 0.0;
}

// Step the LeggedAgent using the optimal pattern generator
void LeggedAgent::PerfectStep(double StepSize)
{
	cout << Leg.Angle << " " << Leg.Omega << endl;
	double force = 0.0;

	// Update the leg effectors
	if (Leg.FootState == 0.0 && Leg.Angle >= ForwardAngleLimit) {Leg.FootState = 1; Leg.Omega = 0;}
	else if (Leg.FootState == 1.0 && (cx - Leg.FootX > 20)) Leg.FootState = 0;
	// Compute the force applied to the body
	if (Leg.FootState == 1.0 && Leg.Angle >= BackwardAngleLimit && Leg.Angle <= ForwardAngleLimit)
		force = MaxLegForce;
	// Update the position of the body
	vx = vx + StepSize * force;
	if (vx < -MaxVelocity) vx = -MaxVelocity;
	if (vx > MaxVelocity) vx = MaxVelocity;
	cx = cx + StepSize * vx;
	// Update the leg geometry
	Leg.JointX = Leg.JointX + StepSize * vx;
	if (Leg.FootState == 1.0) {
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
		Leg.FootX = Leg.JointX + LegLength * sin(Leg.Angle);
		Leg.FootY = Leg.JointY + LegLength * cos(Leg.Angle);
	}
	// If the foot is too far back, the body becomes "unstable" and forward motion ceases
	if (cx - Leg.FootX > 20) vx = 0.0;
}