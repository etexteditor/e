/////////////////////////////////////////////////////////////////////////////
// Name:        jsonreader.cpp
// Purpose:     the wxJSONReader class: a JSON text parser
// Author:      Luciano Cattani
// Created:     2007/10/14
// RCS-ID:      $Id: jsonreader.cpp,v 1.12 2008/03/12 10:48:19 luccat Exp $
// Copyright:   (c) 2007 Luciano Cattani
// Licence:     wxWidgets licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
    #pragma implementation "jsonreader.cpp"
#endif

#include "jsonreader.h"

#include <wx/sstream.h>
#include <wx/debug.h>
#include <wx/log.h>


/*! \class wxJSONReader
 \brief The JSON parser

 The class is a JSON parser which reads a JSON formatted text and stores
 values in the \c wxJSONValue structure.
 The ctor accepts two parameters: the \e style flag, which controls how
 much error-tolerant should the parser be and an integer which is
 the maximum number of errors and warnings that have to be reported.

 If the document does not contain an open/close JSON character the
 function returns an \b empty value object; in other words, the 
 wxJSONValue::IsEmpty() function returns TRUE.
 This is the case of a document that is empty or contains only
 whitespaces or comments.
 If the document contains a starting object/array character immediatly
 followed by a closing object/array character
 (i.e.: \c {} ) then the function returns an \b empty array or object
 JSON value (note the little distinction: \b empty value is not the
 same as \b empty \b array or \b empty \b object ).
 For an empty object or array, the wxJSONValue::IsEmpty() function
 returns FALSE and the wxJSONValue::Size() returns ZERO.

 In order to avoid confusion, you have to think about \b empty JSON values
 as \b invalid JSON values.
 \b Empty \b arrays and \b empty \b objects are valid JSON values.

 \par JSON text

 Note that the wxJSON parser just skips all characters read from the
 input JSON text until the start-object '{' or start-array '[' characters
 are encontered (see the GetStart() function).
 This means that the JSON input text may contain everything
 before the first start-object/array character except these two chars themselves
 unless they are included in a C/C++ comment.
 Comment lines that apear before the first start array/object character,
 are non ignored if the parser is constructed with the wxJSONREADER_STORE_COMMENT
 flag: they are added to the comment's array of the root JSON value.

 Also note that the parsing process stops when the internal DoRead() function
 returns. Because that function is recursive, the top-level close-object
 '}' or close-array ']' character cause the top-level DoRead() function
 to return thus stopping the parsing process regardless the EOF condition.
 This mean that the JSON input text may contain everything \b after
 the top-level close-object/array character.
 Here are some examples:

 Returns a wxJSONTYPE_EMPTY value (invalid JSON value)
 \code
   // this text does not contain an open array/object character
 \endcode

 Returns a wxJSONTYPE_OBJECT value of Size() = 0 
 \code
   {
   }
 \endcode

 Returns a wxJSONTYPE_ARRAY value of Size() = 0 
 \code
   [
   ]
 \endcode

 Text before and after the top-level open/close characters is ignored. 
 \code
   This non-JSON text does not cause the parser to report errors or warnings
   {
   }
   This non-JSON text does not cause the parser to report errors or warnings
 \endcode


 \par Extensions

 The wxJSON parser recognizes all JSON text plus some extensions
 that are not part of the JSON syntax but that many other JSON
 implementations do recognize.
 If the input text contains the following non-JSON text, the parser
 reports the situation as \e warnings and not as \e errors unless
 the parser object was constructed with the wxJSONREADER_STRICT
 flag. In the latter case the wxJSON parser is not tolerant.

 \li C/C++ comments: the parser recognizes C and C++ comments.
	Comments can optionally be stored in the value they refer
	to and can also be written back to the JSON text document.
	To know more about comment storage see \ref wxjson_comments

 \li case tolerance: JSON syntax states that the literals \c null,
	\c true and \c false must be lowercase; the wxJSON parser
	also recognizes mixed case literals such as, for example,
	\b Null or \b FaLSe.  A \e warning is emitted.

 \li wrong or missing closing character: wxJSON parser is tolerant
	about the object / array closing character. When an open-array
	character '[' is encontered, the parser expects the 
	corresponding close-array character ']'. If the character
	encontered is a close-object char '}' a warning is reported.
	A warning is also reported if the character is missing when
	the end-of-file is reached.

 \li multi-line strings: this feature allows a JSON string type to be
	splitted in two or more lines as in the standard C/C++
	languages. The drawback is that this feature is error-prone
	and you have to use it with care.
	For more info about this topic read \ref json_multiline_string

 Note that you can control how much error-tolerant should the parser be
 and also you can specify how many and what extensions are recognized.
 See the constructor's parameters for more details.

 \par Unicode vs ANSI

 The parser can read JSON text from two very different kind of objects:

 \li a string object (\b wxString)
 \li a stream object (\b wxInputStream)

 When the input is from a string object, the character represented in the
 string is platform- and mode- dependant; in other words, characters are
 represented differently: in ANSI builds they depend on the charset in use
 and in Unicode builds they depend on the platform (UCS-2 in win32, UCS-4
 in GNU/Linux).

 When the input is from a stream object, the only recognized encoding format
 is UTF-8 for both ANSI and Unicode builds.

 \par Example:

 \code
  wxJSONValue  value;
  wxJSONReader reader;

  // open a text file that contains the UTF-8 encoded JSON text
  wxFFileInputStream jsonText( _T("filename.utf8"), _T("r"));

  // read the file
  int numErrors = reader.Parse( jsonText, &value );

  if ( numErrors > 0 )  {
    ::MessageBox( _T("Error reading the input file"));
  }
 \endcode

 To know more about ANSI and Unicode mode read \ref wxjson_tutorial_unicode.
*/

// if you have the debug build of wxWidgets and wxJSON you can see
// trace messages by setting the:
// WXTRACE=traceReader StoreComment
// environment variable
static const wxChar* traceMask = _T("traceReader");
static const wxChar* storeTraceMask = _T("StoreComment"); 


//! Ctor
/*!
 Construct a JSON parser object with the given parameters.

 JSON parser objects should always be constructed on the stack but
 it does not hurt to have a global JSON parser.

 \param flags this paramter controls how much error-tolerant should the
		parser be

 \param maxErrors the maximum number of errors (and warnings, too) that are
	reported by the parser. When the number of errors reaches this limit,
	the parser stops to read the JSON input text and no other error is
	reported.

 The \c flag parameter is the combination of ZERO or more of the
 following constants OR'ed toghether:

 \li wxJSONREADER_ALLOW_COMMENTS: C/C++ comments are recognized by the
     parser; a warning is reported by the parser
 \li wxJSONREADER_STORE_COMMENTS: C/C++ comments, if recognized, are
     stored in the value they refer to and can be rewritten back to
     the JSON text
 \li wxJSONREADER_CASE: the parser recognizes mixed-case literal strings
 \li wxJSONREADER_MISSING: the parser allows missing or wrong close-object
     and close-array characters
 \li wxJSONREADER_MULTISTRING: strings may be splitted in two or more
     lines
 \li wxJSONREADER_COMMENTS_AFTER: if STORE_COMMENTS if defined, the parser
     assumes that comment lines apear \b before the value they
     refer to unless this constant is specified. In the latter case, 
     comments apear \b after the value they refer to.

 You can also use the following shortcuts to specify some predefined
 flag's combination:
 
  \li wxJSONREADER_STRICT: all wxJSON extensions are reported as errors, this
      is the same as specifying a ZERO value as \c flags.
  \li wxJSONREADER_TOLERANT: this is the same as ALLOW_COMMENTS | CASE |
      MISSING | MULTISTRING; all wxJSON extensions are turned on but comments
      are not stored in the value objects.

 \par Example:

 The following code fragment construct a JSON parser, turns on all
 wxJSON extensions and also stores C/C++ comments in the value object
 they refer to. The parser assumes that the comments apear before the
 value:

 \code
   wxJSONReader reader( wxJSONREADER_TOLERANT | wxJSONREADER_STORE_COMMENTS );
   wxJSONValue  root;
   int numErrors = reader.Parse( jsonText, &root );
 \endcode
*/
wxJSONReader::wxJSONReader( int flags, int maxErrors )
{
  m_flags     = flags;
  m_maxErrors = maxErrors;
  m_inType    = -1;
  m_inObject  = 0;
}

