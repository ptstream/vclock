// Vertex shader for a rectangle with coordinates (-dx, -dy), (dx, dy) with 2 triangles
attribute highp vec4 vertex;
varying mediump vec2 texc;
void main ()
{
  gl_Position = vertex;
  texc.x = vertex.x < 0.0 ? 0.0 : 1.0;
  texc.y = vertex.y < 0.0 ? 0.0 : 1.0;
}
