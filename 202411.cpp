#include "aoc.h"

#ifdef AOC_TEST_SOLUTION
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"
#endif

#define FMT_HEADER_ONLY
#include "fmt/format.h"

#include <iostream>
#include <list>
#include <map>

using namespace fmt;
using namespace aoc;
using namespace std;

int numDigits(uint64_t number)
{
	int digitCounter = 1;

	while(number /= 10) ++digitCounter;

	return digitCounter;
}

tuple<uint64_t, uint64_t> splitNumber(uint64_t number)
{
	AOC_ASSERT(number >= 1);
	AOC_ASSERT(isEven(numDigits(number)));

	const string numberStr = to_string(number);

	AOC_ASSERT(numberStr.size() > 1);
	AOC_ASSERT(isEven(numberStr.size()));

	const size_t substrSize = numberStr.size() / 2;

	const string leftSubstr = numberStr.substr(0, substrSize);
	const string rightSubstr = StringUtils::ltrim(numberStr.substr(substrSize, substrSize), '0');

	return make_tuple(stoll(leftSubstr),
	                  (!rightSubstr.empty()) ? stoll(rightSubstr) : 0);
}

void parseStoneNumbers(const string& line,
                       list<uint64_t>& stones)
{
	const auto tokens = StringUtils::tokenize(line, ' ', true, true);

	AOC_ENFORCE(tokens.size() > 0);

	for(const auto& token : tokens)
		stones.push_back(stoll(token));
}

void parseStoneNumbers2(const string& line,
                        map<uint64_t, uint64_t>& stones)
{
	const auto tokens = StringUtils::tokenize(line, ' ', true, true);

	AOC_ENFORCE(tokens.size() > 0);

	for(const auto& token : tokens)
		++stones[stoll(token)];
}

void blink(list<uint64_t>& stones)
{
	for(auto it=stones.begin(); it!=stones.end(); ++it)
	{
		if(*it == 0)
		{
			*it = 1;
		}
		else if(isEven(numDigits(*it)))
		{
			string numberStr(to_string(*it));

			const auto [left, right] = splitNumber(*it);

			*it = left;

			it = stones.insert(++it, right);
		}
		else
		{
			*it *= 2024;
		}
	}
}

void blink2(map<uint64_t, uint64_t>& stones)
{
	map<uint64_t, uint64_t> nextStones = stones;

	for(const auto& entry : stones)
	{
		const uint64_t stoneNumber = entry.first;
		const uint64_t stoneCounter = entry.second;

		if(stoneCounter == 0)
			continue;

		nextStones[stoneNumber] -= stoneCounter;

		if(stoneNumber == 0)
		{
			nextStones[1] += stoneCounter;
		}
		else if(isEven(numDigits(stoneNumber)))
		{
			string numberStr(to_string(stoneNumber));

			const auto [leftNumber, rightNumber] = splitNumber(stoneNumber);

			nextStones[leftNumber] += stoneCounter;
			nextStones[rightNumber] += stoneCounter;
		}
		else
		{
			nextStones[stoneNumber * 2024] += stoneCounter;
		}
	}

	stones.swap(nextStones);
}

uint64_t numStones(const map<uint64_t, uint64_t>& stones)
{
	uint64_t numStones = 0;

	for(const auto& entry : stones)
		numStones += entry.second;

	return numStones;
}

string stonesToString(const map<uint64_t, uint64_t>& stones)
{
	string result("[ ");

	for(const auto& entry : stones)
		if(entry.second > 0)
			result += format("{}:{} ", entry.first, entry.second);

	result += "]";

	return result;
}

#ifdef AOC_TEST_SOLUTION

TEST_CASE("numDigits")
{
	CHECK(numDigits(0) == 1);
	CHECK(numDigits(1) == 1);
	CHECK(numDigits(10) == 2);
	CHECK(numDigits(99) == 2);
	CHECK(numDigits(999) == 3);
}

TEST_CASE("splitNumber")
{
	CHECK(splitNumber(10) == make_tuple(1, 0));
	CHECK(splitNumber(99) == make_tuple(9, 9));
	CHECK(splitNumber(1000) == make_tuple(10, 0));
	CHECK(splitNumber(1099) == make_tuple(10, 99));
}

TEST_CASE("parseStoneNumbers")
{
	list<uint64_t> stones;

	parseStoneNumbers("0 1 10 99 999", stones);

	CHECK(stones == list<uint64_t>({ 0, 1, 10, 99, 999 }));
}

