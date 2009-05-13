/////////////////////////////////////////////////////////////////////////////
// Name:        jsonwriter.h
// Purpose:     the generator of JSON text from a JSON value
// Author:      Luciano Cattani
// Created:     2007/09/15
// RCS-ID:      $Id: jsonwriter.h,v 1.4 2008/03/03 19:05:45 luccat Exp $
// Copyright:   (c) 2007 Luciano Cattani
// Licence:     wxWidgets licence
/////////////////////////////////////////////////////////////////////////////

#if !defined( _WX_JSONWRITER_H )
#define _WX_JSONWRITER_H

#ifdef __GNUG__
    #pragma interface "jsonwriter.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"
 
#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWidgets headers)
#ifndef WX_PRECOMP
    #include <wx/stream.h>
    #include <wx/string.h>
#endif

#include "json_defs.h"
#include "jsonval.h"


// The 'style' flags for the writer
// BIT= 9 8 7 6 5 4 3 2 1 0
//            | | | | | | |
//            | | | | | |  -> 1=styled (indentation), 0=not styled (other bits ignored)
//            | | | | |  ---> 0=do not write comments, 1=write comments
//            | | | |  -----> 1=force comments to be written before.the value
//            | | |  -------> 1=forcecomments to be written after the value
//            | |  ---------> 0=do not split strings, 1=split strings
//            |  -----------> 1=allow literal newlines and tabs in strings
//             -------------> 1=escape solidus (only needed when embedding in html)
enum {
  wxJSONWRITER_NONE = 0,
  wxJSONWRITER_STYLED = 1,
  wxJSONWRITER_WRITE_COMMENTS   = 2,
  wxJSONWRITER_COMMENTS_BEFORE  = 4,
  wxJSONWRITER_COMMENTS_AFTER   = 8,
  wxJSONWRITER_SPLIT_STRING     = 16,
  wxJSONWRITER_MULTILINE_STRING = 32,
  wxJSONWRITER_ESCAPE_SOLIDUS   = 64
};

// class declaration

class WXDLLIMPEXP_JSON wxJSONWriter
{
public:
  wxJSONWriter( int style = wxJSONWRITER_STYLED, int indent = 0, int step = 3 );
  ~wxJSONWriter();

  void Write( const wxJSONValue& value, wxString& str );
  void Write( const wxJSONValue& value, wxOutputStream& os );

protected:

  int  DoWrite( const wxJSONValue& value, const wxString* key,
				bool comma );
  int  WriteIndent();
  int  WriteIndent( int num );

  int  WriteString( const wxString& str );
  int  WriteStringValue( const wxString& str );
  int  WritePrimitiveValue( const wxJSONValue& value );
  int  WriteInt( int i );
  int  WriteUInt( unsigned int ui );
  int  WriteBool( bool b );
  int  WriteDouble( double d );
  int  WriteNull();
  int  WriteKey( const wxString& key );
  int  WriteEmpty();
  int  WriteComment( const wxJSONValue& value, bool indent );
  int  WriteChar( wxChar ch );

  int  WriteError( const wxString& err );

private:
  //! The style flag is a combination of wxJSONWRITER_(something) constants.
  int   m_style;

  //! The initial indentation value, in number of spaces.
  int   m_indent;

  //! The indentation increment, in number of spaces.
  int   m_step;

  //! JSON value objects can be nested; this is the level of annidation (used internally).
  int   m_level;

  //! The pointer to the actual output object (a stream or a wxString)
  void* m_outObject;

  //! The type of the output object (0=wxString, 1=wxOutputStream)
  int   m_outType;

  //! The charset conversion object.
  /*!
   This data member contains a pointer to a UTF-8 converter if the output
   type object is a stream.
   If the output object is a string, the pointer is NULL.
  */
  wxMBConv*  m_conv; 
};

#endif			// not defined _WX_JSONWRITER_H
