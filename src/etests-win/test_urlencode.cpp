#include "stdafx.h"
#include <limits.h>
#include "urlencode.h"
#include <gtest/gtest.h>

TEST(urlencodeTest, encodeFilename) {
	wxString encoded = UrlEncode::EncodeFilename(wxT("this is a file / name"));
	EXPECT_STREQ(
		wxT("this is a file %2f name"),
		encoded
	);
}