//! Dtor - does nothing
wxJSONReader::~wxJSONReader()
{
}

//! Parse the JSON document.
/*!
 The two overloaded versions of the \c Parse() function read a
 JSON text stored in a wxString object or in a wxInputStream
 object.

 If \c val is a NULL pointer, the function does not store the
 values: it can be used as a JSON checker in order to check the
 syntax of the document.
 Returns the number of \b errors found in the document.
 If the returned value is ZERO and the parser was constructed
 with the \c wxJSONREADER_STRICT flag, then the parsed document
 is \e well-formed and it only contains valid JSON text.

 If the \c wxJSONREADER_TOLERANT flag was used in the parser's 
 constructor, then a return value of ZERO
 does not mean that the document is \e well-formed because it may
 contain comments and other extensions that are not fatal for the
 wxJSON parser but other parsers may fail to recognize.
 You can use the \c GetWarningCount() function to know how many
 wxJSON extensions are present in the JSON input text.

 Note that the JSON value object \c val is not cleared by this
 function unless its type is of the wrong type.
 In other words, if \c val is of type wxJSONTYPE_ARRAY and it already
 contains 10 elements and the input document starts with a
 '[' (open-array char) then the elements read from the document are
 \b appended to the existing ones.

 On the other hand, if the text document starts with a '{' (open-object) char
 then this function must change the type of the \c val object to
 \c wxJSONTYPE_OBJECT and the old content of 10 array elements will be lost.

 When reading from a \b wxInputStream the JSON text must be encoded
 in UTF-8 format for both Unicode and ANSI builds.
 When reading from a \b wxString object, the input text is encoded
 in different formats depending on the platform and the build
 mode: in Unicode builds, strings are encoded in UCS-2 format on
 Windows and in UCS-4 format on GNU/Linux; in ANSI builds, strings
 contain one-byte locale dependent characters.
*/
int
wxJSONReader:: Parse( const wxString& doc, wxJSONValue* val )
{
  m_inType   = 0;    // a string
  m_inObject = (void*) &doc;
  m_charPos  = 0;
  m_conv     = 0;
  int numErr = Parse( val );
  return numErr;
}

//! \overload Parse( const wxString&, wxJSONValue* )
int
wxJSONReader::Parse( wxInputStream& is, wxJSONValue* val )
{
  wxMBConvUTF8 conv;
  m_conv     = &conv;
  m_inObject = &is;
  m_inType   = 1;
  return Parse( val );
}


//! The general parsing function (internal use)
/*!
 This protected function is called by the public overloaded Parse()
 functions after setting up the internal data members.
*/
int
wxJSONReader::Parse( wxJSONValue* val )
{
  // construct a temporary wxJSONValue that will be passed
  // to DoRead() if val == 0 - note that it will be deleted on exit
  wxJSONValue* temp = 0;
  m_level    = 0;
  m_lineNo   = 1;
  m_colNo    = 1;
  m_peekChar = -1;
  m_errors.clear();
  m_warnings.clear();

  // check the internal data members
  wxASSERT( m_inObject != 0 );
  wxASSERT( m_inType >= 0 );

  if ( val != 0 )  {
    temp = val;
  }
  else {
    temp = new wxJSONValue();
  }

  // set the wxJSONValue object's pointers for comment storage 
  m_next       = temp;
  m_next->SetLineNo( -1 );
  m_lastStored = 0;
  m_current    = 0;

  int ch = GetStart();
  switch ( ch )  {
    case '{' :
      temp->SetType( wxJSONTYPE_OBJECT );
      break;
    case '[' :
      temp->SetType( wxJSONTYPE_ARRAY );
      break;
    default :
      AddError( _T("Cannot find a start object/array character" ));
      return m_errors.size();
      break;
  }

  // returning from DoRead() could be for EOF or for
  // the closing array-object character
  // if -1 is returned, it is as an error because the lack
  // of close-object/array characters
  // note that the missing close-chars error messages are
  // added by the DoRead() function
  ch = DoRead( *temp );

  if ( val == 0 )  {
    delete temp;
  }
  return m_errors.size();
}


//! Returns the start of the document
/*!
 This is the first function called by the Parse() function and it searches
 the input stream for the starting character of a JSON text and returns it.
 JSON text start with '{' or '['.
 If the two starting characters are inside a C/C++ comment, they
 are ignored.
 Returns the JSON-text start character or -1 on EOF.
*/
int
wxJSONReader::GetStart()
{
  int ch = 0;
  do  {
    switch ( ch )  {
      case 0 :
        ch = ReadChar();
        break;
      case '{' :
        return ch;
        break;
      case '[' :
        return ch;
        break;
      case '/' :
        ch = SkipComment();
        StoreComment( 0 );
        break;
      default :
        ch = ReadChar();
        break;
    }
  } while ( ch >= 0 );
  return ch;
}

//! Return a reference to the error message's array.
const wxArrayString&
wxJSONReader::GetErrors() const
{
  return m_errors;
}

//! Return a reference to the warning message's array.
const wxArrayString&
wxJSONReader::GetWarnings() const
{
  return m_warnings;
}

//! Return the size of the error message's array.
int
wxJSONReader::GetErrorCount() const
{
  return m_errors.size();
}

//! Return the size of the warning message's array.
int
wxJSONReader::GetWarningCount() const
{
  return m_warnings.size();
}


//! Read a character from the input JSON document.
/*!
 The function returns a single character from the input JSON document
 as an integer so that all 2^31 unicode characters can be represented
 as a positive integer value.
 In case of errors or EOF, the function returns -1.
 The function also updates the \c m_lineNo and \c m_colNo data
 members and converts all CR+LF sequence in LF.

 Note that this function calls GetChar() in order to retrieve the
 next character in the JSON input text.
*/
int
wxJSONReader::ReadChar()
{
  int ch = GetChar();

  // the function also converts CR in LF. only LF is returned
  // in the case of CR+LF
  // returns -1 on EOF
  if ( ch == -1 )  {
    return ch;
  }

  int nextChar;

  if ( ch == '\r' )  {
    m_colNo = 1;
    nextChar = PeekChar();
    if ( nextChar == -1 )  {
      return -1;
    }
    else if ( nextChar == '\n' )    {
      ch = GetChar();
    }
  }
  if ( ch == '\n' )  {
    ++m_lineNo;
    m_colNo = 1;
  }
  else  {
    ++m_colNo;
  }
  return ch;  
}


