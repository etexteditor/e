/////////////////////////////////////////////////////////////////////////////
// Name:        jsonval.cpp
// Purpose:     the wxJSON class that holds a JSON value
// Author:      Luciano Cattani
// Created:     2007/10/01
// RCS-ID:      $Id: jsonval.cpp,v 1.12 2008/03/06 10:25:18 luccat Exp $
// Copyright:   (c) 2007 Luciano Cattani
// Licence:     wxWidgets licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
    #pragma implementation "jsonval.cpp"
#endif


// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include <wx/log.h>
#include <wx/debug.h>
#include <wx/arrimpl.cpp>

#include "jsonval.h"


WX_DEFINE_OBJARRAY( wxJSONInternalArray );


// the trace mask used in ::wxLogTrace() function
static const wxChar* traceMask = _T("jsonval");
static const wxChar* compareTraceMask = _T("sameas");
static const wxChar* cowTraceMask = _T("traceCOW" );



/*******************************************************************

			class wxJSONRefData

*******************************************************************/


/*! \class wxJSONRefData
 \brief The reference counted JSON value data (internal use).

 Starting from version 0.4, the JSON value class use the reference
 counting tecnique (also know as \e copy-on-write) described in the
 \b wxWidgets documentation in order to speed up processing. 
 The class is used internally by the wxJSONValue class which does
 all processing.
*/

#if defined( WXJSON_USE_VALUE_COUNTER )
// The progressive counter (used for debugging only)
int          wxJSONRefData::sm_progr = 1;
#endif

//! Constructor.
wxJSONRefData::wxJSONRefData()
{
  m_lineNo = -1;
  m_refCount  = 1;

#if defined( WXJSON_USE_VALUE_COUNTER )
  m_progr = sm_progr;
  ++sm_progr;
  ::wxLogTrace( traceMask, _T("(%s) JSON refData ctor progr=%d"),
				 __PRETTY_FUNCTION__, m_progr);
#endif
}

// Dtor - does nothing
wxJSONRefData::~wxJSONRefData()
{
}

int
wxJSONRefData::GetRefCount() const
{
  return m_refCount;
}


/*******************************************************************

			class wxJSONValue

*******************************************************************/


/*! \class wxJSONValue
 \brief The JSON value class implementation.

This class holds a JSON value which may be of the following type:

\li wxJSONTYPE_EMPTY: an empty (invalid) JSON value
\li wxJSONTYPE_NULL: a NULL value
\li wxJSONTYPE_INT: an integer value
\li wxJSONTYPE_UINT: an unsigned integer
\li wxJSONTYPE_DOUBLE: a double precision number
\li wxJSONTYPE_BOOL: a boolean
\li wxJSONTYPE_CSTRING: a C string
\li wxJSONTYPE_STRING: a wxString object
\li wxJSONTYPE_ARRAY: an array of wxJSONValue objects
\li wxJSONTYPE_OBJECT: a hashmap of key/value pairs where \e key is a string
	and \e value is a wxJSONValue object

To know more about the internal representation of JSON values see
\ref pg_json_internals.

Starting from version 0.5 the wxJSON library supports 64-bits integers on
platforms that have native support for very large integers.
Note that the integer type is still stored as a generic wxJSONTYPE_(U)INT
constant regardless the size of the value but the JSON value class defines
functions in order to let the user know if an integer value fits in a
32-bit integer or it is so large that it needs a 64-bits integer storage.
To know more about 64-bits integer support see \ref json_internals_integer
*/


#if defined( WXJSON_USE_VALUE_COUNTER )
// The progressive counter (used for debugging only)
int          wxJSONValue::sm_progr = 1;
#endif

//! Constructors.
/*!
 The overloaded constructors allow the user to construct a JSON value
 object that holds the specified value and type of value.
 The default ctor construct a valid JSON object that constains a \b null
 value.

 If you want to create an \b empty JSON value object you have to use the
 \c wxJSONValue( wxJSONTYPE_EMPTY ) ctor.
 Note that this object is not a valid JSON value - to know more about this
 topic see the SetType() function.

 \par The C-string JSON value object

 The wxJSONValue(const wxChar*) ctor allows you to create a JSON value
 object that contains a string value which is stored as a 
 \e pointer-to-statci-string.
 
 In fact, the ctor DOES NOT copy the string: it only stores the
 pointer in a data member and the pointed-to buffer is not deleted
 by the dtor.
 If the string is not static you have to use the wxJSONValue(const wxString&)
 constructor.

 Also note that this does NOT mean that the value stored in this JSON
 object cannot change: you can assign whatever other value you want,
 an integer, a double or an array of values.
 What I intended is that the pointed-to string must exist for the lifetime
 of the wxJSONValue object.
 The following code is perfectly legal:
 \code
   wxJSONvalue aString( "this is a static string" );
   aString = 10;
 \endcode
 To know more about this topic see \ref json_internals_cstring
*/
wxJSONValue::wxJSONValue()
{
  m_refData = 0;
  Init( wxJSONTYPE_NULL );
}

//! Initialize the JSON value class.
/*!
 The function is called by the ctors and allocates a new instance of
 the wxJSONRefData class and sets the type of the JSON value.
 Note that only the type is set, not the value.
 Also note that the function may be called if the \c m_refData 
 data member is NULL.
*/
wxJSONRefData*
wxJSONValue::Init( wxJSONType type )
{
  wxJSONRefData* data = GetRefData();
  if ( data != 0 ) {
    UnRef();
  }

  // we allocate a new instance of the referenced data
  data = new wxJSONRefData();
  data->m_type = type;
  data->m_commentPos = wxJSONVALUE_COMMENT_BEFORE;
  SetRefData( data );
#if defined( WXJSON_USE_VALUE_COUNTER )
  m_progr = sm_progr;
  ++sm_progr;
  ::wxLogTrace( cowTraceMask, _T("(%s) Init a new object progr=%d"),
			 __PRETTY_FUNCTION__, m_progr ); 
#endif
  return data;
}


//! \overload wxJSONValue()
wxJSONValue::wxJSONValue( wxJSONType type )
{
  m_refData = 0;
  Init( type );
}

//! \overload wxJSONValue()
wxJSONValue::wxJSONValue( int i )
{
  m_refData = 0;
  wxJSONRefData* data = Init( wxJSONTYPE_INT );
  wxASSERT( data );
  if ( data != 0 ) {
    data->m_value.m_valInt = i;
  }
}


//! \overload wxJSONValue()
wxJSONValue::wxJSONValue( unsigned int ui )
{
  m_refData = 0;
  wxJSONRefData* data = Init( wxJSONTYPE_UINT );
  wxASSERT( data );
  if ( data != 0 ) {
    data->m_value.m_valUInt = ui;
  }
}

//! \overload wxJSONValue()
wxJSONValue::wxJSONValue( bool b  )
{
  m_refData = 0;
  wxJSONRefData* data = Init( wxJSONTYPE_BOOL );
  wxASSERT( data );
  if ( data != 0 ) {
    data->m_value.m_valBool = b;
  }
}

//! \overload wxJSONValue()
wxJSONValue::wxJSONValue( double d )
{
  m_refData = 0;
  wxJSONRefData* data = Init( wxJSONTYPE_DOUBLE );
  wxASSERT( data );
  if ( data != 0 ) {
    data->m_value.m_valDouble = d;
  }
}

//! \overload wxJSONValue()
wxJSONValue::wxJSONValue( const wxChar* str )
{
  m_refData = 0;
  wxJSONRefData* data = Init( wxJSONTYPE_CSTRING );
  wxASSERT( data );
  if ( data != 0 ) {
  #if !defined( WXJSON_USE_CSTRING )
    data->m_type = wxJSONTYPE_STRING;
    data->m_value.m_valString.assign( str );
  #else
    data->m_value.m_valCString = str;
  #endif
  }
}

//! \overload wxJSONValue()
wxJSONValue::wxJSONValue( const wxString& str )
{
  m_refData = 0;
  wxJSONRefData* data = Init( wxJSONTYPE_STRING );
  wxASSERT( data );
  if ( data != 0 ) {
    data->m_value.m_valString.assign( str );
  }
}


//! Copy constructor
/*!
 The function makes a copies the content of \c other in this
 object.
 Note that the JSON value object is not really copied;
 the function calls Ref() in order to increment
 the reference count of the \c wxJSONRefData structure.
*/
wxJSONValue::wxJSONValue( const wxJSONValue& other )
{
  m_refData = 0;
  Ref( other );

  // the progressive counter of the ctor is not copied from
  // the other wxJSONValue object: only data is shared, the
  // progressive counter is not shared because this object
  // is a copy of 'other' and it has its own progressive
  #if defined( WXJSON_USE_VALUE_COUNTER )
    m_progr = sm_progr;
    ++sm_progr;
    ::wxLogTrace( cowTraceMask, _T("(%s) Copy ctor - progr=%d other progr=%d"),
 			 __PRETTY_FUNCTION__, m_progr, other.m_progr ); 
  #endif
}


//! Dtor
wxJSONValue::~wxJSONValue()
{
  UnRef();
}


// functions for retreiving the value type: they are all 'const'


//! Return the type of the value stored in the object.
/*!
 This function is the only one that does not ASSERT that the
 \c m_refData data member is not-NULL.
 In fact, if the JSON value object does not contain a pointer
 to a wxJSONRefData structure, the function returns the
 wxJSONTYPE_EMPTY which represent an invalid JSON value object.
 Also note that the pointer to the referenced data structure
 should NEVER be NULL.

 \sa SetType
*/
wxJSONType
wxJSONValue::GetType() const
{
  wxJSONRefData* data = GetRefData();
  wxJSONType type = wxJSONTYPE_EMPTY;
  if ( data )  {
    type = data->m_type;
    // on 64-bits platforms, the function returns wxJSONTYPE_INT64
    // if the integer is too large to fit in 32-bits
#if defined( wxJSON_64BIT_INT )
    if( type == wxJSONTYPE_INT && IsInt64() )  {
      type = wxJSONTYPE_INT64;
    }
    if( type == wxJSONTYPE_UINT && IsUInt64() )  {
      type = wxJSONTYPE_UINT64;
    }
#endif
  }
  return type;
}


