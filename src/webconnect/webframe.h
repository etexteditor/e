///////////////////////////////////////////////////////////////////////////////
// Name:        webframe.h
// Purpose:     wxwebconnect: embedded web browser control library
// Author:      Benjamin I. Williams
// Modified by:
// Created:     2007-04-23
// RCS-ID:      
// Copyright:   (C) Copyright 2006-2009, Kirix Corporation, All Rights Reserved.
// Licence:     wxWindows Library Licence, Version 3.1
///////////////////////////////////////////////////////////////////////////////


#ifndef __WXWEBCONNECT_WEBFRAME_H
#define __WXWEBCONNECT_WEBFRAME_H

// HACK wxWebKit and wxWebconnect both define wxWebFrame class so rename one of them
#define wxWebFrame wxWebconnectFrame


///////////////////////////////////////////////////////////////////////////////
//  wxWebFrame class
///////////////////////////////////////////////////////////////////////////////


// (CLASS) wxWebFrame
// Category: Control
// Description: Used for displaying a web browser control in a popup.
// Remarks: The wxWebFrame class is used for displaying a web browser control in a popup.

// (CONSTRUCTOR) wxWebFrame::wxWebFrame
// Description: Creates a new wxWebFrame object.
//
// Syntax:  wxWebFrame::wxWebFrame(wxWindow* parent,
//                                 wxWindowID id,
//                                 const wxString& title,
//                                 const wxPoint& pos = wxDefaultPosition,
//                                 const wxSize& size = wxDefaultSize,
//                                 long style = wxDEFAULT_FRAME_STYLE);
//
// Remarks: Creates a new wxWebFrame object.

// (METHOD) wxWebFrame::ShouldPreventAppExit
// Syntax: bool wxWebFrame::ShouldPreventAppExit()

// (METHOD) wxWebFrame::SetShouldPreventAppExit
// Syntax: void wxWebFrame::SetShouldPreventAppExit(bool b)

class wxWebControl;
class wxWebFrame : public wxFrame
{
public:

    wxWebFrame(wxWindow* parent,
            wxWindowID id,
            const wxString& title,
            const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize,
            long style = wxDEFAULT_FRAME_STYLE);

    ~wxWebFrame();

    wxWebControl* GetWebControl();
    
    bool ShouldPreventAppExit() const { return m_should_prevent_app_exit; }
    void SetShouldPreventAppExit(bool b) { m_should_prevent_app_exit = b; }
    
private:
    
    wxWebControl* m_ctrl;
    bool m_should_prevent_app_exit;
    
    DECLARE_EVENT_TABLE();
};




///////////////////////////////////////////////////////////////////////////////
//  wxWebDialog class
///////////////////////////////////////////////////////////////////////////////


// (CLASS) wxWebDialog
// Category: Control
// Description: Used for displaying a web browser control in a popup.
// Remarks: The wxWebDialog class is used for displaying a web browser control in a popup.

// (CONSTRUCTOR) wxWebDialog::wxWebDialog
// Description: Creates a new wxWebDialog object.
//
// Syntax:  wxWebDialog::wxWebDialog(wxWindow* parent,
//                                   wxWindowID id,
//                                   const wxString& title,
//                                   const wxPoint& pos = wxDefaultPosition,
//                                   const wxSize& size = wxDefaultSize,
//                                   long style = wxDEFAULT_DIALOG_STYLE);
//
// Remarks: Creates a new wxWebDialog object.

// (METHOD) wxWebDialog::GetWebControl
// Syntax: wxWebControl* wxWebDialog::GetWebControl()

class wxWebDialog : public wxDialog
{
public:

    wxWebDialog(wxWindow* parent,
            wxWindowID id,
            const wxString& title,
            const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize,
            long style = wxDEFAULT_DIALOG_STYLE);

    ~wxWebDialog();

    wxWebControl* GetWebControl();
    
private:
    
    wxWebControl* m_ctrl;
    
    DECLARE_EVENT_TABLE();
};


#endif