//! Return a character from the input JSON document.
/*!
 The function is called by ReadChar() and returns a single character
 from the input JSON document
 as an integer so that all 2^31 unicode characters can be represented
 as a positive integer value.
 In case of errors or EOF, the function returns -1.
 Note that this function behaves differently depending on the build 
 mode (ANSI or Unicode) and the type of the object containing the
 JSON document.

 \par wxString input

 If the input JSON text is stored in a \b wxString object, there is
 no difference between ANSI and Unicode builds: the function just returns
 the next character in the string and updates the \c m_charPos data
 member that points the next character in the string.
 In Unicode mode, the function returns wide characters and in ANSI
 builds it returns only chars.

 \par wxInputStream input

 Stream input is always encoded in UTF-8 format in both ANSI ans
 Unicode builds.
 In order to return a single character, the function calls the
 UTF8NumBytes() function which returns the number of bytes that
 have to be read from the stream in order to get one character.
 The bytes read are then converted to a wide character and
 returned.
 Note that wide chars are also returned in ANSI mode but they
 are processed differently by the parser: before storing the
 wide character in the JSON value, it is converted to the
 locale dependent character if one exists; if not, the \e unicode
 \e escape \e sequence is stored in the JSON value. 
*/
int
wxJSONReader::GetChar()
{
  int ch;
  if ( m_peekChar >= 0 )  {
    ch = m_peekChar;
    m_peekChar = -1;
    return ch;
  }

  if ( m_inType == 0 )  {        // input is a string
    wxString* s = (wxString*) m_inObject;
    size_t strLen = s->length();
    if ( m_charPos >= (int) strLen )  {
      return -1;                    // EOF
    }
    else {
      ch = s->GetChar( m_charPos );
      ++m_charPos;
    }
  }
  else  {
    // input is a stream: it must be UTF-8
    wxInputStream* is = (wxInputStream*) m_inObject;
    wxASSERT( is != 0 );

    // must know the number fo bytes to read
    char buffer[10];
    buffer[0] = is->GetC();

    // check the EOF condition; note that in wxWidgets 2.6.x and below
    // some streams returns EOF after the last char was read and 
    // we should use LastRead() to know the EOF condition
    // wxJSON depends on 2.8.4 and above so there should be no problem
    if ( is->Eof() )  {
      return -1;
    }
    // if ( LastRead() <= 0 )  {
    //   return -1;
    // }

    int numBytes = UTF8NumBytes( buffer[0] );
    wxASSERT( numBytes < 10 );
    if ( numBytes > 1 )  {
      is->Read( buffer + 1, numBytes - 1);
    }

    // check that we read at least 'numBytes' bytes
    int lastRead = is->LastRead();
    if ( lastRead < ( numBytes -1 ))  {
      AddError( _T("Cannot read the number of needed UTF-8 encoded bytes"));
      ch = -1;
      return ch;
    }

    // we convert the UTF-8 char to a wide char; if the conversion
    // fails, returns -1
    wchar_t dst[5];
    wxASSERT( m_conv != 0 );
    size_t outLength = m_conv->ToWChar( dst, 10, buffer, numBytes );
    if ( outLength == wxCONV_FAILED )  {
      AddError( _T("Cannot convert multibyte sequence to wide character"));
      ch = -1;
      return ch;
    }
    else {
      wxASSERT( outLength == 2 );  // seems that a NULL char is appended
      ch = dst[0];
    }
    // we are only interested in the dst[0] wide-chracter; it could be
    // possible that 'outLenght' is 2 because a trailing NULL wchar is
    // appended but it is ignored
    // if the character read is a NULL char, it is returned as a NULL
    // char thus causing the JSON reader to read the next char.
  }
  // for tracing purposes we do not print control characters
  char chPrint = '.';
  if ( ch >= 20 ) {
    chPrint = ch;
  }
  ::wxLogTrace( _T("traceChar"), _T("(%s) ch=%x char=%c"), __PRETTY_FUNCTION__, ch, chPrint );
  return ch;
}

//! Peek a character from the input JSON document
/*!
 This function is much like GetChar() but it does not update
 the stream or string input position.
*/
int
wxJSONReader::PeekChar()
{
  int ch;
  if ( m_peekChar >= 0 )  {
    ch = m_peekChar;
  }
  else   {
    ch = GetChar();
    m_peekChar = ch;
  }
  return ch;
}


//! Reads the JSON text document (internal use)
/*!
 This is a recursive function that is called by \c Parse()
 and by the \c DoRead() function itself when a new object /
 array is encontered.
 The function returns when a EOF condition is encontered or
 when the final close-object / close-array char is encontered.
 The function also increments the \c m_level
 data member when it is entered and decrements it on return.

 The function is the heart of the wxJSON parser class but it is
 also very easy to understand because JSON syntax is very
 easy.

 Returns the last close-object/array character read or -1 on EOF.
*/
int
wxJSONReader::DoRead( wxJSONValue& parent )
{
  ++m_level;
  // the value that has to be read (can be a complex structure)
  // it can also be a 'name' (key) string
  wxJSONValue value( wxJSONTYPE_EMPTY );
  m_next = &value;

  m_current = &parent;
  m_current->SetLineNo( m_lineNo );
  m_lastStored = 0;

  // the 'key' string is stored from 'value' when a ':' is encontered
  wxString  key;

  // the character read: -1=EOF, 0=to be read
  int ch=0;

  do {                   // we read until ch < 0
    switch ( ch )  {
      case 0 :
        ch = ReadChar();
        break;
      case ' ' :
      case '\t' :
      case '\n' :
      case '\r' :
        ch = SkipWhiteSpace();
        break;
      case -1 :   // the EOF
        break;
      case '/' :
        ch = SkipComment();
        StoreComment( &parent );
        break;

      case '{' :
        if ( parent.IsObject() ) {
          if ( key.empty() )   {
            AddError( _T("\'{\' is not allowed here (\'name\' is missing") );
          }
          if ( !value.IsEmpty() )   {
            AddError( _T("\'{\' cannot follow a \'value\'") );
          }
        }
        else if ( parent.IsArray() )  {
          if ( !value.IsEmpty() )   {
            AddError( _T("\'{\' cannot follow a \'value\' in JSON array") );
          }
        }
        else  {
          wxASSERT( 0 );       // always fails
        }

        // althrough there were errors, we go into the subobject
        value.SetType( wxJSONTYPE_OBJECT );
        ch = DoRead( value );
        break;

      case '}' :
        if ( !parent.IsObject() )  {
          AddWarning( wxJSONREADER_MISSING,
			_T("Trying to close an array using the \'}\' (close-object) char" ));
        }
        StoreValue( ch, key, value, parent );
        m_current = &parent;
        m_next    = 0;
        m_current->SetLineNo( m_lineNo );
        ch = ReadChar();
        return ch;
        break;

      case '[' :
        if ( parent.IsObject() ) {
          if ( key.empty() )   {
            AddError( _T("\'[\' is not allowed here (\'name\' is missing") );
          }
          if ( !value.IsEmpty() )   {
            AddError( _T("\'[\' cannot follow a \'value\' text") );
          }
        }
        else if ( parent.IsArray())  {
          if ( !value.IsEmpty() )   {
            AddError( _T("\'[\' cannot follow a \'value\'") );
          }
        }
        else  {
          wxASSERT( 0 );       // always fails
        }

        // althrough there were errors, we go into the subobject
        value.SetType( wxJSONTYPE_ARRAY );
        ch = DoRead( value );
        break;

      case ']' :
        if ( !parent.IsArray() )  {
          AddWarning( wxJSONREADER_MISSING,
		_T("Trying to close an object using the \']\' (close-array) char" ));
        }
        StoreValue( ch, key, value, parent );
        m_current = &parent;
        m_next    = 0;
        m_current->SetLineNo( m_lineNo );
        return 0;   // returning ZERO for reading the next char
        break;

      case ',' :
        StoreValue( ch, key, value, parent );
        key.clear();
        ch = ReadChar();
        break;

      case '\"' :
        ch = ReadString( value );     // store the string in 'value'
        m_current = &value; 
        m_next    = 0;
        break;

      case ':' :   // key / value separator
        m_current = &value; 
        m_current->SetLineNo( m_lineNo );
        m_next    = 0;
        if ( !parent.IsObject() )  {
          AddError( _T( "\':\' cannot be used in array's values" ));
        }
        else if ( !value.IsString() )  {
          AddError( _T( "\':\' follows a value which is not of type \'string\'" ));
        }
        else if ( !key.empty() )  {
          AddError( _T( "\':\' not allowed where a \'name\' string was already available" ));
        }
        else  {
          key = value.AsString();
          value.SetType( wxJSONTYPE_EMPTY );
        }
        ch = ReadChar();
        break;

      // no special chars, is it a value?
      default :
        // errors are checked in the 'ReadValue()' function.
        m_current = &value; 
        m_current->SetLineNo( m_lineNo );
        m_next    = 0;
        ch = ReadValue( ch, value );
        break;
    }

  } while ( ch >= 0 );

  if ( parent.IsArray() )  {
    AddWarning( wxJSONREADER_MISSING, _T("\']\' missing at end of file"));
  }
  else if ( parent.IsObject() )  {
    AddWarning( wxJSONREADER_MISSING, _T("\'}\' missing at end of file"));
  }
  else  {
    wxASSERT( 0 );
  }

  // we store the value, as there is a missing close-object/array char
  StoreValue( ch, key, value, parent );

  --m_level;
  return ch;
}

