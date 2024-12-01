#ifndef AOC_H
#define AOC_H

#define FMT_HEADER_ONLY
#include "fmt/format.h"

#include <cstdlib>
#include <cmath>
#include <source_location>
#include <string>
#include <vector>

#define AOC_FUNCTIONIZE(_a, _b) \
	_a(_b)

#define AOC_STRINGIZE(_a) \
	#_a

#define AOC_TO_STRING(_expression) \
	AOC_FUNCTIONIZE(AOC_STRINGIZE, _expression)

#ifndef NDEBUG

#define AOC_ASSERT(_condition) \
	do \
	{ \
		if(!(_condition)) \
		{ \
			aoc::panic("assertion failed with condition '" AOC_TO_STRING(_condition) "'"); \
		} \
	} while(0)

#define AOC_ASSERT_MSG(_condition, _message) \
	do \
	{ \
		if(!(_condition)) \
		{ \
			aoc::panic(_message); \
		} \
	} while(0)

#else // #ifndef NDEBUG

#define AOC_ASSERT(_condition) \
	do \
	{ \
	} while(0)

#define AOC_ASSERT_MSG(_condition, _message) \
	do \
	{ \
	} while(0)

#endif // #ifndef NDEBUG

#define AOC_ENFORCE(_condition) \
	do \
	{ \
		if(!(_condition)) \
		{ \
			aoc::panic("enforcement failed with condition '" AOC_TO_STRING(_condition) "'"); \
		} \
	} while(0)

#define AOC_ENFORCE_MSG(_condition, _message) \
	do \
	{ \
		if(!(_condition)) \
		{ \
			aoc::panic(_message); \
		} \
	} while(0)

#define AOC_NUM_ELEMENTS(_array) \
	(sizeof(_array) / sizeof(*(_array)))

namespace aoc
{

std::vector<std::string> loadPuzzleInput(const std::string& puzzleInputFilePath);

void panic(const std::string& message,
           const std::source_location sourceLocation=std::source_location::current());

template<class T=int>
class PuzzleSolution
{
public:
	virtual ~PuzzleSolution() {}

public:
	int run(const std::string& puzzleInputFilePath);

protected:
	const std::vector<std::string>& input() const { return input_; }

protected:
	virtual void processInput(const std::string& puzzleInputFilePath);

private:
	virtual T answer1() = 0;
	virtual T answer2() = 0;

private:
	std::vector<std::string> input_;
};

template<class T>
int PuzzleSolution<T>::run(const std::string& puzzleInputFilePath)
{
	processInput(puzzleInputFilePath);

	fmt::println("{}", answer1());
	fmt::println("{}", answer2());

	return EXIT_SUCCESS;
}

template<class T>
void PuzzleSolution<T>::processInput(const std::string& puzzleInputFilePath)
{
	input_ = loadPuzzleInput(puzzleInputFilePath);
}

class MathUtils
{
public:
	template<class T>
	static bool isInRange(const T& value, const T& rangeStart, const T& rangeEnd)
	{
		return (value >= rangeStart) && (value <= rangeEnd);
	}

	static int manhattanDistance(int x1, int y1, int x2, int y2)
	{
		return abs(x2 - x1) + abs(y2 - y1);
	}
};

class StringUtils
{
public:
	static bool contains(const std::string& str,
	                     const std::string& substr);
	static bool contains(const std::string& str, char ch);

	static std::string replaceFirst(const std::string& str,
	                                const std::string& substr,
	                                const std::string& substr2);
	static std::string replaceLast(const std::string& str,
	                               const std::string& substr,
	                               const std::string& substr2);
	static std::string replaceAll(const std::string& str,
	                              const std::string& substr,
	                              const std::string& substr2);

	static std::vector<std::string> tokenize(const std::string& str,
	                                         char delimiter=',',
	                                         bool trimWhitespace=true,
	                                         bool skipEmptyTokens=false);

	static std::string trim(const std::string& str);
	static std::string ltrim(const std::string& str);
	static std::string rtrim(const std::string& str);
};

} // namespace aoc

#endif // #ifndef AOC_H
