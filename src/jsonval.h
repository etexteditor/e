/////////////////////////////////////////////////////////////////////////////
// Name:        jsonval.h
// Purpose:     the wxJSONValue class: it holds a JSON value
// Author:      Luciano Cattani
// Created:     2007/09/15
// RCS-ID:      $Id: jsonval.h,v 1.4 2008/01/10 21:27:15 luccat Exp $
// Copyright:   (c) 2007 Luciano Cattani
// Licence:     wxWidgets licence
/////////////////////////////////////////////////////////////////////////////

#if !defined( _WX_JSONVAL_H )
#define _WX_JSONVAL_H

#ifdef __GNUG__
    #pragma interface "jsonval.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"
 
#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWidgets headers)
#ifndef WX_PRECOMP
    #include <wx/object.h>
    #include <wx/hashmap.h>
    #include <wx/dynarray.h>
    #include <wx/arrstr.h>
#endif


#include "json_defs.h"

// forward declarations
class WXDLLIMPEXP_JSON wxJSONReader;
class WXDLLIMPEXP_JSON wxJSONRefData;
class WXDLLIMPEXP_JSON wxJSONInternalMap;
class WXDLLIMPEXP_JSON wxJSONInternalArray;


//! The type of the value held by the wxJSONRefData class
enum wxJSONType {
    wxJSONTYPE_EMPTY = 0,  /*!< empty type is for uninitialized objects  */
    wxJSONTYPE_NULL,       /*!< the object contains a NULL value         */
    wxJSONTYPE_INT,        /*!< the object contains an integer           */
    wxJSONTYPE_UINT,       /*!< the object contains an unsigned integer  */
    wxJSONTYPE_DOUBLE,     /*!< the object contains a double             */
    wxJSONTYPE_STRING,     /*!< the object contains a wxString object    */
    wxJSONTYPE_CSTRING,    /*!< the object contains a static C-string    */
    wxJSONTYPE_BOOL,       /*!< the object contains a boolean            */
    wxJSONTYPE_ARRAY,      /*!< the object contains an array of values   */
    wxJSONTYPE_OBJECT,     /*!< the object contains a map of keys/values */
    wxJSONTYPE_INT32,      /*!< the object contains a 32-bit integer     */
    wxJSONTYPE_INT64,      /*!< the object contains a 64-bit integer     */
    wxJSONTYPE_UINT32,     /*!< the object contains an unsigned 32-bit integer     */
    wxJSONTYPE_UINT64      /*!< the object contains an unsigned 64-bit integer     */
};

// the comment position: every value only has one comment position
// althrough comments may be splitted into several lines
enum {
  wxJSONVALUE_COMMENT_DEFAULT = 0,
  wxJSONVALUE_COMMENT_BEFORE,
  wxJSONVALUE_COMMENT_AFTER,
  wxJSONVALUE_COMMENT_INLINE,
};

/***********************************************************************

			class wxJSONValue

***********************************************************************/


// class WXDLLIMPEXP_JSON wxJSONValue : public wxObject
class WXDLLIMPEXP_JSON wxJSONValue
{
  friend class wxJSONReader;

public:

  // ctors and dtor
  wxJSONValue();
  wxJSONValue( wxJSONType type );
  wxJSONValue( int i );
  wxJSONValue( unsigned int i );
#if defined( wxJSON_64BIT_INT)
  wxJSONValue( wxInt64 i );
  wxJSONValue( wxUint64 ui );
#endif
  wxJSONValue( bool b  );
  wxJSONValue( double d );
  wxJSONValue( const wxChar* str );     // assume static ASCIIZ strings
  wxJSONValue( const wxString& str );
  wxJSONValue( const wxJSONValue& other );
  virtual ~wxJSONValue();

  // get the value type
  wxJSONType  GetType() const;
  bool IsEmpty() const;
  bool IsNull() const;
  bool IsInt() const;
  bool IsUInt() const;
#if defined( wxJSON_64BIT_INT)
  bool IsInt32() const;
  bool IsInt64() const;
  bool IsUInt32() const;
  bool IsUInt64() const;
#endif
  bool IsBool() const;
  bool IsDouble() const;
  bool IsString() const;
  bool IsCString() const;
  bool IsArray() const;
  bool IsObject() const;

  // get the value as ...
  int          AsInt() const;
  unsigned int AsUInt() const;
#if defined( wxJSON_64BIT_INT)
  int          AsInt32() const;
  unsigned int AsUInt32() const;
  wxInt64      AsInt64() const;
  wxUint64     AsUInt64() const;
#endif
  bool         AsBool() const;
  double       AsDouble() const;
  wxString     AsString() const;
  const wxChar* AsCString() const;
  const wxJSONInternalMap*   AsMap() const;
  const wxJSONInternalArray* AsArray() const;

  // get members names, size and other info
  bool      HasMember( unsigned index ) const;
  bool      HasMember( const wxString& key ) const;
  int       Size() const;
  wxArrayString  GetMemberNames() const;

  // appending items, resizing and deleting items
  wxJSONValue& Insert( unsigned index, const wxJSONValue& value );
  wxJSONValue& Append( const wxJSONValue& value );
  wxJSONValue& Append( bool b );
  wxJSONValue& Append( int i );
  wxJSONValue& Append( unsigned int ui );
#if defined( wxJSON_64BIT_INT )
  wxJSONValue& Append( wxInt64 i );
  wxJSONValue& Append( wxUint64 ui );
#endif
  wxJSONValue& Append( double d );
  wxJSONValue& Append( const wxChar* str );
  wxJSONValue& Append( const wxString& str );
  bool         Remove( int index );
  bool         Remove( const wxString& key );
  void         RemoveAll();
  void         Clear();
  bool         Cat( const wxChar* str );
  bool         Cat( const wxString& str );