//! Return TRUE if the type of the value is wxJSONTYPE_NULL.
bool
wxJSONValue::IsNull() const
{
  wxJSONType type = GetType();
  bool r = false;
  if ( type == wxJSONTYPE_NULL )  {
    r = true;
  }
  return r;
}


//! Return TRUE if the value is of type wxJSONTYPE_EMPTY.
/*!
 If the function returns TRUE, this JSON value object is not
 valid - see the IsValid() function.
*/
bool
wxJSONValue::IsEmpty() const
{
  wxJSONType type = GetType();
  bool r = false;
  if ( type == wxJSONTYPE_EMPTY )  {
    r = true;
  }
  return r;
}

//! Return TRUE if the type of the value stored is integer.
/*!
 This function returns TRUE if and only if the stored value is of
 type \b wxJSONTYPE_INT or \b wxJSONTYPE_INT64.
 The function does not check if the value actually fits in a 32-bit
 integer.
 Also, if the type of the value is \b wxJSONTYPE_UINT but the value
 may be stored in a integer, the function returns FALSE.
 On platforms that support 64-bits integers, you can call \c IsInt32()
 and \c IsInt64() to know if the value is too large to fit in  32-bit
 integer.

 \sa \ref json_internals_integer
*/
bool
wxJSONValue::IsInt() const
{
  wxJSONType type = GetType();
  bool r = false;
  if ( type == wxJSONTYPE_INT || type == wxJSONTYPE_INT64 )  {
    r = true;
  }
  return r;
}

//! Return TRUE if the type of the value stored is a unsigned int.
/*!
 This function returns TRUE if and only if the stored value is of
 type \b wxJSONTYPE_UINT or \b wxJSONTYPE_UINT64.
 Note that unsigned integers are stored in the JSON value object as 64-bits
 unsigned integers on platforms that support 64-bit.
 The function does not check if the value actually fits in a 32-bit
 unsigned integer.
 Also, if the type of the value is \b wxJSONTYPE_INT but the value
 may be stored in an unsigned integer, the function returns FALSE.
 On platforms that support 64-bits integers, you can call \c IsUInt32()
 and \c IsUInt64() to know if the value is too large to fit in  32-bit
 unsigned integer.

 \sa \ref json_internals_integer
*/
bool
wxJSONValue::IsUInt() const
{
  wxJSONType type = GetType();
  bool r = false;
  if ( type == wxJSONTYPE_UINT || type == wxJSONTYPE_UINT64 )  {
    r = true;
  }
  return r;
}

//! Return TRUE if the type of the value stored is a boolean.
bool
wxJSONValue::IsBool() const
{
  wxJSONType type = GetType();
  bool r = false;
  if ( type == wxJSONTYPE_BOOL )  {
    r = true;
  }
  return r;
}

//! Return TRUE if the type of the value stored is a double.
bool
wxJSONValue::IsDouble() const
{
  wxJSONType type = GetType();
  bool r = false;
  if ( type == wxJSONTYPE_DOUBLE )  {
    r = true;
  }
  return r;
}

//! Return TRUE if the type of the value stored is a wxString object.
bool
wxJSONValue::IsString() const
{
  wxJSONType type = GetType();
  bool r = false;
  if ( type == wxJSONTYPE_STRING )  {
    r = true;
  }
  return r;
}

//! Return TRUE if the type of the value stored is a pointer to a static C string.
bool
wxJSONValue::IsCString() const
{
  wxJSONType type = GetType();
  bool r = false;
  if ( type == wxJSONTYPE_CSTRING )  {
    r = true;
  }
  return r;
}

//! Return TRUE if the type of the value stored is an array type.
bool
wxJSONValue::IsArray() const
{
  wxJSONType type = GetType();
  bool r = false;
  if ( type == wxJSONTYPE_ARRAY )  {
    r = true;
  }
  return r;
}

//! Return TRUE if the type of this value is a key/value map.
bool
wxJSONValue::IsObject() const
{
  wxJSONType type = GetType();
  bool r = false;
  if ( type == wxJSONTYPE_OBJECT )  {
    r = true;
  }
  return r;
}

// get the stored value; all these functions are 'const'

//! Return the stored value as an integer.
/*!
 The function returns the integer value stored in this JSON-value
 object.
 If the type of the stored value is not an integer, an unsigned int
 or a bool the function returns an undefined value.
 In debug builds the function ASSERTs that the stored value is of
 compatible type: an \b int, an \b unsigned \b int or a \b boolean.

 Also note that starting from version 0.5, the \b wxJSON library
 supports 64-bits integers on platforms that have native support 
 for this.
 Because integer values are stored internally as 64-bits integers,
 this function returns the low 32-bits of the value stored in
 this objects.
 If the value is too large to fit in a 32 bits signed integer, the
 function fails with an ASSERTION failure.
 \sa AsInt32() AsUInt32()
 \sa \ref json_internals_integer
*/
int
wxJSONValue::AsInt() const
{
  int i;

  // on 64-bits platforms check if the value fits in 32-bits
#if defined( wxJSON_64BIT_INT )
    wxInt32 i32;
    wxASSERT( GetRefData() );
    bool success = AsInt32( &i32 );
    wxASSERT( success );
    i = (int) i32;
#else
  wxJSONRefData* data = GetRefData();
  wxASSERT( data );
  switch ( data->m_type )  {
    case wxJSONTYPE_BOOL :
      i = (int) data->m_value.m_valBool;

    case wxJSONTYPE_INT :
      i = (int) data->m_value.m_valInt;
      break;
    case wxJSONTYPE_UINT :
      ::wxLogWarning( _T("wxJSONValue::AsInt() - value type is unsigned" ));
      i = (int) data->m_value.m_valUInt;
      break;
    default :
      wxFAIL_MSG( _T("wxJSONValue::AsInt() - the value is not compatible with INT"));
      // wxASSERT( 0 );      // always fails
      break;
  } 
#endif

  return i;
}

//! Return the stored value as a boolean.
/*!
 If the type of the stored value is not a boolean the function 
 just returns an undefined result.
 In debug builds the function ASSERTs that the stored value is of
 type \b boolean.
*/
bool
wxJSONValue::AsBool() const
{
  wxJSONRefData* data = GetRefData();
  wxASSERT( data );
  wxASSERT( data->m_type == wxJSONTYPE_BOOL );
  return data->m_value.m_valBool;
}

//! Return the stored value as a double.
/*!
 If the type of the stored value is not a double, an int or an
 unsigned int the function
 returns an undefined result.
 In debug builds the function ASSERTs that the stored value is of
 compatible type: an \b int, an \b unsigned \b int or a \b double.
*/
double
wxJSONValue::AsDouble() const
{
  wxJSONRefData* data = GetRefData();
  wxASSERT( data );
  double d = data->m_value.m_valDouble;
  switch ( data->m_type )  {
    case wxJSONTYPE_INT :
      d = data->m_value.m_valInt;
      break;
    case wxJSONTYPE_UINT :
      d = (int)data->m_value.m_valUInt;
      break;
    default :
      break;
  }
  return d;
}


//! Return the stored value as a wxWidget's string.
/*!
 If the type of the stored value is a wxString or a static
 C-string type the function returns a new string instance
 that contains the stored string.
 If the stored value is of a numeric type (int, unsigned int
 or double) the function returns the string representation
 of the numeric value.
 If the stored value is a boolean, the function returns the
 literal string \b true or \b false.
 If the value is a NULL value the \b null literal string is returned.

 If the value is of type wxJSONTYPE_EMPTY, the literal string \b &lt;empty&gt;
 is returned. Note that this is NOT a valid JSON text.
 If the value is an array or map, an empty string is returned.
*/
wxString
wxJSONValue::AsString() const
{
  wxJSONRefData* data = GetRefData();
  wxASSERT( data );
  wxString s;
  switch ( data->m_type )  {
    case wxJSONTYPE_STRING :
      s.assign( data->m_value.m_valString);
      break;
    case wxJSONTYPE_CSTRING :
      s.assign( data->m_value.m_valCString);
      break;
    case wxJSONTYPE_INT :
#if defined( wxJSON_64BIT_INT )
      s.Printf( _T("%") wxLongLongFmtSpec _T("i"), data->m_value.m_valInt );
#else
      s.Printf( _T("%d"), data->m_value.m_valInt );
#endif
      break;
    case wxJSONTYPE_UINT :
#if defined( wxJSON_64BIT_INT )
      s.Printf( _T("+%") wxLongLongFmtSpec _T("u"), data->m_value.m_valUInt );
#else
      s.Printf( _T("+%u"), data->m_value.m_valInt );
#endif
      break;
    case wxJSONTYPE_DOUBLE :
      s.Printf( _T("%f"), data->m_value.m_valDouble );
      break;
    case wxJSONTYPE_BOOL :
      s.assign( ( data->m_value.m_valBool ? _T("true") : _T("false") ));
      break;
    case wxJSONTYPE_NULL :
      s.assign( _T( "null"));
      break;
    case wxJSONTYPE_EMPTY :
      s.assign( _T( "<empty>"));
      break;
    default :
      break;
  }
  return s;
}

//! Return the stored value as a pointer to a static C string.
/*!
 If the type of the value is stored as a C-string data type the
 function just returns that pointer.
 If the stored value is a wxString object, the function returns the
 pointer returned by the \b wxString::c_str() function.
 If the stored value is all other JSON types, the functions returns a NULL pointer
 (changed in version 0.5 thanks to Robbie Groenewoudt)

 See also \ref json_internals_cstring
*/
const wxChar*
wxJSONValue::AsCString() const
{
  const wxChar* s = 0;
  wxJSONRefData* data = GetRefData();
  wxASSERT( data );
  switch ( data->m_type )  {
    case wxJSONTYPE_CSTRING :
      s = data->m_value.m_valCString;
      break;
    case wxJSONTYPE_STRING :
      s = data->m_value.m_valString.c_str();
      break;
    default :
      break;
  }
  return s;
}

