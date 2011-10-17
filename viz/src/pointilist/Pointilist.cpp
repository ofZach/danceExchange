#include "Pointilist.h"

void Pointilist::draw()
{
	depthSort();
    
    if ( useQuads ) {
        shader.begin();
		bindTextures();
		glEnableClientState( GL_VERTEX_ARRAY );
		glVertexPointer( 4, GL_FLOAT, 0, sortedVertexBuffer );
		
		glClientActiveTexture(GL_TEXTURE0);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glTexCoordPointer(4, GL_FLOAT, 0, sortedTex0infoBuffer);
		
		glDrawArrays( GL_QUADS, 0, numPoints * 4);
		
		glClientActiveTexture(GL_TEXTURE0);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		glDisableClientState(GL_VERTEX_ARRAY);
        shader.end();
    }
	else {
        
        glActiveTexture(GL_TEXTURE0);
        glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
        glEnable(GL_POINT_SPRITE);
        glTexEnvi(GL_POINT_SPRITE, GL_COORD_REPLACE, GL_TRUE);
        
        shader.begin();
        bindTextures();
        
        shader.setUniform1f( "fov", fov );
        shader.setUniform1f( "viewportHeight", viewportHeight );
        shader.setUniform1f( "aspectRatio", aspectRatio );
        
        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(4, GL_FLOAT, 0, sortedVertexBuffer);
        
        glEnableClientState(GL_COLOR_ARRAY);
        glColorPointer(4, GL_FLOAT, 0, sortedColorBuffer);
        
        glClientActiveTexture(GL_TEXTURE0);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glTexCoordPointer(4, GL_FLOAT, 0, sortedTex0infoBuffer);
        
        glClientActiveTexture(GL_TEXTURE1);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glTexCoordPointer(3, GL_FLOAT, 0, sortedTex1infoBuffer);
        
        glDrawArrays(GL_POINTS,	0, numPoints);
        
        glClientActiveTexture(GL_TEXTURE1);
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        glClientActiveTexture(GL_TEXTURE0);
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        
        glDisableClientState(GL_VERTEX_ARRAY);
        glDisableClientState(GL_COLOR_ARRAY);
        
        shader.end();
    }
	
	numPoints = 0;
	glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE1); glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE2); glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE3); glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE4); glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE5); glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE6); glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE7); glBindTexture(GL_TEXTURE_2D, 0);
	
	glActiveTexture(GL_TEXTURE0);
}

