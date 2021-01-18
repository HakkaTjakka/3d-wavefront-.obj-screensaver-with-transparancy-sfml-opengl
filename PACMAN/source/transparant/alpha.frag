//#version 330

uniform sampler2D texture;

void main(){
    vec4 color = texture2D( texture, gl_TexCoord[0].st );
//	color.rgb = color.rgb * color.w;
//    color.w *= 2.5;
//	if (color.a<1.0 && color.a>0.0) gl_FragColor.xyz=vec3(255,0,0);
//	if (color.a<1.0) gl_FragColor=vec4(0,0,0,0);
//    else gl_FragColor = color;
//	color.a=.5;
    gl_FragColor = color;
}
 