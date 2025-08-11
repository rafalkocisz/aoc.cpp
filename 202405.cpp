#include "aoc.h"

#ifdef AOC_TEST_SOLUTION
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"
#endif

#define FMT_HEADER_ONLY
#include "fmt/format.h"

using namespace fmt;
using namespace aoc;
using namespace std;

tuple<int, int> parsePageOrderingRule(const string& line)
{
	auto tokens = StringUtils::tokenize(line, '|', false, true);

	AOC_ENFORCE(tokens.size() == 2);

	return make_tuple(stoi(tokens[0]), stoi(tokens[1]));
}

vector<int> parsePageNumbers(const string& line)
{
	auto tokens = StringUtils::tokenize(line, ',', false, false);

	AOC_ENFORCE(tokens.size() > 0);

	vector<int> pageNumbers;

	for(const auto& token : tokens)
		pageNumbers.push_back(stoi(token));

	return pageNumbers;
}

class PageOrderingRuleSet
{
public:
	void addRule(const tuple<int, int>& rule);
	bool hasRuleFor(int pageNumber) const { return rules_.contains(pageNumber); }
	size_t numRules() const { return rules_.size(); }
	bool check(int pageNumber1, int pageNumber2) const;
	void swap(PageOrderingRuleSet& other) { rules_.swap(other.rules_); }

private:
	map<int, set<int>> rules_;
};

void PageOrderingRuleSet::addRule(const tuple<int, int>& rule)
{
	const auto[precedingPageNumber, followingPageNumber] = rule;

	rules_[precedingPageNumber].insert(followingPageNumber);

	AOC_ASSERT(hasRuleFor(precedingPageNumber));
}

bool PageOrderingRuleSet::check(int pageNumber1, int pageNumber2) const
{
	if(auto found = rules_.find(pageNumber1); found != rules_.end())
		return found->second.contains(pageNumber2);

	return false;
}

PageOrderingRuleSet parsePageOrderingRuleSet(const vector<string>& lines,
                                             size_t& lastPageOrderingRuleLineIndex)
{
	PageOrderingRuleSet ruleSet;

	for(lastPageOrderingRuleLineIndex=0;
	    lastPageOrderingRuleLineIndex<lines.size();
	  ++lastPageOrderingRuleLineIndex)
	{
		if(lines[lastPageOrderingRuleLineIndex].empty())
			return ruleSet;

		ruleSet.addRule(parsePageOrderingRule(lines[lastPageOrderingRuleLineIndex]));
	}

	return ruleSet;
}

tuple<PageOrderingRuleSet, vector<vector<int>>> parsePageOrderingRuleSetAndManualUpdates(const vector<string>& lines)
{
	size_t lastPageOrderingRuleLineIndex = 0;

	PageOrderingRuleSet ruleSet = parsePageOrderingRuleSet(lines, lastPageOrderingRuleLineIndex);

	vector<vector<int>> manualUpdates;

	for(size_t i=lastPageOrderingRuleLineIndex+1; i<lines.size(); ++i)
		manualUpdates.push_back(parsePageNumbers(lines[i]));

	return make_tuple(ruleSet, manualUpdates);
}

bool isManualUpdateInCorrectOrder(const PageOrderingRuleSet& ruleSet,
                                  const vector<int>& manualUpdatePageNumbers)
{
	for(size_t i=0; i<manualUpdatePageNumbers.size(); ++i)
		for(size_t j=0; j<manualUpdatePageNumbers.size(); ++j)
			if(j > i)
				if(!ruleSet.check(manualUpdatePageNumbers[i],
				                  manualUpdatePageNumbers[j]))
					return false;

	return true;
}

bool checkManualUpdateOrderAndRearrange(const PageOrderingRuleSet& ruleSet,
                                        vector<int>& manualUpdatePageNumbers)
{
	for(size_t i=0; i<manualUpdatePageNumbers.size(); ++i)
	{
		for(size_t j=0; j<manualUpdatePageNumbers.size(); ++j)
		{
			if(j > i)
			{
				if(!ruleSet.check(manualUpdatePageNumbers[i],
				                  manualUpdatePageNumbers[j]))
				{
					swap(manualUpdatePageNumbers[i],
					     manualUpdatePageNumbers[j]);

					return false;
				}
			}
		}
	}

	return true;
}

void fixManualUpdate(const PageOrderingRuleSet& ruleSet,
                     vector<int>& manualUpdatePageNumbers)
{
	bool isManualUpdateCorrect = false;

	do
	{
		isManualUpdateCorrect = checkManualUpdateOrderAndRearrange(ruleSet,
	                                                               manualUpdatePageNumbers);
	}
	while(!isManualUpdateCorrect);
}

