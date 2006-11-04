#ifndef C09C874C_0732_4524_A2C7_DC3D72E72659
#define C09C874C_0732_4524_A2C7_DC3D72E72659

#include "OpenGUI_PreRequisites.h"
#include "OpenGUI_Exports.h"
#include "OpenGUI_Types.h"
#include "OpenGUI_Brush.h"

namespace OpenGUI {

	class Screen; // forward declaration

	//! Interface class for Brushes that cache their output in some manner
	class OPENGUI_API Brush_Memory: public Brush {
	public:
		//! Constructor requires a pointer to the Screen that this Brush will eventually be drawn to
		Brush_Memory( Screen* parentScreen );
		//! Destructor
		virtual ~Brush_Memory();

		//! sends the contents of this caching Brush into the output stream of the given Brush
		void emerge( Brush& targetBrush );

		virtual const FVector2& getPPU_Raw() const;
		virtual const FVector2& getUPI_Raw() const;
		virtual bool isRTTContext() const {
			return false;
		}
	protected:
		virtual void appendRenderOperation( RenderOperation &renderOp );
		virtual void onActivate();
		virtual void onClear();
	private:
		Screen* mScreen;
		RenderOperationList mRopList;
	};
} // namespace OpenGUI{

#endif // C09C874C_0732_4524_A2C7_DC3D72E72659
