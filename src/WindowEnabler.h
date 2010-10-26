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

#ifndef __WINDOWENABLER_H__
#define __WINDOWENABLER_H__

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

class WindowEnabler
{
public:
	WindowEnabler() {
		// remember the top level windows which were already enabled, so that we
		// don't disable them later
		m_winDisabled = NULL;

		wxWindowList::compatibility_iterator node;
		for ( node = wxTopLevelWindows.GetFirst(); node; node = node->GetNext() ) {
			wxWindow *winTop = node->GetData();

			// we don't need to enable already enabled windows
			if (!winTop->Enable()) {
				if ( !m_winDisabled )
				{
					m_winDisabled = new wxWindowList;
				}

				m_winDisabled->Append(winTop);
			}
		}
	};
	~WindowEnabler() {
		wxWindowList::compatibility_iterator node;
		for ( node = wxTopLevelWindows.GetFirst(); node; node = node->GetNext() )
		{
			wxWindow *winTop = node->GetData();
			if ( !m_winDisabled || !m_winDisabled->Find(winTop) )
			{
				winTop->Disable();
			}
			//else: had been already enabled, don't disable
		}

		delete m_winDisabled;
	};

private:
    wxWindowList *m_winDisabled;

	DECLARE_NO_COPY_CLASS(WindowEnabler)
};

#endif //__WINDOWENABLER_H__

