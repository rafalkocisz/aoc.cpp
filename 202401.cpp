#include "aoc.h"

#ifdef AOC_TEST_SOLUTION
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"
#endif

#define FMT_HEADER_ONLY
#include "fmt/format.h"

#include <algorithm>

using namespace fmt;
using namespace aoc;
using namespace std;

tuple<int, int> parseLocationIDs(const string& line)
{
	auto tokens = StringUtils::tokenize(line, ' ', false, true);

	AOC_ENFORCE(tokens.size() == 2);

	return make_tuple(stoi(tokens[0]), stoi(tokens[1]));
}

void parseLocationIDLists(const vector<string>& input,
                          vector<int>& locationIDList1,
                          vector<int>& locationIDList2)
{
	for(const auto& line : input)
	{
		const auto [locationID1, locationID2] = parseLocationIDs(line);

		locationIDList1.push_back(locationID1);
		locationIDList2.push_back(locationID2);
	}
}

int distance(int locationID1, int locationID2)
{
	return abs(locationID2 - locationID1);
}

int totalDistance(vector<int> locationIDList1,
                  vector<int> locationIDList2)
{
	AOC_ENFORCE(locationIDList1.size() == locationIDList2.size());

	sort(locationIDList1.begin(), locationIDList1.end());
	sort(locationIDList2.begin(), locationIDList2.end());

	int totalDistance = 0;

	for(size_t i=0; i<locationIDList1.size(); ++i)
		totalDistance += distance(locationIDList1[i],
		                          locationIDList2[i]);

	return totalDistance;
}

int countOf(int locationID, const vector<int>& locationIDList)
{
	return count(locationIDList.begin(),
	             locationIDList.end(),
	             locationID);
}

int similarityScore(const vector<int>& locationIDList1,
                    const vector<int>& locationIDList2)
{
	AOC_ENFORCE(locationIDList1.size() == locationIDList2.size());

	int similarityScore = 0;

	for(size_t i=0; i<locationIDList1.size(); ++i)
		similarityScore += locationIDList1[i] * countOf(locationIDList1[i], locationIDList2);

	return similarityScore;
}

#ifdef AOC_TEST_SOLUTION

TEST_CASE("parseLocationIDs")
{
	CHECK(parseLocationIDs("3   4") == make_tuple(3, 4));
	CHECK(parseLocationIDs("4   3") == make_tuple(4, 3));
	CHECK(parseLocationIDs("2   5") == make_tuple(2, 5));
	CHECK(parseLocationIDs("1   3") == make_tuple(1, 3));
	CHECK(parseLocationIDs("3   9") == make_tuple(3, 9));
	CHECK(parseLocationIDs("3   3") == make_tuple(3, 3));
}

TEST_CASE("parseLocationIDLists")
{
	const vector<string> input =
	{
		"3   4",
		"4   3",
		"2   5",
		"1   3",
		"3   9",
		"3   3",
	};

	vector<int> locationIDList1;
	vector<int> locationIDList2;

	parseLocationIDLists(input, locationIDList1, locationIDList2);

	CHECK(locationIDList1 == vector<int>({ 3, 4, 2, 1, 3, 3 }));
	CHECK(locationIDList2 == vector<int>({ 4, 3, 5, 3, 9, 3 }));
}

TEST_CASE("distance")
{
	CHECK(distance(1, 3) == 2);
	CHECK(distance(2, 3) == 1);
	CHECK(distance(3, 3) == 0);
	CHECK(distance(3, 4) == 1);
	CHECK(distance(3, 5) == 2);
	CHECK(distance(4, 9) == 5);
}

TEST_CASE("totalDistance")
{
	vector<int> locationIDList1({ 3, 4, 2, 1, 3, 3 });
	vector<int> locationIDList2({ 4, 3, 5, 3, 9, 3 });

	CHECK(totalDistance(locationIDList1, locationIDList2) == 11);
}

TEST_CASE("countOf")
{
	vector<int> locationIDList1({ 3, 4, 2, 1, 3, 3 });
	vector<int> locationIDList2({ 4, 3, 5, 3, 9, 3 });

	CHECK(countOf(locationIDList1[0], locationIDList2) == 3);
	CHECK(countOf(locationIDList1[1], locationIDList2) == 1);
	CHECK(countOf(locationIDList1[2], locationIDList2) == 0);
	CHECK(countOf(locationIDList1[3], locationIDList2) == 0);
	CHECK(countOf(locationIDList1[4], locationIDList2) == 3);
	CHECK(countOf(locationIDList1[5], locationIDList2) == 3);
}

TEST_CASE("similarityScore")
{
	vector<int> locationIDList1({ 3, 4, 2, 1, 3, 3 });
	vector<int> locationIDList2({ 4, 3, 5, 3, 9, 3 });

	CHECK(similarityScore(locationIDList1, locationIDList2) == 31);
}

#else // #ifdef AOC_TEST_SOLUTION

class HistorianHysteria : public PuzzleSolution<int>
{
private:
	void processInput(const string& puzzleInputFilePath) override;
	int answer1() override;
	int answer2() override;

private:
	vector<int> locationIDList1_;
	vector<int> locationIDList2_;
};

void HistorianHysteria::processInput(const string& puzzleInputFilePath)
{
	PuzzleSolution::processInput(puzzleInputFilePath);

	parseLocationIDLists(input(), locationIDList1_, locationIDList2_);
}

int HistorianHysteria::answer1()
{
	return totalDistance(locationIDList1_, locationIDList2_);
}

int HistorianHysteria::answer2()
{
	return similarityScore(locationIDList1_, locationIDList2_);
}

int main(int argc, char* argv[])
{
	return HistorianHysteria().run((argc > 1) ? argv[1] : "202401.txt");
}

#endif // #ifdef AOC_TEST_SOLUTION
