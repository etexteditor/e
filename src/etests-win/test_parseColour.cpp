#include "stdafx.h"
#include <limits.h>
#include "Colours.h"
#include <wx/colour.h>
#include <gtest/gtest.h>

TEST(parseColours, invalidColours) {
	wxColour colour;
	unsigned int alpha;

	EXPECT_FALSE( ParseColourAlpha(NULL, colour, alpha) );
	EXPECT_FALSE( ParseColourAlpha("", colour, alpha) );
	EXPECT_FALSE( ParseColourAlpha("#", colour, alpha) );
	EXPECT_FALSE( ParseColourAlpha("1234567", colour, alpha) );
	EXPECT_FALSE( ParseColourAlpha("#7", colour, alpha) );
	EXPECT_FALSE( ParseColourAlpha("#72", colour, alpha) );
	EXPECT_FALSE( ParseColourAlpha("#725", colour, alpha) );
	EXPECT_FALSE( ParseColourAlpha("#7257", colour, alpha) );
	EXPECT_FALSE( ParseColourAlpha("#72572", colour, alpha) );

	EXPECT_FALSE( ParseColourAlpha("#7257257", colour, alpha) );
	EXPECT_FALSE( ParseColourAlpha("#725725743", colour, alpha) );
	EXPECT_FALSE( ParseColourAlpha("#7257257432", colour, alpha) );
}

TEST(parseColours, goodColour) {
	wxColour colour;
	unsigned int alpha = -1;

	EXPECT_TRUE( ParseColourAlpha("#336699", colour, alpha) );
	EXPECT_EQ( 0x33, colour.Red() );
	EXPECT_EQ( 0x66, colour.Green() );
	EXPECT_EQ( 0x99, colour.Blue() );
	EXPECT_EQ( 0, alpha );
}

TEST(parseColours, goodColourWithAlpha) {
	wxColour colour;
	unsigned int alpha = -1;

	EXPECT_TRUE( ParseColourAlpha("#33669970", colour, alpha) );
	EXPECT_EQ( 0x33, colour.Red() );
	EXPECT_EQ( 0x66, colour.Green() );
	EXPECT_EQ( 0x99, colour.Blue() );
	EXPECT_EQ( 0x70, alpha );
}
