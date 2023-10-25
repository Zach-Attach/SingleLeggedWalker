#include "TSearch.h"
#include "LeggedAgent.h"
#include "CTRNN.h"
#include "random.h"
#include <iostream>

#include <fstream>
#include <iterator>
#include <string>
#include <algorithm>

#include <ctime>

#define PRINTOFILE

// Task params
const double StepSize = 0.05; 
const double RunDuration = 220.0; 

// Task params
const double StepSizeAnalysis = 0.01; 
const double RunDurationAnalysis = 2200.0; 

// EA params
const int POPSIZE = 96; //10; //96;
const int GENS = 1000; //50; //10000; 	// --- 5000
const double MUTVAR = 0.05; //0.01 // --- 0.05
const double CROSSPROB = 0.0;
const double EXPECTED = 1.1;
const double ELITISM = 0.05;

// Nervous system params
const int N = 2;
const double WR = 8.0; // absolute weight range from 0
const double BR = 8.0; // absolute bias range from 0
const double TMIN = 1.0; // minimum time constant
const double TMAX = 10.0; // maximum time constant

int	GeneSize = N*N + 2*N + N; // N is added for the coupling parameters
int	VectSize = N*N + 2*N + 5*N;

string version;
// string output;

void printVec(string s, std::vector <int> const &a) {
  std::cout << s;

  for(int i=0; i < a.size(); i++)
		std::cout << a.at(i) << ' ';

	std::cout << endl;
}

// ------------------------------------
// Genotype-Phenotype Mapping Functions
// ------------------------------------
void GenPhenMapping(TVector<double> &gen, TVector<double> &phen) {
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

	int kg = k;
	int kp = k;

	// Couplings
	for (int n = 0; n < N; n++, kg++) {
		double g = (gen(kg) + 1) / 2; // normalize to 0..1
		for (int s = 1; s <= 5; s++, kp++) {
			if (g < pow(3,-s)) {
				g -= pow(3,-s);
				phen(kp) = -1;
			} else if (g < pow(3,-2*s)){
				g -= pow(3, -2*s);
				phen(kp) = 0;
			} else {
				g -= pow(3, -3*s);
				phen(kp) = 1;
			}
		}
	}
}

// ------------------------------------
// Fitness function
// ------------------------------------
double FitnessFunction(TVector<double> &genotype, RandomState &rs) {
	// Map genotype to phenotype
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

	vector<vector<int> > couplings = {{},{},{},{},{}};

	for (int n = 1; n <= N; n++) {
		for (int s = 0; s < 5; s++, k++) {
			if (phenotype[k] != 0)
				couplings[s].push_back(phenotype(k)*n);
		}
	}

	Insect.SetMotorCouplings(couplings[0], couplings[1], couplings[2]);
	Insect.SetSensorCouplings(couplings[3], couplings[4]);

	Insect.Reset(0, 0, 0); // NOTE: Might be unnecessary?

	for (double time = 0; time < RunDuration; time += StepSize) {
		Insect.Step(StepSize);
		// cout << Insect.GetJointX() << " " << Insect.GetJointY() << " ";
		// cout << Insect.GetFootX() << " " << Insect.GetFootY() << " ";
		// cout << Insect.GetFootState() << endl;
	}

	double fitness = Insect.PositionX()/RunDuration;

	// if (fitness > 0.0) {
	// 	for (k = 1; k <= VectSize; k++) {
	// 		output += to_string(phenotype(k)) + " ";
	// 	}

	// 	output += to_string(fitness) + "\n";
	// }
	
	return fitness;
}

// ------------------------------------
// Display functions
// ------------------------------------
void EvolutionaryRunDisplay(int Generation, double BestPerf, double AvgPerf, double PerfVar) {
	cout << Generation << " " << BestPerf << " " << AvgPerf << " " << PerfVar << endl;
}

