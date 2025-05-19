#include "aoc.h"

#ifdef AOC_TEST_SOLUTION
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"
#endif

#define FMT_HEADER_ONLY
#include "fmt/format.h"

#include <iostream>

using namespace fmt;
using namespace aoc;
using namespace std;

class CalibrationEquation
{
	friend CalibrationEquation parseCalibrationEquation(const string& line);

	friend bool operator==(const CalibrationEquation&,
	                       const CalibrationEquation&) = default;

public:
	explicit CalibrationEquation(int64_t testValue)
		: testValue_(testValue)
	{
	}

	CalibrationEquation(int64_t testValue,
	                    const vector<int64_t>& numbers)
		: testValue_(testValue)
		, numbers_(numbers)
	{
		AOC_ASSERT(numbers.size() >= 2);
	}

	string toString() const
	{
		string result = to_string(testValue_);

		result += ": ";

		for(int64_t number : numbers_)
			result += to_string(number) + ' ';

		return result;
	}

public:
	int64_t testValue() const { return testValue_; }
	const vector<int64_t> numbers() const { return numbers_; }
	size_t numOperators() const { return numbers_.size() - 1; }

private:
	int64_t testValue_;
	vector<int64_t> numbers_;
};

CalibrationEquation parseCalibrationEquation(const string& line)
{
	const auto tokens = StringUtils::tokenize(line, ' ', false);

	AOC_ENFORCE(tokens.size() >= 3);
	AOC_ENFORCE(tokens[0].size() >= 2);
	AOC_ENFORCE(tokens[0].back() == ':');

	const string testValueStr = tokens[0].substr(0, tokens[0].size()-1);

	AOC_ASSERT(testValueStr.size() >= 1);
	AOC_ASSERT(testValueStr.back() != ':');

	CalibrationEquation calibrationEquation(strtoll(testValueStr.c_str(), nullptr, 10));

	for(size_t i=1; i<tokens.size(); ++i)
		calibrationEquation.numbers_.push_back(strtoll(tokens[i].c_str(), nullptr, 10));

	return calibrationEquation;
}

void parseCalibrationEquations(const vector<string>& lines,
                               vector<CalibrationEquation>& calibrationEquations)
{
	for(const auto& line : lines)
		calibrationEquations.push_back(parseCalibrationEquation(line));
}

int64_t evaluate(const vector<int64_t>& numbers,
                 const string& operators)
{
	AOC_ASSERT(numbers.size() >= 2);
	AOC_ASSERT(operators.size() == numbers.size() - 1);

	int64_t result = numbers[0];

	const size_t numOperators = operators.size();

	for(int i=0; i<numOperators; ++i)
	{
		switch(operators[i])
		{
			case '+': result += numbers[i+1]; break;
			case '*': result *= numbers[i+1]; break;
			case '|': result = strtoll((to_string(result) + to_string(numbers[i+1])).c_str(), nullptr, 10); break;

			default: panic(format("invalid operator: '{}'", operators[i]));
		}
	}

	return result;
}

size_t numValidOperatorConfigurations(const CalibrationEquation& calibrationEquation,
                                      const set<char>& operators,
                                      size_t* totalCalibrationResult=nullptr)
{
	size_t validOperatorConfigurationCounter = 0;
	size_t numAdds = 0;

	AOC_ASSERT(calibrationEquation.numOperators() >= 1);

	vector<string> operatorsVariations;

	StringUtils::generateVariations(operators,
	                                calibrationEquation.numOperators(),
	                                operatorsVariations);

	for(const auto& operatorsVariation : operatorsVariations)
	{
		int64_t result = evaluate(calibrationEquation.numbers(),
		                          operatorsVariation);

		if(result == calibrationEquation.testValue())
		{
			++validOperatorConfigurationCounter;
		}
	}

	if(totalCalibrationResult && validOperatorConfigurationCounter > 0)
	{
		*totalCalibrationResult += calibrationEquation.testValue();

	}

	return validOperatorConfigurationCounter;
}

size_t totalCalibrationResult(const vector<CalibrationEquation>& calibrationEquations,
                              const set<char>& operators)
{
	size_t totalCalibrationResult = 0;

	for(const auto& calibrationEquation : calibrationEquations)
		numValidOperatorConfigurations(calibrationEquation,
		                               operators,
		                               &totalCalibrationResult);

	return totalCalibrationResult;
}

#ifdef AOC_TEST_SOLUTION

TEST_CASE("parseCalibrationEquation")
{
	CHECK(parseCalibrationEquation("190: 10 19") ==
	      CalibrationEquation(190, vector<int64_t>{ 10, 19 }));
	CHECK(parseCalibrationEquation("3267: 81 40 27") ==
	      CalibrationEquation(3267, vector<int64_t>{ 81, 40, 27 }));
	CHECK(parseCalibrationEquation("83: 17 5") ==
	      CalibrationEquation(83, vector<int64_t>{ 17, 5 }));
	CHECK(parseCalibrationEquation("156: 15 6") ==
	      CalibrationEquation(156, vector<int64_t>{ 15, 6 }));
	CHECK(parseCalibrationEquation("7290: 6 8 6 15") ==
	      CalibrationEquation(7290, vector<int64_t>{ 6, 8, 6, 15 }));
	CHECK(parseCalibrationEquation("161011: 16 10 13") ==
	      CalibrationEquation(161011, vector<int64_t>{ 16, 10, 13 }));
	CHECK(parseCalibrationEquation("192: 17 8 14") ==
	      CalibrationEquation(192, vector<int64_t>{ 17, 8, 14 }));
	CHECK(parseCalibrationEquation("21037: 9 7 18 13") ==
	      CalibrationEquation(21037, vector<int64_t>{ 9, 7, 18, 13 }));
	CHECK(parseCalibrationEquation("292: 11 6 16 20") ==
	      CalibrationEquation(292, vector<int64_t>{ 11, 6, 16, 20 }));
}

