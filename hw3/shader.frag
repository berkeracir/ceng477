#version 410

// Output Color
out vec4 color;

uniform mat4 MVP; // ModelViewProjection Matrix
uniform mat4 MV; // ModelView idMVPMatrix
uniform vec4 cameraPosition;

uniform mat4 projectionMatrix;
uniform mat4 viewingMatrix;
uniform mat4 modelingMatrix;
uniform vec3 eye;

// Texture-related data;
uniform sampler2D rgbTexture;
uniform int widthTexture;
uniform int heightTexture;

// Data from Vertex Shader
in vec2 textureCoordinate;
in vec3 vertexNormal; // For Lighting computation
in vec3 ToLightVector; // Vector from Vertex to Light;
in vec3 ToCameraVector; // Vector from Vertex to Camera;

void main() {

  // Assignment Constants below
  // get the texture color
  vec4 textureColor = texture(rgbTexture, textureCoordinate);

  // apply Phong shading by using the following parameters
  vec4 ka = vec4(0.25,0.25,0.25,1.0); // reflectance coeff. for ambient
  vec4 Ia = vec4(0.3,0.3,0.3,1.0);    // light color for ambient
  vec4 kd = vec4(1.0, 1.0, 1.0, 1.0); // reflectance coeff. for diffuse
  vec4 Id = vec4(1.0, 1.0, 1.0, 1.0); // light color for diffuse
  vec4 ks = vec4(1.0, 1.0, 1.0, 1.0); // reflectance coeff. for specular
  vec4 Is = vec4(1.0, 1.0, 1.0, 1.0); // light color for specular
  int specExp = 100; // specular exponent

  vec4 L = vec4(ToLightVector, 0.0);
  vec4 V = vec4(ToCameraVector, 0.0);
  vec4 H = normalize(L + V);
  vec4 N = vec4(vertexNormal, 0.0);

  float NdotL = dot(N, L);
  float NdotH = dot(N, H);

  // compute ambient component
  vec4 ambient = ka * Ia;
  // compute diffuse component
  vec4 diffuse = kd * Id * max(0, NdotL);
  // compute specular component
  vec4 specular = ks * Is * pow(max(0, NdotH), specExp);

  // compute the color using the following equation
  color = vec4(clamp(textureColor.xyz * vec3(ambient + diffuse + specular), 0.0, 1.0), 1.0);
}
