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

#ifndef __DISPATCHER_H__
#define __DISPATCHER_H__

#include <wx/string.h>
#include <map>

typedef void (*CALL_BACK)(void* clp, const void* data, int filter);

class Dispatcher {
public:
	void SubscribeC(const wxString& notifier_name, CALL_BACK new_callback, void *class_pointer);
	void UnSubscribe(const wxString& notifier_name, CALL_BACK new_callback, void *class_pointer);

	void NotifyInt(const wxString& notifier_name, int data, int filter) const;
	void Notify(const wxString& notifier_name, const void* data, int filter) const;

	bool HasSubscribers(const wxString& notifier_name) const;

private:

	struct call {
		CALL_BACK callback;
		void *class_pointer;
	};

	void (*callback)(void*, void*, int);
	void *class_p;
	std::multimap<wxString, call> subscribers;

};

#endif // __DISPATCHER_H__
