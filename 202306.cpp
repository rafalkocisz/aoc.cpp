#include "aoc.h"

#ifdef AOC_TEST_SOLUTION
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"
#endif

#define FMT_HEADER_ONLY
#include "fmt/format.h"

#include <string>
#include <vector>

using namespace fmt;
using namespace aoc;
using namespace std;

class BoatRaceLogEntry
{
	friend bool operator==(const BoatRaceLogEntry&,
	                       const BoatRaceLogEntry&) = default;

public:
	BoatRaceLogEntry(long long time, long long recordDistance)
		: time_(time)
		, recordDistance_(recordDistance)
	{
	}

public:
	long long time_;
	long long recordDistance_;
};

class BoatRaceLog
{
	friend BoatRaceLog parseBoatRaceLog(const vector<string>& input);

public:
	size_t numEntries() const { return entries_.size(); }
	const BoatRaceLogEntry& entry(size_t index) const;
	const vector<BoatRaceLogEntry>& entries() const { return entries_; }

public:
	vector<BoatRaceLogEntry> entries_;
};

const BoatRaceLogEntry& BoatRaceLog::entry(size_t index) const
{
	AOC_ASSERT(index < numEntries());

	return entries_[index];
}

BoatRaceLog parseBoatRaceLog1(const vector<string>& input)
{
	AOC_ENFORCE_MSG(input.size() == 2, "invalid size of the input: two lines expected");

	const auto timeLogTokens = StringUtils::tokenize(input[0], ':');

	AOC_ENFORCE(timeLogTokens.size() == 2);
	AOC_ENFORCE(timeLogTokens[0] == "Time");

	constexpr auto trimWhitespace = true;
	constexpr auto skipEmptyTokens = true;

	const auto timeValueTokens = StringUtils::tokenize(timeLogTokens[1],
	                                                   ' ',
	                                                   trimWhitespace,
	                                                   skipEmptyTokens);

	vector<long long> timeValues;

	for(auto const& timeValueToken : timeValueTokens)
		timeValues.push_back(stoi(timeValueToken));

	const auto distanceLogTokens = StringUtils::tokenize(input[1], ':');

	AOC_ENFORCE(distanceLogTokens.size() == 2);
	AOC_ENFORCE(distanceLogTokens[0] == "Distance");

	const auto distanceValueTokens = StringUtils::tokenize(distanceLogTokens[1],
	                                                       ' ',
	                                                       trimWhitespace,
	                                                       skipEmptyTokens);

	vector<long long> distanceValues;

	for(auto const& distanceValueToken : distanceValueTokens)
		distanceValues.push_back(stoll(distanceValueToken));

	AOC_ENFORCE(timeValues.size() == distanceValues.size());

	BoatRaceLog log;

	for(size_t i=0; i<timeValues.size(); ++i)
		log.entries_.push_back(BoatRaceLogEntry(timeValues[i],
		                                        distanceValues[i]));

	return log;
}

BoatRaceLog parseBoatRaceLog2(const vector<string>& input)
{
	AOC_ENFORCE_MSG(input.size() == 2, "invalid size of the input: two lines expected");

	const auto timeLogTokens = StringUtils::tokenize(input[0], ':');

	AOC_ENFORCE(timeLogTokens.size() == 2);
	AOC_ENFORCE(timeLogTokens[0] == "Time");

	constexpr auto trimWhitespace = true;
	constexpr auto skipEmptyTokens = true;

	const auto timeValueTokens = StringUtils::tokenize(timeLogTokens[1],
	                                                   ' ',
	                                                   trimWhitespace,
	                                                   skipEmptyTokens);

	string timeStr;

	for(auto const& timeValueToken : timeValueTokens)
		timeStr += timeValueToken;

	const auto distanceLogTokens = StringUtils::tokenize(input[1], ':');

	AOC_ENFORCE(distanceLogTokens.size() == 2);
	AOC_ENFORCE(distanceLogTokens[0] == "Distance");

	const auto distanceValueTokens = StringUtils::tokenize(distanceLogTokens[1],
	                                                       ' ',
	                                                       trimWhitespace,
	                                                       skipEmptyTokens);

	string distanceStr;

	for(auto const& distanceValueToken : distanceValueTokens)
		distanceStr += distanceValueToken;

	BoatRaceLog log;

	log.entries_.push_back(BoatRaceLogEntry(stoll(timeStr),
	                                        stoll(distanceStr)));

	return log;
}

