uniform sampler2D texture;
uniform lowp vec4 bkgnd_color;
uniform bool bkgnd_uniform;
varying mediump vec2 texc;
void main ()
{
  gl_FragColor = bkgnd_uniform ? bkgnd_color : texture2D(texture, texc.st);
}
