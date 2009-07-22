#include "stdafx.h"
#include <limits.h>
#include "eDocumentPath.h"
#include <gtest/gtest.h>

TEST(eDocumentPathTest, isDotDirectory) {
  EXPECT_TRUE( eDocumentPath::IsDotDirectory(wxT(".")) );
  EXPECT_TRUE( eDocumentPath::IsDotDirectory(wxT("..")) );

  EXPECT_FALSE( eDocumentPath::IsDotDirectory(wxT(".git")) );
}


TEST(eDocumentPathTest, isRemotepath) {
	EXPECT_TRUE( eDocumentPath::IsRemotePath(wxT("ftp://example.com/ftp/folder/file.txt")) );
	EXPECT_TRUE( eDocumentPath::IsRemotePath(wxT("http://example.com/ftp/folder/file.txt")) );

	EXPECT_FALSE( eDocumentPath::IsRemotePath(wxT("c:\\Program Files\\SomeFile.txt")) );
}


TEST(eDocumentPathTest, convertsUncPathsToWin) {
	EXPECT_STREQ( L"\\\\server\\share\\path\\to\\file.txt",
		eDocumentPath::CygwinPathToWin(wxT("//server/share/path/to/file.txt")) );
}


TEST(eDocumentPathTest, convertsUncPathsToUnix) {
	wxFileName uncFile( wxT("\\\\server\\share\\path\\to\\file.txt") );
	EXPECT_STREQ( L"//server/share/path/to/file.txt",
		eDocumentPath::WinPathToCygwin(uncFile) );
}
