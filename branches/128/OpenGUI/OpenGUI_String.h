// OpenGUI (http://opengui.sourceforge.net)
// This source code is release under the BSD License
// See LICENSE.TXT for details

#ifndef E5AE9E12_AF34_48ff_B669_2802A0C3DC0A
#define E5AE9E12_AF34_48ff_B669_2802A0C3DC0A

#include "OpenGUI_PreRequisites.h"
#include "OpenGUI_Exports.h"

namespace OpenGUI {
	//! Provides UTF-8 string representation with several implicit conversions for Unicode (wide) / std::string / C-string
	/*! It is \b extremely important that you realize that this class expects that all
	non std::wstring byte streams are already UTF-8 encoded and are simply lacking the
	proper UTF-8 representation. This is true for all constructors, and overloaded operators.
	It will properly handle std::wstring as Unicode, otherwise it fully expects
	pre-formatted UTF-8 streams unless explicitly stated otherwise in the function
	description. For those of us that are English speaking, this means very little, as
	UTF-8 already handles all <128 char values natively without any special decorations
	necessary. Languages that would normally require use of the >127 char values in the
	ASCII table should either provide UTF-8 encoded streams or use wide strings to
	initialize this class.

	\remarks
	Why do we store UTF-8 instead of UTF-32/USC-4 or UTF-16 (like most people)? Well, UTF-32
	is horribly large. In fact it will quadruple the size of any ASCII encoded string, and
	all of the additional data that it adds in that case is nothing but zeros. Even for most
	European languages, the majority of UTF-32 data is just wasted space. UTF-16 (wchar_t
	and std::wstring) is arguably just as bad. Not only does UTF-16 not protect you from
	having to employ surrogate data pairs to represent all Unicode values (many Eastern
	languages will use surrogate pairs extensively), but again, you suffer the wrath of
	unnecessary data bloat when representing most characters from Western languages.
	On the other hand, UTF-8 is an effective way to encode Unicode without adding a ton
	of additional cruft, and it naturally preserves the most important aspects of C string
	functionality, while the unpreserved functionality is still very cost efficient.
	Additionally, converting from UTF-32 to UTF-8 and back is trivial and very efficient,
	comprised mostly of simple bit-shifts. So in short, %OpenGUI utilizes UTF-8 internally
	because, not only is it more space efficient, but that additional space savings tends
	to also contribute to better performance. This is especially important to us due to
	the extensive use of string indexed maps within %OpenGUI.

	\see
	For additional information on UTF-8 encoding: http://en.wikipedia.org/wiki/UTF-8
	*/
	class OPENGUI_API UTF8String {
		friend class iterator;
		//! represents a single data point (byte) in the UTF-8 stream, not a UCS code point!
		typedef unsigned char data_point;
		typedef std::basic_string<data_point> ustring;

	public:
		//! size type used to indicate character positions within the string
		typedef size_t size_type;
		//! the usual constant representing: not found, no limit, etc
		static const size_type npos;
		//! type for representing a UTF-32/UCS-4 code point, which is the only reliable way to always represent a single character using an integral type
		typedef UINT32 code_point;

		//! This exception is used when invalid data streams are encountered
	class invalid_data: public std::runtime_error { // i don't know why the beautifier is freaking out on this line
		public:
			//! constructor takes a string message that can be later retrieved by the what() function
			explicit invalid_data( const std::string& _Message ): std::runtime_error( _Message ) {
				/* The thing is, Bob, it's not that I'm lazy, it's that I just don't care. */
			}
		};

		//! iterator for UTF8String
		/*! \warning
		Due to the use of variable length encoding, this iterator it strongly tied to the UFT8String
		object, utilizing an internal variable to track the version of the UTF8String. The version of
		the UTF8String changes whenever the contents of the string are altered, which signals the
		iterator that it needs to resynchronize on its next usage. Synchronization is an O(n) operation
		in respect to the iterator index. The only way to synchronize properly is to walk the entire
		UTF-8 stream until we reach the proper index. Here's the caveat part: UTF8String is only capable
		of representing \c size_t versions before the version indicator overflows. So if you plan on doing
		upwards of 4.2 billion alterations to a UTF8String before you use an attached iterator again
		(which is quite unlikely, but still), you should try to call the sync() function a few times in
		the middle to ensure that you don't win the 1 in 4.2 billion odds lottery that the version indicator
		falls exactly back where it was when you first started, causing unexpected iterator drift.
		*/
		class iterator {
		public:
			typedef code_point value_type;
			typedef value_type & reference;
			typedef value_type * pointer;

