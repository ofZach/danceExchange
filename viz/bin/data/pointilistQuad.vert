uniform ivec2 tex0dim;
uniform ivec2 tex1dim;
uniform ivec2 tex2dim;
uniform ivec2 tex3dim;
uniform ivec2 tex4dim;
uniform ivec2 tex5dim;
uniform ivec2 tex6dim;
uniform ivec2 tex7dim;

vec2 uvs;

varying float alpha;
varying float texUnit;

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

void getUVs(int unit, int gridcell)
{
	if (unit == 0)
		modUVs(tex0dim.x, tex0dim.y, gridcell);
	else if (unit == 1)
		modUVs(tex1dim.x, tex1dim.y, gridcell);
	else if (unit == 2)
		modUVs(tex2dim.x, tex2dim.y, gridcell);
	else if (unit == 3)
		modUVs(tex3dim.x, tex3dim.y, gridcell);
	else if (unit == 4)
		modUVs(tex4dim.x, tex4dim.y, gridcell);
	else if (unit == 5)
		modUVs(tex5dim.x, tex5dim.y, gridcell);
	else if (unit == 6)
		modUVs(tex6dim.x, tex6dim.y, gridcell);
	else if (unit == 7)
		modUVs(tex7dim.x, tex7dim.y, gridcell);

}

void main()
{
	int unit = round( gl_MultiTexCoord0.z );
	uvs.xy = gl_MultiTexCoord0.xy;
	getUVs( unit, round( gl_MultiTexCoord0.w ) );
	
	alpha = gl_Vertex.w;
	texUnit = float(unit);
	
	gl_TexCoord[0].xy = uvs;
	
    vec4 wpos = vec4( gl_Vertex.xyz, 1.0 );
    vec4 epos = gl_ModelViewMatrix * wpos;
    gl_Position = gl_ProjectionMatrix * epos;
	
	
	
}