void Pointilist::depthSort()
{
	glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
	glGetDoublev(GL_PROJECTION_MATRIX, projection);
	glGetIntegerv(GL_VIEWPORT, (GLint*)viewport);
	
    ofMatrix4x4 transform;
    ofVec3f pos;
	transform.set(modelview);
	
	double winX, winY, winZ;
	float screenZ[numPoints];
	//int orderedResult[numPoints];
	for (int i=0; i<numPoints; i++)
		orderedResult[i] = i;
	
	for (int i=0; i<numPoints; i++)
	{
		double screenPos[3];
		gluProject(vertexBuffer[i*4+0], vertexBuffer[i*4+1], vertexBuffer[i*4+2], modelview, projection, (GLint*)viewport, &winX, &winY, &winZ);
		screenZ[i] = 1 - winZ;
	}
	
	RadixSort* sorter = new RadixSort(numPoints);
	sorter->sort(screenZ, numPoints, orderedResult);
	
	for (int i=0; i<numPoints; i++)
	{
		int index = orderedResult[i];
        if ( useQuads ) {
			// make a quad here, alpha is stored in the w component
			float halfHeight = vertexBuffer[ index * 4 + 3 ] / 2.0f;
			float halfWidth = halfHeight / aspectRatio;
			// first vert (upper left)
			sortedVertexBuffer[i * 16 + 0] = vertexBuffer[index * 4 + 0] - halfWidth;
			sortedVertexBuffer[i * 16 + 1] = vertexBuffer[index * 4 + 1] - halfHeight;
			sortedVertexBuffer[i * 16 + 2] = vertexBuffer[index * 4 + 2];
			sortedVertexBuffer[i * 16 + 3] = colorBuffer[ index * 4 + 3 ];
			sortedTex0infoBuffer[i * 16 + 0] = 0;	// U
			sortedTex0infoBuffer[i * 16 + 1] = 0;	// V
			sortedTex0infoBuffer[i * 16 + 2] = tex0infoBuffer[index * 4 + 0];	// Texture Number
			sortedTex0infoBuffer[i * 16 + 3] = tex0infoBuffer[index * 4 + 2];	// Grid Cell
			// second vert (upper right)
			sortedVertexBuffer[i * 16 + 4] = vertexBuffer[index * 4 + 0] + halfWidth;
			sortedVertexBuffer[i * 16 + 5] = vertexBuffer[index * 4 + 1] - halfHeight;
			sortedVertexBuffer[i * 16 + 6] = vertexBuffer[index * 4 + 2];
			sortedVertexBuffer[i * 16 + 7] = colorBuffer[ index * 4 + 3 ];
			sortedTex0infoBuffer[i * 16 + 4] = 1;	// U
			sortedTex0infoBuffer[i * 16 + 5] = 0;	// V
			sortedTex0infoBuffer[i * 16 + 6] = tex0infoBuffer[index * 4 + 0];	// Texture Number
			sortedTex0infoBuffer[i * 16 + 7] = tex0infoBuffer[index * 4 + 2];	// Grid Cell
			// third vert (lower right)
			sortedVertexBuffer[i * 16 + 8] = vertexBuffer[index * 4 + 0] + halfWidth;
			sortedVertexBuffer[i * 16 + 9] = vertexBuffer[index * 4 + 1] + halfHeight;
			sortedVertexBuffer[i * 16 + 10] = vertexBuffer[index * 4 + 2];
			sortedVertexBuffer[i * 16 + 11] = colorBuffer[ index * 4 + 3 ];
			sortedTex0infoBuffer[i * 16 + 8] = 1;	// U
			sortedTex0infoBuffer[i * 16 + 9] = 1;	// V
			sortedTex0infoBuffer[i * 16 + 10] = tex0infoBuffer[index * 4 + 0];	// Texture Number
			sortedTex0infoBuffer[i * 16 + 11] = tex0infoBuffer[index * 4 + 2];	// Grid Cell
			// fourth vert ( lower left)
			sortedVertexBuffer[i * 16 + 12] = vertexBuffer[index * 4 + 0] - halfWidth;
			sortedVertexBuffer[i * 16 + 13] = vertexBuffer[index * 4 + 1] + halfHeight;
			sortedVertexBuffer[i * 16 + 14] = vertexBuffer[index * 4 + 2];
			sortedVertexBuffer[i * 16 + 15] = colorBuffer[ index * 4 + 3 ];
			sortedTex0infoBuffer[i * 16 + 12] = 0;	// U
			sortedTex0infoBuffer[i * 16 + 13] = 1;	// V
			sortedTex0infoBuffer[i * 16 + 14] = tex0infoBuffer[index * 4 + 0];	// Texture Number
			sortedTex0infoBuffer[i * 16 + 15] = tex0infoBuffer[index * 4 + 2];	// Grid Cell
            
        }
        else {
            sortedVertexBuffer[i * 4 + 0] = vertexBuffer[index * 4 + 0];
            sortedVertexBuffer[i * 4 + 1] = vertexBuffer[index * 4 + 1];
            sortedVertexBuffer[i * 4 + 2] = vertexBuffer[index * 4 + 2];
            sortedVertexBuffer[i * 4 + 3] = vertexBuffer[index * 4 + 3];
            sortedColorBuffer[i * 4 + 0] = colorBuffer[index * 4 + 0];
            sortedColorBuffer[i * 4 + 1] = colorBuffer[index * 4 + 1];
            sortedColorBuffer[i * 4 + 2] = colorBuffer[index * 4 + 2];
            sortedColorBuffer[i * 4 + 3] = colorBuffer[index * 4 + 3];
            
            sortedTex0infoBuffer[i * 4 + 0] = tex0infoBuffer[index * 4 + 0];
            sortedTex0infoBuffer[i * 4 + 1] = tex0infoBuffer[index * 4 + 1];
            sortedTex0infoBuffer[i * 4 + 2] = tex0infoBuffer[index * 4 + 2];
            sortedTex0infoBuffer[i * 4 + 3] = tex0infoBuffer[index * 4 + 3];
            
            sortedTex1infoBuffer[i * 3 + 0] = tex1infoBuffer[index * 3 + 0];
            sortedTex1infoBuffer[i * 3 + 1] = tex1infoBuffer[index * 3 + 1];
            sortedTex1infoBuffer[i * 3 + 2] = tex1infoBuffer[index * 3 + 2];
        }
	}
	
	delete sorter;
	return;
}

