#ifndef DC3E54A4_1432_4fa1_A635_D76797D7ED89
#define DC3E54A4_1432_4fa1_A635_D76797D7ED89

#include "OpenGUI_PreRequisites.h"
#include "OpenGUI_Singleton.h"
#include "OpenGUI_Exports.h"

#include "OpenGUI_Event.h"

#include "OpenGUI_MouseButton.h"


//! The base namespace contains the classes that applications will deal with the most.
namespace OpenGUI {
	//forward declarations

	//class WidgetFactoryManager;
	//class WidgetTemplateManager;

	class TimerManager;
	class LogManager;
	class LogListener;
	class LogListenerToFile;
	class CursorManager;
	class ResourceProvider;
	class FontManager;
	class PluginManager;
	class Renderer;
	class ImageryManager;

	/*!
		\brief
		The System object is the base of the OpenGUI project.
		It is responsible for distributing events, accepting input, managing the
		cursor, and managing the GUI sheets. \n \b [Singleton]

		This class is implemented using the Singleton system. There can only be one System
		object instantiated at any point in time.

		\todo implement me
	*/
	class OPENGUI_API System : public Singleton<System> {
		//friend class Element;
	public:


		//! The constructor for the System object.
		/*! Firstly, the System object is a singleton, so only one can exist
			in an application at any given point in time.
			
			In order to create an instance of the System object, you must
			provide a pointer to a valid Renderer. The resourceProvider pointer
			is optional, as a generic implementation of that comes pre-built
			with the library.

			\param renderer pointer to a valid Renderer object
			\param resourceProvider pointer to a valid ResourceProvider object, or 0
				to use the built in generic ResourceProvider
			\param logFile name of the log file to write log message to. If "" is used,
				no log file will be created
		*/
		System( Renderer* renderer, ResourceProvider* resourceProvider = NULL, std::string logFile = "OpenGUI.log" );
		/*! As the other constructor except a log listener is passed in place of a log filename.
			\param renderer pointer to a valid Renderer object
			\param resourceProvider pointer to a valid ResourceProvider object, or 0
				to use the built in generic ResourceProvider
			\param logListener A valid pointer to a LogListener, or 0 for no logging.
		*/
		System( Renderer* renderer, ResourceProvider* resourceProvider, LogListener* logListener );
		//System(Renderer* renderer);

		~System();

		//Reimplementation required for this style of singleton implementation to work across DLLs
		//! Retrieve the current singleton, if one exists. If none exists, this will cause an error.
		static System& getSingleton( void );

		//Reimplementation required for this style of singleton implementation to work across DLLs
		//! Retrieve a pointer to the current singleton, if one exists. If none exists, this will return 0.
		static System* getSingletonPtr( void );

		//! loads a plugin by filename
		void loadPlugin( std::string filename );
		//! unloads a plugin by filename
		void unloadPlugin( std::string filename );

		/*! \brief Renders the GUI using the current render system.	*/
		void renderGUI();

		//! Returns a pointer to the registered ResourceProvider. Apps really shouldn't need, or use, this.
		ResourceProvider* _getResourceProvider() {
			return mResourceProvider;
		}
		//! Returns a pointer to the registered Renderer. Apps really shouldn't need, or use, this.
		Renderer* _getRenderer() {
			return mRenderer;
		}

		//! Notify the System that the dimensions of the viewport it is drawing to have changed.
		/*! This \b should be automatically called by the Renderer implementation via calling
			Renderer::alertViewportDimensionsChanged(), however, if for some unknown reason
			the renderer cannot be relied upon to make the call, the application can call
			this function to inform the System of the dimension change instead.
		*/
		void notifyViewportDimensionsChanged();


		//! Notify the System that the dimensions of the screen have changed.
		/*! This \b should be automatically called by the Renderer implementation via calling
			Renderer::alertScreenDimensionsChanged(), however, if for some unknown reason
			the renderer cannot be relied upon to make the call, the application can call
			this function to inform the System of the dimension change instead.
		*/
		void notifyScreenDimensionsChanged();

		//! Returns the current viewport resolution
		IVector2 getViewportResolution();
		//! Returns the current screen resolution
		IVector2 getScreenResolution();


		//! Returns the current FPS
		/*! The FPS value returned is an average over the last 5 frames. If less than
			5 frames have been rendered, then the average is based on as many frames
			as available. If no frames have been rendered, the returned value will be 0.0f.
			\note A "frame" is defined as a single call to System::renderGUI(). Since most
			applications will call this function once per scene frame, this is an adequate
			metric.
		*/
		float statRenderFPS();

	protected:
		
	private:
		// This is the actual constructor, the other 2 both call this one after getting the logs up and running.
		void doConstructor( Renderer* renderer, ResourceProvider* resourceProvider );

		//Generic
		

		//Statistics
		void _stat_UpdateFPS();


		//Logging Facilities
		LogManager* m_LogManager;
		LogListenerToFile* mDefaultLogListener;

		//Font Manager
		FontManager* mFontManager;

		//Plugin Manager
		PluginManager* m_PluginManager;


		//TimerManager
		TimerManager* mTimerManager;
		bool m_PerformAutoTicks;

		//Resource Provider Related Members
		ResourceProvider* mResourceProvider; //pointer to the resource provider
		bool mUsingGenericResourceProvider; //if we're using the generic resource provider, we are responsible for the delete

		//Renderer Related Members
		Renderer* mRenderer;
		IVector2 mScreenResolution; //resolution of the entire screen (used for auto font scaling)
		IVector2 mWindowResolution; //resolution of the viewport that we are rendering to (used for pixel alignment)

		//ImageryManager Related Members
		ImageryManager* mImageryManager; //root pointer to the created ImageryManager singleton.

	};
};

#endif