TEST_CASE("blink")
{
	list<uint64_t> stones1;

	parseStoneNumbers("0 1 10 99 999", stones1);

	blink(stones1);

	CHECK(stones1 == list<uint64_t>({ 1, 2024, 1, 0, 9, 9, 2021976 }));

	list<uint64_t> stones2;

	parseStoneNumbers("125 17", stones2);

	for(size_t blinkCounter=0; blinkCounter<6; ++blinkCounter)
		blink(stones2);

	CHECK(stones2 == list<uint64_t>({ 2097446912, 14168, 4048, 2, 0, 2, 4, 40, 48, 2024, 40, 48, 80, 96, 2, 8, 6, 7, 6, 0, 3, 2 }));
}

TEST_CASE("parseStoneNumbers2")
{
	map<uint64_t, uint64_t> stones;

	parseStoneNumbers2("0 1 10 99 999", stones);

	CHECK(stones == map<uint64_t, uint64_t>({ { 0, 1 },
	                                          { 1, 1 },
	                                          { 10, 1 },
	                                          { 99, 1 },
	                                          { 999, 1 } }));
}

TEST_CASE("numStones")
{
	map<uint64_t, uint64_t> stones;

	parseStoneNumbers2("0 0 1 1 99 999 1000", stones);

	CHECK(numStones(stones) == 7);
}

TEST_CASE("blink2")
{
	map<uint64_t, uint64_t> stones1;

	parseStoneNumbers2("0 1 10 99 999", stones1);
	CHECK(numStones(stones1) == 5);
	CHECK(stonesToString(stones1) == "[ 0:1 1:1 10:1 99:1 999:1 ]");

	blink2(stones1);
	CHECK(numStones(stones1) == 7);
	CHECK(stonesToString(stones1) == "[ 0:1 1:2 9:2 2024:1 2021976:1 ]");

	map<uint64_t, uint64_t> stones2;

	parseStoneNumbers2("125 17", stones2); // Initial arrangement
	CHECK(numStones(stones2) == 2);
	CHECK(stonesToString(stones2) == "[ 17:1 125:1 ]");

	blink2(stones2); // After 1 blink
	CHECK(numStones(stones2) == 3);
	CHECK(stonesToString(stones2) == "[ 1:1 7:1 253000:1 ]");

	blink2(stones2); // After 2 blinks
	CHECK(numStones(stones2) == 4);
	CHECK(stonesToString(stones2) == "[ 0:1 253:1 2024:1 14168:1 ]");

	blink2(stones2); // After 3 blinks
	CHECK(numStones(stones2) == 5);
	CHECK(stonesToString(stones2) == "[ 1:1 20:1 24:1 512072:1 28676032:1 ]");

	blink2(stones2); // After 4 blinks
	CHECK(numStones(stones2) == 9);
	CHECK(stonesToString(stones2) == "[ 0:1 2:2 4:1 72:1 512:1 2024:1 2867:1 6032:1 ]");

	blink2(stones2); // After 5 blinks
	CHECK(numStones(stones2) == 13);
	CHECK(stonesToString(stones2) == "[ 1:1 2:1 7:1 20:1 24:1 28:1 32:1 60:1 67:1 4048:2 8096:1 1036288:1 ]");

	blink2(stones2); // After 6 blinks
	CHECK(numStones(stones2) == 22);
	CHECK(stonesToString(stones2) == "[ 0:2 2:4 3:1 4:1 6:2 7:1 8:1 40:2 48:2 80:1 96:1 2024:1 4048:1 14168:1 2097446912:1 ]");
}

#else // #ifdef AOC_TEST_SOLUTION

class PlutonianPebbles : public PuzzleSolution<uint64_t>
{
private:
	void processInput(const string& puzzleInputFilePath) override;
	uint64_t answer1() override;
	uint64_t answer2() override;

private:
	list<uint64_t> stones_;
	map<uint64_t, uint64_t> stones2_;
};

void PlutonianPebbles::processInput(const string& puzzleInputFilePath)
{
	PuzzleSolution::processInput(puzzleInputFilePath);

	AOC_ENFORCE(input().size() == 1);

	parseStoneNumbers(input()[0], stones_);
	parseStoneNumbers2(input()[0], stones2_);
}

uint64_t PlutonianPebbles::answer1()
{
	for(size_t blinkCounter=0; blinkCounter<25; ++blinkCounter)
		blink(stones_);

	return stones_.size();
}

uint64_t PlutonianPebbles::answer2()
{
	for(size_t blinkCounter=0; blinkCounter<75; ++blinkCounter)
		blink2(stones2_);

	return numStones(stones2_);
}

int main(int argc, char* argv[])
{
	return PlutonianPebbles().run((argc > 1) ? argv[1] : "202411.txt");
}

#endif // #ifdef AOC_TEST_SOLUTION