int middlePageNumber(const vector<int>& manualUpdatePageNumbers)
{
	AOC_ASSERT(manualUpdatePageNumbers.size() > 0);
	AOC_ASSERT(isOdd(manualUpdatePageNumbers.size()));

	return manualUpdatePageNumbers[manualUpdatePageNumbers.size() / 2];
}

int sumOfCorrectManualUpdatesMiddlePageNumbers(const PageOrderingRuleSet& ruleSet,
                                               const vector<vector<int>>& manualUpdates)
{
	int sum = 0;

	for(const auto& manualUpdatePageNumbers : manualUpdates)
		if(isManualUpdateInCorrectOrder(ruleSet, manualUpdatePageNumbers))
			sum += middlePageNumber(manualUpdatePageNumbers);

	return sum;
}

int sumOfCorrectedManualUpdatesMiddlePageNumbers(const PageOrderingRuleSet& ruleSet,
                                                 vector<vector<int>>& manualUpdates)
{
	int sum = 0;

	for(auto& manualUpdatePageNumbers : manualUpdates)
	{
		if(!isManualUpdateInCorrectOrder(ruleSet, manualUpdatePageNumbers))
		{
			fixManualUpdate(ruleSet, manualUpdatePageNumbers);

			sum += middlePageNumber(manualUpdatePageNumbers);
		}
	}

	return sum;
}

#ifdef AOC_TEST_SOLUTION

TEST_CASE("parsePageOrderingRule")
{
	CHECK(parsePageOrderingRule("47|53") == make_tuple(47, 53));
}

TEST_CASE("parsePageNumbers")
{
	CHECK(parsePageNumbers("75,47,61,53,29") == vector<int>{ 75, 47, 61, 53, 29 });
}

TEST_CASE("PageOrderingRuleSet")
{
	PageOrderingRuleSet ruleSet;

	ruleSet.addRule(make_tuple(47, 53));

	CHECK(ruleSet.hasRuleFor(47));
	CHECK(!ruleSet.hasRuleFor(97));

	ruleSet.addRule(make_tuple(97, 13));

	CHECK(ruleSet.hasRuleFor(47));
	CHECK(ruleSet.hasRuleFor(97));

	CHECK( ruleSet.check(47, 53));
	CHECK( ruleSet.check(97, 13));
	CHECK(!ruleSet.check(47, 52));
	CHECK(!ruleSet.check(98, 13));
}

TEST_CASE("parsePageOrderingRuleSet")
{
	const vector<string> lines =
	{
		"47|53",
		"97|13",
		"97|61",
		"97|47",
		"75|29",
		"61|13",
		"75|53",
		"29|13",
		"97|29",
		"53|29",
		"61|53",
		"97|53",
		"61|29",
		"47|13",
		"75|47",
		"97|75",
		"47|61",
		"75|61",
		"47|29",
		"75|13",
		"53|13",
	};

	size_t lastPageOrderingRuleLineIndex = 0;

	PageOrderingRuleSet ruleSet = parsePageOrderingRuleSet(lines,
	                                                       lastPageOrderingRuleLineIndex);

	CHECK(lastPageOrderingRuleLineIndex == lines.size());
	CHECK(ruleSet.numRules() == 6);

	CHECK(ruleSet.hasRuleFor(47));
	CHECK(ruleSet.hasRuleFor(97));
	CHECK(ruleSet.hasRuleFor(75));
	CHECK(ruleSet.hasRuleFor(61));
	CHECK(ruleSet.hasRuleFor(29));
	CHECK(ruleSet.hasRuleFor(53));
}

