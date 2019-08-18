#ifdef GL_ES
// Set default precision to medium
precision mediump int;
precision mediump float;
#endif

uniform mat4 viewFrustrum;
uniform mat4 transform;

attribute vec3 aPosition;
attribute vec2 aTexCoord;

varying vec2 vTexCoord;

void main()
{
    // Calculate vertex position in screen space
    gl_Position = viewFrustrum * transform * vec4(aPosition, 1.0);

    vTexCoord = aTexCoord;
}