			iterator() {
				mStringPtr = 0;
				mIndex = 0;
			}
			iterator( UTF8String* stringPtr, ustring::iterator init, size_type pos ) {
				mPos = init;
				mStringPtr = stringPtr;
				mVersion = mStringPtr->mVersion;
				mIndex = pos;
			}
			iterator( const iterator& copy ) {
				mPos       = copy.mPos;
				mStringPtr = copy.mStringPtr;
				mIndex     = copy.mIndex;
				mVersion   = copy.mVersion;
			}
			//! prefix ++ operator
			iterator& operator++() {
				sync();
				_seek( 1 );
				return *this;
			}
			//! postfix ++ operator
			iterator operator++( int ) {
				iterator ret( *this );
				operator++();
				return ret;
			}
			//! prefix -- operator
			iterator& operator--() {
				sync();
				_seek_rev( 1 );
				return *this;
			}
			//! postfix -- operator
			iterator operator--( int ) {
				iterator ret( *this );
				operator--();
				return ret;
			}
			//! the += operator
			iterator& operator+=( int c ) {
				sync();
				_seek( c );
				return *this;
			}
			//! the -= operator
			iterator& operator-=( int c ) {
				sync();
				_seek_rev( c );
				return *this;
			}
			//! the + operator, returns copy of iterator at new position
			iterator operator+( int c ) {
				iterator ret( *this );
				ret.operator += ( c );
				return ret;
			}
			//! the - operator, returns copy of iterator at new position
			iterator operator-( int c ) {
				iterator ret( *this );
				ret.operator -= ( c );
				return ret;
			}

			//! iterator equality operator
			bool operator==( const iterator& r ) {
				if ( mStringPtr != r.mStringPtr )
					throw std::runtime_error( "cannot compare iterators from 2 independent streams" );
				sync();
				r.sync();
				return mPos == r.mPos;
			}
			//! iterator inequality operator
			bool operator!=( const iterator& r ) {
				if ( mStringPtr != r.mStringPtr )
					throw std::runtime_error( "cannot compare iterators from 2 independent streams" );
				sync();
				r.sync();
				return mPos != r.mPos;
			}

			//! resynchronizes the iterator position if the pointed UTF8String's version has changed
			/*! \return \c true if a resync was performed, \c false if it was skipped because it was not necessary */
			bool sync() const {
				if ( !mStringPtr )
					throw std::runtime_error( "invalid iterator" );
				if ( mStringPtr->mVersion != mVersion ) {
					_sync();
					return true;
				} else {
					return false;
				}
			}

		private:
			void _seek( int c ) {
				if ( c < 0 ) {
					_seek_rev( -c );
				} else {
					while ( c-- ) {
						size_t l = UTF8String::_getSequenceLen(( *mPos ) );
						mPos += l;
						mIndex++;
					}
				}
			}
			void _seek_rev( int c ) {
				if ( c < 0 ) {
					_seek( -c );
				} else {
					while ( c-- ) {
						while ( UTF8String::_isContByte(( *( --mPos ) ) ) );
						mIndex--;
					}
				}
			}
			void _sync() const {
				mVersion = mStringPtr->mVersion;
				if ( mIndex == npos ) {
					mPos = mStringPtr->mData.end();
					mIndex = mStringPtr->mLength; // we can't stay at npos forever
				} else {
					size_t c = mIndex;
					mPos = mStringPtr->mData.begin();
					while ( c-- ) {
						size_t l = UTF8String::_getSequenceLen(( *mPos ) );
						mPos += l;
					}
				}
			}
			mutable ustring::iterator mPos;
			UTF8String* mStringPtr;
			mutable size_t mIndex;
			mutable size_t mVersion;
		};




		//! default constructor, creates an empty string
		UTF8String();
		//! destructor
		~UTF8String();
		//! copy constructor
		UTF8String( const UTF8String& copy );
		//! nul-terminated C-string initialized constructor
		UTF8String( const char* cstr );
		//! std::string initialized constructor
		UTF8String( const std::string& str );
		//! std::wstring (wide string) initialized constructor
		UTF8String( const std::wstring& wstr );

