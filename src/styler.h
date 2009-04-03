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

class Lines;
class Revision;

// pre-definitions
class StyleRun;

// Abstract base class for stylers
class Styler {
public:
    virtual ~Styler() {};

	virtual void Style(StyleRun& sr) = 0;
};

#endif // __STYLER_H__