//! Return the stored value as a unsigned int.
/*!
 The function returns the value stored in this JSON-value
 object as an unsigned integer
 If the type of the stored value is not an integer, an unsigned int
 or a bool the function returns an undefined value.
 In debug builds the function ASSERTs that the stored value is of
 compatible type: an \b int, an \b unsigned \b int or a \b boolean.

 Also note that starting from version 0.5, the \b wxJSON library
 supports 64-bits integers on platforms that have native support 
 for this.
 Because integer values are stored internally as 64-bits integers,
 this function returns the low 32-bits of the value stored in
 this objects.
 If the value is too large to fit in a 32 bits unsigned integer, the
 function fails with an ASSERTION failure.
 \sa AsInt32 AsUInt32 AsInt
 \sa \ref json_internals_integer
*/
unsigned int
wxJSONValue::AsUInt() const
{
  unsigned int ui;

#if defined( wxJSON_64BIT_INT )
    wxASSERT( GetRefData() );
    wxUint32 ui32;
    bool success = AsUInt32( &ui32 );
    wxASSERT( success );
    ui = (unsigned int) ui32;
#else
  wxJSONRefData* data = GetRefData();
  wxASSERT( data );
  ui = data->m_value.m_valUInt;
  switch ( data->m_type )  {
    case wxJSONTYPE_BOOL :
      ui = data->m_value.m_valBool;
      break;
    case wxJSONTYPE_UINT :
      break;
    case wxJSONTYPE_INT :
      ::wxLogWarning( _T("wxJSONValue::AsUInt() - value type is signed integer" ));
      ui = data->m_value.m_valInt;
      break;
    default :
      wxASSERT( 0 );
      break;
  }
#endif
  return ui;
}

// internal use

//! Return the stored value as a map object.
/*!
 This function is for testing and debugging purposes and you shold never use it.
 To retreive values from an array or map JSON object use the \c Item()
 function or the subscript operator.
 If the stored value is not a map type, returns a NULL pointer.
*/
const wxJSONInternalMap*
wxJSONValue::AsMap() const
{
  wxJSONRefData* data = GetRefData();
  wxASSERT( data );

  const wxJSONInternalMap* v = 0;
  if ( data->m_type == wxJSONTYPE_OBJECT ) {
    v = &( data->m_value.m_valMap );
  }
  return v;
}

//! Return the stored value as an array object.
/*!
 This function is for testing and debugging purposes and you shold never use it.
 To retreive values from an array or map JSON object use the \c Item()
 function or the subscript operator.
 If the stored value is not an array type, returns a NULL pointer.
*/
const wxJSONInternalArray*
wxJSONValue::AsArray() const
{
  wxJSONRefData* data = GetRefData();
  wxASSERT( data );

  const wxJSONInternalArray* v = 0;
  if ( data->m_type == wxJSONTYPE_ARRAY ) {
    v = &( data->m_value.m_valArray );
  }
  return v;
}

// retrieve the members and other info


//! Return TRUE if the object contains an element at the specified index.
/*!
 If the stoerd value is not an array or a map, the function returns FALSE.
*/
bool
wxJSONValue::HasMember( unsigned index ) const
{
  bool r = false;
  int size = Size();
  if ( index < (unsigned) size )  {
    r = true;
  }
  return r;
}

//! Return TRUE if the object contains an element at the specified key.
/*!
 If the stored value is not a key/map map, the function returns FALSE.
*/
bool
wxJSONValue::HasMember( const wxString& key ) const
{
  bool r = false;
  wxJSONRefData* data = GetRefData();
  wxASSERT( data );

  if ( data->m_type == wxJSONTYPE_OBJECT )  {
    wxJSONInternalMap::iterator it = data->m_value.m_valMap.find( key );
    if ( it != data->m_value.m_valMap.end() )  {
      r = true;
    }
  }
  return r;
}

//! Return the size of the array or map stored in this value.
/*!
 Note that both the array and the key/value map may have a size of
 ZERO elements.
 If the stored value is not an array nor a key/value hashmap, the 
 function returns -1.
*/
int
wxJSONValue::Size() const
{
  wxJSONRefData* data = GetRefData();
  wxASSERT( data );

  int size = -1;
  if ( data->m_type == wxJSONTYPE_ARRAY )  {
    size = (int) data->m_value.m_valArray.GetCount();
  }
  if ( data->m_type == wxJSONTYPE_OBJECT )  {
    size = (int) data->m_value.m_valMap.size();
  }
  return size;
}

//! Return an array of keys
/*!
 If the stored value is a key/value map, the function returns an
 array of strings containing the \e key of all elements.
 Note that the returned array may be empty if the map has ZERO
 elements.
 An empty string array is also returned if the stored value is
 not a key/value map.
*/
wxArrayString
wxJSONValue::GetMemberNames() const
{
  wxJSONRefData* data = GetRefData();
  wxASSERT( data );

  wxArrayString arr;
  if ( data->m_type != wxJSONTYPE_OBJECT )   {
    return arr;
  }
  wxJSONInternalMap::iterator it;
  for ( it = data->m_value.m_valMap.begin(); it != data->m_value.m_valMap.end(); it++ )  {
    arr.Add( it->first );
  }
  return arr;
}


// appending items, resizing and deleting items
// NOTE: these functions are not 'const' so we have to call
// the COW() function before accessing data

//! Insert the item at the specified index.
/*!
 The function inserts the item at index \c index in the array.
 If this object does not contain an array type, the actual content is
 deleted and a new array is created.
 Returns a reference to the appended object.
*/
wxJSONValue&
wxJSONValue::Insert( unsigned index, const wxJSONValue& value )
{
  wxJSONRefData* data = COW();
  wxASSERT( data );
  if ( data->m_type != wxJSONTYPE_ARRAY )  {
    // we have to change the type of the actual object to the array type
    SetType( wxJSONTYPE_ARRAY );
  }

  // Insert the wxJSONValue object to the array
  data->m_value.m_valArray.Insert( value, index );
  wxJSONValue& v = data->m_value.m_valArray.Item(index);
  return v;
}

//! Append the specified value in the array.
/*!
 The function appends the value specified in the parameter to the array
 contained in this object.
 If this object does not contain an array type, the actual content is
 deleted and a new array is created.
 Returns a reference to the appended object.
*/
wxJSONValue&
wxJSONValue::Append( const wxJSONValue& value )
{
  wxJSONRefData* data = COW();
  wxASSERT( data );
  if ( data->m_type != wxJSONTYPE_ARRAY )  {
    // we have to change the type of the actual object to the array type
    SetType( wxJSONTYPE_ARRAY );
  }
  // we add the wxJSONValue object to the wxHashMap: note that the
  // hashmap makes a copy of the JSON-value object by calling its
  // copy ctor thus not doing a deep-copy
  data->m_value.m_valArray.Add( value );
  wxJSONValue& v = data->m_value.m_valArray.Last();
  return v;
}


//! \overload Append( const wxJSONValue& )
wxJSONValue&
wxJSONValue::Append( int i )
{
  wxJSONValue v( i );
  wxJSONValue& r = Append( v );
  return r;
}

//! \overload Append( const wxJSONValue& )
wxJSONValue&
wxJSONValue::Append( bool b )
{
  wxJSONValue v( b );
  wxJSONValue& r = Append( v );
  return r;
}

//! \overload Append( const wxJSONValue& )
wxJSONValue&
wxJSONValue::Append( unsigned int ui )
{
  wxJSONValue v( ui );
  wxJSONValue& r = Append( v );
  return r;
}

//! \overload Append( const wxJSONValue& )
wxJSONValue&
wxJSONValue::Append( double d )
{
  wxJSONValue v( d );
  wxJSONValue& r = Append( v );
  return r;
}

//! \overload Append( const wxJSONValue& )
wxJSONValue&
wxJSONValue::Append( const wxChar* str )
{
  wxJSONValue v( str );
  wxJSONValue& r = Append( v );
  return r;
}

//! \overload Append( const wxJSONValue& )
wxJSONValue&
wxJSONValue::Append( const wxString& str )
{
  wxJSONValue v( str );
  wxJSONValue& r = Append( v );
  return r;
}

//! Concatenate a string to this string object.
/*!
 The function concatenates \c str to the string contained
 in this object and returns TRUE if the operation is succefull.
 If the value stored in this value is not a string object
 the function does nothing and returns FALSE.
 Note that in order to be successfull, the value must contain
 a \b wxString object and not a pointer to C-string.
*/
bool
wxJSONValue::Cat( const wxString& str )
{
  wxJSONRefData* data = GetRefData();
  wxASSERT( data );

  bool r = false;
  if ( data->m_type == wxJSONTYPE_STRING )  { 
    wxJSONRefData* data = COW();
    wxASSERT( data );
    data->m_value.m_valString.append( str );
    r = true;
  }
  return r;
}

//! \overload Cat( const wxString& )
bool
wxJSONValue::Cat( const wxChar* str )
{
  wxJSONRefData* data = GetRefData();
  wxASSERT( data );

  bool r = false;
  if ( data->m_type == wxJSONTYPE_STRING )  { 
    wxJSONRefData* data = COW();
    wxASSERT( data );
    data->m_value.m_valString.append( str );
    r = true;
  }
  return r;
}


//! Remove the item at the specified index or key.
/*!
 The function removes the item at index \c index or at the specified
 key in the array or map.
 If this object does not contain an array (for a index parameter) or a map
 (for a key parameter), the function does nothing and returns FALSE.
 If the element does not exist, FALSE is returned.
*/
bool
wxJSONValue::Remove( int index )
{
  wxJSONRefData* data = COW();
  wxASSERT( data );

  bool r = false;
  if ( data->m_type == wxJSONTYPE_ARRAY )  {
    data->m_value.m_valArray.RemoveAt( index );
    r = true;
  }
  return r;
}