TEST_CASE("parsePageOrderingRuleSetAndManualUpdates")
{
	const vector<string> lines =
	{
		"47|53",
		"97|13",
		"97|61",
		"97|47",
		"75|29",
		"61|13",
		"75|53",
		"29|13",
		"97|29",
		"53|29",
		"61|53",
		"97|53",
		"61|29",
		"47|13",
		"75|47",
		"97|75",
		"47|61",
		"75|61",
		"47|29",
		"75|13",
		"53|13",
		"",
		"75,47,61,53,29",
		"97,61,53,29,13",
		"75,29,13",
		"75,97,47,61,53",
		"61,13,29",
		"97,13,75,29,47",
	};

	const auto[ruleSet, manualUpdates] = parsePageOrderingRuleSetAndManualUpdates(lines);

	CHECK(ruleSet.check(47, 53));
	CHECK(ruleSet.check(97, 13));
	CHECK(ruleSet.check(97, 61));
	CHECK(ruleSet.check(97, 47));
	CHECK(ruleSet.check(75, 29));
	CHECK(ruleSet.check(61, 13));
	CHECK(ruleSet.check(75, 53));
	CHECK(ruleSet.check(29, 13));
	CHECK(ruleSet.check(97, 29));
	CHECK(ruleSet.check(53, 29));
	CHECK(ruleSet.check(61, 53));
	CHECK(ruleSet.check(97, 53));
	CHECK(ruleSet.check(61, 29));
	CHECK(ruleSet.check(47, 13));
	CHECK(ruleSet.check(75, 47));
	CHECK(ruleSet.check(97, 75));
	CHECK(ruleSet.check(47, 61));
	CHECK(ruleSet.check(75, 61));
	CHECK(ruleSet.check(47, 29));
	CHECK(ruleSet.check(75, 13));
	CHECK(ruleSet.check(53, 13));

	CHECK(manualUpdates.size() == 6);

	CHECK(manualUpdates[0] == vector<int>{ 75, 47, 61, 53, 29 });
	CHECK(manualUpdates[1] == vector<int>{ 97, 61, 53, 29, 13 });
	CHECK(manualUpdates[2] == vector<int>{ 75, 29, 13 });
	CHECK(manualUpdates[3] == vector<int>{ 75, 97, 47, 61, 53 });
	CHECK(manualUpdates[4] == vector<int>{ 61, 13, 29 });
	CHECK(manualUpdates[5] == vector<int>{ 97, 13, 75, 29, 47 });
}

TEST_CASE("isManualUpdateInCorrectOrder")
{
	const vector<string> lines =
	{
		"47|53",
		"97|13",
		"97|61",
		"97|47",
		"75|29",
		"61|13",
		"75|53",
		"29|13",
		"97|29",
		"53|29",
		"61|53",
		"97|53",
		"61|29",
		"47|13",
		"75|47",
		"97|75",
		"47|61",
		"75|61",
		"47|29",
		"75|13",
		"53|13",
	};

	size_t lastPageOrderingRuleLineIndex = 0;

	PageOrderingRuleSet ruleSet = parsePageOrderingRuleSet(lines,
	                                                       lastPageOrderingRuleLineIndex);

	CHECK(lastPageOrderingRuleLineIndex == lines.size());
	CHECK(ruleSet.numRules() == 6);

	CHECK( isManualUpdateInCorrectOrder(ruleSet, vector<int>{ 75, 47, 61, 53, 29 }));
	CHECK( isManualUpdateInCorrectOrder(ruleSet, vector<int>{ 97, 61, 53, 29, 13 }));
	CHECK( isManualUpdateInCorrectOrder(ruleSet, vector<int>{ 75, 29, 13 }));
	CHECK(!isManualUpdateInCorrectOrder(ruleSet, vector<int>{ 75, 97, 47, 61, 53 }));
	CHECK(!isManualUpdateInCorrectOrder(ruleSet, vector<int>{ 61, 13, 29 }));
	CHECK(!isManualUpdateInCorrectOrder(ruleSet, vector<int>{ 97, 13, 75, 29, 47 }));
}

TEST_CASE("middlePageNumber")
{
	CHECK(middlePageNumber(vector<int>{ 75, 47, 61, 53, 29 }) == 61);
	CHECK(middlePageNumber(vector<int>{ 97, 61, 53, 29, 13 }) == 53);
	CHECK(middlePageNumber(vector<int>{ 75, 29, 13 }) == 29);
	CHECK(middlePageNumber(vector<int>{ 75, 97, 47, 61, 53 }) == 47);
	CHECK(middlePageNumber(vector<int>{ 61, 13, 29 }) == 13);
	CHECK(middlePageNumber(vector<int>{ 97, 13, 75, 29, 47 }) == 75);
}

TEST_CASE("sumOfCorrectManualUpdatesMiddlePageNumbers")
{
	const vector<string> lines =
	{
		"47|53",
		"97|13",
		"97|61",
		"97|47",
		"75|29",
		"61|13",
		"75|53",
		"29|13",
		"97|29",
		"53|29",
		"61|53",
		"97|53",
		"61|29",
		"47|13",
		"75|47",
		"97|75",
		"47|61",
		"75|61",
		"47|29",
		"75|13",
		"53|13",
		"",
		"75,47,61,53,29",
		"97,61,53,29,13",
		"75,29,13",
		"75,97,47,61,53",
		"61,13,29",
		"97,13,75,29,47",
	};

	const auto[ruleSet, manualUpdates] = parsePageOrderingRuleSetAndManualUpdates(lines);

	CHECK(sumOfCorrectManualUpdatesMiddlePageNumbers(ruleSet, manualUpdates) == 143);
}

