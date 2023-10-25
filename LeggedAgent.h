// *************************
// A class for legged agents
//
// RDB 2/16/96
// *************************

#pragma once

#include "CTRNN.h"
#include<vector>

// Global constants


// The Leg class declaration

class TLeg {
	public:
		// The constructor
		TLeg() {};
		// The destructor
		~TLeg() {};
		
		// Accessors
		double Angle, Omega, ForwardForce, BackwardForce;
		double FootX, FootY, JointX, JointY;
		bool FootState;
};


// The LeggedAgent class declaration

class LeggedAgent {
	public:
		// The constructor
		LeggedAgent(double ix = 0.0, double iy = 0.0)
		{
			Reset(ix,iy);
		};
		// The destructor
		~LeggedAgent() {};
		
		// Accessors
		double PositionX(void) {return cx;};
		void SetPositionX(double newx) {cx = newx;};
		
		// Control
    void Reset(double ix, double iy, int randomize = 0);
    void Reset(double ix, double iy, int randomize, RandomState &rs);
		void resetHelper(double ix, double iy, int randomize);
		void Step(double StepSize);
		void Step2RPG(double StepSize);
		void Step1(double StepSize);
		void PerfectStep(double StepSize);

		double cx; // position along x axis?
		double cy; // position along y axis?
		double vx; // velocity along x axis
		TLeg Leg;

		double GetJointX();
		double GetJointY();
		double GetFootX();
		double GetFootY();
		bool GetFootState();
		CTRNN NervousSystem;

		void SetMotorCouplings(std::vector<int> foot, std::vector<int> forward, std::vector<int> backward);
		void SetSensorCouplings(std::vector<int> foot, std::vector<int> angle);
	private:
		double SumOutput(std::vector<int> neurons);
		void SensoryUpdate(std::vector<int> neurons, double stimulus);
};