void Pointilist::fakeSort()
{
	for (int i=0; i<numPoints; i++)
	{
		int index = i;
		sortedVertexBuffer[i * 4 + 0] = vertexBuffer[index * 4 + 0];
		sortedVertexBuffer[i * 4 + 1] = vertexBuffer[index * 4 + 1];
		sortedVertexBuffer[i * 4 + 2] = vertexBuffer[index * 4 + 2];
		sortedVertexBuffer[i * 4 + 3] = vertexBuffer[index * 4 + 3];
		sortedColorBuffer[i * 4 + 0] = colorBuffer[index * 4 + 0];
		sortedColorBuffer[i * 4 + 1] = colorBuffer[index * 4 + 1];
		sortedColorBuffer[i * 4 + 2] = colorBuffer[index * 4 + 2];
		sortedColorBuffer[i * 4 + 3] = colorBuffer[index * 4 + 3];
		
		sortedTex0infoBuffer[i * 4 + 0] = tex0infoBuffer[index * 4 + 0];
		sortedTex0infoBuffer[i * 4 + 1] = tex0infoBuffer[index * 4 + 1];
		sortedTex0infoBuffer[i * 4 + 2] = tex0infoBuffer[index * 4 + 2];
		sortedTex0infoBuffer[i * 4 + 3] = tex0infoBuffer[index * 4 + 3];
		
		sortedTex1infoBuffer[i * 3 + 0] = tex0infoBuffer[index * 3 + 0];
		sortedTex1infoBuffer[i * 3 + 1] = tex0infoBuffer[index * 3 + 1];
		sortedTex1infoBuffer[i * 3 + 2] = tex0infoBuffer[index * 3 + 2];
	}
}

void Pointilist::bindTextures()
{
	for (int i=0; i<8; i++)
	{
		if (currentTextures[i] == 0)
			continue;
		
		if (i == 0)
		{
			glActiveTexture(GL_TEXTURE0);
			glUniform2iARB(tex0dimParam, currentTextures[i]->cols, currentTextures[i]->rows);
		}
		else if (i == 1)
		{
			glActiveTexture(GL_TEXTURE1);
			glUniform2iARB(tex1dimParam, currentTextures[i]->cols, currentTextures[i]->rows);
		}
		else if (i == 2)
		{
			glActiveTexture(GL_TEXTURE2);
			glUniform2iARB(tex2dimParam, currentTextures[i]->cols, currentTextures[i]->rows);
		}
		else if (i == 3)
		{
			glActiveTexture(GL_TEXTURE3);
			glUniform2iARB(tex3dimParam, currentTextures[i]->cols, currentTextures[i]->rows);
		}
		else if (i == 4)
		{
			glActiveTexture(GL_TEXTURE4);
			glUniform2iARB(tex4dimParam, currentTextures[i]->cols, currentTextures[i]->rows);
		}
		else if (i == 5)
		{
			glActiveTexture(GL_TEXTURE5);
			glUniform2iARB(tex5dimParam, currentTextures[i]->cols, currentTextures[i]->rows);
		}
		else if (i == 6)
		{
			glActiveTexture(GL_TEXTURE6);
			glUniform2iARB(tex6dimParam, currentTextures[i]->cols, currentTextures[i]->rows);
		}
		else if (i == 7)
		{
			glActiveTexture(GL_TEXTURE7);
			glUniform2iARB(tex7dimParam, currentTextures[i]->cols, currentTextures[i]->rows);
		}
		
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, currentTextures[i]->id);
	}
}

