// OpenGUI (http://opengui.sourceforge.net)
// This source code is released under the BSD License
// See LICENSE.TXT for details

#ifndef B955248E_7EDD_47CA_B588_BAA9C55E4380
#define B955248E_7EDD_47CA_B588_BAA9C55E4380

#include "OpenGUI_PreRequisites.h"
#include "OpenGUI_Exports.h"
#include "OpenGUI_String.h"
#include "OpenGUI_Types.h"
#include "OpenGUI_Event.h"
#include "OpenGUI_Imagery.h"
#include "OpenGUI_Face.h"
#include "OpenGUI_Font.h"
#include "OpenGUI_BrushModifier.h"
#include "OpenGUI_BrushModifierStack.h"
#include "OpenGUI_RenderOperation.h"
#include "OpenGUI_StrConv.h"

namespace OpenGUI {

	class Brush; //forward declaration

	//! Provides drawing methods for a few geometric primitives.
	class OPENGUI_API BrushPrimitive {
		friend class Brush;
	private:
		BrushPrimitive() {
			mParentBrush = 0;
		}
		void setBrush( Brush* brush ) {
			mParentBrush = brush;
		}
		Brush* mParentBrush;
	public:
		~BrushPrimitive() {}
		//! Draws a line between the given points at the given thickness.
		/*! \param start_point The beginning of the line
			\param end_point The end of the line
			\param thickness The thickness of the line (line width)
		*/
		void drawLine( const FVector2& start_point, const FVector2& end_point, float thickness );
		//! Draws a line between the given points at the given thickness.
		/*! \param start_point The beginning of the line
			\param end_point The end of the line
			\param thickness The thickness of the line in pixels (line width)
		*/
		void drawLine( const FVector2& start_point, const FVector2& end_point, int thickness = 1 );
		//! Draws a filled rect using the given \c rect for position as size
		void drawRect( const FRect& rect );
		//! Draws a rect outline using the given \c rect for position as size, and \c thickness for line width
		void drawOutlineRect( const FRect& rect, float thickness );
		//! Draws a rect outline using the given \c rect for position as size, and \c thickness for line width
		void drawOutlineRect( const FRect& rect, int thickness = 1 );
	};

	//! Provides methods for drawing Imagery covered rects
	class OPENGUI_API BrushImagery {
		friend class Brush;
	private:
		BrushImagery() {
			mParentBrush = 0;
		}
		void setBrush( Brush* brush ) {
			mParentBrush = brush;
		}
		Brush* mParentBrush;
	public:
		~BrushImagery() {}

		//! Draws the given Imagery using the given rect
		void drawImage( const ImageryPtr& imageryPtr, const FRect& rect );
		//! Draws the given Imagery at the given position and size
		void drawImage( const ImageryPtr& imageryPtr, const FVector2& position, const FVector2& size );
		//! Draws the given Imagery at the given position at it's native pixel size
		void drawImageUnscaled( const ImageryPtr& imageryPtr, const FVector2& position );
		//! Draws the given Imagery filling the given rect using the given number of tiles
		void drawImageTiled( const ImageryPtr& imageryPtr, const FRect& rect, float x_tiles, float y_tiles );
		//! Draws the given Imagery at the given position at it's native pixel size, tiling or clipping it as necessary to fill the area
		void drawImageUnscaledAndTiled( const ImageryPtr& imageryPtr, const FRect& rect );
		//! Draws the given Face, filling the given rect
		void drawFace( const FacePtr& facePtr, const FRect& rect );
		//! Draws the given Face, filling a rect constructed from the given position and size
		void drawFace( const FacePtr& facePtr, const FVector2& position, const FVector2& size );
	};

	//! Provides methods for rendering text
	class OPENGUI_API BrushText {
		friend class Brush;
	private:
		BrushText() {
			mParentBrush = 0;
		}
		void setBrush( Brush* brush ) {
			mParentBrush = brush;
		}
		Brush* mParentBrush;

		//! \internal Performs word wrapping on \a strList_in_out, assuming each character is \a charWidth and lines are allowed a maximum of \a wrapWidth
		void _WordWrapText( StringList& strList_in_out, unsigned int charWidth, unsigned int wrapWidth );

	public:
		~BrushText() {}

