#include "stdafx.h"
#include <limits.h>
#include "Strings.h"
#include <gtest/gtest.h>
#include <tuple>

typedef std::tr1::tuple<wxChar, int> CONFIG;

using ::testing::TestWithParam;
using ::testing::Values;

class HexDigitTest: public TestWithParam<CONFIG>{};

TEST_P(HexDigitTest, Convert) {
	CONFIG config = GetParam();
	wxChar hexdigit = std::tr1::get<0>(config);
	int as_number = std::tr1::get<1>(config);

	int converted = HexToNumber(hexdigit);
	EXPECT_EQ(as_number, converted);
}

INSTANTIATE_TEST_CASE_P(HexDigitTest1, 
	HexDigitTest, 
	Values(
		CONFIG(wxT('0'), 0),
		CONFIG(wxT('1'), 1),
		CONFIG(wxT('2'), 2),
		CONFIG(wxT('3'), 3),
		CONFIG(wxT('4'), 4),
		CONFIG(wxT('5'), 5),
		CONFIG(wxT('6'), 6),
		CONFIG(wxT('7'), 7),
		CONFIG(wxT('8'), 8),
		CONFIG(wxT('9'), 9),

		CONFIG(wxT('A'), 10),
		CONFIG(wxT('B'), 11),
		CONFIG(wxT('C'), 12),
		CONFIG(wxT('D'), 13),
		CONFIG(wxT('E'), 14),
		CONFIG(wxT('F'), 15),
		
		CONFIG(wxT('a'), 10),
		CONFIG(wxT('b'), 11),
		CONFIG(wxT('c'), 12),
		CONFIG(wxT('d'), 13),
		CONFIG(wxT('e'), 14),
		CONFIG(wxT('f'), 15),

		CONFIG(wxT('X'), -1),
		CONFIG(wxT('@'), -1)
));
