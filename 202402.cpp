#include "aoc.h"

#ifdef AOC_TEST_SOLUTION
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"
#endif

#define FMT_HEADER_ONLY
#include "fmt/format.h"

#include <vector>

using namespace fmt;
using namespace aoc;
using namespace std;

int levelDifference(int level1, int level2)
{
	return abs(level1 - level2);
}

int isLevelIncreasing(int level1, int level2)
{
	return level1 < level2;
}

int isLevelDecreasing(int level1, int level2)
{
	return level1 > level2;
}

template<class Report>
bool isReportSafe(const Report& report)
{
	const size_t numLevels = report.numLevels();

	if(numLevels <= 1)
		return true;

	AOC_ASSERT(numLevels >= 2);

	constexpr int minDifference = 1;
	constexpr int maxDifference = 3;

	bool isLevelIncreasing = false;
	bool isLevelDecreasing = false;

	for(size_t i=0; i<numLevels-1; ++i)
	{
		const int level1 = report.level(i);
		const int level2 = report.level(i+1);

		if(MathUtils::isInRange(levelDifference(level1, level2),
		                        minDifference,
		                        maxDifference))
		{
			if(::isLevelIncreasing(level1, level2))
				isLevelIncreasing = true;
			else if(::isLevelDecreasing(level1, level2))
				isLevelDecreasing = true;
			else
				return false;

			if(isLevelIncreasing && isLevelDecreasing)
				return false;
		}
		else
			return false;
	}

	return true;
}

class Report
{
public:
	bool isSafe() const;

	int level(size_t index) const
	{
		AOC_ASSERT(index < numLevels());

		return levels_[index];
	}

	const vector<int>& levels() const { return levels_; }
	size_t numLevels() const { return levels_.size(); }

public:
	void addLevel(int level) { levels_.push_back(level); }

private:
	vector<int> levels_;
};

bool Report::isSafe() const
{
	return isReportSafe(*this);
}

Report parseReport(const string& line)
{
	auto tokens = StringUtils::tokenize(line, ' ');

	AOC_ENFORCE(tokens.size() >= 2);

	Report report;

	for(const auto& token : tokens)
		report.addLevel(stoi(token));

	return report;
}

class ReportAdapter
{
public:
	ReportAdapter(const Report& report, size_t levelIndexToSkip)
		: report_(report)
		, levelIndexToSkip_(levelIndexToSkip)
	{
		AOC_ASSERT(levelIndexToSkip < report_.numLevels());
	}

public:
	int level(size_t index) const
	{
		AOC_ASSERT(index < numLevels());

		if(index >= levelIndexToSkip_)
			++index;

		return report_.level(index);
	}

	size_t numLevels() const { return report_.numLevels() - 1; }

private:
	const Report& report_;
	size_t levelIndexToSkip_;
};

class ProblemDampener
{
public:
	static int isSafeAnyway(const Report& report);
};

int ProblemDampener::isSafeAnyway(const Report& report)
{
	AOC_ASSERT(!report.isSafe());

	const size_t numLevels = report.numLevels();

	size_t numBadLevels = 0;

	for(int i=0; i<numLevels; ++i)
	{
		ReportAdapter adapter(report, i);

		if(isReportSafe(adapter))
			return true;
	}

	return false;
}

class UnusualData
{
public:
	const Report& report(size_t index) const
	{
		AOC_ASSERT(index < numReports());

		return reports_[index];
	}

	size_t numReports() const { return reports_.size(); }
	size_t numSafeReports() const;
	size_t numSafeReportsWithProblemDampenerApplied() const;

public:
	void addReport(const Report& report) { reports_.push_back(report); }

private:
	vector<Report> reports_;
};

size_t UnusualData::numSafeReports() const
{
	size_t numSafeReports = 0;

	for(const auto& report : reports_)
		if(report.isSafe())
			++numSafeReports;

	return numSafeReports;
}

size_t UnusualData::numSafeReportsWithProblemDampenerApplied() const
{
	size_t numSafeReports = 0;

	for(const auto& report : reports_)
	{
		if(report.isSafe())
		{
			++numSafeReports;
		}
		else
		{
			if(ProblemDampener::isSafeAnyway(report))
			{
				++numSafeReports;
			}
		}
	}

	return numSafeReports;
}

UnusualData parseUnusualData(const vector<string>& input)
{
	UnusualData unusualData;

	for(const auto& line : input)
		unusualData.addReport(parseReport(line));

	return unusualData;
}

#ifdef AOC_TEST_SOLUTION

TEST_CASE("levelDifference")
{
	CHECK(levelDifference(2, 7) == 5);
	CHECK(levelDifference(6, 2) == 4);
	CHECK(levelDifference(1, 3) == 2);
	CHECK(levelDifference(3, 2) == 1);
	CHECK(levelDifference(4, 4) == 0);
	CHECK(levelDifference(3, 6) == 3);
}

TEST_CASE("isLevelIncreasing")
{
	CHECK( isLevelIncreasing(2, 7));
	CHECK(!isLevelIncreasing(6, 2));
	CHECK( isLevelIncreasing(1, 3));
	CHECK(!isLevelIncreasing(3, 2));
	CHECK(!isLevelIncreasing(4, 4));
	CHECK( isLevelIncreasing(3, 6));
}

TEST_CASE("isLevelDecreasing")
{
	CHECK(!isLevelDecreasing(2, 7));
	CHECK( isLevelDecreasing(6, 2));
	CHECK(!isLevelDecreasing(1, 3));
	CHECK( isLevelDecreasing(3, 2));
	CHECK(!isLevelDecreasing(4, 4));
	CHECK(!isLevelDecreasing(3, 6));
}

