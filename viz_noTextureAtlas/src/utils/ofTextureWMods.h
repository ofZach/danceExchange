

#pragma once

#include "ofTexture.h"

//static void retain(GLuint id);	// we should make this public in OF

class ofTextureWMods : public ofTexture {

	public: 
		
	void allocate(int w, int h, int internalGlDataType, bool bUseARBExtention){
		//our graphics card might not support arb so we have to see if it is supported.
//#ifndef TARGET_OPENGLES
//		if (bUseARBExtention && GL_ARB_texture_rectangle){
//			texData.tex_w = w;
//			texData.tex_h = h;
//			texData.tex_t = w;
//			texData.tex_u = h;
//			texData.textureTarget = GL_TEXTURE_RECTANGLE_ARB;
//		} else 
//#endif
		{
			//otherwise we need to calculate the next power of 2 for the requested dimensions
			//ie (320x240) becomes (512x256)
			texData.tex_w = w; //ofNextPow2(w);
			texData.tex_h = h; //ofNextPow2(h);
			texData.tex_t = 1.0f;
			texData.tex_u = 1.0f;
			texData.textureTarget = GL_TEXTURE_2D;
		}
		
		texData.glTypeInternal = internalGlDataType;
		// get the glType (format) and pixelType (type) corresponding to the glTypeInteral (internalFormat)
		ofGetGlFormatAndType(texData.glTypeInternal, texData.glType, texData.pixelType);
		
		// attempt to free the previous bound texture, if we can:
		clear();
		
		glGenTextures(1, (GLuint *)&texData.textureID);   // could be more then one, but for now, just one
		retain(texData.textureID);
		
		glEnable(texData.textureTarget);
		
		glBindTexture(texData.textureTarget, (GLuint)texData.textureID);
#ifndef TARGET_OPENGLES
		// can't do this on OpenGL ES: on full-blown OpenGL, 
		// glInternalFormat and glFormat (GL_LUMINANCE below)
		// can be different; on ES they must be exactly the same.
		//		glTexImage2D(texData.textureTarget, 0, texData.glTypeInternal, (GLint)texData.tex_w, (GLint)texData.tex_h, 0, GL_LUMINANCE, PIXEL_TYPE, 0);  // init to black...
		glTexImage2D(texData.textureTarget, 0, texData.glTypeInternal, (GLint)texData.tex_w, (GLint)texData.tex_h, 0, texData.glType, texData.pixelType, 0);  // init to black...
#else
		glTexImage2D(texData.textureTarget, 0, texData.glTypeInternal, texData.tex_w, texData.tex_h, 0, texData.glTypeInternal, texData.pixelType, 0);
#endif
		
		
		glTexParameterf(texData.textureTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(texData.textureTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameterf(texData.textureTarget, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameterf(texData.textureTarget, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		
		glDisable(texData.textureTarget);
		
		texData.width = w;
		texData.height = h;
		texData.bFlipTexture = false;
		texData.bAllocated = true;
	}
	
};