//! Store a value in the parent object.
/*!
 The function is called by \c DoRead() when a the comma
 or a close-object character is encontered and stores the current
 value read by the parser in the parent object.
 The function checks that \c value is not empty and that \c key is
 not an empty string if parent is an object.

 \param ch	the character read: a comma or close objecty/array char
 \param key	the \b key string: may be empty if parent ss an array
 \param value	the current JSON value to be stored in \c parent
 \param parent	the JSON value that holds \c value.
*/
void
wxJSONReader::StoreValue( int ch, const wxString& key, wxJSONValue& value, wxJSONValue& parent )
{
  // if 'ch' == } or ] than value AND key may be empty when a open object/array
  // is immediatly followed by a close object/array
  //
  // if 'ch' == , (comma) value AND key (for TypeMap) cannot be empty
  //
  ::wxLogTrace( traceMask, _T("(%s) ch=%d char=%c"), __PRETTY_FUNCTION__, ch, (char) ch);
  ::wxLogTrace( traceMask, _T("(%s) value=%s"), __PRETTY_FUNCTION__, value.AsString().c_str());

  m_current = 0;
  m_next    = &value;
  m_lastStored = 0;
  m_next->SetLineNo( -1 );

  if ( value.IsEmpty() && key.empty() ) {
      // OK, if the char read is a close-object or close-array
    if ( ch == '}' || ch == ']' )  {
      m_lastStored = 0;
      ::wxLogTrace( traceMask, _T("(%s) key and value are empty, returning"),
							 __PRETTY_FUNCTION__);
    }
    else  {
      AddError( _T("key or value is missing for JSON value"));
    }
  }
  else  {
    // key or value are not empty
    if ( parent.IsObject() )  {
      if ( value.IsEmpty() ) {
        AddError( _T("cannot store the value: \'value\' is missing for JSON object type"));
      }
      else if ( key.empty() ) {
        AddError( _T("cannot store the value: \'key\' is missing for JSON object type"));
      }
      else  {
        ::wxLogTrace( traceMask, _T("(%s) adding value to key:%s"),
					 __PRETTY_FUNCTION__, key.c_str());
        parent[key] = value;
        m_lastStored = &(parent[key]);
        m_lastStored->SetLineNo( m_lineNo );
      }
    }
    else if ( parent.IsArray() ) {
      if ( value.IsEmpty() ) {
        AddError( _T("cannot store the item: \'value\' is missing for JSON array type"));
      }
      if ( !key.empty() ) {
        AddError( _T("cannot store the item: \'key\' (\'%s\') is not permitted in JSON array type"), key);
      }
      ::wxLogTrace( traceMask, _T("(%s) appending value to parent array"), __PRETTY_FUNCTION__ );
      parent.Append( value );
      const wxJSONInternalArray* arr = parent.AsArray();
      wxASSERT( arr );
      m_lastStored = &(arr->Last());
      m_lastStored->SetLineNo( m_lineNo );
    }
    else  {
      wxASSERT( 0 );  // should never happen
    }
  }
  value.SetType( wxJSONTYPE_EMPTY );
  value.ClearComments();
}

//! Add a error message to the error's array
/*!
 The overloaded versions of this function add an error message to the
 error's array stored in \c m_errors.
 The error message is formatted as follows:

 \code
   Error: line xxx, col xxx - <error_description>
 \endcode

 The \c msg parameter is the description of the error; line's and column's
 number are automatically added by the functions.
 The \c fmt parameter is a format string that has the same syntax as the \b printf
 function.
 Note that it is the user's responsability to provide a format string suitable
 with the arguments: another string or a character. 
*/
void
wxJSONReader::AddError( const wxString& msg )
{
  wxString err;
  err.Printf( _T("Error: line %d, col %d - %s"), m_lineNo, m_colNo, msg.c_str() );

  ::wxLogTrace( traceMask, _T("(%s) %s"), __PRETTY_FUNCTION__, err.c_str());

  if ( (int) m_errors.size() < m_maxErrors )  {
    m_errors.Add( err );
  }
  else if ( (int) m_errors.size() == m_maxErrors )  {
    m_errors.Add( _T("Error: too many error messages - ignoring further errors"));
  }
  // else if ( m_errors > m_maxErrors ) do nothing, thus ignore the error message
}

//! \overload AddError( const wxString& )
void
wxJSONReader::AddError( const wxString& fmt, const wxString& str )
{
  wxString s;
  s.Printf( fmt.c_str(), str.c_str() );
  AddError( s );
}

//! \overload AddError( const wxString& )
void
wxJSONReader::AddError( const wxString& fmt, wxChar c )
{
  wxString s;
  s.Printf( fmt.c_str(), c );
  AddError( s );
}

//! Add a warning message to the warning's array
/*!
 The warning description is as follows:
 \code
   Warning: line xxx, col xxx - <warning_description>
 \endcode

 Warning messages are generated by the parser when the JSON
 text that has been read is not well-formed but the
 error is not fatal and the parser recognizes the text
 as an extension to the JSON standard (see the parser's ctor
 for more info about wxJSON extensions).

 Note that the parser has to be constructed with a flag that
 indicates if each individual wxJSON extension is on.
 If the warning message is related to an extension that is not
 enabled in the parser's \c m_flag data member, this function
 calls AddError() and the warning message becomes an error
 message.
 The \c type parameter is one of the same constants that
 specify the parser's extensions.
*/
void
wxJSONReader::AddWarning( int type, const wxString& msg )
{
  // if 'type' AND 'm_flags' == 1 than the extension is
  // ON. Otherwise it is OFF anf the function calls AddError()
  if ( ( type & m_flags ) == 0 )  {
    AddError( msg );
    return;
  }

  wxString err;
  err.Printf( _T( "Warning: line %d, col %d - %s"), m_lineNo, m_colNo, msg.c_str() );

  ::wxLogTrace( traceMask, _T("(%s) %s"), __PRETTY_FUNCTION__, err.c_str());
  if ( (int) m_warnings.size() < m_maxErrors )  {
    m_warnings.Add( err );
  }
  else if ( (int) m_warnings.size() == m_maxErrors )  {
    m_warnings.Add( _T("Error: too many warning messages - ignoring further warnings"));
  }
  // else do nothing, thus ignore the warning message
}

//! Skip all whitespaces.
/*!
 The function reads characters from the input text
 and returns the first non-whitespace character read or -1
 if EOF.
 Note that the function does not rely on the \b isspace function
 of the C library but checks the space constants: space, TAB and
 LF.
*/
int
wxJSONReader::SkipWhiteSpace()
{
  int ch;
  do {
    ch = ReadChar();
    if ( ch < 0 )  {
      break;
    }
  }
  while ( ch == ' ' || ch == '\n' || ch == '\t' );
  ::wxLogTrace( traceMask, _T("(%s) end whitespaces line=%d col=%d"),
			 __PRETTY_FUNCTION__, m_lineNo, m_colNo );
  return ch;
}

