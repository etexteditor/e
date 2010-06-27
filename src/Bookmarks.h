#ifndef __BOOKMARKS_H__
#define __BOOKMARKS_H__

#include <vector>
#include <map>

const unsigned int NO_BOOKMARK = (unsigned int)-1;

class ILinePositions;
struct cxBookmark;
struct cxChange;


class Bookmarks {
public:
	Bookmarks(const ILinePositions& lines): lines(lines){};

	const std::vector<cxBookmark>& GetBookmarks() const;

	void AddBookmark(unsigned int line_id, bool toggle=false);
	void DeleteBookmark(unsigned int line_id);
	void Clear();

	// Returns the lineid of the next bookmark after the given one.
	unsigned int NextBookmark(const unsigned int line_id) const;
	unsigned int PrevBookmark(const unsigned int line_id) const;

	// Adjust positions up for inserted characters
	void InsertChars(unsigned int pos, unsigned int length);

	// Adjust positions down for deleted characters
	void DeleteChars(unsigned int start, unsigned int end);

	void ApplyChanges(const std::vector<cxChange>& changes);

	void BuildMap(std::map<int, bool>& bookmarksMap) const;

private:
	std::vector<cxBookmark> bookmarks;
	const ILinePositions& lines;

	Bookmarks& operator = (const Bookmarks& other);
};

#endif
