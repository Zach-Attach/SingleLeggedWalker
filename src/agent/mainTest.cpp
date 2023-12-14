#include "TSearch.h"
#include "LeggedAgent.h"
#include "CTRNN.h"
#include "random.h"
#include <iostream>

#include <fstream>
#include <iterator>
#include <string>
#include <algorithm>
#include<vector>

#include <ctime>

#define PRINTOFILE

// #include <csignal>
// #include <execinfo.h>
// #include <unistd.h>

// #include <iostream>
// #include <chrono>

// void handler(int sig) {
//   void *array[10];
//   size_t size;

//   // get void*'s for all entries on the stack
//   size = backtrace(array, 10);

//   // print out all the frames to stderr
//   fprintf(stderr, "Error: signal %d:\n", sig);
//   backtrace_symbols_fd(array, size, STDERR_FILENO);
//   exit(1);
// }

// Task params
const double StepSize = 0.1; //0.001;//0.05; 
const double RunDuration = 500; //500;//220.0; 

// Task params
const double StepSizeAnalysis = 0.01; 
const double RunDurationAnalysis = 2200.0; 

// EA params
const int POPSIZE = 5000; //8; //96; 80000 = 0.47 
const int GENS = 10000;//250; //10; //1000; 	// --- 5000
const double MUTVAR = 0.2;//0.05; //0.01 // --- 0.05
const double CROSSPROB = 0.0;
const double EXPECTED = 1.1;
const double ELITISM = 0.1;//0.05;

// Nervous system params
int N; // 1, 2, or 3
const double WR = 8.0; // absolute weight range from 0
const double BR = 8.0; // absolute bias range from 0
const double TMIN = 1.0; // minimum time constant
const double TMAX = 10.0; // maximum time constant

int	GeneSize;// = N*N + 2*N + N; // N is added for the coupling parameters
// int	VectSize;// = N*N + 2*N + 5*N;

std::vector<std::vector<int>> COUPLINGS = {{},{},{},{},{}};

string fileName;
// string output;

string evolOutput;
double bestbestPerf = 0.0;

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
	phen(k) = MapSearchParameter(gen(k), 0.0001, 8.0);
	k++;
	phen(k) = MapSearchParameter(gen(k), 0.0001, 8.0);
}

// ------------------------------------
// Fitness function
// ------------------------------------
double FitnessFunction(TVector<double> &genotype, RandomState &rs) {
	// Map genotype to phenotype
	TVector<double> phenotype;
	phenotype.SetBounds(1, GeneSize);
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

	Insect.footWeight = phenotype(k);
	k++;
	Insect.angleWeight = phenotype(k);

	Insect.SetMotorCouplings(COUPLINGS[0], COUPLINGS[1], COUPLINGS[2]);
	Insect.SetSensorCouplings(COUPLINGS[3], COUPLINGS[4]);

	Insect.Reset(0, 0, 0); // NOTE: Might be unnecessary?

	for (double time = 0; time < RunDuration; time += StepSize) {
		Insect.Step(StepSize);
		// cout << Insect.GetJointX() << " " << Insect.GetJointY() << " ";
		// cout << Insect.GetFootX() << " " << Insect.GetFootY() << " ";
		// cout << Insect.GetFootState() << endl;
	}

	return Insect.PositionX()/RunDuration;
}

// ------------------------------------
// Display functions
// ------------------------------------
void EvolutionaryRunDisplay(int Generation, double BestPerf, double AvgPerf, double PerfVar) {
	// cout << Generation << " " << BestPerf << " " << AvgPerf << " " << PerfVar << endl;
	// evolOutput += to_string(BestPerf) + " " + to_string(AvgPerf) + " " + to_string(PerfVar) + "\n";

	// if (BestPerf > bestbestPerf) {
	bestbestPerf = BestPerf;
	// }
}