  // retrieve an item
  wxJSONValue& Item( unsigned index );
  wxJSONValue& Item( const wxString& key );
  wxJSONValue  ItemAt( unsigned index ) const;
  wxJSONValue  ItemAt( const wxString& key ) const;

  wxJSONValue& operator [] ( unsigned index );
  wxJSONValue& operator [] ( const wxString& key );

  wxJSONValue& operator = ( int i );
  wxJSONValue& operator = ( unsigned int ui );
#if defined( wxJSON_64BIT_INT )
  wxJSONValue& operator = ( wxInt64 i );
  wxJSONValue& operator = ( wxUint64 ui );
#endif
  wxJSONValue& operator = ( bool b );
  wxJSONValue& operator = ( double d );
  wxJSONValue& operator = ( const wxChar* str );
  wxJSONValue& operator = ( const wxString& str );
  wxJSONValue& operator = ( const wxJSONValue& value );

  // get the value of a default value
  wxJSONValue  Get( const wxString& key, const wxJSONValue& defaultValue ) const;

  // comparison function
  bool         IsSameAs( const wxJSONValue& other ) const;

  // comment-related functions
  int      AddComment( const wxString& str, int position = wxJSONVALUE_COMMENT_DEFAULT );
  int      AddComment( const wxArrayString& comments, int position = wxJSONVALUE_COMMENT_DEFAULT );
  wxString GetComment( int idx = -1 ) const;
  int      GetCommentPos() const;
  int      GetCommentCount() const;
  void     ClearComments();
  const wxArrayString& GetCommentArray() const;

  // debugging functions
  wxString         GetInfo() const;
  wxString         Dump( bool deep = false, int mode = 0 ) const;

  wxJSONRefData*   GetRefData() const;
  wxJSONRefData*   SetType( wxJSONType type );

  int              GetLineNo() const;
  void             SetLineNo( int num );

  static  wxString TypeToString( wxJSONType type );

protected:
  wxJSONValue*  Find( unsigned index ) const;
  wxJSONValue*  Find( const wxString& key ) const;
  //  void          DeleteObj();
  void          DeepCopy( const wxJSONValue& other );

  wxJSONRefData*  Init( wxJSONType type );
  wxJSONRefData*  COW();

#if defined( wxJSON_64BIT_INT )
  bool             AsInt32( wxInt32* i) const;
  bool             AsUInt32( wxUint32* i ) const;
#endif

  // overidden from wxObject
  virtual wxJSONRefData*  CloneRefData(const wxJSONRefData *data) const;
  virtual wxJSONRefData*  CreateRefData() const;

  void            SetRefData(wxJSONRefData* data);
  void            Ref(const wxJSONValue& clone);
  void            UnRef();
  void            UnShare();
  void            AllocExclusive();

  //! the referenced data
  wxJSONRefData*  m_refData;

  // used for debugging purposes: only in debug builds.
#if defined( WXJSON_USE_VALUE_COUNTER )
  int         m_progr;
  static int  sm_progr;
#endif
};


/***********************************************************************

			class wxJSONRefData

***********************************************************************/



WX_DECLARE_OBJARRAY( wxJSONValue, wxJSONInternalArray );
WX_DECLARE_STRING_HASH_MAP( wxJSONValue, wxJSONInternalMap );


//! The actual value held by the wxJSONValue class (internal use)
/*!
 Note that this structure is not a \b union as in the previous versions.
 This allow to store instances of the string, ObjArray and HashMap objects
 (no more pointers to them)
*/
struct wxJSONValueHolder  {
#if defined( wxJSON_64BIT_INT )
    wxInt64       m_valInt;
    wxUint64      m_valUInt;
#else
    int           m_valInt;
    unsigned      m_valUInt;
#endif
    double        m_valDouble;
    const wxChar* m_valCString;
    bool          m_valBool;
    wxString      m_valString;
    wxJSONInternalArray m_valArray;
    wxJSONInternalMap   m_valMap;
  };



// class WXDLLIMPEXP_JSON wxJSONRefData : public wxObjectRefData
class WXDLLIMPEXP_JSON wxJSONRefData
{
  // friend class wxJSONReader;
  friend class wxJSONValue;

public:

  wxJSONRefData();
  virtual ~wxJSONRefData();

  int GetRefCount() const;

  // there is no need to define copy ctor
  // wxJSONRefData( const wxJSONRefData& other );

protected:
  //! the references count
  int               m_refCount;

  //! The actual type of the value held by this object.
  wxJSONType        m_type;

  //! The JSON value held by this object.
  /*!
   This data member contains the JSON data types defined by the
   JSON syntax with the exception of wxJSONTYPE_EMPTY which is an
   internal type used by the parser class.
  */
  wxJSONValueHolder m_value;

  //! The position of the comment line(s), if any.
  /*!
   The data member contains one of the following constants:
   \li \c wxJSONVALUE_COMMENT_BEFORE
   \li \c wxJSONVALUE_COMMENT_AFTER
   \li \c wxJSONVALUE_COMMENT_INLINE
  */
  int               m_commentPos;

  //! The array of comment lines; may be empty.
  wxArrayString     m_comments;

  //! The line number when this value was read
  /*!
   This data member is used by the wxJSONReader class and it is
   used to store the line number of the JSON text document where
   the value appeared. This value is compared to the line number
   of a comment line in order to obtain the value which a
   comment refersto.
  */ 
  int               m_lineNo;


  // used for debugging purposes: only in debug builds.
  #if defined( WXJSON_USE_VALUE_COUNTER )
    int         m_progr;
    static int  sm_progr;
  #endif
};



#endif			// not defined _WX_JSONVAL_H