//! \overload Remove( int )
bool
wxJSONValue::Remove( const wxString& key )
{
  wxJSONRefData* data = COW();
  wxASSERT( data );

  bool r = false;
  if ( data->m_type == wxJSONTYPE_OBJECT )  {
    wxJSONInternalMap::size_type count = data->m_value.m_valMap.erase( key );
    if ( count > 0 )  {
      r = true;
    }
  }
  return r;
}

//! Remove all members in Array or Object
void
wxJSONValue::RemoveAll()
{
  wxJSONRefData* data = COW();
  wxASSERT( data );

  if ( data->m_type == wxJSONTYPE_OBJECT )  {
    data->m_value.m_valMap.clear();
  }
  else if ( data->m_type == wxJSONTYPE_ARRAY )  {
    data->m_value.m_valArray.Empty();
  }
}

//! Clear the object value.
/*!
 This function causes the object to be empty.
 The function simply calls UnRef() making this object to become
 invalid and set its type to wxJSONTYPE_EMPTY.
*/
void
wxJSONValue::Clear()
{
  UnRef();
  SetType( wxJSONTYPE_EMPTY );
}

// retrieve an item

//! Return the item at the specified index.
/*!
 The function returns a reference to the object at the specified
 index.
 If the element does not exist, the array is enlarged to \c index + 1
 elements and a reference to the last element will be returned.
 New elements will contain NULL values.
 If this object does not contain an array, the old value is
 replaced by an array object which will be enlarged to the needed
 dimension.
*/
wxJSONValue&
wxJSONValue::Item( unsigned index )
{
  wxJSONRefData* data = COW();
  wxASSERT( data );

  if ( data->m_type != wxJSONTYPE_ARRAY )  {
    data = SetType( wxJSONTYPE_ARRAY );
  }
  int size = Size();
  wxASSERT( size >= 0 ); 
  // if the desired element does not yet exist, we create as many
  // elements as needed; the new values will be 'null' values
  if ( index >= (unsigned) size )  {
    wxJSONValue v( wxJSONTYPE_NULL);
    int missing = index - size + 1;
    data->m_value.m_valArray.Add( v, missing );
  }
  return data->m_value.m_valArray.Item( index );
}

//! Return the item at the specified key.
/*!
 The function returns a reference to the object in the map
 that has the specified key.
 If \c key does not exist, a new NULL value is created with
 the provided key and a reference to it is returned.
 If this object does not contain a map, the old value is
 replaced by a map object.
*/
wxJSONValue&
wxJSONValue::Item( const wxString& key )
{
  ::wxLogTrace( traceMask, _T("(%s) searched key=\'%s\'"), __PRETTY_FUNCTION__, key.c_str());
  ::wxLogTrace( traceMask, _T("(%s) actual object: %s"), __PRETTY_FUNCTION__, GetInfo().c_str());

  wxJSONRefData* data = COW();
  wxASSERT( data );

  if ( data->m_type != wxJSONTYPE_OBJECT )  {
    // deletes the contained value;
    data = SetType( wxJSONTYPE_OBJECT );
    return data->m_value.m_valMap[key];
  }
  ::wxLogTrace( traceMask, _T("(%s) searching key \'%s' in the actual object"),
				 __PRETTY_FUNCTION__, key.c_str() ); 
  return data->m_value.m_valMap[key];
}


//! Return the item at the specified index.
/*!
 The function returns a copy of the object at the specified
 index.
 If the element does not exist, the function returns an 'empty' value.
*/
wxJSONValue
wxJSONValue::ItemAt( unsigned index ) const
{
  wxJSONRefData* data = GetRefData();
  wxASSERT( data );

  wxJSONValue v( wxJSONTYPE_EMPTY );
  if ( data->m_type == wxJSONTYPE_ARRAY )  {
    int size = Size();
    wxASSERT( size >= 0 ); 
    if ( index < (unsigned) size )  {
      v = data->m_value.m_valArray.Item( index );
    }
  }
  return v;
}

//! Return the item at the specified key.
/*!
 The function returns a copy of the object in the map
 that has the specified key.
 If \c key does not exist, an 'empty' value is returned.
*/
wxJSONValue
wxJSONValue::ItemAt( const wxString& key ) const
{
  ::wxLogTrace( traceMask, _T("(%s) searched key=\'%s\'"), __PRETTY_FUNCTION__, key.c_str());
  ::wxLogTrace( traceMask, _T("(%s) actual object: %s"), __PRETTY_FUNCTION__, GetInfo().c_str());

  wxJSONRefData* data = GetRefData();
  wxASSERT( data );

  wxJSONValue v( wxJSONTYPE_EMPTY );
  if ( data->m_type == wxJSONTYPE_OBJECT )  {
    wxJSONInternalMap::const_iterator it = data->m_value.m_valMap.find( key );
    if ( it != data->m_value.m_valMap.end() )  {
      v = it->second;
    }
  }
  return v;
}


//! Return the item at the specified index.
/*!
 The function returns a reference to the object at the specified
 index.
 If the element does not exist, the array is enlarged to \c index + 1
 elements and a reference to the last element will be returned.
 New elements will contain NULL values.
 If this object does not contain an array, the old value is
 replaced by an array object.
*/
wxJSONValue&
wxJSONValue::operator [] ( unsigned index )
{
  wxJSONValue& v = Item( index );
  return v;
}

//! Return the item at the specified key.
/*!
 The function returns a reference to the object in the map
 that has the specified key.
 If \c key does not exist, a new NULL value is created with
 the provided key and a reference to it is returned.
 If this object does not contain a map, the old value is
 replaced by a map object.
*/
wxJSONValue&
wxJSONValue::operator [] ( const wxString& key )
{
  wxJSONValue& v = Item( key );
  return v;
}

//
// assignment operators
// note that reference counting is only used if the original
// value is a wxJSONValue object
// in all other cases, the operator= function deletes the old
// content and assigns the new one 


//! Assign the specified value to this object replacing the old value.
/*!
 The function assigns to this object the value specified in the
 right operand of the assignment operator.
 Note that the old value is deleted but not the other data members
 in the wxJSONRefData structure.
 This is particularly usefull for the parser class which stores
 comment lines in a temporary wxJSONvalue object that is of type
 wxJSONTYPE_EMPTY.
 As comment lines may apear before the value they refer to, comments
 are stored in a value that is not yet being read.
 when the value is read, it is assigned to the temporary JSON value
 object without deleting the comment lines.
*/
wxJSONValue&
wxJSONValue::operator = ( int i )
{
  wxJSONRefData* data = SetType( wxJSONTYPE_INT );
  data->m_value.m_valInt = i;
  return *this;
}


//! \overload operator = (int)
wxJSONValue&
wxJSONValue::operator = ( bool b )
{
  wxJSONRefData* data = SetType( wxJSONTYPE_BOOL );
  data->m_value.m_valBool = b;
  return *this;
}

//! \overload operator = (int)
wxJSONValue&
wxJSONValue::operator = ( unsigned int i )
{
  wxJSONRefData* data = SetType( wxJSONTYPE_UINT );
  data->m_value.m_valUInt = i;
  return *this;
}

//! \overload operator = (int)
wxJSONValue&
wxJSONValue::operator = ( double d )
{
  wxJSONRefData* data = SetType( wxJSONTYPE_DOUBLE );
  data->m_value.m_valDouble = d;
  return *this;
}


//! \overload operator = (int)
wxJSONValue&
wxJSONValue::operator = ( const wxChar* str )
{
  wxJSONRefData* data = SetType( wxJSONTYPE_CSTRING );
  data->m_value.m_valCString = str;
#if !defined( WXJSON_USE_CSTRING )
  data->m_type = wxJSONTYPE_STRING;
  data->m_value.m_valString.assign( str );
#endif
  return *this;
}

//! \overload operator = (int)
wxJSONValue&
wxJSONValue::operator = ( const wxString& str )
{
  wxJSONRefData* data = SetType( wxJSONTYPE_STRING );
  data->m_value.m_valString.assign( str );
  return *this;
}

//! Assignment operator using reference counting.
/*!
 Unlike all other assignment operators, this one makes a
 swallow copy of the other JSON value object.
 The function calls \c Ref() to get a shared referenced
 data.
 \sa \ref json_internals_cow
*/
wxJSONValue&
wxJSONValue::operator = ( const wxJSONValue& other )
{
  Ref( other );
  return *this;
}


// finding elements


//! Return a value or a default value.
/*!
 This function returns a copy of the value object for the specified key.
 If the key is not found, a copy of \c defaultValue is returned.
*/
wxJSONValue
wxJSONValue::Get( const wxString& key, const wxJSONValue& defaultValue ) const
{
  // NOTE: this function does many wxJSONValue copies.
  // so implementing COW is a good thing

  // this is the first copy (the default value)
  wxJSONValue v( defaultValue );

  wxJSONRefData* data = GetRefData();
  wxASSERT( data );
  if ( data->m_type == wxJSONTYPE_OBJECT )  {
    wxJSONInternalMap::iterator it = data->m_value.m_valMap.find( key );
    if ( it != data->m_value.m_valMap.end() )  {
      v = it->second;
    }
  }
  return v;
}


// protected functions

//! Find an element
/*!
 The function returns a pointer to the element at index \c index
 or a NULL pointer if \c index does not exist.
 A NULL pointer is also returned if the object does not contain an
 array nor a key/value map.
*/
wxJSONValue*
wxJSONValue::Find( unsigned index ) const
{
  wxJSONRefData* data = GetRefData();
  wxASSERT( data );

  wxJSONValue* vp = 0;

  if ( data->m_type == wxJSONTYPE_ARRAY )  {
    size_t size = data->m_value.m_valArray.GetCount();
    if ( index < size )  {
      vp = &(data->m_value.m_valArray.Item( index ));
    }
  }
  return vp;
}

