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

class CharacterHistogram
{
public:
	CharacterHistogram() { reset(); }

public:
	void reset();

public:
	void update(char character);
	size_t numCharactersDetected() const;
	char findTheHighestFrequencyCharacter() const;
	char findTheLowestFrequencyCharacter() const;

public:
	static constexpr size_t asciiAlphabetSize = 'z' - 'a' + 1;

#ifdef AOC_TEST_SOLUTION
public:
#else
private:
#endif
	inline static char characterToCharacterIndex(char character);
	inline static char characterIndexToCharacter(size_t characterIndex);

private:
	size_t frequencyFor_[asciiAlphabetSize];
};

void CharacterHistogram::reset()
{
	for(size_t i=0; i<asciiAlphabetSize; ++i)
		frequencyFor_[i] = 0;
}

void CharacterHistogram::update(char character)
{
	AOC_ENFORCE(character >= 'a' && character <= 'z');

	++frequencyFor_[characterToCharacterIndex(character)];
}

size_t CharacterHistogram::numCharactersDetected() const
{
	size_t numCharactersDetected = 0;

	for(size_t i=0; i<asciiAlphabetSize; ++i)
		if(frequencyFor_[i] > 0)
			++numCharactersDetected;

	return numCharactersDetected;
}

char CharacterHistogram::findTheHighestFrequencyCharacter() const
{
	AOC_ENFORCE(numCharactersDetected() > 0);

	size_t i = 0;
	size_t maxFrequency = 0;
	size_t maxFrequencyCharacterIndex = 0;

	for(; i<asciiAlphabetSize; ++i)
	{
		if(frequencyFor_[i] > 0)
		{
			maxFrequency = frequencyFor_[i];
			maxFrequencyCharacterIndex = i;

			break;
		}
	}

	for(; i<asciiAlphabetSize; ++i)
	{
		if(frequencyFor_[i] > 0 &&
		   frequencyFor_[i] > maxFrequency)
		{
			maxFrequency = frequencyFor_[i];
			maxFrequencyCharacterIndex = i;
		}
	}

	return characterIndexToCharacter(maxFrequencyCharacterIndex);
}

char CharacterHistogram::findTheLowestFrequencyCharacter() const
{
	AOC_ENFORCE(numCharactersDetected() > 0);

	size_t i = 0;
	size_t minFrequency = 0;
	size_t minFrequencyCharacterIndex = 0;

	for(; i<asciiAlphabetSize; ++i)
	{
		if(frequencyFor_[i] > 0)
		{
			minFrequency = frequencyFor_[i];
			minFrequencyCharacterIndex = i;

			break;
		}
	}

	for(; i<asciiAlphabetSize; ++i)
	{
		if(frequencyFor_[i] > 0 &&
		   frequencyFor_[i] < minFrequency)
		{
			minFrequency = frequencyFor_[i];
			minFrequencyCharacterIndex = i;
		}
	}

	return characterIndexToCharacter(minFrequencyCharacterIndex);
}

inline char CharacterHistogram::characterToCharacterIndex(char character)
{
	AOC_ASSERT(character >= 'a' && character <= 'z');

	return character - 'a';
}

inline char CharacterHistogram::characterIndexToCharacter(size_t characterIndex)
{
	AOC_ASSERT(characterIndex < asciiAlphabetSize);

	return 'a' + characterIndex;
}

char findTheHighestFrequencyCharacterForPosition(const vector<string>& input,
                                                 size_t position)
{
	CharacterHistogram histogram;

	for(const auto& line : input)
	{
		AOC_ENFORCE(position < line.size());

		histogram.update(line[position]);
	}

	return histogram.findTheHighestFrequencyCharacter();
}

char findTheLowestFrequencyCharacterForPosition(const vector<string>& input,
                                                size_t position)
{
	CharacterHistogram histogram;

	for(const auto& line : input)
	{
		AOC_ENFORCE(position < line.size());

		histogram.update(line[position]);
	}

	return histogram.findTheLowestFrequencyCharacter();
}

string generateErrorCorrectedMessage(const vector<string>& input)
{
	AOC_ENFORCE(input.size() > 0);

	const size_t inputLineLength = input[0].size();

	string errorCorrectedMessage;

	for(size_t position=0; position<inputLineLength; ++position)
		errorCorrectedMessage += findTheHighestFrequencyCharacterForPosition(input, position);

	return errorCorrectedMessage;
}

