// *************************
// A class for legged agents
//
// RDB 2/16/96
// *************************

#pragma once

#include "CTRNN.h"

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
		double PositionX();
		void SetPositionX(double newx);
		
		// Control
    void Reset(double ix, double iy, int randomize = 0);
    void Reset(double ix, double iy, int randomize, RandomState &rs);
		void Step(double StepSize, vector<int> forwardNeurons, vector<int> backwardNeurons);
		void Step2(double StepSize);
		void Step2RPG(double StepSize);
		void Step1(double StepSize);
		void PerfectStep(double StepSize);

		double GetJointX();
		double GetJointY();
		double GetFootX();
		double GetFootY();
		bool GetFootState();
		CTRNN NervousSystem;
	private:
		double calculateTotalOutput(vector<int> neurons);
};