#define USE_8_TEXTURE_UNITS

uniform sampler2D tex0;
uniform sampler2D tex1;
uniform sampler2D tex2;
uniform sampler2D tex3;

uniform ivec2 tex0dim;
uniform ivec2 tex1dim;
uniform ivec2 tex2dim;
uniform ivec2 tex3dim;

//#ifdef USE_8_TEXTURE_UNITS
uniform sampler2D tex4;
uniform sampler2D tex5;
uniform sampler2D tex6;
uniform sampler2D tex7;
uniform ivec2 tex4dim;
uniform ivec2 tex5dim;
uniform ivec2 tex6dim;
uniform ivec2 tex7dim;
//#endif

uniform float nearClip;
uniform float farClip;
uniform float nearFade;

varying vec4 texLevels;
varying vec4 tex0info;
varying vec3 tex1info;

varying float zDist;

vec2 uvs;


void getUVs(float angle)
{
	if (angle == 0.0)
		uvs = gl_TexCoord[0].xy;
	else
	{
		angle += 1.570796326794897;
		float ca = cos(angle);
		float sa = sin(angle);
		uvs = vec2( ( ( (gl_TexCoord[0].x-.5) * sa ) - ( (gl_TexCoord[0].y-.5) * ca ) ) + .5,
					 ( ( (gl_TexCoord[0].x-.5) * ca ) + ( (gl_TexCoord[0].y-.5) * sa ) ) + .5);
	}
}

void modUVs(int cols, int rows, int cell)
{
	if ( !(cols==1 && rows == 1) )
	{
		int numCells = cols * rows;
		cell -= (cell/numCells) * numCells;
		
		uvs /= vec2( float(cols), float(rows) );
		uvs.x += fract( float(cell) / float(cols));
		uvs.y += float(cell / cols) / float(rows);
	}
	
}

int round(float num)
{
	return int(sign(num)*floor(abs(num)+.5));
}

vec4 sampleFromUnit(int unit, int gridcell, float rotation)
{
	if (unit == 0)
	{
		getUVs(rotation);
		modUVs(tex0dim.x, tex0dim.y, gridcell);
		return texture2D(tex0, uvs.xy);
	}
	else if (unit == 1)
	{
		getUVs(rotation);
		modUVs(tex1dim.x, tex1dim.y, gridcell);
		return texture2D(tex1, uvs.xy);
	}
	else if (unit == 2)
	{
		getUVs(rotation);
		modUVs(tex2dim.x, tex2dim.y, gridcell);
		return texture2D(tex2, uvs.xy);
	}
	else if (unit == 3)
	{
		getUVs(rotation);
		modUVs(tex3dim.x, tex3dim.y, gridcell);
		return texture2D(tex3, uvs.xy);
	}
//#ifdef USE_8_TEXTURE_UNITS
	else if (unit == 4)
	{
		getUVs(rotation);
		modUVs(tex4dim.x, tex4dim.y, gridcell);
		//return vec4(1,0,0,.5);
		return texture2D(tex4, uvs.xy);	
	}
	else if (unit == 5)
	{
		getUVs(rotation);
		modUVs(tex5dim.x, tex5dim.y, gridcell);
		return texture2D(tex5, uvs.xy);	
	}
	else if (unit == 6)
	{
		getUVs(rotation);
		modUVs(tex6dim.x, tex6dim.y, gridcell);
		return texture2D(tex6, uvs.xy);	
	}
	else if (unit == 7)
	{
		getUVs(rotation);
		modUVs(tex7dim.x, tex7dim.y, gridcell);
		return texture2D(tex7, uvs.xy);	
	}
//#endif
	else
		return vec4(0,0,0,0);	
}
float map(float value, float istart, float istop, float ostart, float ostop)
{
    return ostart + (ostop - ostart) * ((value - istart) / (istop - istart));
}

void main(void)
{   
    int unit;
    
    vec4 colorOne;
    vec4 colorTwo = vec4(0,0,0,0);

    unit = round(tex0info.x);    	
    colorOne = sampleFromUnit(unit, round(tex0info.z), tex0info.y);
    
    // if the crossfade between the two textures is greater than 0, we sample from the other texture
	if (tex0info.w > 0.0)
	{
		unit = round(tex1info.x);
		colorTwo = sampleFromUnit(unit, round(tex1info.z), tex1info.z);
	}
    
    vec4 finalColor = mix(colorOne, colorTwo, tex0info.w) * gl_Color;
//    float alphaMultiplier = clamp(map(zDist, nearClip, nearClip + nearFade, 0.0, 1.0),0.0,1.0);
//    finalColor.a *= alphaMultiplier;
    
    gl_FragColor = finalColor;
}