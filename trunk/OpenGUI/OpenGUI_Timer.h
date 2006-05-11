#ifndef E79A4AB7_6B23_4903_AB1E_FA5D21DF1AEA
#define E79A4AB7_6B23_4903_AB1E_FA5D21DF1AEA

#include "OpenGUI_PreRequisites.h"
#include "OpenGUI_Exports.h"
#include "OpenGUI_RefPtr.h"

namespace OpenGUI{

	//! Timers provide a simple method of retrieving time deltas from the TimerManager.
	class OPENGUI_API Timer{
	public:
		//! Re-Syncs this Timer's internal time point marker with the TimerManager's current time.
		void reset();
		//! Returns the number of milliseconds passed since object creation, or the last reset, whichever occurred last.
		/*! (Milliseconds are 1/1000th of a second) */
		unsigned int getMilliseconds();
	private:
		unsigned int m_TimePoint;
	};
	typedef RefPtr<Timer> TimerPtr;
};

#endif


