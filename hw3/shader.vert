#version 410

layout(location = 0) in vec3 position;

// Data from CPU 
uniform mat4 MVP; // ModelViewProjection Matrix
uniform mat4 MV; // ModelView idMVPMatrix
uniform vec4 cameraPosition;
uniform float heightFactor;

uniform mat4 projectionMatrix;
uniform mat4 viewingMatrix;
uniform mat4 modelingMatrix;
uniform vec3 eye;

// Texture-related data
uniform sampler2D rgbTexture;
uniform int widthTexture;
uniform int heightTexture;

// Output to Fragment Shader
out vec2 textureCoordinate; // For texture-color
out vec3 vertexNormal; // For Lighting computation
out vec3 ToLightVector; // Vector from Vertex to Light;
out vec3 ToCameraVector; // Vector from Vertex to Camera;

void main() {
    textureCoordinate = vec2(1-float(position.x)/widthTexture, 1-float(position.z)/heightTexture);

    vec4 texture_color = texture(rgbTexture, textureCoordinate);
    
    vec3 pWorld = position;
    pWorld.y = heightFactor * (0.2126*texture_color.x + 0.7152*texture_color.y + 0.0722*texture_color.z);

    // get texture value, compute height
    // compute normal vector using also the heights of neighbor vertices

    // compute toLight vector vertex coordinate in VCS
   
    // set gl_Position variable correctly to give the transformed vertex position

    //gl_Position = vec4(0,0,0,0); // this is a placeholder. It does not correctly set the position
    //gl_Position = vec4(position, 1.0f);

    //vec4 pWorld = modelingMatrix * vec4(position, 1);

    gl_Position = projectionMatrix * viewingMatrix * modelingMatrix * vec4(pWorld, 1.0f);

}