//! Skip a comment
/*!
 The function is called by DoRead() when a '/' (slash) character
 is read from the input stream assuming that a C/C++ comment is starting.
 Returns the first character that follows the comment or
 -1 on EOF.
 The function also adds a warning message because comments are not 
 valid JSON text.
 The function also stores the comment, if any, in the \c m_comment data
 member: it can be used by the DoRead() function if comments have to be
 stored in the value they refer to.
*/
int
wxJSONReader::SkipComment()
{
  static const wxChar* warn = 
	_T("Comments may be tolerated in JSON text but they are not part of JSON syntax");

  // if it is a comment, then a warning is added to the array
  // otherwise it is an error: values cannot start with a '/'
  int ch = ReadChar();
  if ( ch < 0 )  {
    return -1;
  }

  ::wxLogTrace( storeTraceMask, _T("(%s) start comment line=%d col=%d"),
			 __PRETTY_FUNCTION__, m_lineNo, m_colNo );

  if ( ch == '/' )  {         // C++ comment, read until end-of-line
    AddWarning( wxJSONREADER_ALLOW_COMMENTS, warn );
    m_commentLine = m_lineNo;
    m_comment.append( _T("//") );
    while ( ch != '\n' && ch >= 0 )  {
      ch = ReadChar();
      m_comment.append( 1, ch );
    }
    // now ch contains the '\n' character;
  }
  else if ( ch == '*' )  {     // C-style comment
    AddWarning(wxJSONREADER_ALLOW_COMMENTS, warn );
    m_commentLine = m_lineNo;
    m_comment.append( _T("/*") );
    while ( ch >= 0 ) {
      ch = ReadChar();
      m_comment.append( 1, ch );
      if ( ch == '*' && PeekChar() == '/' )  {
        m_comment.append( 1, '/' );
        break;
      }
    }
    // now ch contains '*' followed by '/'; we read two characters
    ch = ReadChar();
    ch = ReadChar();
  }
  else  {   // it is not a comment, return the character next the first '/'
    AddError( _T( "Strange '/' (did you want to insert a comment?)"));
    // we read until end-of-line OR end of C-style comment OR EOF
    // because a '/' should be a start comment
    while ( ch >= 0 ) {
      ch = ReadChar();
      if ( ch == '*' && PeekChar() == '/' )  {
        break;
      }
      if ( ch == '\n' )  {
        break;
      }
    }
    ch = ReadChar();
  }
  ::wxLogTrace( traceMask, _T("(%s) end comment line=%d col=%d"),
			 __PRETTY_FUNCTION__, m_lineNo, m_colNo );
  ::wxLogTrace( storeTraceMask, _T("(%s) end comment line=%d col=%d"),
			 __PRETTY_FUNCTION__, m_lineNo, m_colNo );
  ::wxLogTrace( storeTraceMask, _T("(%s) comment=%s"),
			 __PRETTY_FUNCTION__, m_comment.c_str());
  return ch;
}

//! Read a string value
/*!
 The function reads a string value from input stream and it is
 called by the \c DoRead() function when it enconters the
 double quote characters.
 The function read all characters up to the next double quotes
 unless it is escaped.
 Also, the function recognizes the escaped characters defined
 in the JSON syntax.

 The string is also stored in the provided wxJSONValue argument
 provided that it is empty or it contains a string value.
 This is because the parser class recognizes multi-line strings
 like the following one:
 \code
   [  "line-1\n"
      "line-2\n"
      "line-3\n"
   ]
 \endcode
 Because of the lack of the value separator (,) the parser
 assumes that the string was split into several double-quoted
 strings.
 If the value does not contain a string then an error is
 reported.
 Splitted strings cause the parser to report a warning.
*/
int
wxJSONReader::ReadString( wxJSONValue& val )
{
  long int hex;

  // the char read is the opening qoutes (")
  wxString s;

  int ch = ReadChar();
  while ( ch > 0 && ch != '\"' ) {
    if ( ch == '\\' )  {    // an escape sequence
      ch = ReadChar();
      switch ( ch )  {
        case -1 :
          break;
        case 't' :
          s.append( 1, '\t' );
          break;
        case 'n' :
          s.append( 1, '\n' );
          break;
        case 'b' :
          s.append( 1, '\b' );
          break;
        case 'r' :
          s.append( 1, '\r' );
          break;
        case '\"' :
          s.append( 1, '\"' );
          break;
        case '\\' :
          s.append( 1, '\\' );
          break;
        case '/' :
          s.append( 1, '/' );
          break;
        case '\f' :
          s.append( 1, '\f' );
          break;
        case 'u' :
          ch = ReadUnicode( hex );
          if ( hex < 0 ) {
            AddError( _T( "unicode sequence must contain 4 hex digits"));
          }
          else  {
            // append the unicode escaped character to the string 's'
            AppendUnicodeSequence( s, hex );
          }
          // many thanks to Bryan Ashby who discovered this bug
          continue;
          // break;
        default :
          AddError( _T( "Unknow escaped character \'\\%c\'"), ch );
      }
    }
    else {
      // we have read a non-escaped character so we have to append it to
      // the string. note that in ANSI builds we have to convert the char
      // to a locale dependent charset; if the char cannot be converted,
      // store its unicode escape sequence
      #if defined( wxJSON_USE_UNICODE )
        s.Append( (wxChar) ch, 1 );
      #else
        wchar_t wchar = ch;
        char buffer[5];
        size_t len = wxConvLibc.FromWChar( buffer, 5, &wchar, 1 );
        if ( len != wxCONV_FAILED )  {
          s.Append( buffer[0], 1 );
        }
        else  {
          snprintf( buffer, 10, _T("\\u%04X" ), ch );
          s.Append( buffer );
        }
      #endif

    }
    ch = ReadChar();  // read the next char until closing quotes
  }

  ::wxLogTrace( traceMask, _T("(%s) line=%d col=%d"),
			 __PRETTY_FUNCTION__, m_lineNo, m_colNo );
  ::wxLogTrace( traceMask, _T("(%s) string read=%s"),
			 __PRETTY_FUNCTION__, s.c_str() );
  ::wxLogTrace( traceMask, _T("(%s) value=%s"),
			 __PRETTY_FUNCTION__, val.AsString().c_str() );

  // now assign the string to the JSON-value 'value'
  // must check that:
  //   'value'  is empty
  //   'value'  is a string; concatenate it but emit warning

  if ( val.IsEmpty() )   {
    ::wxLogTrace( traceMask, _T("(%s) assigning the string to value"), __PRETTY_FUNCTION__ );
    val = s ;
  }
  else if ( val.IsString() )  {
    AddWarning( wxJSONREADER_MULTISTRING,
		_T("Multiline strings are not allowed by JSON syntax") );
    ::wxLogTrace( traceMask, _T("(%s) concatenate the string to value"), __PRETTY_FUNCTION__ );
    val.Cat( s );
  }
  else  {
    AddError( _T( "String value \'%s\' cannot follow another value"), s );
  }

  // store the input text's line number when the string was stored in 'val'
  val.SetLineNo( m_lineNo );

  // read the next char after the closing quotes and returns it
  if ( ch > 0 )  {
    ch = ReadChar();
  }

  return ch;
}