		//! clears the contents of the string
		void clear();

		//! Returns the maximum number of elements the string can hold before it will need to allocate more space
		/*! The returned value assumes the shortest possible encoding for each code_point (1 byte each). UTF-8 is
		a variable length encoding scheme, so this is the best we can do. */
		size_type capacity() const {
			return mData.capacity();
		}

		//! Returns the maximum number of elements that the string can be guaranteed capable to hold
		/*! This is barring memory limitations, which we can't detect. Also, the returned value is
		conservative, assuming maximum UTF-8 encoding expansion. You'll likely be able to hold much
		more than we report back, but again, variable length encoding means we have to take some
		defensive measures. */
		size_type max_size() const {
			return mData.max_size() / 6;
		}

		//! Returns the number of elements (stored Unicode code points) in the current string
		/*! \see byte_length() */
		size_type length() const {
			return size();
		}

		//! Returns the number of elements (stored Unicode code points) in the current string
		/*! \see byte_length() */
		size_type size() const {
			return mLength;
		}

		//! Returns the number of bytes in the UTF-8 stream
		/*! This is the byte array length of the encoded stream. */
		size_type byte_length() const {
			return mData.size();
		}

		//! returns an iterator at the beginning of the string
		iterator begin();
		//! returns an iterator at the end of the string
		iterator end();

		//! assign \c str to the current string (\c str is treated as a UTF-8 stream)
		UTF8String& assign( const std::string& str );
		//! assign \c wstr to the current string
		UTF8String& assign( const std::wstring& wstr );
		//! assign \c c_str to the current string (\c c_str is treated as a UTF-8 stream)
		UTF8String& assign( const char* c_str );
		//! assign the first \c num characters of \c c_str to the current string (\c c_str is treated as a UTF-8 stream)
		UTF8String& assign( const char* c_str, size_type num );

		//! appends \c str on to the end of the current string
		UTF8String& append( const std::string& str ) {
			ustring tmp = ( const data_point* )str.c_str();
			size_type len = _verifyUTF8( tmp );
			_append( tmp );
			mLength += len;
			_versionChange();
			return *this;
		}
		//! appends \c wstr on to the end of the current string
		UTF8String& append( const std::wstring& wstr ) {
			ustring tmp;
			size_type len = _loadWString( wstr, tmp );
			_append( tmp );
			mLength += len;
			_versionChange();
			return *this;
		}
		//! appends \c c_str (UTF-8 data) on to the end of the current string
		UTF8String& append( const char* c_str ) {
			ustring tmp = ( const data_point* )c_str;
			size_type len = _verifyUTF8( tmp );
			_append( tmp );
			mLength += len;
			_versionChange();
			return *this;
		}

		//! appends \c num bytes of UTF-8 data from \c c_str on to the end of the current string
		/*! While \c c_str is expected to be UTF-8 encoded data, \c num is the number of bytes of
		that stream (not the number of characters). */
		UTF8String& append( const char* c_str, size_type num ) {
			ustring tmp;
			tmp.append(( const data_point* )c_str, num );
			size_type len = _verifyUTF8( tmp );
			_append( tmp );
			mLength += len;
			_versionChange();
			return *this;
		}

		//! appends a substring of \c str starting at \c index that is \c len characters long on to the end of the current string
		UTF8String& append( const std::string& str, size_type index, size_type len );

		//! appends \c num repetitions of \c ch on to the end of the current string
		UTF8String& append( size_type num, code_point ch ) {
			_utf32_to_utf8( ch, mData );
			mLength++;
			_versionChange();
			return *this;
		}

		//! appends the sequence denoted by \c start and \c end on to the end of the current string
		UTF8String& append( iterator start, iterator end ) {
			while ( start != end ) {
				throw 0;
				//code_point& ch;
				//append(1,ch);
				++start;
			}
			return *this;
		}


		/* Waiting on Append
		//! gives the current string the values from \c start to \c end
		void assign( iterator start, iterator end );
		//! assign a substring of \c ustr starting at \c index that is \c len characters long to the current string
		UTF8String& assign( const UTF8String& ustr, size_type index, size_type len );
		//! assign the string to \c num copies of \c val
		UTF8String& assign( size_type num, const char& val );
		//! assign the string to \c num copies of \c val
		UTF8String& assign( size_type num, const code_point& val );
		*/