void Pointilist::addTexture( string name, int textureID, int cols, int rows ) {
    TexInfo* ti = new TexInfo();
    ti->cols = cols;
    ti->rows = rows;
    ti->id = textureID;
//    cout << "adding texture named " << name << " with textureID of " << textureID << endl;
    textureHash[name] = ti;
}

void Pointilist::setTexture(string name)
{
	setTexture(name, 0);
}

void Pointilist::setTexture(string name, int unit)
{
	if (unit < 0 || unit > 7)
		return;
	
	TexInfo* ti = textureHash[name];
//	printf("TexInfo - name: %s\tcols: %i\trows: %i\tid: %i\n", name.c_str(), ti->cols, ti->rows, ti->id);
	currentTextures[unit] = ti;
}

void Pointilist::loadTexture(string name, unsigned char * data, int w, int h)
{
	loadTexture(name, data, w, h, 1, 1);
}

void Pointilist::loadTexture(string name, unsigned char * data, int w, int h, int cols, int rows)
{
	int tex[1];
	
	glGenTextures(1, (GLuint *)tex);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, tex[0]);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, w, h, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glBindTexture(GL_TEXTURE_2D, 0);
	
	TexInfo* ti = new TexInfo();
	ti->id = tex[0];
	ti->cols = cols;
	ti->rows = rows;
	
	textureHash[name] = ti;
}

void Pointilist::addPoint(float x, float y, float z)
{
	addPoint(x, y, z, 1.0, 1.0, 1.0, 1.0, 1.0, 0, 0, 0);
}

void Pointilist::addPoint(float x, float y, float z, float size)
{
	addPoint(x, y, z, size, 1.0, 1.0, 1.0, 1.0, 0, 0, 0);
}

void Pointilist::addPoint(float x, float y, float z, float size,
						  float r, float g, float b, float a)
{
	addPoint(x, y, z, size, r, g, b, a, 0, 0, 0);
}

void Pointilist::addPoint(float x, float y, float z, float size,
						  float r, float g, float b, float a,
						  int t0unit, float t0rot, float t0cell)
{
	if (numPoints==maxPoints)
		return;
	
	vertexBuffer[numPoints * 4 + 0] = x;
	vertexBuffer[numPoints * 4 + 1] = y;
	vertexBuffer[numPoints * 4 + 2] = z;
	vertexBuffer[numPoints * 4 + 3] = size;
	
	colorBuffer[numPoints * 4 + 0] = r;
	colorBuffer[numPoints * 4 + 1] = g;
	colorBuffer[numPoints * 4 + 2] = b;
	colorBuffer[numPoints * 4 + 3] = a;
	
	tex0infoBuffer[numPoints * 4 + 0] = t0unit;
	tex0infoBuffer[numPoints * 4 + 1] = t0rot;
	tex0infoBuffer[numPoints * 4 + 2] = t0cell;
	tex0infoBuffer[numPoints * 4 + 3] = 0;
	
	tex1infoBuffer[numPoints * 3 + 0] = -1;
	tex1infoBuffer[numPoints * 3 + 1] = 0;
	tex1infoBuffer[numPoints * 3 + 2] = 0;
	
	numPoints++;
	
}

