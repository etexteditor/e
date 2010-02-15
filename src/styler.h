/*******************************************************************************
 *
 * Copyright (C) 2009, Alexander Stigsen, e-texteditor.com
 *
 * This software is licensed under the Open Company License as described
 * in the file license.txt, which you should have received as part of this
 * distribution. The terms are also available at http://opencompany.org/license.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either express or implied.
 *
 ******************************************************************************/

#ifndef __STYLER_H__
#define __STYLER_H__

class StyleRun;

// Abstract base class for stylers
class Styler {
public:
    virtual ~Styler() {};
	virtual void Style(StyleRun& sr) = 0;
	virtual void Clear() {}
	virtual void Invalidate() {}
	virtual void Insert(unsigned int WXUNUSED(pos), unsigned int WXUNUSED(length)) {}
	virtual void Delete(unsigned int WXUNUSED(start_pos), unsigned int WXUNUSED(end_pos)) {}
	virtual void ApplyDiff(const std::vector<cxChange>& WXUNUSED(changes)) {
		Invalidate();
	}
};

#endif // __STYLER_H__