//! Find an element
/*!
 The function returns a pointer to the element with key \c key
 or a NULL pointer if \c key does not exist.
 A NULL pointer is also returned if the object does not contain a
 key/value map.
*/
wxJSONValue*
wxJSONValue::Find( const wxString& key ) const
{
  wxJSONRefData* data = GetRefData();
  wxASSERT( data );

  wxJSONValue* vp = 0;

  if ( data->m_type == wxJSONTYPE_OBJECT )  {
    wxJSONInternalMap::iterator it = data->m_value.m_valMap.find( key );
    if ( it != data->m_value.m_valMap.end() )  {
      vp = &(it->second);
    }
  }
  return vp;
}



//! Return a string description of the type
/*!
 This static function is only usefull for debugging purposes and
 should not be used by users of this class.
 It simply returns a string representation of the JSON value
 type stored in a object.
 For example, if \c type is wxJSONTYPE_INT the function returns the
 string "wxJSONTYPE_INT".
 If \c type is out of range, an empty string is returned (should
 never happen).
*/
wxString
wxJSONValue::TypeToString( wxJSONType type )
{
  static const wxChar* str[] = {
    _T( "wxJSONTYPE_EMPTY" ),   // 0
    _T( "wxJSONTYPE_NULL" ),    // 1
    _T( "wxJSONTYPE_INT" ),     // 2
    _T( "wxJSONTYPE_UINT" ),    // 3
    _T( "wxJSONTYPE_DOUBLE" ),  // 4
    _T( "wxJSONTYPE_STRING" ),  // 5
    _T( "wxJSONTYPE_CSTRING" ), // 6
    _T( "wxJSONTYPE_BOOL" ),    // 7
    _T( "wxJSONTYPE_ARRAY" ),   // 8
    _T( "wxJSONTYPE_OBJECT" ),  // 9
    _T( "wxJSONTYPE_INT32" ),   // 10
    _T( "wxJSONTYPE_INT64" ),   // 11
    _T( "wxJSONTYPE_UINT32" ),  // 12
    _T( "wxJSONTYPE_UINT64" ),  // 13
  };

  wxString s;
  int idx = (int) type;
  if ( idx >= 0 && idx < 14 )  {
    s = str[idx];
  }
  return s;
}


//! Returns informations about the object
/*!
 The function is only usefull for debugging purposes and will probably
 be dropped in future versions.
 Returns a string that contains info about the object such as:

 \li the type of the object
 \li the size 
 \li the progressive counter
 \li the pointer to referenced data
 \li the progressive counter of referenced data
 \li the number of share of referenced data

The \c deep parameter is used to specify if the function will be called
recursively in order to dump sub-items. If the parameter is TRUE than a
deep dump is executed.

The \c indent is the initial indentation: it is incremented by 3 every
time the Dump() function is called recursively.
*/
wxString
wxJSONValue::Dump( bool deep, int indent ) const
{
  wxJSONRefData* data = GetRefData();
  wxASSERT( data );

  wxJSONType type = GetType();

  wxString s;
  if ( indent > 0 )   {
    s.append( indent, ' ' );
  }

  wxString s1;
  wxString s2;
#if defined( WXJSON_USE_VALUE_COUNTER )
  s1.Printf( _T("Object: Progr=%d Type=%s Size=%d comments=%d\n"),
		m_progr,
		TypeToString( type ).c_str(),
		Size(),
		data->m_comments.GetCount() );
  s2.Printf(_T("      : RefData=%p Progr=%d Num shares=%d\n"),
			data, data->m_progr, data->GetRefCount() );
#else
  s1.Printf( _T("Object: Type=%s Size=%d comments=%d\n"),
			TypeToString( type ).c_str(),
			Size(),
			data->m_comments.GetCount() );
  s2.Printf(_T("      : RefData=%p Num shares=%d\n"),
			data, data->GetRefCount() );
#endif
  s.append( s1 );
  if ( indent > 0 )   {
    s.append( indent, ' ' );
  }
  s.append( s2 );

  wxString sub;

  // if we have to do a deep dump, we call the Dump() function for
  // every sub-item
  if ( deep )   {
    indent += 3;
    const wxJSONInternalMap* map;
    int size;;
    wxJSONInternalMap::const_iterator it; 
    switch ( type )    {
      case wxJSONTYPE_OBJECT :
        map = AsMap();
        size = Size();
        for ( it = map->begin(); it != map->end(); ++it )  {
          const wxJSONValue& v = it->second;
          sub = v.Dump( true, indent );
          s.append( sub );
        }
        break;
      case wxJSONTYPE_ARRAY :
		  {
			size = Size();
			for ( int i = 0; i < size; i++ )  {
			  const wxJSONValue* v = Find( i );
			  wxASSERT( v );
			  sub = v->Dump( true, indent );
			  s.append( sub );
			}
		  }
        break;
      default :
        break;
    }
  }
  return s;
}



//! Returns informations about the object
/*!
 The function is only usefull for debugging purposes and will probably
 be dropped in future versions.
 You should not rely on this function to exist in future versions.
*/
wxString
wxJSONValue::GetInfo() const
{
  wxJSONRefData* data = GetRefData();
  wxASSERT( data );

  wxString s;
#if defined( WXJSON_USE_VALUE_CONTER )
  s.Printf( _T("Object: Progr=%d Type=%s Size=%d comments=%d\n"),
			data->m_progr,
			wxJSONValue::TypeToString( data->m_type ).c_str(),
			Size(),
			data->m_comments.GetCount() );
#else
  s.Printf( _T("Object: Type=%s Size=%d comments=%d\n"),
			wxJSONValue::TypeToString( data->m_type ).c_str(),
			Size(), data->m_comments.GetCount() );
#endif
  if ( data->m_type == wxJSONTYPE_OBJECT ) {
    wxArrayString arr = GetMemberNames();
    for ( unsigned int i = 0; i < arr.size(); i++ )  {
      s.append( _T("    Member name: "));
      s.append( arr[i] );
      s.append( _T("\n") );
    }
  }
  return s;
}

//! The comparison function
/*!
 This function returns TRUE if this object looks like \c other.
 Note that this class does not define a comparison operator
 (the classical \b operator== function) because the notion
 of \b equal for JSON values objects is not applicable.
 The comment strings array are not compared: JSON value objects
 are \b the \b same if they contains the same values, regardless the
 comment's strings.

 Note that the function does not return the element that cause the
 comparison to return FALSE. There is not a good structure to
 tell this information.
 If you need it for debugging purposes, you have to turn on the
 \b sameas tracing feature by setting the WXTRACE environment
 variable (you need a debug version of the application):

 \code
   export WXTRACE=sameas     // for unix systems that use bash
 \endcode

 Note that if the two JSON value objects share the same referenced
 data, the function immediatly returns TRUE without doing a deep
 comparison which is, sure, useless.
 For further info see \ref json_internals_compare.

 \bug comparing very large 64-bits integers that differ by a small
	value may cause the function to return TRUE intead of FALSE
*/
bool
wxJSONValue::IsSameAs( const wxJSONValue& other ) const
{
  // this is a recursive function: it calls itself
  // for every 'value' object in an array or map
  bool r = true;

  // some variables used in the switch statement
  int size; wxJSONInternalMap::const_iterator it; int* usedElements;

  // get the referenced data for the two objects
  wxJSONRefData* data = GetRefData();
  wxJSONRefData* otherData = other.GetRefData();

  if ( data == otherData ) {
    ::wxLogTrace( compareTraceMask, _T("(%s) objects share the same referenced data - r=TRUE"),
			 __PRETTY_FUNCTION__ );
    return true;
  }


  // if the type does not match the function compares the values if
  // they are of compatible types such as INT, UINT and DOUBLE
  if ( data->m_type != otherData->m_type )  {
    // if the types are not compatible, returns false
    double val, otherVal;
    switch ( data->m_type )  {
      case wxJSONTYPE_INT :
        val = data->m_value.m_valInt;
        break;
      case wxJSONTYPE_UINT :
        val = (int)data->m_value.m_valUInt;
        break;
      case wxJSONTYPE_DOUBLE :
        val = data->m_value.m_valDouble;
        break;
      default:
        return false;
        break;
    }
    switch ( otherData->m_type )  {
      case wxJSONTYPE_INT :
        otherVal = otherData->m_value.m_valInt;
        break;
      case wxJSONTYPE_UINT :
        otherVal = (int)otherData->m_value.m_valUInt;
        break;
      case wxJSONTYPE_DOUBLE :
        otherVal = otherData->m_value.m_valDouble;
        break;
      default:
        return false;
        break;
    }
    bool r = false;
    if ( val == otherVal )  {
      r = true;
    }
    return r;
  }

  // for comparing wxJSONTYPE_CSTRING we use two temporary wxString
  // objects: this is to avoid using strcmp() and wcscmp() which
  // may not be available on all platforms
  wxString s1, s2;

  switch ( data->m_type )  {
    case wxJSONTYPE_EMPTY :
    case wxJSONTYPE_NULL :
      // there is no need to compare the values
      break;
    case wxJSONTYPE_INT :
      if ( data->m_value.m_valInt != otherData->m_value.m_valInt )  {
        r = false;
      }
      break;
    case wxJSONTYPE_UINT :
      if ( data->m_value.m_valUInt != otherData->m_value.m_valUInt )  {
        r = false;
      }
      break;
    case wxJSONTYPE_DOUBLE :
      if ( data->m_value.m_valDouble != otherData->m_value.m_valDouble )  {
        r = false;
      }
      break;
    case wxJSONTYPE_CSTRING :
      s1 = wxString( data->m_value.m_valCString );
      s2 = wxString( otherData->m_value.m_valCString );
      // i = strcmp( m_value.m_valCString, other.m_value.m_valCString );
      // i = wcscmp( m_value.m_valCString, other.m_value.m_valCString );
      if ( s1 != s2 )  {
        r = false;
      }
      break;
    case wxJSONTYPE_BOOL :
      if ( data->m_value.m_valBool != otherData->m_value.m_valBool )  {
        r = false;
      }
      break;
    case wxJSONTYPE_STRING :
      if ( data->m_value.m_valString != otherData->m_value.m_valString )  {
        r = false;
      }
      break;
    case wxJSONTYPE_ARRAY :
		{
		  size = Size();
		  ::wxLogTrace( compareTraceMask, _T("(%s) Comparing an array object - size=%d"),
				 __PRETTY_FUNCTION__, size );

		  if ( size != other.Size() )  {
			::wxLogTrace( compareTraceMask, _T("(%s) Sizes does not match"),
				 __PRETTY_FUNCTION__ );
			return false;
		  }
		  // for every element in this object iterates through all
		  // items in the 'other' object searching for a matching
		  // value. if not found, returns FALSE.
		  // note that when an element is found in other it must be marked as
		  // 'already used' (see 'test/testjson4.cpp')
		  usedElements = new int[size];
		  for ( int y = 0; y < size; y++ )  {
			usedElements[y] = 0;
		  } 
		  for ( int i = 0; i < size; i++ )  {
			::wxLogTrace( compareTraceMask, _T("(%s) Comparing array element=%d"),
  				 __PRETTY_FUNCTION__, i );
			r = false;
			for ( int y = 0; y < size; y++ )  {
			  if ( ItemAt( i ).IsSameAs( other.ItemAt( y )) && usedElements[y] == 0 )  {
				r = true;
				usedElements[y] = 1;
				break;
			  } 
			}
			if ( r == false )  {
			  delete [] usedElements;
			  ::wxLogTrace( compareTraceMask, _T("(%s) Comparison failed"), __PRETTY_FUNCTION__ );
			  return false;
			}
		  }
		  delete [] usedElements;
		}
      break;
    case wxJSONTYPE_OBJECT :
      size = Size();
      ::wxLogTrace( compareTraceMask, _T("(%s) Comparing a map obejct - size=%d"),
			 __PRETTY_FUNCTION__, size );

      if ( size != other.Size() )  {
        ::wxLogTrace( compareTraceMask, _T("(%s) Comparison failed - sizes does not match"),
			 __PRETTY_FUNCTION__ );
        return false;
      }
      // for every key, calls itself on the value found in
      // the other object. if 'key' does no exist, returns FALSE
      // wxJSONInternalMap::const_iterator it;
      for ( it = data->m_value.m_valMap.begin(); it != data->m_value.m_valMap.end(); it++ )  {
        wxString key = it->first;
        ::wxLogTrace( compareTraceMask, _T("(%s) Comparing map object - key=%s"),
			 __PRETTY_FUNCTION__, key.c_str() );
        wxJSONValue otherVal = other.ItemAt( key );
        bool isSame = it->second.IsSameAs( otherVal );
        if ( !isSame )  {
          ::wxLogTrace( compareTraceMask, _T("(%s) Comparison failed for the last object"),
			 __PRETTY_FUNCTION__ );
          return false;
        }
      }
      break;
    default :
      // should never happen
      wxFAIL_MSG( _T("wxJSONValue::IsSameAs() unexpected wxJSONType"));
      // wxASSERT( 0 );
      break;
  }
  return r;
}

