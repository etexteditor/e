#include "stdafx.h"
#include <limits.h>
#include <wx/string.h>
#include "tmKey.h"
#include <gtest/gtest.h>
#include <tuple>

// Inputs - int key, int (wxKeyModifier) mod
// Outputs - wxString readable binding
typedef std::tr1::tuple<int, int, wxString> CONFIG;

using ::testing::TestWithParam;
using ::testing::Values;

class TmKeyTest: public TestWithParam<CONFIG>{};

TEST_P(TmKeyTest, KeyBinding) {
	CONFIG config = GetParam();
	int key = std::tr1::get<0>(config);
	int mod = std::tr1::get<1>(config);
	wxString expected_binding = std::tr1::get<2>(config);

	tmKey tm_key(key, mod);
	wxString actual_binding = tm_key.getBinding();

	EXPECT_STREQ(expected_binding, actual_binding);
}

INSTANTIATE_TEST_CASE_P(TmKeyBindingTest, 
	TmKeyTest, 
	Values(
		CONFIG('a', wxMOD_ALT, _("^a")),
		CONFIG('a', wxMOD_CONTROL, _("@a")),
		CONFIG('a', wxMOD_WIN, _("~a")),

		CONFIG('a', wxMOD_ALT|wxMOD_SHIFT, _("^A")),
		CONFIG('a', wxMOD_CONTROL|wxMOD_SHIFT, _("@A")),
		CONFIG('a', wxMOD_WIN|wxMOD_SHIFT, _("~A")),

		// 47 = slash/question mark
		CONFIG(47, wxMOD_CONTROL, _("@/")),
		CONFIG(47, wxMOD_CONTROL|wxMOD_SHIFT, _("@?"))
));
