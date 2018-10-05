
// Passed from the fragment shader
varying highp vec3 vertex; // Vertex position from vertex shader
varying highp vec3 vertNormal; // Oriented normal from vertex shader

uniform highp vec3 cameraPos; // Camera position

uniform highp float shininess; // Size of specular Material shinisess (simulation relectance).
uniform highp vec3  specularColor; // The specular component is what makes a surface look shiny
uniform highp vec4  surfaceColor; // Color material

uniform int numLights; // Number of lights
uniform struct SLight
{
  highp vec4 position; // Homogenious coordinates of the light position
  highp vec3 intensities; //The color of the light
  highp float attenuation; // Light attenuation
  highp float ambientCoefficient; // Ambient coefficient
  highp float coneAngle; // Cone angle of point light
  highp vec3 coneDirection; // Cone direction of point light
} lights[10]; // Max lights 10.

uniform bool digit; // Digit currently drawn
uniform sampler2D texture; // Texture only if texActivated=true
uniform lowp int texActivated; // Texture is activated
varying mediump vec2 texc;// Texture coordinates only if texActivated!=0

// Derived from http://www.geeks3d.com/20100831/shader-library-noise-and-pseudo-random-number-generator-in-glsl/
highp float pseudoRand (highp float v)
{
  int n = int (v * 2147483647.0);
  n     = (n * 8192) + n;
  n     = min (n * (n * n * 15731 + 789221) + 1376312589, 2147483647);
  return float (n) / 2147483647.0;
}

// Compute the infuence of a ligth
highp vec3 applyLight (SLight light, highp vec3 surfaceColor, highp vec3 surfacePos, highp vec3 surfaceToCamera)
{
  highp float attenuation    = 1.0;
  highp vec3  surfaceToLight = normalize (light.position.xyz - surfacePos);

  // Diffuse coefficient is independant of normal for lit digits.
  highp float diffuseCoefficient = 1.0;
  if (digit)
  { // For digits change the light intensities to enforce red color component.
    // The surface color is perturbated randomly.
    highp float nl = abs (pseudoRand (dot (vertNormal, surfaceToLight)));
    surfaceColor   = vec3 (surfaceColor.r / nl, surfaceColor.g * nl, surfaceColor.b * nl);
  }
  else
  {
    diffuseCoefficient = max (0.0, dot (vertNormal, surfaceToLight));
  }

  if (light.position.w != 0.0)
  {
    // Point light
    highp float distanceToLight = length (light.position.xyz - surfacePos);
    attenuation                 = 1.0 / (1.0 + light.attenuation * distanceToLight * distanceToLight);

    // Cone restrictions (affects attenuation)
    highp float lightToSurfaceAngle = degrees (acos (dot (-surfaceToLight, normalize (light.coneDirection))));
    if (lightToSurfaceAngle > light.coneAngle)
    {
      attenuation = 0.0;
    }
  }

  // Ambient
  highp vec3 ambient = light.ambientCoefficient * surfaceColor.rgb * light.intensities;

  // Diffuse
  highp vec3 diffuse = diffuseCoefficient * surfaceColor.rgb * light.intensities;

  // Specular
  highp float specularCoefficient = 0.0;
  if (diffuseCoefficient > 0.0)
  {
    specularCoefficient = pow (max (0.0, dot (surfaceToCamera, reflect (-surfaceToLight, vertNormal))), shininess);
  }

  highp vec3 specular = specularCoefficient * specularColor * light.intensities;

  // Linear color (color before gamma correction)
  return ambient + attenuation * (diffuse + specular);
}

void main ()
{
  highp vec3 surfaceToCamera = normalize (cameraPos - vertex);
  highp vec4 color           = texActivated != 0 ? texture2D(texture, texc.st) : surfaceColor;

  // Combine color from all the lights
  highp vec3 linearColor = vec3(0);
  for (int i = 0; i < numLights; ++i)
  {
    linearColor += applyLight (lights[i], color.rgb, vertex, surfaceToCamera);
  }

  // Final color (after gamma correction)
  highp vec3 gamma   = vec3 (1.0 / 2.2);
  gl_FragColor = vec4 (pow (linearColor, gamma), surfaceColor.a);
}
