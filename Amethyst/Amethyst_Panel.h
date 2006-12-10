#ifndef Amethyst_Panel_h__
#define Amethyst_Panel_h__

#include "OpenGUI.h"
#include "Amethyst_Exports.h"

namespace OpenGUI {
	namespace Amethyst {

		//! This is a standard panel container
		class AMETHYST_API Panel: public ContainerControl {
		public:
			Panel();
			virtual ~Panel();

			//! Sets the background to draw for this panel
			void setFaceBG( FacePtr backgroundFace );
			//! Gets the background for this panel
			FacePtr getFaceBG() const;

			void setPaddingTop( float padding );
			float getPaddingTop() const;
			void setPaddingBottom( float padding );
			float getPaddingBottom() const;
			void setPaddingLeft( float padding );
			float getPaddingLeft() const;
			void setPaddingRight( float padding );
			float getPaddingRight() const;

			static Widget* createPanelFactory();
			virtual ObjectAccessorList* getAccessors();

		protected:
			virtual void onDrawBG( Object* sender, Draw_EventArgs& evtArgs );

			FacePtr mFaceBG;
		};

	} // namespace Amethyst{
} // namespace OpenGUI{

#endif // Amethyst_Panel_h__