//! Reads a token string
/*!
 This function is called by the ReadValue() when the
 first character encontered is not a special char
 and it is not a string.
 It stores the token in the provided string argument
 and returns the next character read which is a
 whitespace or a special JSON character.

 A token cannot include \e unicode \e escaped \e sequences
 so this function does not try to interpret such sequences.
*/
int
wxJSONReader::ReadToken( int ch, wxString& s )
{
  int nextCh = ch;
  while ( nextCh >= 0 ) {
    switch ( nextCh ) {
      case ' ' :
      case ',' :
      case ':' :
      case '[' :
      case ']' :
      case '{' :
      case '}' :
      case '\t' :
      case '\n' :
      case '\r' :
      case '\b' :
        ::wxLogTrace( traceMask, _T("(%s) line=%d col=%d"),
			 __PRETTY_FUNCTION__, m_lineNo, m_colNo );
        ::wxLogTrace( traceMask, _T("(%s) token read=%s"),
			 __PRETTY_FUNCTION__, s.c_str() );
        return nextCh;
        break;
      default :
        #if defined( wxJSON_USE_UNICODE )
          s.Append( (wxChar) nextCh, 1 );
        #else
          // In ANSI builds we have to convert the char
          // to a locale dependent charset; if the char cannot be converted,
          // store its unicode escape sequence
          wchar_t wchar = nextCh;
          char buffer[10];
          size_t len = wxConvLibc.FromWChar( buffer, 10, &wchar, 1 );
          if ( len != wxCONV_FAILED )  {
            s.Append( buffer[0], 1 );
          }
          else  {
            snprintf( buffer, 10, _T("\\u%04x" ), ch );
            s.Append( buffer );
          }
        #endif
        break;
    }
    // read the next character
    nextCh = ReadChar();
  }
  ::wxLogTrace( traceMask, _T("(%s) EOF on line=%d col=%d"),
			 __PRETTY_FUNCTION__, m_lineNo, m_colNo );
  ::wxLogTrace( traceMask, _T("(%s) EOF - token read=%s"),
			 __PRETTY_FUNCTION__, s.c_str() );
  return nextCh;
}

//! Read a value from input stream
/*!
 The function is called by DoRead() when it enconters a char that is
 not a special char nor a double-quote.
 It assumes that the string is a numeric value or a 'null' or a
 boolean value and stores it in the wxJSONValue object \c val.

 The function also checks that \c val is of type wxJSONTYPE_EMPTY otherwise
 an error is reported becasue a value cannot follow another value:
 maybe a (,) or (:) is missing.
 Returns the next character or -1 on EOF.
*/
int
wxJSONReader::ReadValue( int ch, wxJSONValue& val )
{
  wxString s;
  int nextCh = ReadToken( ch, s );
  ::wxLogTrace( traceMask, _T("(%s) value=%s"),
			 __PRETTY_FUNCTION__, val.AsString().c_str() );

  if ( !val.IsEmpty() )  {
    AddError( _T( "Value \'%s\' cannot follow a value: \',\' or \':\' missing?"), s );
    return nextCh;
  }

  // variables used in the switch statement
  bool r;  double d;

  // on 64-bits platforms, integers are stored in a wx(U)Int64 data type
#if defined( wxJSON_64BIT_INT )
  wxInt64  i64;
  wxUint64 ui64;
#else
  unsigned long int ul; long int l;
#endif

  // try to convert to a number if the token starts with a digit
  // or a sign character
  switch ( ch )  {
    case '0' :
    case '1' :
    case '2' :
    case '3' :
    case '4' :
    case '5' :
    case '6' :
    case '7' :
    case '8' :
    case '9' :
      // first try a signed integer, then a unsigned integer, then a double
#if defined( wxJSON_64BIT_INT)
      r = Strtoll( s, &i64 );
      ::wxLogTrace( traceMask, _T("(%s) convert to wxInt64 result=%d"),  __PRETTY_FUNCTION__, r );
      if ( r )  {
        // store the value
        val = i64;
        return nextCh;
      }
#else
      r = s.ToLong( &l );
      ::wxLogTrace( traceMask, _T("(%s) convert to int result=%d"),  __PRETTY_FUNCTION__, r );
      if ( r )  {
        // store the value
        val = (int) l;
        return nextCh;
      }
#endif

      // try a unsigned integer
#if defined( wxJSON_64BIT_INT)
      r = Strtoull( s, &ui64 );
      ::wxLogTrace( traceMask, _T("(%s) convert to wxUint64 result=%d"),
							  __PRETTY_FUNCTION__, r );
      if ( r )  {
        // store the value
        val = ui64;
        return nextCh;
      }
#else
      r = s.ToULong( &ul );
      ::wxLogTrace( traceMask, _T("(%s) convert to int result=%d"),  __PRETTY_FUNCTION__, r );
      if ( r )  {
        // store the value
        val = (unsigned int) ul;
        return nextCh;
      }
#endif
      // number is very large or it is in exponential notation, try double
      r = s.ToDouble( &d );
      ::wxLogTrace( traceMask, _T("(%s) convert to double result=%d"),  __PRETTY_FUNCTION__, r );
      if ( r )  {
        // store the value
        val = d;
        return nextCh;
      }
      else  {  // the value is not syntactically correct
        AddError( _T( "Value \'%s\' is incorrect (did you forget quotes?)"), s );
        return nextCh;
      }

      break;

    case '+' :
      // the plus sign forces a unsigned integer
#if defined( wxJSON_64BIT_INT)
      r = Strtoull( s, &ui64 );
      ::wxLogTrace( traceMask, _T("(%s) convert to wxUint64 result=%d"),
							  __PRETTY_FUNCTION__, r );
      if ( r )  {
        // store the value
        val = ui64;
        return nextCh;
      }
#else
      r = s.ToULong( &ul );
      ::wxLogTrace( traceMask, _T("(%s) convert to int result=%d"),  __PRETTY_FUNCTION__, r );
      if ( r )  {
        // store the value
        val = (unsigned int) ul;
        return nextCh;
      }
#endif
      // number is very large or it is in exponential notation, try double
      r = s.ToDouble( &d );
      ::wxLogTrace( traceMask, _T("(%s) convert to double result=%d"),  __PRETTY_FUNCTION__, r );
      if ( r )  {
        // store the value
        val = d;
        return nextCh;
      }
      else  {  // the value is not syntactically correct
        AddError( _T( "Value \'%s\' is incorrect (did you forget quotes?)"), s );
        return nextCh;
      }

      break;

    case '-' :
      // try a signed integer, then a double
#if defined( wxJSON_64BIT_INT)
      r = Strtoll( s, &i64 );
      ::wxLogTrace( traceMask, _T("(%s) convert to wxInt64 result=%d"),  __PRETTY_FUNCTION__, r );
      if ( r )  {
        // store the value
        val = i64;
        return nextCh;
      }
#else
      r = s.ToLong( &l );
      ::wxLogTrace( traceMask, _T("(%s) convert to int result=%d"),  __PRETTY_FUNCTION__, r );
      if ( r )  {
        // store the value
        val = (int) l;
        return nextCh;
      }
#endif
      // number is very large or it is in exponential notation, try double
      r = s.ToDouble( &d );
      ::wxLogTrace( traceMask, _T("(%s) convert to double result=%d"),  __PRETTY_FUNCTION__, r );
      if ( r )  {
        // store the value
        val = d;
        return nextCh;
      }
      else  {  // the value is not syntactically correct
        AddError( _T( "Value \'%s\' is incorrect (did you forget quotes?)"), s );
        return nextCh;
      }

      break;


    // if it is not a number than try the literals
    // JSON syntax states that constant must be lowercase
    // but this parser tolerate mixed-case literals but
    // reports a warning.
    default: 
      if ( s == _T("null") ) {
        val.SetType( wxJSONTYPE_NULL );
        ::wxLogTrace( traceMask, _T("(%s) value = NULL"),  __PRETTY_FUNCTION__ );
        return nextCh;
      }
      else if ( s.CmpNoCase( _T( "null" )) == 0 ) {
        ::wxLogTrace( traceMask, _T("(%s) value = NULL"),  __PRETTY_FUNCTION__ );
        AddWarning( wxJSONREADER_CASE, _T( "the \'null\' literal must be lowercase" ));
        val.SetType( wxJSONTYPE_NULL );
        return nextCh;
      }
      else if ( s == _T("true") ) {
        ::wxLogTrace( traceMask, _T("(%s) value = TRUE"),  __PRETTY_FUNCTION__ );
        val = true;
        return nextCh;
      }
      else if ( s.CmpNoCase( _T( "true" )) == 0 ) {
        ::wxLogTrace( traceMask, _T("(%s) value = TRUE"),  __PRETTY_FUNCTION__ );
        AddWarning( wxJSONREADER_CASE, _T( "the \'true\' literal must be lowercase" ));
        val = true;
        return nextCh;
      }
      else if ( s == _T("false") ) {
        ::wxLogTrace( traceMask, _T("(%s) value = FALSE"),  __PRETTY_FUNCTION__ );
        val = false;
        return nextCh;
      }
      else if ( s.CmpNoCase( _T( "false" )) == 0 ) {
        ::wxLogTrace( traceMask, _T("(%s) value = FALSE"),  __PRETTY_FUNCTION__ );
        AddWarning( wxJSONREADER_CASE, _T( "the \'false\' literal must be lowercase" ));
        val = false;
        return nextCh;
      }
      else {
        AddError( _T( "Unrecognized literal \'%s\' (did you forget quotes?)"), s );
        return nextCh;
      }
  }
}