TEST_CASE("fixManualUpdate")
{
	const vector<string> lines =
	{
		"47|53",
		"97|13",
		"97|61",
		"97|47",
		"75|29",
		"61|13",
		"75|53",
		"29|13",
		"97|29",
		"53|29",
		"61|53",
		"97|53",
		"61|29",
		"47|13",
		"75|47",
		"97|75",
		"47|61",
		"75|61",
		"47|29",
		"75|13",
		"53|13",
	};

	size_t lastPageOrderingRuleLineIndex = 0;

	PageOrderingRuleSet ruleSet = parsePageOrderingRuleSet(lines,
	                                                       lastPageOrderingRuleLineIndex);

	CHECK(lastPageOrderingRuleLineIndex == lines.size());
	CHECK(ruleSet.numRules() == 6);

	{
		vector<int> manualUpdate1{ 75, 97, 47, 61, 53 };

		CHECK(!isManualUpdateInCorrectOrder(ruleSet, manualUpdate1));

		fixManualUpdate(ruleSet, manualUpdate1);

		CHECK(isManualUpdateInCorrectOrder(ruleSet, manualUpdate1));
		CHECK(manualUpdate1 == vector<int>{ 97, 75, 47, 61, 53 });
	}

	{
		vector<int> manualUpdate2{ 61, 13, 29 };

		CHECK(!isManualUpdateInCorrectOrder(ruleSet, manualUpdate2));

		fixManualUpdate(ruleSet, manualUpdate2);

		CHECK(isManualUpdateInCorrectOrder(ruleSet, manualUpdate2));
		CHECK(manualUpdate2 == vector<int>{ 61, 29, 13 });
	}

	{
		vector<int> manualUpdate3{ 97, 13, 75, 29, 47 };

		CHECK(!isManualUpdateInCorrectOrder(ruleSet, manualUpdate3));

		fixManualUpdate(ruleSet, manualUpdate3);

		CHECK(isManualUpdateInCorrectOrder(ruleSet, manualUpdate3));
		CHECK(manualUpdate3 == vector<int>{ 97, 75, 47, 29, 13 });
	}
}

TEST_CASE("sumOfCorrectedManualUpdatesMiddlePageNumbers")
{
	const vector<string> lines =
	{
		"47|53",
		"97|13",
		"97|61",
		"97|47",
		"75|29",
		"61|13",
		"75|53",
		"29|13",
		"97|29",
		"53|29",
		"61|53",
		"97|53",
		"61|29",
		"47|13",
		"75|47",
		"97|75",
		"47|61",
		"75|61",
		"47|29",
		"75|13",
		"53|13",
		"",
		"75,47,61,53,29",
		"97,61,53,29,13",
		"75,29,13",
		"75,97,47,61,53",
		"61,13,29",
		"97,13,75,29,47",
	};

	auto[ruleSet, manualUpdates] = parsePageOrderingRuleSetAndManualUpdates(lines);

	CHECK(sumOfCorrectedManualUpdatesMiddlePageNumbers(ruleSet, manualUpdates) == 123);
}

#else // #ifdef AOC_TEST_SOLUTION

class PrintQueue : public PuzzleSolution<int>
{
private:
	void processInput(const string& puzzleInputFilePath) override;
	int answer1() override;
	int answer2() override;

private:
	PageOrderingRuleSet ruleSet_;
	vector<vector<int>> manualUpdates_;
};

void PrintQueue::processInput(const string& puzzleInputFilePath)
{
	PuzzleSolution::processInput(puzzleInputFilePath);

	auto[ruleSet, manualUpdates] = parsePageOrderingRuleSetAndManualUpdates(input());

	ruleSet_.swap(ruleSet);
	manualUpdates_.swap(manualUpdates);
}

int PrintQueue::answer1()
{
	return sumOfCorrectManualUpdatesMiddlePageNumbers(ruleSet_, manualUpdates_);
}

int PrintQueue::answer2()
{
	return sumOfCorrectedManualUpdatesMiddlePageNumbers(ruleSet_, manualUpdates_);
}

int main(int argc, char* argv[])
{
	return PrintQueue().run((argc > 1) ? argv[1] : "202405.txt");
}

#endif // #ifdef AOC_TEST_SOLUTION
