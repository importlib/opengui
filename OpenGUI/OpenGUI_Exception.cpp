// OpenGUI (http://opengui.sourceforge.net)
// This source code is released under the BSD License
// See LICENSE.TXT for details

#include "OpenGUI_Exception.h"
#include "OpenGUI_LogSystem.h"

namespace OpenGUI {


	Exception::Exception( int code, const String& desc, const String& srcArea )
			: mCode( code ), mLine( 0 ), mMsg( desc ), mSrc( srcArea ), mFile( 0 ) {
		if ( LogManager::getSingletonPtr() )
			LogManager::SlogMsg( "EXCEPTION", OGLL_ERR ) << Exception::getFullMessage() << Log::endlog;
	}

	Exception::Exception( int code, const String& desc, const String& srcArea, char* file, long line )
			: mCode( code ), mLine( line ), mMsg( desc ), mSrc( srcArea ), mFile( file ) {
		if ( LogManager::getSingletonPtr() )
			LogManager::SlogMsg( "EXCEPTION", OGLL_ERR ) << Exception::getFullMessage() << Log::endlog;
	}

	Exception::Exception( const Exception& rhs )
		: mCode( rhs.mCode ), mLine( rhs.mLine ), mMsg( rhs.mMsg ), mSrc( rhs.mSrc ), mFile( rhs.mFile ) {}

	void Exception::operator = ( const Exception& rhs ) {
		mLine = rhs.mLine;
		mCode = rhs.mCode;
		mMsg = rhs.mMsg;
		mSrc = rhs.mSrc;
		mFile = rhs.mFile;
	}

	String Exception::getFullMessage() const {
		std::stringstream ss;
		ss << mSrc << "[" << getCodeStr( mCode ) << "] " << mMsg;
		if ( mFile ) {
			ss << " ## " << mFile << " : " << mLine;
		}
		return ss.str();
	}

	String Exception::getCodeStr( int code ) {
		switch ( code ) {
		case OP_FAILED:
			return "OP_FAILED";
		case ERR_INVALIDPARAMS:
			return "ERR_INVALIDPARAMS";
		case ERR_DUPLICATE_ITEM:
			return "ERR_DUPLICATE_ITEM";
		case ERR_ITEM_NOT_FOUND:
			return "ERR_ITEM_NOT_FOUND";
		case ERR_FILE_NOT_FOUND:
			return "ERR_FILE_NOT_FOUND";
		case ERR_FILE_NOT_READABLE:
			return "ERR_FILE_NOT_READABLE";
		case ERR_FILE_NOT_WRITABLE:
			return "ERR_FILE_NOT_WRITABLE";
		case ERR_INTERNAL_ERROR:
			return "ERR_INTERNAL_ERROR";
		case ERR_NOT_IMPLEMENTED:
			return "ERR_NOT_IMPLEMENTED";
		default:
			return "!UNKNOWN_ERROR_CODE!";

		}
	}

}
