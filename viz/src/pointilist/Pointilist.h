#ifndef _POINTILIST
#define _POINTILIST

#include "ofMain.h"
#include "ofShader.h"
#include "RadixSort.h"
#include "ofMatrix4x4.h"
#include <map>

struct TexInfo {
	int id;
	int cols;
	int rows;
};

class Pointilist
{
public:
	// variables
    
    bool                            useQuads;
	
	ofShader						shader;
	map<string, TexInfo*>			textureHash;
	TexInfo*						currentTextures[8];
	
	float							fov;
	float							viewportHeight;
	float							nearClip;
	float							farClip;
	float							nearFade;
    float                           aspectRatio;
	
	int								fovParam;
	int								viewportHeightParam;
	int								nearClipParam;
	int								farClipParam;
	int								nearFadeParam;
	
	int								tex0dimParam;
	int								tex1dimParam;
	int								tex2dimParam;
	int								tex3dimParam;
	int								tex4dimParam;
	int								tex5dimParam;
	int								tex6dimParam;
	int								tex7dimParam;
	
	int								maxPoints;
	int								numPoints;
	
	int*							orderedResult;
	
	float*							vertexBuffer;
	float*							sortedVertexBuffer;
	float*							colorBuffer;
	float*							sortedColorBuffer;
	float*							tex0infoBuffer;
	float*							sortedTex0infoBuffer;
	float*							tex1infoBuffer;
	float*							sortedTex1infoBuffer;
	
	double							modelview[16];
	double							projection[16];
	int								viewport[4];
	
	// functions
	void init(int maxPoints = 1000, bool useQuads = true);
	void draw();
	void bindTextures();
	
	void setTexture(string name);
	void setTexture(string name, int unit);
	
	void loadTexture(string name, unsigned char * data, int w, int h);
	void loadTexture(string name, unsigned char * data, int w, int h, int cols, int rows);
    
    void addTexture( string name, int textureID, int cols, int rows );
	
	void addPoint(float x, float y, float z);
	void addPoint(float x, float y, float z, float size);
	void addPoint(float x, float y, float z, float size, float r, float g, float b, float a);
	void addPoint(float x, float y, float z, float size, float r, float g, float b, float a, int t0unit, float t0rot, float t0cell);
	void addPoint(float x, float y, float z, float size, float r, float g, float b, float a, int t0unit, float t0rot, float t0cell, int t1unit, float t1rot, float t1cell, float lerp);
	
private:
	void initBuffers();
	void depthSort();
	void fakeSort();
};

#endif