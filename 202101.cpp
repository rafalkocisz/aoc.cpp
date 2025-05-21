#include "aoc.h"

#ifdef AOC_TEST_SOLUTION
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"
#endif

using namespace fmt;
using namespace aoc;
using namespace std;

int numMeasurementIncreases(const vector<int>& measurements)
{
	const size_t numMeasurements = measurements.size();

	AOC_ENFORCE(numMeasurements > 1);

	int numIncreases = 0;

	for(size_t i=1; i<numMeasurements; ++i)
		if(measurements[i] > measurements[i-1])
			++numIncreases;

	return numIncreases;
}

int numWindows(const vector<int>& measurements)
{
	const size_t numMeasurements = measurements.size();

	AOC_ENFORCE(numMeasurements > 2);

	return numMeasurements - 2;
}

int sumOfWindowMeasurements(const vector<int>& measurements,
                            size_t windowIndex)
{
	AOC_ENFORCE(windowIndex < numWindows(measurements));

	return measurements[windowIndex    ] +
	       measurements[windowIndex + 1] +
	       measurements[windowIndex + 2];
}

int numWindowIncreases(const vector<int>& measurements)
{
	const size_t numWindows = ::numWindows(measurements);

	AOC_ENFORCE(numWindows > 1);

	int numIncreases = 0;

	for(size_t i=1; i<numWindows; ++i)
		if(sumOfWindowMeasurements(measurements, i) >
		   sumOfWindowMeasurements(measurements, i-1))
			++numIncreases;

	return numIncreases;
}

#ifdef AOC_TEST_SOLUTION

TEST_CASE("numMeasurementIncreases")
{
	CHECK(numMeasurementIncreases({199, 200, 208, 210, 200, 207, 240, 269, 260, 263}) == 7);
}

TEST_CASE("numWindows")
{
	CHECK(numWindows({199, 200, 208}) == 1);
	CHECK(numWindows({199, 200, 208, 210, 200, 207, 240, 269, 260, 263}) == 8);
}

TEST_CASE("sumOfWindowMeasurements")
{
	const vector<int> measurements{199, 200, 208, 210, 200, 207, 240, 269, 260, 263};

	CHECK(sumOfWindowMeasurements(measurements, 0) == 607);
	CHECK(sumOfWindowMeasurements(measurements, 1) == 618);
	CHECK(sumOfWindowMeasurements(measurements, 2) == 618);
	CHECK(sumOfWindowMeasurements(measurements, 3) == 617);
	CHECK(sumOfWindowMeasurements(measurements, 4) == 647);
	CHECK(sumOfWindowMeasurements(measurements, 5) == 716);
	CHECK(sumOfWindowMeasurements(measurements, 6) == 769);
	CHECK(sumOfWindowMeasurements(measurements, 7) == 792);
}

TEST_CASE("numWindowIncreases")
{
	CHECK(numWindowIncreases({199, 200, 208, 210, 200, 207, 240, 269, 260, 263}) == 5);
}

#else // #ifdef AOC_TEST_SOLUTION

class SonarSweep : public PuzzleSolution<int>
{
private:
	void processInput(const string& puzzleInputFilePath) override;
	int answer1() override;
	int answer2() override;

private:
	vector<int> depthMeasurements_;
};

void SonarSweep::processInput(const string& puzzleInputFilePath)
{
	PuzzleSolution::processInput(puzzleInputFilePath);

	for(const auto& line : input())
		depthMeasurements_.push_back(stoi(line));
}

int SonarSweep::answer1()
{
	return numMeasurementIncreases(depthMeasurements_);
}

int SonarSweep::answer2()
{
	return numWindowIncreases(depthMeasurements_);
}

int main(int argc, char* argv[])
{
	return SonarSweep().run((argc > 1) ? argv[1] : "202101.txt");
}

#endif // #ifdef AOC_TEST_SOLUTION