//! Add a comment to this JSON value object.
/*!
 The function adds a comment string to this JSON value object and returns
 the total number of comment strings belonging to this value.
 Note that the comment string must be a valid C/C++ comment because the
 wxJSONWriter does not modify it.
 In other words, a C++ comment string must start with '//' and must end with
 a new-line character. If the final LF char is missing, the 
 automatically adds it.
 You can also add C-style comments which must be enclosed in the usual
 C-comment characters.
 For C-style comments, the function does not try to append the final comment
 characters but allows trailing whitespaces and new-line chars.
 The \c position parameter is one of:

 \li wxJSONVALUE_COMMENT_BEFORE: the comment will be written before the value
 \li wxJSONVALUE_COMMENT_INLINE: the comment will be written on the same line
 \li wxJSONVALUE_COMMENT_AFTER: the comment will be written after the value
 \li wxJSONVALUE_COMMENT_DEFAULT: the old value of comment's position is not
	changed; if no comments were added to the value object this is the
	same as wxJSONVALUE_COMMENT_BEFORE.

 To know more about comment's storage see \ref json_comment_add

*/
int
wxJSONValue::AddComment( const wxString& str, int position )
{
  wxJSONRefData* data = COW();
  wxASSERT( data );

  ::wxLogTrace( traceMask, _T("(%s) comment=%s"), __PRETTY_FUNCTION__, str.c_str() );
  int r = -1;
  int len = str.length();
  if ( len < 2 )  {
    ::wxLogTrace( traceMask, _T("     error: len < 2") );
    return -1;
  }
  if ( str[0] != '/' )  {
    ::wxLogTrace( traceMask, _T("     error: does not start with\'/\'") );
    return -1;
  }
  if ( str[1] == '/' )  {       // a C++ comment: check that it ends with '\n'
    ::wxLogTrace( traceMask, _T("     C++ comment" ));
    if ( str.GetChar(len - 1) != '\n' )  {
      wxString temp( str );
      temp.append( 1, '\n' );
      data->m_comments.Add( temp );
      ::wxLogTrace( traceMask, _T("     C++ comment: LF added") );
    }
    else  {
      data->m_comments.Add( str );
    }
    r = data->m_comments.size();
  }
  else if ( str[1] == '*' )  {  // a C-style comment: check that it ends with '*/'
    ::wxLogTrace( traceMask, _T("     C-style comment") );
    int lastPos = len - 1;
    wxChar ch = str.GetChar( lastPos );
    // skip leading whitespaces
    while ( ch == ' ' || ch == '\n' || ch == '\t' )  {
      --lastPos;
      ch = str.GetChar( lastPos );
    }
    if ( str.GetChar( lastPos ) == '/' &&  str.GetChar( lastPos - 1 ) == '*' ) {
      data->m_comments.Add( str );
      r = data->m_comments.size();
    }  }
  else  {
    ::wxLogTrace( traceMask, _T("     error: is not a valid comment string") );
    r = -1;
  }
  // if the comment was stored, store the position
  if ( r >= 0 && position != wxJSONVALUE_COMMENT_DEFAULT )  {
    data->m_commentPos = position;
  }
  return r;
}

//! Add one or more comments to this JSON value object.
/*!
 The function adds the strings contained in \c comments to the comment's
 string array of this value object by calling the AddComment( const wxString&,int)
 function for every string in the \c comment array.
 Returns the number of strings correctly added.
*/
int
wxJSONValue::AddComment( const wxArrayString& comments, int position )
{
  int siz = comments.GetCount(); int r = 0;
  for ( int i = 0; i < siz; i++ ) {
    int r2 = AddComment( comments[i], position );
    if ( r2 >= 0 )  {
      ++r;
    }
  }
  return r;
}

//! Return a comment string.
/*!
 The function returns the comment string at index \c idx.
 If \c idx is out of range, an empty string is returned.
 If \c idx is equal to -1, then the function returns a string
 that contains all comment's strings stored in the array.
*/
wxString
wxJSONValue::GetComment( int idx ) const
{
  wxJSONRefData* data = GetRefData();
  wxASSERT( data );

  wxString s;
  int size = data->m_comments.GetCount();
  if ( idx < 0 )  {
    for ( int i = 0; i < size; i++ )  {
      s.append( data->m_comments[i] );
    }
  }
  else if ( idx < size )  {
    s = data->m_comments[idx];
  }
  return s;
}

//! Return the number of comment strings.
int
wxJSONValue::GetCommentCount() const
{
  wxJSONRefData* data = GetRefData();
  wxASSERT( data );

  int d = data->m_comments.GetCount();
  ::wxLogTrace( traceMask, _T("(%s) comment count=%d"), __PRETTY_FUNCTION__, d );
  return d;
}

//! Return the comment position.
int
wxJSONValue::GetCommentPos() const
{
  wxJSONRefData* data = GetRefData();
  wxASSERT( data );
  return data->m_commentPos;
}

//! Get the comment string's array.
const wxArrayString&
wxJSONValue::GetCommentArray() const
{
  wxJSONRefData* data = GetRefData();
  wxASSERT( data );

  return data->m_comments;
}

//! Clear all comment strings
void
wxJSONValue::ClearComments()
{
  wxJSONRefData* data = COW();
  wxASSERT( data );

  data->m_comments.clear();
}