		//! draws the given string of \a text at the given \a position, using the given \a font, and optionally adjusting the glyph spacing by \a spacing_adjust
		void drawText( const String& text, const FVector2& position, Font& font, float spacing_adjust = 0.0f );

		//! draws the given string within the given rect, using the given font, while applying the given text alignments and performing any necessary word wrapping
		void drawTextArea( const String& text, const FRect& area, Font& font, bool wrap = false, const TextAlignment alignment = TextAlignment() );

		//! draws the given \a character at the current PenPosition using the given \a font.
		void drawCharacter( const Char character, Font& font );

		//! Converts the given \a pointSize to the individual X and Y pixel sizes according to PPU and UPI
		IVector2 pointsToPixels( float pointSize );

		//! The position that the next font glyph should be drawn to continue the last string of text.
		/*! PenPosition is used by drawCharacter to support typewriter-like text operations. */
		FVector2 PenPosition;
	};

	//! The base of all widget rendering operations
	/*! The Brush object is provided during widget onDraw events, and is used
		by the widget to draw itself. Functions that should use the Brush object
		will be provided one as a function parameter.


		\par Modifier Stack:
		Brushes implement stack modifiers that are applied to all output they generate.
		The available modifiers are color, clipping rects, output mask, position offsets,
		and rotations about the origin. These modifiers are \b not retroactive.
		For instance, changing the color modifier will not alter the color of operations
		you have already performed, it will only affect new operations.

		\par
		Each brush is created with a completely empty modifier stack. That doesn't mean
		that the brush you may have received in a draw function has an empty stack (as the
		caller will likely have initialized the brush with some stack pushes)

		\par
		The stack is always processed from top to bottom. (The last modifier you
		pushed is applied first, and the first modifier pushed is applied last.)
		Also, certain stack modifiers are "sticky". This means that the first one of
		these "sticky" modifiers evaluated will be the only modifier of that type that
		is evaluated. For instance, color is sticky. If you push the color Blue and then
		later push the color Red, the output will be Red (not purple) until the Red is
		popped, at which time the color will return to Blue. Conversely, the position
		modifier is additive. So multiple pushed position modifiers will result in an overall
		origin translation that equals their sum.

		\par
		Another important concept to understand is that stack operations are
		non-commutative. In other words, they affect each other in order dependent ways.
		What this means is that if you pushPosition and then pushRotation, the rotation
		will occur at the \b new origin, not the original origin. The same applies for
		clipping rects and masks. You can perform a rotation to an arbitrary angle, and then
		apply a clipping rect or mask, followed by the inverse of the previously used angle,
		and you will have created a non-axis aligned clipping rect or mask. Very powerful indeed.

		\par
		As much fun as these modifiers are, they come with a price tag. The more stack operations
		you add, the more operations must be applied to your output. We do our best to optimize
		the process, but nothing is perfect. Basically, if you're responsible you'll never run
		into performance issues.

		\par Position Modifier:
		 - Function: pushPosition()

		\par
		Position modifiers are offsets that will be applied (added) to each
		operation you perform. This is effectively like translating the origin to
		the given position.

		\par Rotation Modifier:
		- Function: pushRotation()

		\par
		Rotation modifiers are origin centered rotations that will be applied to each
		operation you perform.


		\par Color Modifier:
		- \b sticky
		- Function: pushColor()

		\par
		The last pushed color modifier will be used as the base color for all output.
		If no color has ever been pushed, the default color is Color(1.0, 1.0, 1.0, 1.0)

		\par Clipping Rect Modifier:
		- Function: pushClippingRect()

		\par
		Clipping rects will perform geometric clipping to the operations performed.
		Position and rotation modifiers can be combined with clipping rects to perform
		non-axis aligned clips.


		\par Mask Modifier:
		- \b sticky
		- Function: pushMask(), pushMaskUnscaled()

		\par
		Masks are imagery that are used to determine alpha visibility on a texel level.
		You can create alpha masks of any shape and apply them using this modifier.
	*/
	class OPENGUI_API Brush {
		//friend classes to access addRenderOperation
		friend class BrushText;
		friend class BrushPrimitive;
		friend class BrushImagery;
	public:
		//! Constructor
		Brush();
		//! Destructor
		virtual ~Brush();
		//! access to primitive drawing operations
		BrushPrimitive Primitive;
		//! access to imagery based drawing operations
		BrushImagery Image;
		//! access to text drawing operations
		BrushText Text;

