#include "Renderer_OpenGL.h"
#include "OpenGUI_OGLTexture.h"

#include "OpenGUI_FBO.h"

namespace OpenGUI {
	//###########################################################
	OGLTexture::OGLTexture() {
		textureId = 0;
	}
	//###########################################################
	OGLTexture::~OGLTexture() {
		/**/
	}
	//###########################################################


	//###########################################################
	//###########################################################
	//###########################################################


	//###########################################################
	OGLRTexture::OGLRTexture() {
		textureId = 0;
		fboId = 0;
		glGenFramebuffersEXT( 1, &fboId );
	}
	//###########################################################
	OGLRTexture::~OGLRTexture() {
		/**/
		glDeleteFramebuffersEXT( 1, &fboId );
	}
	//###########################################################
}
;//namespace OpenGUI{