string generateErrorCorrectedMessage2(const vector<string>& input)
{
	AOC_ENFORCE(input.size() > 0);

	const size_t inputLineLength = input[0].size();

	string errorCorrectedMessage;

	for(size_t position=0; position<inputLineLength; ++position)
		errorCorrectedMessage += findTheLowestFrequencyCharacterForPosition(input, position);

	return errorCorrectedMessage;
}

#ifdef AOC_TEST_SOLUTION

TEST_CASE("CharacterHistogram")
{
	// 01234567890123456789012345
	// abcdefghijklmnopqrstuvwxyz

	SUBCASE("characterToCharacterIndex()")
	{
		CHECK(CharacterHistogram::characterToCharacterIndex('a') == 0);
		CHECK(CharacterHistogram::characterToCharacterIndex('k') == 10);
		CHECK(CharacterHistogram::characterToCharacterIndex('u') == 20);
		CHECK(CharacterHistogram::characterToCharacterIndex('z') == CharacterHistogram::asciiAlphabetSize - 1);
	}

	SUBCASE("characterIndexToCharacter()")
	{
		CHECK(CharacterHistogram::characterIndexToCharacter(0) == 'a');
		CHECK(CharacterHistogram::characterIndexToCharacter(10) == 'k');
		CHECK(CharacterHistogram::characterIndexToCharacter(20) == 'u');
		CHECK(CharacterHistogram::characterIndexToCharacter(CharacterHistogram::asciiAlphabetSize - 1) == 'z');
	}

	CharacterHistogram histogram;

	SUBCASE("smoke")
	{
		CHECK(histogram.numCharactersDetected() == 0);

		histogram.update('a');
		CHECK(histogram.numCharactersDetected() == 1);
		CHECK(histogram.findTheHighestFrequencyCharacter() == 'a');
		CHECK(histogram.findTheLowestFrequencyCharacter() == 'a');

		histogram.update('z');
		CHECK(histogram.numCharactersDetected() == 2);
		CHECK(histogram.findTheHighestFrequencyCharacter() == 'a');
		CHECK(histogram.findTheLowestFrequencyCharacter() == 'a');

		histogram.update('a');
		CHECK(histogram.numCharactersDetected() == 2);
		CHECK(histogram.findTheHighestFrequencyCharacter() == 'a');
		CHECK(histogram.findTheLowestFrequencyCharacter() == 'z');

		histogram.update('q');
		histogram.update('q');
		histogram.update('q');
		histogram.update('q');
		histogram.update('z');
		histogram.update('z');
		CHECK(histogram.numCharactersDetected() == 3);
		CHECK(histogram.findTheHighestFrequencyCharacter() == 'q');
		CHECK(histogram.findTheLowestFrequencyCharacter() == 'a');

		histogram.reset();
		CHECK(histogram.numCharactersDetected() == 0);
	}
}

const vector<string> input =
{
	"eedadn",
	"drvtee",
	"eandsr",
	"raavrd",
	"atevrs",
	"tsrnev",
	"sdttsa",
	"rasrtv",
	"nssdts",
	"ntnada",
	"svetve",
	"tesnvt",
	"vntsnd",
	"vrdear",
	"dvrsen",
	"enarar",
};

TEST_CASE("findTheHighestFrequencyCharacterForPosition()")
{
	CHECK(findTheHighestFrequencyCharacterForPosition(input, 0) == 'e');
	CHECK(findTheHighestFrequencyCharacterForPosition(input, 1) == 'a');
	CHECK(findTheHighestFrequencyCharacterForPosition(input, 2) == 's');
	CHECK(findTheHighestFrequencyCharacterForPosition(input, 3) == 't');
	CHECK(findTheHighestFrequencyCharacterForPosition(input, 4) == 'e');
	CHECK(findTheHighestFrequencyCharacterForPosition(input, 5) == 'r');
}

TEST_CASE("generateErrorCorrectedMessage()")
{
	CHECK(generateErrorCorrectedMessage(input) == "easter");
}

TEST_CASE("generateErrorCorrectedMessage2()")
{
	CHECK(generateErrorCorrectedMessage2(input) == "advent");
}

#else // #ifdef AOC_TEST_SOLUTION

class SignalsAndNoise : public PuzzleSolution<string>
{
private:
	string answer1() override;
	string answer2() override;
};

string SignalsAndNoise::answer1()
{
	return generateErrorCorrectedMessage(input());
}

string SignalsAndNoise::answer2()
{
	return generateErrorCorrectedMessage2(input());
}

int main(int argc, char* argv[])
{
	return SignalsAndNoise().run((argc > 1) ? argv[1] : "201606.txt");
}

#endif // #ifdef AOC_TEST_SOLUTION
