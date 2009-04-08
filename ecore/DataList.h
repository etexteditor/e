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

#ifndef __DATALIST_H__
#define __DATALIST_H__

#include "Catalyst.h"

// pre-declarations
class cDataList;

class DataList {
public:
	DataList(const Catalyst& cat, const doc_id& di, const node_ref& headnode_id);
	~DataList();

	// Document
	void Create();

	// Property handling
	bool HasProperty(const wxString& name) const;
	void DeleteProperty(const wxString& name);
	bool GetProperty(const wxString& name, bool& value) const;
	bool GetProperty(const wxString& name, wxLongLong_t& value) const;
	bool GetProperty(const wxString& name, wxString& value) const;
	void SetProperty(const wxString& name, const bool value);
	void SetProperty(const wxString& name, const wxLongLong_t value);
	void SetProperty(const wxString& name, const wxString& value);

	// Nodes
	node_ref GetHeadnode() const;
	void SetHeadnode(node_ref new_headnode);

	// Consolidation
	void Freeze();
	node_ref Consolidate(const doc_id& di);

private:
	cDataList* m_dl;
};

#endif // __CDATALIST_H__

