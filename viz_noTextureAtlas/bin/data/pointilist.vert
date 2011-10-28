varying vec4 tex0info;
varying vec3 tex1info;

varying float zDist;

uniform float viewportHeight;
uniform float fov;

#define PI 3.14159265

void main( void )
{
    vec4 wpos = vec4( gl_Vertex.xyz, 1.0 );
    vec4 epos = gl_ModelViewMatrix * wpos;
    gl_Position = gl_ProjectionMatrix * epos;
    gl_FrontColor = gl_Color;
    float dist = sqrt(dot(epos, epos));
    
	float cameraZ = ((viewportHeight/2.0) / tan(PI*fov/360.0));
	zDist = dist;
	
    gl_PointSize =  (1.0 / dist) * cameraZ * gl_Vertex.w;
    tex0info = gl_MultiTexCoord0;
    tex1info = gl_MultiTexCoord1.xyz;
}