void Pointilist::addPoint(float x, float y, float z, float size,
						  float r, float g, float b, float a,
						  int t0unit, float t0rot, float t0cell,
						  int t1unit, float t1rot, float t1cell, float lerp)
{
	if (numPoints==maxPoints)
		return;
	
	vertexBuffer[numPoints * 4 + 0] = x;
	vertexBuffer[numPoints * 4 + 1] = y;
	vertexBuffer[numPoints * 4 + 2] = z;
	vertexBuffer[numPoints * 4 + 3] = size;
	
	colorBuffer[numPoints * 4 + 0] = r;
	colorBuffer[numPoints * 4 + 1] = g;
	colorBuffer[numPoints * 4 + 2] = b;
	colorBuffer[numPoints * 4 + 3] = a;
	
	tex0infoBuffer[numPoints * 4 + 0] = t0unit;
	tex0infoBuffer[numPoints * 4 + 1] = t0rot;
	tex0infoBuffer[numPoints * 4 + 2] = t0cell;
	tex0infoBuffer[numPoints * 4 + 3] = lerp;
	
	tex1infoBuffer[numPoints * 3 + 0] = t1unit;
	tex1infoBuffer[numPoints * 3 + 1] = t1rot;
	tex1infoBuffer[numPoints * 3 + 2] = t1cell;
	
	numPoints++;
}

void Pointilist::init(int maxPoints, bool useQuads)
{
    this->useQuads      = useQuads;
    
	this->maxPoints		= maxPoints;
	this->numPoints		= 0;
	
	for (int i=0; i<8; i++)
		currentTextures[i] = 0;
	
	fov					= 60.0;
	viewportHeight		= ofGetHeight(); 
    aspectRatio         = 0.75;
	
	orderedResult = new int[maxPoints];
	
    if ( useQuads )
        shader.load( "pointilistQuad" );
    else
        shader.load( "pointilist" );
	
    int programObject = shader.getProgram();
	
    shader.begin();
	
	for (int i=0; i<8; i++)
	{
		string st = "tex" + ofToString(i);
//		printf("tex%i location: %i\n", i, glGetUniformLocation(programObject, st.c_str()) );
	}
	
	glUniform1iARB(glGetUniformLocation(programObject, "tex0"), 0);
	glUniform1iARB(glGetUniformLocation(programObject, "tex1"), 1);
	glUniform1iARB(glGetUniformLocation(programObject, "tex2"), 2);
	glUniform1iARB(glGetUniformLocation(programObject, "tex3"), 3);
	glUniform1iARB(glGetUniformLocation(programObject, "tex4"), 4);
	glUniform1iARB(glGetUniformLocation(programObject, "tex5"), 5);
	glUniform1iARB(glGetUniformLocation(programObject, "tex6"), 6);
	glUniform1iARB(glGetUniformLocation(programObject, "tex7"), 7);
	
	tex0dimParam		= glGetUniformLocation(programObject, "tex0dim");
	tex1dimParam		= glGetUniformLocation(programObject, "tex1dim");
	tex2dimParam		= glGetUniformLocation(programObject, "tex2dim");
	tex3dimParam		= glGetUniformLocation(programObject, "tex3dim");
	tex4dimParam		= glGetUniformLocation(programObject, "tex4dim");
	tex5dimParam		= glGetUniformLocation(programObject, "tex5dim");
	tex6dimParam		= glGetUniformLocation(programObject, "tex6dim");
	tex7dimParam		= glGetUniformLocation(programObject, "tex7dim");
	
	fovParam			= glGetUniformLocation(programObject, "fov");
	viewportHeightParam	= glGetUniformLocation(programObject, "viewportHeight");
	nearClipParam		= glGetUniformLocation(programObject, "nearClip");
	farClipParam		= glGetUniformLocation(programObject, "farClip");
	nearFadeParam		= glGetUniformLocation(programObject, "nearFade");
	
    shader.end();
	
	initBuffers();
}

void Pointilist::initBuffers()
{
	vertexBuffer			= new float[maxPoints * 4];
	sortedVertexBuffer		= new float[maxPoints * 4 * (useQuads?4:1)];
	colorBuffer				= new float[maxPoints * 4];
	sortedColorBuffer		= new float[maxPoints * 4];
	tex0infoBuffer			= new float[maxPoints * 4];
	sortedTex0infoBuffer	= new float[maxPoints * 4 * (useQuads?4:1)];
	tex1infoBuffer			= new float[maxPoints * 3];
	sortedTex1infoBuffer	= new float[maxPoints * 3];
}