//! Set the type of the stored value.
/*!
 The function sets the type of the stored value as specified in
 the provided argument.
 If the actual type is equal to \c type, nothing happens and this
 JSON value object retains the original type and value.
 If the type differs, however, the original type and value are
 lost.

 The function just sets the type of the object and not the
 value itself.
 If the object does not have a data structure it is allocated
 using the CreateRefData() function unless the type to be set
 is wxJSONTYPE_EMPTY. In this case and if a data structure is
 not yet allocated, it is not allocated.

 If the object already contains a data structure it is not deleted
 but the type is changed in the original data structure.
 Complex values in the old structure are cleared.
 The \c type argument can be one of the following:

  \li wxJSONTYPE_EMPTY: an empty (not initialized) JSON value
  \li wxJSONTYPE_NULL: a NULL value
  \li wxJSONTYPE_INT: an integer value
  \li wxJSONTYPE_UINT: an unsigned integer
  \li wxJSONTYPE_DOUBLE: a double precision number
  \li wxJSONTYPE_BOOL: a boolean
  \li wxJSONTYPE_CSTRING: a C string
  \li wxJSONTYPE_STRING: a wxString object
  \li wxJSONTYPE_ARRAY: an array of wxJSONValue objects
  \li wxJSONTYPE_OBJECT: a hashmap of key/value pairs where \e value is a wxJSONValue object
  \li wxJSONTYPE_INT32: a 32-bits integer value
  \li wxJSONTYPE_UINT32: an unsigned 32-bits integer
  \li wxJSONTYPE_INT64: a 64-bits integer value
  \li wxJSONTYPE_UINT64: an unsigned 64-bits integer

 The integer storage depends on the platform: for platforms that support 64-bits
 integers, integers are always stored as 64-bits integers.
 You should never use the wxJSON_TYPE(U)INTxx types: integers are always stored
 as the generic wxJSONTYPE_(U)INT 
 To know more about the internal representation of integers, read
 \ref json_internals_integer.

 Note that there is no need to set a type for the object in order to assign
 a value to it.
 In other words, if you want to construct a JSON value which holds an integer
 value of 10, just use the specific constructor:
 \code
   wxJSONValue value( 10 );
 \endcode
 which sets the integer type and also the numeric value.
 Moreover, there is no need to set the type for none of the handled types,
 not only for primitive types but for complex types, too.
 For example, if you want to construct an array of JSON values, just use
 the default ctor and call the Append() member function which will append the
 first element to the array and will set the array type:
 \code
   wxJSONValue value;
   value.Append( "a string" );
 \endcode

 The only exception is for empty (not initialized) values:
 \code
   // construct an empty value
   wxJSONValue v1( wxJSONTYPE_EMPTY );

   // this is the same but the old structure is not deleted
   wxJSONValue v2;
   v2.SetType( wxJSONTYPE_EMPTY );
 \endcode

 Maybe I have to spend some words about the (subtle) difference
 between an \b empty value, a \b null value and an \b empty 
 JSON \b object or \b array.

 A \b empty value is a JSONvalue object that was not initialized.
 Its type is \b wxJSONTYPE_EMPTY and it is used internally by the
 wxJSON library. You should never write empty values to JSON text
 because the output is not a valid JSON text.
 If you write an \e empty value to the wxJSONWriter class you 
 obtain the following text (which is not a JSON text):
 \code
   <empty>
 \endcode

 A \b null value is of type \b wxJSONTYPE_NULL and it is constructed
 using the default cosntructor.
 Its text output is valid JSON text:
 \code
   null
 \endcode

 An \b empty JSON \b object or an \b empty JSON \b array is a JSONvalue
 object of type wxJSONTYPE_OBJECT or wxJSONTYPE_ARRAY but they
 do not contain any element.
 Their output is valid JSON text:
 \code
   [ ]
   { }
 \endcode
*/
wxJSONRefData*
wxJSONValue::SetType( wxJSONType type )
{
  wxJSONRefData* data = GetRefData();
  wxJSONType oldType = GetType();

  // check that type is within the allowed range
  wxASSERT((type >= wxJSONTYPE_EMPTY) && (type <= wxJSONTYPE_OBJECT));
  if ( (type < wxJSONTYPE_EMPTY) || (type > wxJSONTYPE_OBJECT) )  {
    type = wxJSONTYPE_EMPTY;
  }

  // the function unshares the referenced data but does not delete the
  // structure. This is because the wxJSON reader stores comments
  // that apear before the value in a temporary value of type wxJSONTYPE_EMPTY
  // which is invalid and, next, it stores the JSON value in the same
  // wxJSONValue object.
  // If we would delete the structure using 'Unref()' we loose the
  // comments
  data = COW();

  // do nothing if the actual type is the same as 'type'
  if ( type == oldType )  {
    return data;
  }

  // change the type of the referened structure
  // NOTE: integer types are always stored as the generic integer types
  if ( type == wxJSONTYPE_INT32 || type == wxJSONTYPE_INT64 )  {
    type = wxJSONTYPE_INT;
  }
  if ( type == wxJSONTYPE_UINT32 || type == wxJSONTYPE_UINT64 )  {
    type = wxJSONTYPE_UINT;
  }
  wxASSERT( data );
  data->m_type = type;

  // clears complex objects of the old type
  switch ( oldType )  {
    case wxJSONTYPE_STRING:
      data->m_value.m_valString.clear();
      break;
    case wxJSONTYPE_ARRAY:
      data->m_value.m_valArray.Clear();
      break;
    case wxJSONTYPE_OBJECT:
      data->m_value.m_valMap.clear();
      break;
    default :
      // there is not need to clear primitive types
      break;
  }

  // if the WXJSON_USE_CSTRING macro is not defined, the class forces
  // C-string to be stored as wxString objects
#if !defined( WXJSON_USE_CSTRING )
  if ( data->m_type == wxJSONTYPE_CSTRING )  {
    data->m_type = wxJSONTYPE_STRING;
  }
#endif
  return data;
}

//! Return the line number of this JSON value object
/*!
 The line number of a JSON value object is set to -1 when the
 object is constructed.
 The line number is set by the parser class, wxJSONReader, when
 a JSON text is read from a stream or a string.
 it is used when reading a comment line: comment lines that apear
 on the same line as a value are considered \b inline comments of
 the value.
*/
int
wxJSONValue::GetLineNo() const
{
  // return ZERO if there is not a referenced data structure
  int n = 0;
  wxJSONRefData* data = GetRefData();
  if ( data != 0 ) {
    n = data->m_lineNo;
  }
  return n;
}

//! Set the line number of this JSON value object.
void
wxJSONValue::SetLineNo( int num )
{
  wxJSONRefData* data = COW();
  wxASSERT( data );
  data->m_lineNo = num;
}

//! Set the pointer to the referenced data.
void
wxJSONValue::SetRefData(wxJSONRefData* data)
{
  m_refData = data;
}

//! Increments the referenced data counter.
void
wxJSONValue::Ref(const wxJSONValue& clone)
{
  // nothing to be done
  if (m_refData == clone.m_refData)
    return;

  // delete reference to old data
  UnRef();

  // reference new data
  if ( clone.m_refData )
  {
    m_refData = clone.m_refData;
    ++(m_refData->m_refCount);
  }
}

//! Unreferences the shared data
/*!
 The function decrements the number of shares in wxJSONRefData::m_refCount
 and if it is ZERO, deletes the referenced data.
 It is called by the destructor.
*/
void
wxJSONValue::UnRef()
{
  if ( m_refData )   {
    wxASSERT_MSG( m_refData->m_refCount > 0, _T("invalid ref data count") );

    if ( --m_refData->m_refCount == 0 )
      delete m_refData;
      m_refData = NULL;
    }
}

//! Makes an exclusive copy of shared data
void
wxJSONValue::UnShare()
{
  AllocExclusive();
}


//! Do a deep copy of the other object.
/*!
 This function allocates a new ref-data structure and copies it
 from the object \c other.
*/
void
wxJSONValue::DeepCopy( const wxJSONValue& other )
{
  UnRef();
  wxJSONRefData* data = CloneRefData( other.m_refData );
  SetRefData( data );
}

//! Return the pointer to the referenced data structure.
wxJSONRefData*
wxJSONValue::GetRefData() const
{
  wxJSONRefData* data = m_refData;
  return data;
}


//! Make a copy of the referenced data.
/*!
 The function allocates a new instance of the wxJSONRefData
 structure, copies the content of \c other and returns the pointer
 to the newly created structure.
 This function is called by the wxObject::UnRef() function
 when a non-const member function is called on multiple
 referenced data.
*/
wxJSONRefData*
wxJSONValue::CloneRefData( const wxJSONRefData* otherData ) const
{
  wxASSERT( otherData );

  // make a static cast to pointer-to-wxJSONRefData
  const wxJSONRefData* other = otherData;

  // allocate a new instance of wxJSONRefData
  wxJSONRefData* data = new wxJSONRefData();

  // wxObjectRefData is private and cannot be accessed by this
  // wxJSONValue class - but we need to set the reference counter
  // to 1 so we cannot simply use the default copy ctor of 
  // wxJSONRefData* data = new wxJSONRefData( *other );
  // wxJSONRefData structure.
  //data->m_count = 1;

  // copy the referenced data structure's data members
  data->m_type       = other->m_type;
  data->m_value      = other->m_value;
  data->m_commentPos = other->m_commentPos;
  data->m_comments   = other->m_comments;
  data->m_lineNo     = other->m_lineNo;

  ::wxLogTrace( cowTraceMask, _T("(%s) CloneRefData() PROGR: other=%d data=%d"),
			 __PRETTY_FUNCTION__, other->GetRefCount(), data->GetRefCount() );

  return data;
}

//! Create a new data structure
/*!
 The function allocates a new instance of the wxJSONRefData
 structure and returns its pointer.
 The type of the JSON value is set to wxJSONTYPE_EMPTY (=
 a not initialized value).
*/
wxJSONRefData*
wxJSONValue::CreateRefData() const
{
  wxJSONRefData* data = new wxJSONRefData();
  data->m_type = wxJSONTYPE_EMPTY;
  return data;
}



//! Make sure the referenced data is unique
/*!
 This function is called by all non-const member functions and makes
 sure that the referenced data is unique by calling \b UnShare()
 If the referenced data is shared acrosss other wxJSONValue instances,
 the \c UnShare() function makes a private copy of the shared data.
*/
wxJSONRefData*
wxJSONValue::COW()
{
  wxJSONRefData* data = GetRefData();
  ::wxLogTrace( cowTraceMask, _T("(%s) COW() START data=%p data->m_count=%d"),
			 __PRETTY_FUNCTION__, data, data->GetRefCount());
  UnShare();
  data = GetRefData();
  ::wxLogTrace( cowTraceMask, _T("(%s) COW() END data=%p data->m_count=%d"),
			 __PRETTY_FUNCTION__, data, data->GetRefCount());
  return GetRefData();
}

//! Makes a private copy of the referenced data
void
wxJSONValue::AllocExclusive()
{
  if ( !m_refData )
  {
     m_refData = CreateRefData();
  }
  else if ( m_refData->GetRefCount() > 1 )
  {
     // note that ref is not going to be destroyed in this case
     const wxJSONRefData* ref = m_refData;
     UnRef();

     // ... so we can still access it
     m_refData = CloneRefData(ref);
  }
  //else: ref count is 1, we are exclusive owners of m_refData anyhow

  wxASSERT_MSG( m_refData && m_refData->GetRefCount() == 1,
                  _T("wxObject::AllocExclusive() failed.") );
}


/*************************************************************************

			64-bits integer support

*************************************************************************/

#if defined( wxJSON_64BIT_INT)


//! \overload wxJSONValue()
wxJSONValue::wxJSONValue( wxInt64 i )
{
  m_refData = 0;
  wxJSONRefData* data = Init( wxJSONTYPE_INT );
  wxASSERT( data );
  if ( data != 0 ) {
    data->m_value.m_valInt = i;
  }
}

