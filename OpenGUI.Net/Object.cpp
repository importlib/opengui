// OpenGUI (http://opengui.sourceforge.net)
// This source code is release under the BSD License
// See LICENSE.TXT for details

#include "Stdafx.h"

#include "Object.h"
#include "ObjectHandle.h"
#include "HandleSystem.h"

namespace OpenGUI_Net {

	//############################################################################
	Object::Object() {
		/**/
	}
	//############################################################################
	Object::~Object() {
		/**/
	}
	//############################################################################
	void Object::_setHandle( ObjectHandle* handle ) {
		mNativeHandle = handle;
	}
	//############################################################################
	ObjectHandle* Object::getHandle() {
		return mNativeHandle;
	}
	//############################################################################

} // namespace OpenGUI_Net {
