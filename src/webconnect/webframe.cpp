///////////////////////////////////////////////////////////////////////////////
// Name:        webframe.cpp
// Purpose:     wxwebconnect: embedded web browser control library
// Author:      Benjamin I. Williams
// Modified by:
// Created:     2007-04-23
// RCS-ID:      
// Copyright:   (C) Copyright 2006-2009, Kirix Corporation, All Rights Reserved.
// Licence:     wxWindows Library Licence, Version 3.1
///////////////////////////////////////////////////////////////////////////////


#include <wx/wx.h>
#include "webframe.h"
#include "webcontrol.h"



///////////////////////////////////////////////////////////////////////////////
//  wxWebFrame class implementation
///////////////////////////////////////////////////////////////////////////////


BEGIN_EVENT_TABLE(wxWebFrame, wxFrame)
END_EVENT_TABLE()


wxWebFrame::wxWebFrame(wxWindow* parent,
                       wxWindowID id,
                       const wxString& title,
                       const wxPoint& pos,
                       const wxSize& size,
                       long style) : wxFrame(parent, id, title, pos, size, style)
{
    m_should_prevent_app_exit = true;
    
    m_ctrl = new wxWebControl(this, -1, wxPoint(0,0), wxSize(200,200));
    
    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(m_ctrl, 1, wxEXPAND);
    
    SetSizer(sizer);
}

wxWebFrame::~wxWebFrame()
{
}

wxWebControl* wxWebFrame::GetWebControl()
{
    return m_ctrl;
}




///////////////////////////////////////////////////////////////////////////////
//  wxWebDialog class implementation
///////////////////////////////////////////////////////////////////////////////


BEGIN_EVENT_TABLE(wxWebDialog, wxDialog)
END_EVENT_TABLE()


wxWebDialog::wxWebDialog(wxWindow* parent,
                       wxWindowID id,
                       const wxString& title,
                       const wxPoint& pos,
                       const wxSize& size,
                       long style) : wxDialog(parent, id, title, pos, size, style)
{
    m_ctrl = new wxWebControl(this, -1, wxPoint(0,0), wxSize(200,200));
    
    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(m_ctrl, 1, wxEXPAND);
    
    SetSizer(sizer);
}

wxWebDialog::~wxWebDialog()
{
}

wxWebControl* wxWebDialog::GetWebControl()
{
    return m_ctrl;
}