//! Read a 4-hex-digit unicode character.
/*!
 The function is called by ReadString() when the \b \\u sequence is
 encontered; the sequence introduces a unicode character.
 The function reads four chars from the input text by calling ReadChar()
 four times: if -1( EOF) is encontered before reading four chars, -1 is
 also returned and no conversion takes place.
 The function tries to convert the 4-hex-digit sequence in an integer which
 is returned in the \ hex parameter.
 If the string cannot be converted, the function stores -1 in \c hex
 and reports an error.

 Returns the character after the hex sequence or -1 if EOF or if the
 four characters cannot be converted to a hex number.
*/
int
wxJSONReader::ReadUnicode( long int& hex )
{
  wxString s; int ch;
  for ( int i = 0; i < 4; i++ )  {
    ch = ReadChar();
    if ( ch < 0 )  {
      return ch;
    }
    s.append( 1, (wxChar) ch );
  }

  bool r = s.ToLong( &hex, 16 );
  if ( !r )  {
    hex = -1;
  }
  ::wxLogTrace( traceMask, _T("(%s) unicode sequence=%s code=%d"),
			  __PRETTY_FUNCTION__, s.c_str(), (int) hex );
  ch = ReadChar();
  return ch;
}

//! Store the comment string in the value it refers to.
/*!
 The function searches a suitable value object for storing the
 comment line that was read by the parser and temporarly
 stored in \c m_comment.
 The function searches the three values pointed to by:
 \li \c m_next
 \li \c m_current
 \li \c m_lastStored
 and checks the comment flag (BEFORE or AFTER).

 Note that the comment line is only stored if the wxJSONREADER_STORE_COMMENTS
 flag was used when the parser object was constructed; otherwise, the
 function does nothing and immediatly returns.
 Also note that if the comment line has to be stored in a value and the
 function cannot find a suitable value to add the comment line to,
 an error is reported (note: not a warning but an error).
*/
void
wxJSONReader::StoreComment( const wxJSONValue* parent )
{
  ::wxLogTrace( storeTraceMask, _T("(%s) m_comment=%s"),  __PRETTY_FUNCTION__, m_comment.c_str());
  ::wxLogTrace( storeTraceMask, _T("(%s) m_flags=%d m_commentLine=%d"),
			  __PRETTY_FUNCTION__, m_flags, m_commentLine );
  ::wxLogTrace( storeTraceMask, _T("(%s) m_current=%p"), __PRETTY_FUNCTION__, m_current );
  ::wxLogTrace( storeTraceMask, _T("(%s) m_next=%p"), __PRETTY_FUNCTION__, m_next );
  ::wxLogTrace( storeTraceMask, _T("(%s) m_lastStored=%p"), __PRETTY_FUNCTION__, m_lastStored );

  // first check if the 'store comment' bit is on
  if ( (m_flags & wxJSONREADER_STORE_COMMENTS) == 0 )  {
    m_comment.clear();
    return;
  }

  // check if the comment is on the same line of one of the
  // 'current', 'next' or 'lastStored' value
  if ( m_current != 0 )  {
    ::wxLogTrace( storeTraceMask, _T("(%s) m_current->lineNo=%d"),
			 __PRETTY_FUNCTION__, m_current->GetLineNo() );
    if ( m_current->GetLineNo() == m_commentLine ) {
      ::wxLogTrace( storeTraceMask, _T("(%s) comment added to \'m_current\' INLINE"),
			 __PRETTY_FUNCTION__ );
      m_current->AddComment( m_comment, wxJSONVALUE_COMMENT_INLINE );
      m_comment.clear();
      return;
    }
  }
  if ( m_next != 0 )  {
   ::wxLogTrace( storeTraceMask, _T("(%s) m_next->lineNo=%d"),
			 __PRETTY_FUNCTION__, m_next->GetLineNo() );
    if ( m_next->GetLineNo() == m_commentLine ) {
      ::wxLogTrace( storeTraceMask, _T("(%s) comment added to \'m_next\' INLINE"),
			 __PRETTY_FUNCTION__ );
      m_next->AddComment( m_comment, wxJSONVALUE_COMMENT_INLINE );
      m_comment.clear();
      return;
    }
  }
  if ( m_lastStored != 0 )  {
   ::wxLogTrace( storeTraceMask, _T("(%s) m_lastStored->lineNo=%d"),
			 __PRETTY_FUNCTION__, m_lastStored->GetLineNo() );
    if ( m_lastStored->GetLineNo() == m_commentLine ) {
      ::wxLogTrace( storeTraceMask, _T("(%s) comment added to \'m_lastStored\' INLINE"),
			 __PRETTY_FUNCTION__ );
      m_lastStored->AddComment( m_comment, wxJSONVALUE_COMMENT_INLINE );
      m_comment.clear();
      return;
    }
  }

  // if comment is BEFORE, store the comment in the 'm_next'
  // or 'm_current' value
  // if comment is AFTER, store the comment in the 'm_lastStored'
  // or 'm_current' value

  if ( m_flags & wxJSONREADER_COMMENTS_AFTER )  {  // comment AFTER
    if ( m_current )  {
      if ( m_current == parent || m_current->IsEmpty()) {
        AddError( _T("Cannot find a value for storing the comment (flag AFTER)"));
      }
      else  {
        ::wxLogTrace( storeTraceMask, _T("(%s) comment added to m_current (AFTER)"),
			 __PRETTY_FUNCTION__ );
        m_current->AddComment( m_comment, wxJSONVALUE_COMMENT_AFTER );
      }
    }
    else if ( m_lastStored )  {
     ::wxLogTrace( storeTraceMask, _T("(%s) comment added to m_lastStored (AFTER)"),
			 __PRETTY_FUNCTION__ );
      m_lastStored->AddComment( m_comment, wxJSONVALUE_COMMENT_AFTER );
    }
    else   {
     ::wxLogTrace( storeTraceMask, _T("(%s) cannot find a value for storing the AFTER comment"),
			 __PRETTY_FUNCTION__ );
      AddError(_T("Cannot find a value for storing the comment (flag AFTER)"));
    }
  }
  else {       // comment BEFORE can only be added to the 'next' value
    if ( m_next )  {
     ::wxLogTrace( storeTraceMask, _T("(%s) comment added to m_next (BEFORE)"),
			 __PRETTY_FUNCTION__ );
      m_next->AddComment( m_comment, wxJSONVALUE_COMMENT_BEFORE );
    }
    else   {
      // cannot find a value for storing the comment
      AddError(_T("Cannot find a value for storing the comment (flag BEFORE)"));
    }
  }
  m_comment.clear();
}


