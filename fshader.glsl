#ifdef GL_ES
// Set default precision to medium
precision mediump int;
precision mediump float;
#endif

uniform sampler2D texture;
//uniform samplerCube tex;

varying vec2 vTexCoord;

void main()
{
    gl_FragColor = texture2D(texture, vTexCoord);
}