		//! Pushes a position offset onto the stack
		void pushPosition( float x_offset, float y_offset );
		//! Pushes a position offset onto the stack
		void pushPosition( const FVector2& offset );
		//! Pushes a rotation offset onto the stack.
		/*! Positive values result in clockwise rotation. */
		void pushRotation( const Radian& angle );
		//! Pushes a new color onto the stack
		void pushColor( const Color& color );
		//! Pushes another layer of alpha transparency onto the stack
		void pushAlpha( float alpha );
		//! Pushes a new clipping rect onto the stack
		void pushClippingRect( const FRect& rect );
		//! Pushes an imagery based mask onto the stack
		void pushMask( ImageryPtr mask_imagery, const FRect& mask_rect );
		//! Pushes an imagery based mask onto the stack
		void pushMaskUnscaled( ImageryPtr mask_imagery, const FVector2& mask_position );

		//! Pushes a position offset onto the stack that aligns the origin with the nearest pixel of this Brush's output target
		/*! This is as close to pixel alignment as you can get. Things like rotation will cause inaccurate results,
		but in those cases pixel alignment is neither achievable nor necessary. */
		void pushPixelAlignment();

		//! Pops the last modifier off the stack
		void pop();

		//! \internal Pushes a marker onto the stack used to pop back to the same marker later.
		void _pushMarker( void* markerID );
		//! \internal Pops the stack until the marker with the given \c markerID is found.
		void _popMarker( void* markerID );

		//! Returns the raw (uncorrected) PPU (pixels per unit) of this Brush. \see Screen::getPPU()
		virtual const FVector2& getPPU_Raw() const = 0;
		//! Returns the raw (uncorrected) UPI (units per inch) of this Brush. \see Screen::getUPI()
		virtual const FVector2& getUPI_Raw() const = 0;
		//const IRect& getDrawArea_Pixels();

		//! returns the Brush's current total rotation
		const Radian& getRotation();
		//! Returns the PPU mapped against the Brush's current rotation.  \see Screen::getPPU()
		const FVector2& getPPU();
		//! Returns the UPI mapped against the Brush's current rotation.  \see Screen::getPPU()
		const FVector2& getUPI();

		//! returns true if the brush context is a render to texture surface
		virtual bool isRTTContext() const = 0;

		//! \internal Adds a raw render operation to the output
		void _addRenderOperation( RenderOperation& renderOp );

	protected:
		//! Final output RenderOperations are passed to this function.
		/*! It is up to specialized Brush implementations to capture final
		output with this function. At this point, the Brush is guaranteed
		to already to be active. */
		virtual void appendRenderOperation( RenderOperation& renderOp ) {
			/* This is overridden by more specific brush classes */
		}

		//! Called automatically when this Brush becomes the active Brush
		virtual void onActivate() = 0;

		//! Called when this Brush has been told to clear the contents of its render surface
		/*! The Brush is guaranteed to be active at this point */
		virtual void onClear() = 0;

		//! Marks this brush as the active brush, if it is not already
		void markActive();
		//! returns \c true if this is marked as the active brush
		bool isActive();

		//! Clears the contents of this Brush's output target
		/*! This is not something that most users would want to do. */
		void _clear();

	private:
		//! \internal Adds the given render operation to the brush's output.
		/*! This is used by the brush primitives to send rendering output.
		Any modifier stack operations will be automatically applied in
		this function.
		*/
		void addRenderOperation( RenderOperation& renderOp );

		BrushModifierStack mModifierStack;

		bool m_RotationCacheValid; // cache validity state var
		FVector2 m_PPUcache; // cache for rotated PPU
		FVector2 m_UPIcache; // cache for rotated UPI
		void _UpdateRotationCache(); // updates the rotation dependent cache objects

		static Brush* ActiveBrush;
	};


	//! Specialization of EventArgs for Draw events
	class OPENGUI_API Draw_EventArgs: public EventArgs {
	public:
		//! Constructor requires a valid Brush reference to contain
		Draw_EventArgs( Brush& brush ): brush( brush ) {}
		virtual ~Draw_EventArgs() {}
		//! Pointer to the parent being attached/detached to/from
		Brush& brush;
	};

} // namespace OpenGUI{

#endif // B955248E_7EDD_47CA_B588_BAA9C55E4380

