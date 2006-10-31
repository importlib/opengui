#include "OpenGUI_ImageryManager.h"
#include "OpenGUI_Imageset.h"
#include "OpenGUI_Exception.h"
#include "OpenGUI_LogSystem.h"

namespace OpenGUI {

	//############################################################################
	Imageset::Imageset( TexturePtr texturePtr, std::string sourceImageFilename )
			: mFilename( sourceImageFilename ), mpTexture( texturePtr ) {
		LogManager::SlogMsg( "Imageset", OGLL_INFO ) << "(" << mFilename << ") " << "Creation" << Log::endlog;
	}
	//############################################################################
	Imageset::~Imageset() {
		LogManager::SlogMsg( "Imageset", OGLL_INFO ) << "(" << mFilename << ") " << "Destruction" << Log::endlog;
		Imageset::destroyAllImagery();
	}
	//############################################################################
	const std::string& Imageset::getName() {
		return mFilename;
	}
	//############################################################################
	size_t Imageset::getImageryCount() const {
		return mChildImageryList.size();
	}
	//############################################################################
	ImageryPtr Imageset::createImagery( std::string imageryName ) {
		return Imageset::createImagery( imageryName, FRect( 0.0f, 0.0f, 1.0f, 1.0f ) );
	}
	//############################################################################
	ImageryPtr Imageset::createImagery( std::string imageryName, FRect areaRect, IRect imagesetRect ) {
		if ( imageryName == "" ) {
			imageryName = ImageryManager::_generateRandomName();
		}

		LogManager::SlogMsg( "Imageset", OGLL_INFO2 ) << "(" << mFilename << ") "
		<< "Create Imagery: " << imageryName << " " << areaRect.toStr() << Log::endlog;

		if ( Imageset::getImagery( imageryName ) ) {
			LogManager::SlogMsg( "Imageset", OGLL_WARN ) << "(" << mFilename << ") "
			<< "Duplicate Imagery found, removing old Imagery: " << imageryName << Log::endlog;
			//this function redefines existing imagery with clashing names
			destroyImagery( imageryName );
		}

		if ( imagesetRect.getWidth() == 0 || imagesetRect.getHeight() == 0 ) {
			//the imagesetRect was not provided or is possibly invalid, so try to generate a new one
			if ( mpTexture ) { //but only if we have a texture to use, otherwise forget it
				IVector2 texSize = mpTexture->getSize();
				imagesetRect.min.x = ( int )( areaRect.min.x * ( float )texSize.x );
				imagesetRect.min.y = ( int )( areaRect.min.y * ( float )texSize.y );
				imagesetRect.max.x = ( int )( areaRect.max.x * ( float )texSize.x );
				imagesetRect.max.y = ( int )( areaRect.max.y * ( float )texSize.y );
			}
		}

		ImageryPtr imgptr = new Imagery( imageryName, areaRect, imagesetRect, mpTexture );
		mChildImageryList.push_back( imgptr );
		return imgptr;
	}
	//############################################################################
	ImageryPtr Imageset::createImagery( std::string imageryName, IRect areaRect ) {
		if ( !mpTexture ) //if this imageset has no texture, then use the default createImage (which uses full image extents for UVs)
			return Imageset::createImagery( imageryName );

		//convert the IRect pixels into UV addresses and pass it along
		IVector2 texSize = mpTexture->getSize();
		FRect frect;
		frect.min.x = (( float )areaRect.min.x ) / (( float )texSize.x );
		frect.min.y = (( float )areaRect.min.y ) / (( float )texSize.y );
		frect.max.x = (( float )areaRect.max.x ) / (( float )texSize.x );
		frect.max.y = (( float )areaRect.max.y ) / (( float )texSize.y );
		return Imageset::createImagery( imageryName, frect, areaRect );
	}
	//############################################################################
	void Imageset::destroyImagery( ImageryPtr imageryPtr ) {
		Imageset::destroyImagery( imageryPtr.get() );
	}
	//############################################################################
	void Imageset::destroyImagery( Imagery* pImagery ) {
		LogManager::SlogMsg( "Imageset", OGLL_INFO2 ) << "(" << mFilename << ") "
		<< "Destroying Imagery: " << pImagery->getName() << Log::endlog;

		ImageryPtrList::iterator iter = mChildImageryList.begin();
		while ( mChildImageryList.end() != iter ) {
			if (( *iter ) == pImagery ) {
				mChildImageryList.erase( iter );
				return;
			}
			iter++;
		}
		std::stringstream ss;
		ss << "(0x" << ( unsigned int ) pImagery << ") " << pImagery->getName();
		OG_THROW( Exception::ERR_ITEM_NOT_FOUND, "Could not find Imagery: " + ss.str(), "Imageset::destroyImagery" );
	}
	//############################################################################
	void Imageset::destroyImagery( std::string name ) {
		Imageset::destroyImagery( Imageset::getImagery( name ).get() );
	}
	//############################################################################
	ImageryPtr Imageset::getImagery( std::string imageryName ) {
		ImageryPtrList::iterator iter = mChildImageryList.begin();
		while ( mChildImageryList.end() != iter ) {
			if (( *iter )->mName == imageryName ) {
				return ImageryPtr(( *iter ) );
			}
			iter++;
		}
		return ImageryPtr( 0 );
	}
	//############################################################################
	TexturePtr Imageset::getTexture() {
		return mpTexture;
	}
	//############################################################################
	void Imageset::destroyAllImagery() {
		LogManager::SlogMsg( "Imageset", OGLL_INFO2 ) << "(" << mFilename << ") "
		<< "Destroying All Imagery..." << Log::endlog;
		ImageryPtrList tmpList = mChildImageryList;
		ImageryPtrList::iterator iter = tmpList.begin();
		while ( tmpList.end() != iter ) {
			Imageset::destroyImagery(( *iter ).get() );
			iter++;
		}
		mChildImageryList.clear();
	}
	//############################################################################
	Imageset::ImageryList Imageset::getImageryList() {
		ImageryList retval;
		for ( ImageryPtrList::iterator iter = mChildImageryList.begin(); mChildImageryList.end() != iter; iter++ ) {
			retval.push_back(( *iter )->mName );
		}
		retval.sort();
		return retval;
	}
	//############################################################################
	void Imageset::finalize() {
		delete this;
	}
	//############################################################################
}
;//namespace OpenGUI{







