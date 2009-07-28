///////////////////////////////////////////////////////////////////////////////
// Name:        domprivate.h
// Purpose:     wxwebconnect: embedded web browser control library
// Author:      Benjamin I. Williams
// Modified by:
// Created:     2006-09-30
// RCS-ID:      
// Copyright:   (C) Copyright 2006-2009, Kirix Corporation, All Rights Reserved.
// Licence:     wxWindows Library Licence, Version 3.1
///////////////////////////////////////////////////////////////////////////////


#ifndef __WXWEBCONNECT_DOMPRIVATE_H
#define __WXWEBCONNECT_DOMPRIVATE_H


struct wxDOMNodeData
{
    void setNode(nsISupports* ptr)
    {
        ns_smartptr<nsISupports> supports = ptr;
        node_ptr = supports;
        attr_ptr = supports;
        text_ptr = supports;
        element_ptr = supports;
        doc_ptr = supports;
        htmlelement_ptr = supports;
        anchor_ptr = supports;
        button_ptr = supports;
        input_ptr = supports;
        link_ptr = supports;
        option_ptr = supports;
        param_ptr = supports;
        select_ptr = supports;
        textarea_ptr = supports;
    }
    
    ns_smartptr<nsIDOMNode> node_ptr;
    ns_smartptr<nsIDOMAttr> attr_ptr;
    ns_smartptr<nsIDOMText> text_ptr;
    ns_smartptr<nsIDOMElement> element_ptr;
    ns_smartptr<nsIDOMDocument> doc_ptr;
    ns_smartptr<nsIDOMHTMLElement> htmlelement_ptr;
    ns_smartptr<nsIDOMHTMLAnchorElement> anchor_ptr;
    ns_smartptr<nsIDOMHTMLButtonElement> button_ptr;
    ns_smartptr<nsIDOMHTMLInputElement> input_ptr;
    ns_smartptr<nsIDOMHTMLLinkElement> link_ptr;
    ns_smartptr<nsIDOMHTMLOptionElement> option_ptr;
    ns_smartptr<nsIDOMHTMLParamElement> param_ptr;
    ns_smartptr<nsIDOMHTMLSelectElement> select_ptr;
    ns_smartptr<nsIDOMHTMLTextAreaElement> textarea_ptr;
};


struct wxDOMNodeListData
{
    ns_smartptr<nsIDOMNodeList> ptr;
};


struct wxDOMNamedNodeMapData
{
    ns_smartptr<nsIDOMNamedNodeMap> ptr;
};


struct wxDOMEventData
{
    void setPtr(nsISupports* ptr)
    {
        ns_smartptr<nsISupports> supports = ptr;
        event_ptr = supports;
        mouseevent_ptr = supports;
    }
    
    ns_smartptr<nsIDOMNode> event_ptr;
    ns_smartptr<nsIDOMAttr> mouseevent_ptr;
};


#endif