TEST_CASE("parseReport")
{
	CHECK(parseReport("7 6 4 2 1").levels() == vector<int>{ 7, 6, 4, 2, 1 });
	CHECK(parseReport("1 2 7 8 9").levels() == vector<int>{ 1, 2, 7, 8, 9 });
	CHECK(parseReport("9 7 6 2 1").levels() == vector<int>{ 9, 7, 6, 2, 1 });
	CHECK(parseReport("1 3 2 4 5").levels() == vector<int>{ 1, 3, 2, 4, 5 });
	CHECK(parseReport("8 6 4 4 1").levels() == vector<int>{ 8, 6, 4, 4, 1 });
	CHECK(parseReport("1 3 6 7 9").levels() == vector<int>{ 1, 3, 6, 7, 9 });
}

TEST_CASE("Report")
{
	SUBCASE("isSafe")
	{
		CHECK( parseReport("7 6 4 2 1").isSafe());
		CHECK(!parseReport("1 2 7 8 9").isSafe());
		CHECK(!parseReport("9 7 6 2 1").isSafe());
		CHECK(!parseReport("1 3 2 4 5").isSafe());
		CHECK(!parseReport("8 6 4 4 1").isSafe());
		CHECK( parseReport("1 3 6 7 9").isSafe());
	}
}

TEST_CASE("ReportAdapter")
{
	auto report = parseReport("7 6 4 2 1");

	ReportAdapter adapter1(report, 0);

	CHECK(adapter1.numLevels() == 4);
	CHECK(adapter1.level(0) == 6);
	CHECK(adapter1.level(1) == 4);
	CHECK(adapter1.level(2) == 2);
	CHECK(adapter1.level(3) == 1);

	ReportAdapter adapter2(report, 2);

	CHECK(adapter2.numLevels() == 4);
	CHECK(adapter2.level(0) == 7);
	CHECK(adapter2.level(1) == 6);
	CHECK(adapter2.level(2) == 2);
	CHECK(adapter2.level(3) == 1);

	ReportAdapter adapter3(report, 4);

	CHECK(adapter3.numLevels() == 4);
	CHECK(adapter3.level(0) == 7);
	CHECK(adapter3.level(1) == 6);
	CHECK(adapter3.level(2) == 4);
	CHECK(adapter3.level(3) == 2);
}

TEST_CASE("ProblemDampener")
{
	SUBCASE("isSafeAnyway")
	{
		CHECK(!ProblemDampener::isSafeAnyway(parseReport("1 2 7 8 9")));
		CHECK(!ProblemDampener::isSafeAnyway(parseReport("9 7 6 2 1")));
		CHECK( ProblemDampener::isSafeAnyway(parseReport("1 3 2 4 5")));
		CHECK( ProblemDampener::isSafeAnyway(parseReport("8 6 4 4 1")));
		CHECK( ProblemDampener::isSafeAnyway(parseReport("4 4 3 2 1")));
		CHECK( ProblemDampener::isSafeAnyway(parseReport("1 1 2 3 4")));
	}
}

TEST_CASE("parseUnusualData")
{
	const vector<string> input =
	{
		"7 6 4 2 1",
		"1 2 7 8 9",
		"9 7 6 2 1",
		"1 3 2 4 5",
		"8 6 4 4 1",
		"1 3 6 7 9",
	};

	auto unusualData = parseUnusualData(input);

	CHECK(unusualData.report(0).levels() == vector<int>{ 7, 6, 4, 2, 1 });
	CHECK(unusualData.report(1).levels() == vector<int>{ 1, 2, 7, 8, 9 });
	CHECK(unusualData.report(2).levels() == vector<int>{ 9, 7, 6, 2, 1 });
	CHECK(unusualData.report(3).levels() == vector<int>{ 1, 3, 2, 4, 5 });
	CHECK(unusualData.report(4).levels() == vector<int>{ 8, 6, 4, 4, 1 });
	CHECK(unusualData.report(5).levels() == vector<int>{ 1, 3, 6, 7, 9 });
}

TEST_CASE("UnusualData")
{
	const vector<string> input =
	{
		"7 6 4 2 1",
		"1 2 7 8 9",
		"9 7 6 2 1",
		"1 3 2 4 5",
		"8 6 4 4 1",
		"1 3 6 7 9",
	};

	auto unusualData = parseUnusualData(input);

	SUBCASE("numSafeReports")
	{
		CHECK(unusualData.numSafeReports() == 2);
	}

	SUBCASE("numSafeReportsWithProblemDampenerApplied")
	{
		CHECK(unusualData.numSafeReportsWithProblemDampenerApplied() == 4);
	}
}

#else // #ifdef AOC_TEST_SOLUTION

class RedNosedReports : public PuzzleSolution<int>
{
private:
	void processInput(const string& puzzleInputFilePath) override;
	int answer1() override;
	int answer2() override;

private:
	UnusualData unusualData_;
};

void RedNosedReports::processInput(const string& puzzleInputFilePath)
{
	PuzzleSolution::processInput(puzzleInputFilePath);

	unusualData_ = parseUnusualData(input());
}

int RedNosedReports::answer1()
{
	return unusualData_.numSafeReports();
}

int RedNosedReports::answer2()
{
	return unusualData_.numSafeReportsWithProblemDampenerApplied();
}

int main(int argc, char* argv[])
{
	return RedNosedReports().run((argc > 1) ? argv[1] : "202402.txt");
}

#endif // #ifdef AOC_TEST_SOLUTION
