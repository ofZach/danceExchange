uniform sampler2D tex0;
uniform sampler2D tex1;
uniform sampler2D tex2;
uniform sampler2D tex3;
uniform sampler2D tex4;
uniform sampler2D tex5;
uniform sampler2D tex6;
uniform sampler2D tex7;

varying float alpha;
varying float texUnit;


int round(float num)
{
	return int(sign(num)*floor(abs(num)+.5));
}

void main()
{
	vec4 alphaMult = vec4(1.0, 1.0, 1.0, alpha);
	int unit = round(texUnit);
	if ( unit == 0 )
		gl_FragColor = texture2D( tex0, gl_TexCoord[0].xy ) * alphaMult;
	else if ( unit == 1 )
		gl_FragColor = texture2D( tex1, gl_TexCoord[0].xy ) * alphaMult;
	else if ( unit == 2 )
		gl_FragColor = texture2D( tex2, gl_TexCoord[0].xy ) * alphaMult;
	else if ( unit == 3 )
		gl_FragColor = texture2D( tex3, gl_TexCoord[0].xy ) * alphaMult;
	else if ( unit == 4 )
		gl_FragColor = texture2D( tex4, gl_TexCoord[0].xy ) * alphaMult;
	else if ( unit == 5 )
		gl_FragColor = texture2D( tex5, gl_TexCoord[0].xy ) * alphaMult;
	else if ( unit == 6 )
		gl_FragColor = texture2D( tex6, gl_TexCoord[0].xy ) * alphaMult;
	else if ( unit == 7 )
		gl_FragColor = texture2D( tex7, gl_TexCoord[0].xy ) * alphaMult;

//	gl_FragColor = texture2D(tex0, gl_TexCoord[0].xy);
//	gl_FragColor = vec4( gl_TexCoord[0].x, 0.0, gl_TexCoord[0].y, 1.0 );
//	gl_FragColor = vec4( gl_TexCoord[0].x, 0.0, 0.0, 1.0 );
}