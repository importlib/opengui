
#include "OpenGUI.h" // <-- This is the one place I would consider this acceptable

namespace OpenGUI {
	//############################################################################
	template<> System* Singleton<System>::mptr_Singleton = 0;
	//############################################################################
	System& System::getSingleton( void ) {
		assert( mptr_Singleton );
		return ( *mptr_Singleton );
	}
	//############################################################################
	System* System::getSingletonPtr( void ) {
		return mptr_Singleton;
	}
	//############################################################################
	System::System( Renderer* renderer, ResourceProvider* resourceProvider, std::string logFile ) {
		if ( logFile == "" ) {
			System( renderer, resourceProvider, 0 );
		} else {
			mDefaultLogListener = new LogListenerToFile( logFile );
			m_LogManager = new LogManager( mDefaultLogListener );
			System::doConstructor( renderer, resourceProvider );
		}
	}
	//############################################################################
	System::System( Renderer* renderer, ResourceProvider* resourceProvider, LogListener* logListener ) {
		m_LogManager = new LogManager( logListener );
		mDefaultLogListener = 0;
		System::doConstructor( renderer, resourceProvider );
	}
	//############################################################################
	void System::doConstructor( Renderer* renderer, ResourceProvider* resourceProvider ) {
		LogManager::SlogMsg( "INIT", OGLL_MSG ) << "OpenGUI (ver ";
		LogManager::SlogMsg( "INIT", OGLL_MSG ) << OPENGUI_VERSION_STR << ") ";
		LogManager::SlogMsg( "INIT", OGLL_MSG ) << "http://OpenGUI.SourceForge.net/" << Log::endlog;
		LogManager::SlogMsg( "INIT", OGLL_INFO ) << "System Init Started" << Log::endlog;
		mTimerManager = new TimerManager; //get this up asap

		m_PerformAutoTicks = true;

		//mActiveGUISheet = 0;
		//m_CurrentMouseElement = 0;
		//mCapturerMouse = 0;
		//mCapturerKeyboard = 0;

		//mUserDblClickRate = 500;
		//mUserClickAccuracy = 0.05f;

		assert( renderer );
		mRenderer = renderer;
		if ( !mRenderer ) {
			OG_THROW( Exception::ERR_INVALIDPARAMS, "No valid Renderer provided", "System" );
		}

		mRenderer->getViewportDimensions( mViewportResolution ); //get the viewport resolution

		LogManager::SlogMsg( "INIT", OGLL_INFO3 ) << "Initial Viewport Resolution: " << mViewportResolution.toStr() << Log::endlog;

		m_PluginManager = new PluginManager;

		//mWidgetFactoryManager = new WidgetFactoryManager();
		//mWidgetTemplateManager = new WidgetTemplateManager();

		if ( resourceProvider ) {
			mResourceProvider = resourceProvider;
			mUsingGenericResourceProvider = false;
			LogManager::SlogMsg( "INIT", OGLL_INFO3 ) << "Using custom resource provider: " << mResourceProvider << Log::endlog;
		} else {
			LogManager::SlogMsg( "INIT", OGLL_INFO3 ) << "Using built in resource provider" << Log::endlog;
			mResourceProvider = new GenericResourceProvider();
			mUsingGenericResourceProvider = true;
		}

		mTextureManager = new TextureManager( renderer ); //create the texture manager
		mImageryManager = new ImageryManager( mResourceProvider ); //create the imagery manager


		mFontManager = new FontManager();

		mScreenManager = new ScreenManager();

		LogManager::SlogMsg( "INIT", OGLL_INFO ) << "System Init Complete" << Log::endlog;
	}

	//############################################################################
	System::~System() {
		LogManager::SlogMsg( "SHUTDOWN", OGLL_INFO ) << "OpenGUI Shutdown Started" << Log::endlog;

		//System::_destroyAllGUISheets();
		if ( mScreenManager )
			delete mScreenManager;

		/*
		plugins are always unloaded as soon as possible to ensure that
		all other subsystems are still available to the pluginStop
		functions
		*/
		if ( m_PluginManager )
			delete m_PluginManager;

		if ( mFontManager )
			delete mFontManager;

		if ( mImageryManager )
			delete mImageryManager;

		if ( mTextureManager )
			delete mTextureManager;

		if ( mUsingGenericResourceProvider )
			delete mResourceProvider;

		if ( mTimerManager )
			delete mTimerManager; //delete this last

		LogManager::SlogMsg( "SHUTDOWN", OGLL_INFO ) << "OpenGUI Shutdown Ending" << Log::endlog;

		//End logging facilities last
		if ( m_LogManager )
			delete m_LogManager;
		if ( mDefaultLogListener )
			delete mDefaultLogListener; //very lastly close default log listener if it was used
	}
	//############################################################################
	void System::loadPlugin( std::string filename ) {
		PluginManager::getSingleton().loadPlugin( filename );
	}
	//############################################################################
	void System::unloadPlugin( std::string filename ) {
		PluginManager::getSingleton().unloadPlugin( filename );
	}
	//############################################################################
	void System::notifyViewportDimensionsChanged() {
		/*
		//!\todo make sure this is all that needs to be done to properly handle viewport dimension changes
		mRenderer->getViewportDimensions( mWindowResolution );
		if ( mActiveGUISheet )
		mActiveGUISheet->dirtyCache_Recursive();
		*/
	}
	//############################################################################
	void System::updateScreens() {
		mScreenManager->updateScreens();

		//if ( m_PerformAutoTicks ) //only do this if we aren't getting time injections from the app
		//	mTimerManager->_DoAutoTickInject();

		/*
		if ( mActiveGUISheet ) {

			mRenderer->preRenderSetup();

			mActiveGUISheet->renderGUISheet( mRenderer );


			Render::RenderOperationList renderOpList_Cursor;
			renderOpList_Cursor = mCursorManager->getCursorRenderOpList();

			Render::RenderOperationList::iterator iterC = renderOpList_Cursor.begin();
			while ( iterC != renderOpList_Cursor.end() ) {
				mRenderer->doRenderOperation(( *iterC ) );
				iterC++;
			}

			mRenderer->postRenderCleanup();
		}
		*/

		//System::_stat_UpdateFPS();
	}
	//############################################################################
	TimerPtr statFPSTimer;
	typedef std::list<unsigned int> FPSList;
	FPSList statFPSList;
	float System::statRenderFPS() {
		if ( statFPSList.size() > 0 ) {
			float fpsSum = 0.0f;
			for ( FPSList::iterator iter = statFPSList.begin(); iter != statFPSList.end(); iter++ ) {
				unsigned int delta = ( *iter );
				fpsSum += ( float )delta;
			}

			fpsSum = fpsSum / ( float )statFPSList.size();
			return 1000.0f / fpsSum;
		} else
			return 0.0f;
	}
	//############################################################################
	void System::_stat_UpdateFPS() {
		if ( statFPSTimer.isNull() ) {
			statFPSTimer = TimerManager::getSingletonPtr()->getTimer();
		} else {
			statFPSList.push_back( statFPSTimer->getMilliseconds() );
			statFPSTimer->reset();
			while ( statFPSList.size() > 100 )
				statFPSList.pop_front();
		}
	}
	//############################################################################
}
;//namespace OpenGUI{