//! \overload wxJSONValue()
wxJSONValue::wxJSONValue( wxUint64 ui )
{
  m_refData = 0;
  wxJSONRefData* data = Init( wxJSONTYPE_UINT );
  wxASSERT( data );
  if ( data != 0 ) {
    data->m_value.m_valUInt = ui;
  }
}

//! Return TRUE if the stored value is a 32-bits integer
/*!
 This function is only available on 64-bits platforms and returns
 TRUE if, and only if, the stored value is of type \b wxJSONTYPE_INT
 and the numeric value fits in a 32-bits signed integer.
*/
bool
wxJSONValue::IsInt32() const
{
  wxJSONRefData* data = GetRefData();
  wxASSERT( data );

  bool r = false;
  switch ( data->m_type )  {
    case wxJSONTYPE_INT : 
      if ( data->m_value.m_valInt <= INT_MAX &&
		data->m_value.m_valInt >= INT_MIN )  {
        r = true;
      }
      break;
    default :
      break;
  }
  return r;
}

//! Return TRUE if the stored value is a 64-bits integer
/*!
 This function is only available on 64-bits platforms and returns
 TRUE if, and only if, the stored value is of type \b wxJSONTYPE_INT
 and the numeric value is too large to fit in a 32-bits signed integer.
*/
bool
wxJSONValue::IsInt64() const
{
  wxJSONRefData* data = GetRefData();
  wxASSERT( data );

  bool r = false;
  switch ( data->m_type )  {
    case wxJSONTYPE_INT : 
      if ( data->m_value.m_valInt > INT_MAX ||
		data->m_value.m_valInt < INT_MIN )  {
        r = true;
      }
      break;
    default :
      break;
  }
  return r;
}


//! Return TRUE if the stored value is a unsigned 32-bits integer
/*!
 This function is only available on 64-bits platforms and returns
 TRUE if, and only if, the stored value is of type \b wxJSONTYPE_UINT
 and the numeric value fits in a 32-bits unsigned integer.
*/
bool
wxJSONValue::IsUInt32() const
{
  wxJSONRefData* data = GetRefData();
  wxASSERT( data );

  bool r = false;
  switch ( data->m_type )  {
    case wxJSONTYPE_UINT :
      if ( data->m_value.m_valUInt <= UINT_MAX )  {
        r = true;
      }
      break;
    default :
      break;
  }
  return r;
}

//! Return TRUE if the stored value is a 64-bits unsigned integer
/*!
 This function is only available on 64-bits platforms and returns
 TRUE if, and only if, the stored value is of type \b wxJSONTYPE_UINT
 and the numeric value is too large to fit in a 32-bits unsigned integer.
*/
bool
wxJSONValue::IsUInt64() const
{
  wxJSONRefData* data = GetRefData();
  wxASSERT( data );

  bool r = false;
  switch ( data->m_type )  {
    case wxJSONTYPE_UINT :
      if ( data->m_value.m_valUInt > UINT_MAX )    { 
        r = true;
      }
      break;
    default :
      break;
  }
  return r;
}

//! Returns the low-order 32 bits of the value as an integer
/*!
 This function is only available on 64-bits platforms and returns
 the low-order 32-bits of the integer stored in the JSON value.
 Note that all integer types are stored as \b wx(U)Int64 data types by
 the JSON value class and that the function does not check that the
 numeric value fits in a 32-bit integer.
 If you want to get ASSERTION failures in this case, call the \c AsInt()
 memberfunction.
*/
wxInt32
wxJSONValue::AsInt32() const
{
  wxInt32 i;
  AsInt32( &i );
  return i;
}

//! Returns the low-order 32 bits of the value as an unsigned integer
/*!
 This function is only available on 64-bits platforms and returns
 the low-order 32-bits of the integer stored in the JSON value.
 Note that all integer types are stored as \b wx(U)Int64 data types by
 the JSON value class and that the function does not check that the
 numeric value fits in a 32-bit integer.
 If you want to get ASSERTION failures in this case, call the \c AsUInt()
 memberfunction.
*/
unsigned int
wxJSONValue::AsUInt32() const
{
  wxUint32 ui;
  AsUInt32( &ui );
  return ui;
}


//! Return the numeric value as a 64-bit integer.
/*!
 This function is only available on 64-bits platforms and returns
 the numeric value as a 64-bit integer.
.The function checks that the type of the stored value is of type
 wxJSONTYPE_INT or wxJSONTYPE_UINT.
 No warning is reported if the sign does not match.
*/
wxInt64
wxJSONValue::AsInt64() const
{
  wxJSONRefData* data = GetRefData();
  wxASSERT( data );

  wxInt64 i64;
  switch ( data->m_type )  {
    case wxJSONTYPE_INT :
      i64 = data->m_value.m_valInt;
      break;
    case wxJSONTYPE_UINT :
      i64 = (wxInt64) data->m_value.m_valUInt;
      break;
    default :
      wxFAIL_MSG( _T("wxJSONValue::AsInt64(): The type is not compatible with INT"));
      break;
  }
  return i64;
}

//! Return the numeric value as a 64-bit unsigned integer.
/*!
 This function is only available on 64-bits platforms and returns
 the numeric value as a 64-bit unsigned integer.
.The function checks that the type of the stored value is of type
 wxJSONTYPE_INT or wxJSONTYPE_UINT.
 No warning is reported if the sign does not match.
*/
wxUint64
wxJSONValue::AsUInt64() const
{
  wxJSONRefData* data = GetRefData();
  wxASSERT( data );

  wxUint64 ui64;
  switch ( data->m_type )  {
    case wxJSONTYPE_INT :
      ui64 = (wxUint64) data->m_value.m_valInt;
      break;
    case wxJSONTYPE_UINT :
      ui64 = data->m_value.m_valUInt;
      break;
    default :
      wxFAIL_MSG( _T("wxJSONValue::AsUInt64(): The type is not compatible with INT"));
      break;
  }
  return ui64;
}

//! \overload Append( const wxJSONValue& )
wxJSONValue&
wxJSONValue::Append( wxInt64 i )
{
  wxJSONValue v( i );
  wxJSONValue& r = Append( v );
  return r;
}

//! \overload Append( const wxJSONValue& )
wxJSONValue&
wxJSONValue::Append( wxUint64 ui )
{
  wxJSONValue v( ui );
  wxJSONValue& r = Append( v );
  return r;
}


//! \overload operator = (int)
wxJSONValue&
wxJSONValue::operator = ( wxInt64 i )
{
  wxJSONRefData* data = SetType( wxJSONTYPE_INT );
  data->m_value.m_valInt = i;
  return *this;
}

//! \overload operator = (int)
wxJSONValue&
wxJSONValue::operator = ( wxUint64 ui )
{
  wxJSONRefData* data = SetType( wxJSONTYPE_UINT );
  data->m_value.m_valUInt = ui;
  return *this;
}

//! Returns the low 32-bit of an integer value
/*!
 This function is only available on 64-bits platforms and returns
 the low 32-bits of the integer value stored in this object.
 The value is stored in the integer pointed to by \c i and the
 function returns TRUE on success.
 If the value stored in the 64-bit integer is too large to fit
 in a signed integer, the function returns FALSE.

 If the value stored in not of type integer or unsigned int, the
 function returns FALSE and the variable pointed to by \c i contains
 an undefined result.
 However, in debug builds, the function calls FAIL_MSG if the type
 of the stored object is not compatible with integer.
*/
bool
wxJSONValue::AsInt32( wxInt32* i) const
{
  wxJSONRefData* data = GetRefData();
  wxASSERT( data );

  bool r = false;
  wxInt64* result = 0;

  switch ( data->m_type )  {
    case wxJSONTYPE_INT : 
      r = true;
      result = &(data->m_value.m_valInt );
      break;
    case wxJSONTYPE_UINT :
      result = (wxInt64*) &(data->m_value.m_valUInt );
      r = true;
      break;
    default :
      result = &(data->m_value.m_valInt );  // undefined value
      wxFAIL_MSG( _T("wxJSONValue::AsInt32() - the value is not compatible with INT"));
      break;
  }

  // check if the integer fits in a 32-bit signed integer
  if ( r )  {
      if ( *result > INT_MAX || *result < INT_MIN )  {
        r = false;
      }
  }

  // now cast the 64-bit integer in a 32-bit integer
  *i = (wxInt32) *result;
  return r;
}

//! Returns the low 32-bit of an integer value
/*!
 This function is only available on 64-bits platforms and returns
 the low 32-bits of the integer value stored in this object.
 The value is stored in the unsigned 32-bit integer pointed to by \c ui and the
 function returns TRUE on success.
 If the value stored in the 64-bit integer is too large to fit
 in a unsigned integer, the function returns FALSE.

 If the value stored in not of type integer or unsigned int, the
 function returns FALSE and the variable pointed to by \c ui contains
 an undefined result.
 However, in debug builds, the function calls FAIL_MSG if the type
 of the stored object is not compatible with integer.
*/
bool
wxJSONValue::AsUInt32( wxUint32* ui ) const
{
  wxJSONRefData* data = GetRefData();
  wxASSERT( data );

  bool r = false;
  wxUint64* result = 0;

  switch ( data->m_type )  {
    case wxJSONTYPE_INT : 
      r = true;
      result = (wxUint64*) &(data->m_value.m_valInt );
      break;
    case wxJSONTYPE_UINT :
      result = &(data->m_value.m_valUInt );
      r = true;
      break;
    default :
      result = &(data->m_value.m_valUInt );  // undefined value
      wxFAIL_MSG( _T("wxJSONValue::AsUInt32() - the value is not compatible with UINT"));
      break;
  }

  // check if the integer fits in a 32-bit signed integer
  if ( r )  {
      if ( *result > UINT_MAX || *result < 0 )  {
        r = false;
      }
  }

  // now cast the 64-bit integer in a 32-bit integer
  *ui = (wxUint32) *result;
  return r;
}

#endif  // defined( wxJSON_64BIT_INT )