		/* Waiting on code_point to become a class
		//! returns a reference to the element in the string at index \c loc
		value_type& at( size_type loc );
		//! returns a reference to the element in the string at index \c loc
		const value_type& at( size_type loc ) const;
		*/

		//! exchanges the elements of the current string with those of \c from. This function operates in constant time.
		void swap( UTF8String& from ) {
			mData.swap( from.mData );
			size_type tmp = mLength;
			mLength = from.mLength;
			from.mLength = tmp;
			_versionChange();
			from._versionChange();
		}

	private:
		ustring mData; // this is the actual UTF-8 data we are storing
		size_type mLength; // we cache the length internally because we don't like iterating constantly for length
		size_type mVersion; // version indicator, used to indicate a resync condition to iterators

		//////////////////////////////////////////////////////////////////////////
		// utility functions
		// dumbly appends the given string, no verification performed
		void _append( const ustring& str );
		// dumbly assigns the given string, no verification performed
		void _assign( const ustring& str );

		void _versionChange() {
			mVersion++;
		}

		//! predicts the number of UTF-8 stream bytes that will be needed to represent the given UCS-4 character
		static size_t _predictBytes( const code_point& c );
		//! returns the length of the sequence starting with \c s
		static size_t _getSequenceLen( const data_point& s );
		//! returns \c TRUE if the data point is a continuation byte
		static bool _isContByte( const data_point& s );

		//! loads \c in_wstr as a UTF-8 stream, appending it to the end \c out_ustr, returns the UTF-8 character length of \c out_ustr
		size_type _loadWString( const std::wstring& in_wstr, ustring& out_ustr ) const;

		//! returns the UCS-4 code point in the UTF-8 stream at the given position
		static code_point _utf8_to_utf32( const data_point* utf8_str );
		//! decodes the given code point and appends the stream bytes to the given string
		void _utf32_to_utf8( code_point c, ustring& out ) const;

		//! converts UTF-16 to UTF-32
		/*! We store the UTF-16 in a 32-bit code-point because UTF-16 can still require
		surrogate pairs for some values. Place the lead surrogate in the high bits if one exists. */
		static void _utf16_to_utf32( const code_point& c, code_point& c_out );
		//! converts UTF-32 to UTF-16
		/*! We store the UTF-16 in a 32-bit code-point because UTF-16 can still require
		surrogate pairs for some values. We place the lead surrogate in the high bits if one exists. */
		static void _utf32_to_utf16( const code_point& c, code_point& c_out );

		//! runs the held stream through the other _verifyUTF8() test
		size_type _verifyUTF8() const {
			return _verifyUTF8( mData );
		};

		//! returns the length of the given UTF-8 stream and tests for proper continuation bytes and sequence length identifiers
		static size_type _verifyUTF8( const ustring& str );
		//! tests the given std::string as a UTF-8 stream for proper continuation bytes and sequence length identifiers, returns the number of UCS characters in the stream (not byte length)
		static size_type _verifyUTF8( const std::string& str );

		///////////////////////////////////////////////////////////////////////
		// Scratch buffer
		void _init(); //!< common constructor operations
		void _cleanBuffer() const; //!< auto cleans the scratch buffer using the proper delete for the stored type
		void _getBufferCStr( size_t len ) const; //!< creates a scratch buffer of at least \c len size

		// data type identifier
		enum BufferType {
			none,
			string,
			wstring,
			cstring
		};

		mutable BufferType m_bufferType; // identifies the data type held in m_buffer
		mutable size_t m_bufferSize; // size of the CString buffer

		// multi-purpose buffer used everywhere we need a throw-away buffer. (Yes, we're that brave ;-)
		union {
			mutable void* mVoidBuffer;
			mutable char* mCStrBuffer;
			mutable std::string* mStrBuffer;
			mutable std::wstring* mWStrBuffer;
		}
		m_buffer;

	};

	//////////////////////////////////////////////////////////////////////////
	// Define the base types used throughout the rest of the library
	typedef char Char;
	typedef std::string String;
	/*
	//! maps the String type to UTF8String
	typedef UTF8String String;
	//! maps the Char type to wchar_t
	typedef wchar_t Char; // a single character is only usefully representable using wide characters
	*/

} // namespace OpenGUI{

#endif // E5AE9E12_AF34_48ff_B669_2802A0C3DC0A
