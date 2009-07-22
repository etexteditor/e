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

#include "Dispatcher.h"

// Current events:
//		format (id, data, filter)
//
//	Application:
//		APP_ACTIVEWINDOW	window_id			-
//		APP_SETDOCUMENT		(doc_id, rev_id)	-
//
//	Window:
//		WIN_SETDOCUMENT		(doc_id, rev_id)	window_id
//		WIN_CHANGEDOC		(doc_id, rev_id)	window_id
//
//	Document:
//		DOC_NEWREVISION		revision_id			doc_id
//		DOC_UPDATEREVISION	revision_id			doc_id
//		DOC_DELETED			doc_id


// Callback has to be a pointer to a static member function
void Dispatcher::SubscribeC(const wxString& notifier_name, CALL_BACK new_callback, void *class_pointer) {
	wxASSERT(!notifier_name.empty());

	call new_subscriber = {new_callback, class_pointer};
	subscribers.insert(std::pair<wxString, call>(notifier_name, new_subscriber));
}

void Dispatcher::UnSubscribe(const wxString& notifier_name, CALL_BACK cb, void *class_pointer) {
	wxASSERT(!notifier_name.empty());
	typedef std::multimap<wxString, call>::iterator I;
	std::pair<I,I> b; I i;

	b = subscribers.equal_range(notifier_name);
	for (i = b.first; i != b.second; ++i) {
		if ((i->second).callback == cb && (i->second).class_pointer == class_pointer) {
			subscribers.erase(i);
			break;
		}
	}
}

void Dispatcher::NotifyInt(const wxString& notifier_name, int data, int filter) const {
	wxASSERT(!notifier_name.empty());

	Notify(notifier_name, &data, filter);
}

void Dispatcher::Notify(const wxString& notifier_name, const void* data, int filter) const {
	wxASSERT(!notifier_name.empty());

	typedef std::multimap<wxString, call>::const_iterator I;
	std::pair<I,I> b; I i;

	b = subscribers.equal_range(notifier_name);
	for (i = b.first; i != b.second; ++i) {
		(i->second).callback((i->second).class_pointer, data, filter);
	}
}

bool Dispatcher::HasSubscribers(const wxString& notifier_name) const {
	return (subscribers.find(notifier_name) != subscribers.end());
}