TEST_CASE("parseCalibrationEquations")
{
	const vector<string> lines =
	{
		"190: 10 19",
		"3267: 81 40 27",
		"83: 17 5",
		"156: 15 6",
		"7290: 6 8 6 15",
		"161011: 16 10 13",
		"192: 17 8 14",
		"21037: 9 7 18 13",
		"292: 11 6 16 20",
	};

	vector<CalibrationEquation> calibrationEquations;

	parseCalibrationEquations(lines, calibrationEquations);

	CHECK(calibrationEquations[0] ==
	      CalibrationEquation(190, vector<int64_t>{ 10, 19 }));
	CHECK(calibrationEquations[1] ==
	      CalibrationEquation(3267, vector<int64_t>{ 81, 40, 27 }));
	CHECK(calibrationEquations[2] ==
	      CalibrationEquation(83, vector<int64_t>{ 17, 5 }));
	CHECK(calibrationEquations[3] ==
	      CalibrationEquation(156, vector<int64_t>{ 15, 6 }));
	CHECK(calibrationEquations[4] ==
	      CalibrationEquation(7290, vector<int64_t>{ 6, 8, 6, 15 }));
	CHECK(calibrationEquations[5] ==
	      CalibrationEquation(161011, vector<int64_t>{ 16, 10, 13 }));
	CHECK(calibrationEquations[6] ==
	      CalibrationEquation(192, vector<int64_t>{ 17, 8, 14 }));
	CHECK(calibrationEquations[7] ==
	      CalibrationEquation(21037, vector<int64_t>{ 9, 7, 18, 13 }));
	CHECK(calibrationEquations[8] ==
	      CalibrationEquation(292, vector<int64_t>{ 11, 6, 16, 20 }));
}

TEST_CASE("evaluate")
{
	CHECK(evaluate(vector<int64_t>({ 10, 19 }), "+") == 29);
	CHECK(evaluate(vector<int64_t>({ 10, 19 }), "*") == 190);
	CHECK(evaluate(vector<int64_t>({ 81, 40, 27 }), "+*") == 3267);
	CHECK(evaluate(vector<int64_t>({ 81, 40, 27 }), "*+") == 3267);
	CHECK(evaluate(vector<int64_t>({ 11, 6, 16, 20 }), "+*+") == 292);
	CHECK(evaluate(vector<int64_t>({ 15, 6 }), "|") == 156);
	CHECK(evaluate(vector<int64_t>({ 6, 8, 6, 15 }), "*|*") == 7290);
	CHECK(evaluate(vector<int64_t>({ 17, 8, 14 }), "|+") == 192);
}

TEST_CASE("numValidOperatorConfigurations")
{
	set<char> ops1({ '+', '*' });

	CHECK(numValidOperatorConfigurations(CalibrationEquation(190, vector<int64_t>{ 10, 19 }), ops1) == 1);
	CHECK(numValidOperatorConfigurations(CalibrationEquation(3267, vector<int64_t>{ 81, 40, 27 }), ops1) == 2);
	CHECK(numValidOperatorConfigurations(CalibrationEquation(292, vector<int64_t>{ 11, 6, 16, 20 }), ops1) == 1);

	set<char> ops2({ '+', '*', '|' });

	CHECK(numValidOperatorConfigurations(CalibrationEquation(156, vector<int64_t>{ 15, 6 }), ops2) == 1);
	CHECK(numValidOperatorConfigurations(CalibrationEquation(7290, vector<int64_t>{ 6, 8, 6, 15 }), ops2) == 1);
	CHECK(numValidOperatorConfigurations(CalibrationEquation(192, vector<int64_t>{ 17, 8, 14 }), ops2) == 1);
}

TEST_CASE("totalCalibrationResult")
{
	const vector<string> lines =
	{
		"190: 10 19",
		"3267: 81 40 27",
		"83: 17 5",
		"156: 15 6",
		"7290: 6 8 6 15",
		"161011: 16 10 13",
		"192: 17 8 14",
		"21037: 9 7 18 13",
		"292: 11 6 16 20",
	};

	vector<CalibrationEquation> calibrationEquations;

	parseCalibrationEquations(lines, calibrationEquations);

	set<char> ops1({ '+', '*' });

	CHECK(totalCalibrationResult(calibrationEquations, ops1) == 3749);

	set<char> ops2({ '+', '*', '|' });

	CHECK(totalCalibrationResult(calibrationEquations, ops2) == 11387);
}

#else // #ifdef AOC_TEST_SOLUTION

class BridgeRepair : public PuzzleSolution<int64_t>
{
private:
	void processInput(const string& puzzleInputFilePath) override;
	int64_t answer1() override;
	int64_t answer2() override;

private:
	vector<CalibrationEquation> calibrationEquations_;
};

void BridgeRepair::processInput(const string& puzzleInputFilePath)
{
	PuzzleSolution::processInput(puzzleInputFilePath);

	parseCalibrationEquations(input(), calibrationEquations_);
}

int64_t BridgeRepair::answer1()
{
	return totalCalibrationResult(calibrationEquations_,
	                              set<char>({ '+', '*' }));
}

int64_t BridgeRepair::answer2()
{
	return totalCalibrationResult(calibrationEquations_,
	                              set<char>({ '+', '*', '|' }));
}

int main(int argc, char* argv[])
{
	return BridgeRepair().run((argc > 1) ? argv[1] : "202407.txt");
}

#endif // #ifdef AOC_TEST_SOLUTION