//! Return the number of bytes that contains a unicode char in various encodings
/*!
 This function was used by the GetChar() function in Unicode builds when the
 character has to be read from a stream object and returns the number
 of bytes that has to be read from the stream in order to convert them to
 a wide character.
 For locale dependent formats, the number of bytes is always 1,
 for Unicode formats it can be 2 (UCS-2) or 4 (UCS-4) or a variable
 number of bytes if the encoding format is UTF-8.

 The function is now no more used because input streams can only be
 encoded in UTF-8 format so it was substituted by the UTF8NumBytes()
 function.
 Calling this function always cause the program to abort for an ASSERTION
 failure.
*/
int
wxJSONReader::NumBytes()
{
  // always fails
  wxASSERT( 0 );
  return 0;
}

//! Compute the number of bytes that makes a UTF-8 encoded wide character.
/*!
 The function counts the number of '1' bit in the character \c ch and
 returns it.
 The UTF-8 encoding specifies the number of bytes needed by a wide character
 by coding it in the first byte. See below.

 Note that if the character does not contain a valid UTF-8 encoding
 the function returns -1.

\code
   UCS-4 range (hex.)    UTF-8 octet sequence (binary)
   -------------------   -----------------------------
   0000 0000-0000 007F   0xxxxxxx
   0000 0080-0000 07FF   110xxxxx 10xxxxxx
   0000 0800-0000 FFFF   1110xxxx 10xxxxxx 10xxxxxx
   0001 0000-001F FFFF   11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
   0020 0000-03FF FFFF   111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
   0400 0000-7FFF FFFF   1111110x 10xxxxxx ... 10xxxxxx
\endcode
*/
int
wxJSONReader::UTF8NumBytes( char ch )
{
  int num = 0;    // the counter of '1' bits
  for ( int i = 0; i < 8; i++ )  {
    if ( (ch & 0x80) == 0 )  {
      break;
    }
  ++num;
  ch = ch << 1;
  }

  // note that if the char contains more than six '1' bits it is not 
  // a valid UTF-8 encoded character
  if ( num > 6 )  {
    num = -1;
  }
  else if ( num == 0 )  {
    num = 1;
  }
  return num;
}

//! The function appends the wide character to the string value
/*!
 This function is called by \c ReadString() when a \e unicode \e escaped
 \e sequence is read from the input text as for example (the greek letter
 alpha):

 \code
  \u03B1
 \endcode

 In unicode mode, the function just appends the wide character code 
 stored in \c hex to the string \c s.
 In ANSI mode, the function converts the wide character code to the
 corresponding character if it is convertible using the locale dependent
 character set.
 If the wide char cannot be converted, the function appends the
 \e unicode \e escape \e sequence to the string \c s. 
 Returns ZERO if the character was not converted to a unicode escape
 sequence.
*/
int
wxJSONReader::AppendUnicodeSequence( wxString& s, int hex )
{
  int r = 0;
  #ifndef wxJSON_USE_UNICODE
  wchar_t ch = hex;
  #endif

  #if defined( wxJSON_USE_UNICODE )
    s.append( 1, (wxChar) hex );
  #else
    char buffer[10];
    size_t len = wxConvLibc.FromWChar( buffer, 10, &ch, 1 ); 
    if ( len != wxCONV_FAILED )  {
      s.append( 1, buffer[0] );
    }
    else  {
      snprintf( buffer, 10, _T("\\u%04X"), hex );
      s.append( buffer );
      r = 1;
    }
  #endif
  return r;
}

#if defined( wxJSON_64BIT_INT )
// Converts a decimal string to a 64-bit signed integer
/*
 This is an undcumented function which implements a simple variant
 of the 'strtoll' C-library function.
 I needed this implementation because the wxString::To(U)LongLong
 function does not work on my system:

  \li GNU/Linux Fedora Core 6
  \li GCC version 4.1.1
  \li libc.so.6

 The wxWidgets library (actually I have installed version 2.8.7)
 relies on 'strtoll' in order to do the conversion from a string
 to a long long integer but, in fact, it does not work because
 the 'wxHAS_STRTOLL' macro is not defined on my system. 

 Note that this implementation is not a complete substitute of the
 strtoll function because it only converts decimal strings (only base
 10 is implemented).
*/
bool
wxJSONReader::Strtoll( const wxString& str, wxInt64* i64 )
{
  wxChar sign = ' ';
  wxUint64 ui64;
  bool r = DoStrto_ll( str, &ui64, &sign );

  // check overflow for signed long long
  switch ( sign )  {
    case '-' :
      if ( ui64 > (wxUint64) LLONG_MAX + 1 )  {
        r = false;
      }
      else  {
        *i64 = (wxInt64) (ui64 * -1);
      }
      break;

    // case '+' :
    default :
      if ( ui64 > LLONG_MAX )  {
        r = false;
      }
      else  {
        *i64 = (wxInt64) ui64;
      }
      break;
  }
  return r;
}


bool
wxJSONReader::Strtoull( const wxString& str, wxUint64* ui64 )
{
  wxChar sign = ' ';
  bool r = DoStrto_ll( str, ui64, &sign );
  if ( sign == '-' )  {
    r = false;
  }
  return r;
}


bool
wxJSONReader::DoStrto_ll( const wxString& str, wxUint64* ui64, wxChar* sign )
{
  // the conversion is done by multiplying the individual digits
  // in reverse order to the corresponding power of 10
  //
  //  10's power:  987654321.9876543210
  //
  // LLONG_MAX:     9223372036854775807
  // LLONG_MIN:    -9223372036854775808
  // ULLONG_MAX:   18446744073709551615
  //
  // the function does not take into account the sign: only a
  // unsigned long long int is returned

  int maxDigits = 20;       // 20 + 1 (for the sign)

  wxUint64 power10[] = {
	wxULL(1),
	wxULL(10),
	wxULL(100),
	wxULL(1000),
	wxULL(10000),
	wxULL(100000),
	wxULL(1000000),
	wxULL(10000000),
	wxULL(100000000),
	wxULL(1000000000),
	wxULL(10000000000),
	wxULL(100000000000),
	wxULL(1000000000000),
	wxULL(10000000000000),
	wxULL(100000000000000),
	wxULL(1000000000000000),
	wxULL(10000000000000000),
	wxULL(100000000000000000),
	wxULL(1000000000000000000),
	wxULL(10000000000000000000)
  };


  wxUint64 temp1 = wxULL(0);   // the temporary converted integer

  int strLen = str.length();
  if ( strLen == 0 )  {
    // an empty string is converted to a ZERO value: the function succeeds
    *ui64 = wxLL(0);
    return true;
  }

  int index = 0;
  wxChar ch = str[0];
  if ( ch == '+' || ch == '-' )  {
    *sign = ch;
    ++index;
    ++maxDigits;
  }

  if ( strLen > maxDigits )  {
    return false;
  }

  // check the overflow: check the string length and the individual digits
  // of the string; the overflow is checked for unsigned long long
  if ( strLen == maxDigits )  {
    wxString uLongMax( _T("18446744073709551615"));
    int j = 0;
    for ( int i = index; i < strLen - 1; i++ )  {
      ch = str[i];
      if ( ch < '0' || ch > '9' ) {
        return false;
      }
      if ( ch > uLongMax[j] ) {
        return false;
      }
      if ( ch < uLongMax[j] ) {
        break;
      }
      ++j;
    }
  }

  // get the digits in the reverse order and multiply them by the
  // corresponding power of 10
  int exponent = 0;
  for ( int i = strLen - 1; i >= index; i-- )   {
    wxChar ch = str[i];
    if ( ch < '0' || ch > '9' ) {
      return false;
    }
    ch = ch - '0';
    // compute the new temporary value
    temp1 += ch * power10[exponent];
    ++exponent;
  }
  *ui64 = temp1;
  return true;
}

#endif       // defined( wxJSON_64BIT_INT )
