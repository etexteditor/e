#include "stdafx.h"
#include <limits.h>
#include "eDocumentPath.h"
#include <gtest/gtest.h>

/*
bool eDocumentPath::IsDotDirectory(const wxString& path) {
	return path == wxT(".") || path == wxT("..");
}
*/


TEST(eDocumentPathTest, isDotDirectory) {
  EXPECT_TRUE( eDocumentPath::IsDotDirectory(wxT(".")) );
  EXPECT_TRUE( eDocumentPath::IsDotDirectory(wxT("..")) );
  EXPECT_FALSE( eDocumentPath::IsDotDirectory(wxT(".git")) );
}
