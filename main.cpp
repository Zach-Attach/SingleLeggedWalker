#include "TSearch.h"
#include "LeggedAgent.h"
#include "CTRNN.h"
#include "random.h"
#include <iostream>

#define PRINTOFILE

// Task params
const double StepSize = 0.05; 
const double RunDuration = 220.0; 

// Task params
const double StepSizeAnalysis = 0.01; 
const double RunDurationAnalysis = 2200.0; 

// EA params
const int POPSIZE = 96;
const int GENS = 10000; 	// --- 5000
const double MUTVAR = 0.01; // --- 0.05
const double CROSSPROB = 0.0;
const double EXPECTED = 1.1;
const double ELITISM = 0.05;

// Nervous system params
const int N = 3;
const double WR = 8.0; // absolute weight range from 0
const double BR = 16.0; // absolute bias range from 0
const double TMIN = 1.0; // minimum time constant
const double TMAX = 10.0; // maximum time constant

int	VectSize = N*N + 2*N;

// ------------------------------------
// Genotype-Phenotype Mapping Functions
// ------------------------------------
void GenPhenMapping(TVector<double> &gen, TVector<double> &phen)
{
	int k = 1;
	// TODO: Parallelize this, may need to upgrade to C++17 see: https://stackoverflow.com/questions/36246300/parallel-loops-in-c
	// Time-constants
	for (int i = 1; i <= N; i++) {
		phen(k) = MapSearchParameter(gen(k), TMIN, TMAX);
		k++;
	}
	// Bias
	for (int i = 1; i <= N; i++) {
		phen(k) = MapSearchParameter(gen(k), -BR, BR);
		k++;
	}
	// Weights
	for (int i = 1; i <= N; i++) {
		for (int j = 1; j <= N; j++) {
			phen(k) = MapSearchParameter(gen(k), -WR, WR);
			k++;
		}
	}
}

// ------------------------------------
// Fitness function
// ------------------------------------
double FitnessFunction(TVector<double> &genotype, RandomState &rs)
{
	// Map genootype to phenotype
	TVector<double> phenotype;
	phenotype.SetBounds(1, VectSize);
	GenPhenMapping(genotype, phenotype);

	// Create the agent
	LeggedAgent Insect;

	// Instantiate the nervous system
	Insect.NervousSystem.SetCircuitSize(N);
	
	int k = 1;

	// Time-constants
	for (int i = 1; i <= N; i++) {
		Insect.NervousSystem.SetNeuronTimeConstant(i,phenotype(k));
		k++;
	}
	// Bias
	for (int i = 1; i <= N; i++) {
		Insect.NervousSystem.SetNeuronBias(i,phenotype(k));
		k++;
	}
	// Weights
	for (int i = 1; i <= N; i++) {
		for (int j = 1; j <= N; j++) {
			Insect.NervousSystem.SetConnectionWeight(i,j,phenotype(k));
			k++;
		}
	}

	Insect.Reset(0, 0, 0); // NOTE: Might be unnecessary?

	// Run the agent
	for (double time = 0; time < RunDuration; time += StepSize) {
		Insect.Step(StepSize);
		cout << Insect.Leg.JointX << " " << Insect.Leg.JointY << " ";
		cout << Insect.Leg.FootX << " " << Insect.Leg.FootY << " ";
		cout << Insect.Leg.FootState << endl;
	}

	return Insect.cx/RunDuration;
}

// ------------------------------------
// Display functions
// ------------------------------------
void EvolutionaryRunDisplay(int Generation, double BestPerf, double AvgPerf, double PerfVar)
{
	cout << Generation << " " << BestPerf << " " << AvgPerf << " " << PerfVar << endl;
}

void ResultsDisplay(TSearch &s)
{
	TVector<double> bestVector;
	ofstream BestIndividualFile;
	TVector<double> phenotype;
	phenotype.SetBounds(1, VectSize);

	// Save the genotype of the best individual
	bestVector = s.BestIndividual();
	BestIndividualFile.open("best.gen.dat");
	BestIndividualFile << bestVector << endl;
	BestIndividualFile.close();

	// Also show the best individual in the Circuit Model form
	BestIndividualFile.open("best.ns.dat");
	GenPhenMapping(bestVector, phenotype);
	LeggedAgent Insect;
	// Instantiate the nervous system
	Insect.NervousSystem.SetCircuitSize(N);
	int k = 1;

	// Time-constants
	for (int i = 1; i <= N; i++) {
		Insect.NervousSystem.SetNeuronTimeConstant(i,phenotype(k));
		k++;
	}
	// Bias
	for (int i = 1; i <= N; i++) {
		Insect.NervousSystem.SetNeuronBias(i,phenotype(k));
		k++;
	}
	// Weights
	for (int i = 1; i <= N; i++) {
		for (int j = 1; j <= N; j++) {
			Insect.NervousSystem.SetConnectionWeight(i,j,phenotype(k));
			k++;
		}
	}
	
	BestIndividualFile << Insect.NervousSystem << endl;

	for (int i = 1; i <= VectSize; i++) {
		BestIndividualFile << phenotype(k) << " ";
	}

	BestIndividualFile << endl;

	BestIndividualFile.close();
}

// ------------------------------------
// The main program
// ------------------------------------
int main (int argc, const char* argv[]) {

	long randomseed = static_cast<long>(time(NULL));
	if (argc == 2)
		randomseed += atoi(argv[1]);

	TSearch s(VectSize);

	// #ifdef PRINTOFILE
	// ofstream file;
	// file.open("evol.dat");
	// cout.rdbuf(file.rdbuf());
	// #endif

	// Configure the search
	s.SetRandomSeed(randomseed);
	s.SetSearchResultsDisplayFunction(ResultsDisplay);
	s.SetPopulationStatisticsDisplayFunction(EvolutionaryRunDisplay);
	s.SetSelectionMode(RANK_BASED);
	s.SetReproductionMode(GENETIC_ALGORITHM);
	s.SetPopulationSize(POPSIZE);
	s.SetMaxGenerations(GENS);
	s.SetCrossoverProbability(CROSSPROB);
	s.SetCrossoverMode(UNIFORM);
	s.SetMutationVariance(MUTVAR);
	s.SetMaxExpectedOffspring(EXPECTED);
	s.SetElitistFraction(ELITISM);
	s.SetSearchConstraint(1);

	// Run Stage 1
	s.SetEvaluationFunction(FitnessFunction);
	s.ExecuteSearch();
	
	// Analysis of best evolved circuit
	// ifstream genefile;
	// genefile.open( "best.gen.dat");
	// TVector<double> genotype(1, VectSize);
	// genefile >> genotype;
	// Map(genotype);
	// // Traces(genotype);
	// genefile.close();

	return 0;
}