void ResultsDisplay(TSearch &s) {
	TVector<double> bestVector;
	ofstream BestIndividualFile;
	TVector<double> phenotype;
	phenotype.SetBounds(1, GeneSize);

	// Save the genotype of the best individual
	bestVector = s.BestIndividual();
	// BestIndividualFile.open("../../out/"+to_string(N)+"/best/gen/"+fileName +".dat");
	// BestIndividualFile << bestVector << endl;
	// BestIndividualFile.close();

	// Also show the best individual in the Circuit Model form
	BestIndividualFile.open("../../out/"+to_string(N)+"/best/ns/"+fileName +".dat");
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

	Insect.SetMotorCouplings(COUPLINGS[0], COUPLINGS[1], COUPLINGS[2]);
	Insect.SetSensorCouplings(COUPLINGS[3], COUPLINGS[4]);

	BestIndividualFile << Insect.NervousSystem << endl;
	BestIndividualFile << Insect.footWeight << " " << Insect.angleWeight << endl;

	// for (int i = 1; i <= GeneSize; i++) {
	// 	BestIndividualFile << phenotype(i) << " ";
	// }

	BestIndividualFile << endl;

	BestIndividualFile.close();

	BestIndividualFile.open("../../out/"+to_string(N)+"/best/fit/"+fileName +".dat");
	BestIndividualFile << bestbestPerf << endl;
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
	// auto start = std::chrono::high_resolution_clock::now();

	// signal(SIGSEGV, handler); // Install our handler

	N = argc - 1;

	GeneSize = N*N + 2*N + 2; //added 2 for the sensory weights

	std::time_t t = std::time(nullptr);
	// create randomseed from time in seconds and add version number*10^6 and remove 10^9 seconds to prevent hitting max long value and overflowing
	// long randomseed = ((long) t) - pow(10,9) + stol(version)*pow(10,6);
	long randomseed = (long) t;

	// long randomseed = 765234;

	fileName = to_string(randomseed);

	for (int i = 1; i <= N; i++){
		fileName += "_" + string(argv[i]);
		int val = stoi(argv[i]);
		for (int j = 0; j < 5; j++) {
			if (val%3 != 1){
				COUPLINGS[j].push_back(i*(val%3 - 1));
			}
			val /= 3;
		}
	}

	TSearch s(GeneSize);

	#ifdef PRINTOFILE
	std::ofstream outfile("../../out/"+to_string(N)+"/evol/" + fileName + ".dat");
	auto cout_buff = std::cout.rdbuf();
	std::cout.rdbuf(outfile.rdbuf()); // anything written to std::cout will now go to myFile.txt instead...
	#endif

		// Map genotype to phenotype
	TVector<double> phenotype;
	phenotype.SetBounds(1, GeneSize);

// 1 size

// 1 tc

// 8 bias

// 1 gain

// 7.878351714224154811461175995646 weights

// 3.9525251667299723534125503429458e-323 4.9406564584124654417656879286822e-324

	// Create the agent
	LeggedAgent Insect;

	// Instantiate the nervous system

	Insect.NervousSystem.SetCircuitSize(1);
	Insect.NervousSystem.SetNeuronTimeConstant(1,1.7578659355782622064623410551576 );
	Insect.NervousSystem.SetNeuronBias(1,1.9006074974882725125979732183623  );
	Insect.NervousSystem.SetConnectionWeight(1,1,8);
	Insect.footWeight = 0.5;
	Insect.angleWeight = 7.9876847488831028698541558696888;	

	// Insect.NervousSystem.SetCircuitSize(2);
	// Insect.NervousSystem.SetNeuronTimeConstant(1,1.0029510347274239734360889997333);
	// Insect.NervousSystem.SetNeuronTimeConstant(2,1.0185517433341857440609601326287 );
	// Insect.NervousSystem.SetNeuronBias(1,3.2083708403850947910029844933888);
	// Insect.NervousSystem.SetNeuronBias(2,0.11275871429775562171382574661038 );
	// Insect.NervousSystem.SetConnectionWeight(1,1,5.3468881224772255578159274591599);
	// Insect.NervousSystem.SetConnectionWeight(1,2,-7.9947438514648387908323456940707);
	// Insect.NervousSystem.SetConnectionWeight(2,1,-6.5215476918018691065981329302303);
	// Insect.NervousSystem.SetConnectionWeight(2,2,5.8865878396508044545498705701903);
	// Insect.footWeight = 7.9942258576825349081218519131653;
	// Insect.angleWeight = 7.5593982662150143170265437220223;


	Insect.SetMotorCouplings(COUPLINGS[0], COUPLINGS[1], COUPLINGS[2]);
	Insect.SetSensorCouplings(COUPLINGS[3], COUPLINGS[4]);

	Insect.Reset(0, 0, 0); // NOTE: Might be unnecessary?

	for (double time = 0; time < RunDuration; time += StepSize) {
		Insect.Step(StepSize);
		// Insect.PerfectStep(StepSize);
		// cout << Insect.GetJointX() << " " << Insect.GetJointY() << " ";
		// cout << Insect.GetFootX() << " " << Insect.GetFootY() << " ";
		// cout << Insect.GetFootState() << endl;
	}

	// cout << Insect.cx/RunDuration;

	// cout << evolOutput;

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

	// auto stop = std::chrono::high_resolution_clock::now();
	// auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
	// std::cout << "Time taken by function: "
  //   << duration.count() << " microseconds" << std::endl;

	return 0;
}