void ResultsDisplay(TSearch &s) {
	TVector<double> bestVector;
	ofstream BestIndividualFile;
	TVector<double> phenotype;
	phenotype.SetBounds(1, VectSize);

	std::time_t t = std::time(nullptr);

	// Save the genotype of the best individual
	bestVector = s.BestIndividual();
	BestIndividualFile.open("out/"+to_string(N)+"/"+"best/gen/"+to_string(t)+"_" + version +".dat");
	BestIndividualFile << bestVector << endl;
	BestIndividualFile.close();

	// Also show the best individual in the Circuit Model form
	BestIndividualFile.open("out/"+to_string(N)+"/"+"best/ns/"+to_string(t)+"_" + version +".dat");
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

	vector<vector<int> > couplings = {{},{},{},{},{}};

	for (int n = 1; n <= N; n++) {
		for (int s = 0; s < 5; s++) {
			if (phenotype[k] != 0)
				couplings[s].push_back(phenotype(k)*n);
			k++;
		}
	}

	Insect.SetMotorCouplings(couplings[0], couplings[1], couplings[2]);
	Insect.SetSensorCouplings(couplings[3], couplings[4]);

	BestIndividualFile << Insect.NervousSystem << endl;

	for (int i = 1; i <= VectSize; i++) {
		BestIndividualFile << phenotype(i) << " ";
	}

	BestIndividualFile << endl;

	BestIndividualFile.close();
}

bool compareFiles(const std::string& p1, const std::string& p2) {
  std::ifstream f1(p1, std::ifstream::binary|std::ifstream::ate);
  std::ifstream f2(p2, std::ifstream::binary|std::ifstream::ate);

  if (f1.fail() || f2.fail()) {
    return false; //file problem
  }

  if (f1.tellg() != f2.tellg()) {
    return false; //size mismatch
  }

  //seek back to beginning and use std::equal to compare contents
  f1.seekg(0, std::ifstream::beg);
  f2.seekg(0, std::ifstream::beg);
  return std::equal(std::istreambuf_iterator<char>(f1.rdbuf()),
                    std::istreambuf_iterator<char>(),
                    std::istreambuf_iterator<char>(f2.rdbuf()));
}

void test () {
	cout << "Comparing files" << endl;
	cout << "Best Gen Match: " << compareFiles("best.gen.dat", "test/best.gen.expected.dat") << endl;
	cout << "Best NS Match: " << compareFiles("best.ns.dat", "test/best.ns.expected.dat") << endl;
	cout << "Evol Match: " << compareFiles("evol.dat", "test/evol.expected.dat") << endl;
}

int main (int argc, const char* argv[]) {

	version = argv[1];
	std::time_t t = std::time(nullptr);
	// create randomseed from time in seconds and add version number*10^6 and remove 10^9 seconds to prevent hitting max long value and overflowing
	long randomseed = ((long) t) - pow(10,9) + stol(version)*pow(10,6);

	// long randomseed = 765234;

	TSearch s(GeneSize);

	// #ifdef PRINTOFILE
	// ofstream file;
	// file.open("evol.dat");
	// cout.rdbuf(file.rdbuf());
	// #endif

	#ifdef PRINTOFILE
	std::ofstream outfile("out/"+to_string(N)+"/"+"evol/" + to_string(t)+ "_" + version + ".dat");
	auto cout_buff = std::cout.rdbuf();
	std::cout.rdbuf(outfile.rdbuf()); // anything written to std::cout will now go to myFile.txt instead...
	#endif

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

	// std::cout.rdbuf(cout_buff);
	// anything written to std::cout will now go to STDOUT again...

	// test();
	
	// Analysis of best evolved circuit
	// ifstream genefile;
	// genefile.open( "best.gen.dat");
	// TVector<double> genotype(1, VectSize);
	// genefile >> genotype;
	// Map(genotype);
	// // Traces(genotype);
	// genefile.close();

	// cout << output;

	return 0;
}
