attribute vec4 hvertex; // Homogenious vertex homogenious coordinates
attribute vec3 normal; // Normal coordinates

uniform mat4 projMatrix; // Projection matrix
uniform mat4 mvMatrix; // Model view matrix
uniform mat3 normalMatrix; // Model view orientation
uniform lowp int texActivated; // Texture is activated

// Pass some variables to the fragment shader
varying vec3 vertNormal; // Oriented normal
varying vec3 vertex; // Vertex position
varying mediump vec2 texc; // Texture coordinate (only if texActivated!=0)

void main ()
{
  // Pass values to the fragment shader
  vertex     = hvertex.xyz; // Assum hvertex.w = 1.
  vertNormal = normalMatrix * normal; // Orients the normal. To speedup fragment
  if (texActivated == 1)
  {
    texc.x = vertex.x < 0.0 ? 0.0 : 1.0;
    texc.y = vertex.y < 0.0 ? 0.0 : 1.0;
  }
  else if (texActivated == 2)
  {
    texc.x = vertex.x < 0.0 ? 0.0 : 1.0;
    texc.y = vertex.z < 0.0 ? 0.0 : 1.0;
  }
  else if (texActivated == 3)
  {
    texc.x = vertex.x < 0.0 ? 0.0 : 1.0;
    texc.y = vertex.y < 0.0 ? 0.0 : 1.0;
  }

  // Apply matrix transformations to vertex
  gl_Position = projMatrix * mvMatrix * hvertex; // Orients and projects the vertex
}