long long calculateBoatTravelDistance(long long boatSpeed, long long travelTime)
{
	return boatSpeed * travelTime;
}

long long calculateNumberOfWaysToBeatTheRecord(const BoatRaceLogEntry& entry)
{
	long long numberOfWaysToBeatTheRecord = 0;

	for(long long boatSpeed=0; boatSpeed<entry.time_; ++boatSpeed)
		if(calculateBoatTravelDistance(boatSpeed, entry.time_-boatSpeed) > entry.recordDistance_)
			++numberOfWaysToBeatTheRecord;

	return numberOfWaysToBeatTheRecord;
}

#ifdef AOC_TEST_SOLUTION

TEST_CASE("parseBoatRaceLog1")
{
	vector<string> input =
	{
		"Time:      7  15   30",
		"Distance:  9  40  200"
	};

	auto log = parseBoatRaceLog1(input);

	CHECK(log.numEntries() == 3);
	CHECK(log.entry(0) == BoatRaceLogEntry(7, 9));
	CHECK(log.entry(1) == BoatRaceLogEntry(15, 40));
	CHECK(log.entry(2) == BoatRaceLogEntry(30, 200));
}

TEST_CASE("parseBoatRaceLog2")
{
	vector<string> input =
	{
		"Time:      7  15   30",
		"Distance:  9  40  200"
	};

	auto log = parseBoatRaceLog2(input);

	CHECK(log.numEntries() == 1);
	CHECK(log.entry(0) == BoatRaceLogEntry(71530, 940200));
}

TEST_CASE("calculateBoatTravelDistance")
{
	CHECK(calculateBoatTravelDistance(0, 7) == 0);
	CHECK(calculateBoatTravelDistance(1, 6) == 6);
	CHECK(calculateBoatTravelDistance(2, 5) == 10);
	CHECK(calculateBoatTravelDistance(3, 4) == 12);
	CHECK(calculateBoatTravelDistance(4, 3) == 12);
	CHECK(calculateBoatTravelDistance(5, 2) == 10);
	CHECK(calculateBoatTravelDistance(6, 1) == 6);
	CHECK(calculateBoatTravelDistance(7, 0) == 0);
}

TEST_CASE("calculateNumberOfWaysToBeatTheRecord")
{
	CHECK(calculateNumberOfWaysToBeatTheRecord(BoatRaceLogEntry(7, 9)) == 4);
	CHECK(calculateNumberOfWaysToBeatTheRecord(BoatRaceLogEntry(15, 40)) == 8);
	CHECK(calculateNumberOfWaysToBeatTheRecord(BoatRaceLogEntry(30, 200)) == 9);
	CHECK(calculateNumberOfWaysToBeatTheRecord(BoatRaceLogEntry(71530, 940200)) == 71503);
}

#else // #ifdef AOC_TEST_SOLUTION

class WaitForIt : public PuzzleSolution<long long>
{
private:
	void processInput(const string& puzzleInputFilePath) override;
	long long answer1() override;
	long long answer2() override;

private:
	BoatRaceLog boatRaceLog1_;
	BoatRaceLog boatRaceLog2_;
};

void WaitForIt::processInput(const string& puzzleInputFilePath)
{
	PuzzleSolution::processInput(puzzleInputFilePath);

	boatRaceLog1_ = parseBoatRaceLog1(input());
	boatRaceLog2_ = parseBoatRaceLog2(input());
}

long long WaitForIt::answer1()
{
	long long answer1 = 1;

	for(const auto& entry : boatRaceLog1_.entries())
		answer1 *= calculateNumberOfWaysToBeatTheRecord(entry);

	return answer1;
}

long long WaitForIt::answer2()
{
	AOC_ASSERT(boatRaceLog2_.numEntries() == 1);

	return calculateNumberOfWaysToBeatTheRecord(boatRaceLog2_.entry(0));
}

int main(int argc, char* argv[])
{
	return WaitForIt().run((argc > 1) ? argv[1] : "202306.txt");
}

#endif // #ifdef AOC_TEST_SOLUTION
