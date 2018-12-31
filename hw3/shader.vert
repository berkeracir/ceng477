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
uniform mat4 normalMatrix;
uniform vec3 eye;

// Texture-related data
uniform sampler2D rgbTexture;
uniform int widthTexture;
uniform int heightTexture;

vec3 lightPosition = vec3(widthTexture/2, widthTexture+heightTexture, heightTexture/2);

// Output to Fragment Shader
out vec2 textureCoordinate; // For texture-color
out vec3 vertexNormal; // For Lighting computation
out vec3 ToLightVector; // Vector from Vertex to Light;
out vec3 ToCameraVector; // Vector from Vertex to Camera;

vec3 get_height(vec3 pos) {
    vec4 texture_color = texture(rgbTexture, vec2(pos.x/widthTexture, pos.z/heightTexture));
    float h = heightFactor * (0.2126*texture_color.x + 0.7152*texture_color.y + 0.0722*texture_color.z);
    return vec3(pos.x, h, pos.z);
}

vec3 calculate_normal(vec3 pos) {
    vec3 normal;

    float x = widthTexture - pos.x;
    float y = pos.y;
    float z = heightTexture - pos.z;
    vec3 v = get_height(vec3(x, y, z));

    if (x == 0) {
        if (z == 0) {
            vec3 adj1 = get_height(vec3(x+1, 0, z));
            vec3 adj2 = get_height(vec3(x+1, 0, z+1));
            vec3 adj3 = get_height(vec3(x, 0, z+1));

            vec3 v1 = adj1 - v;
            vec3 v2 = adj2 - v;
            vec3 v3 = adj3 - v;

            vec3 n1 = cross(v2, v1);
            vec3 n2 = cross(v3, v2);

            normal = normalize(n1 + n2);
        }
        else if (z == heightTexture) {
            vec3 adj1 = get_height(vec3(x, 0, z-1));
            vec3 adj2 = get_height(vec3(x+1, 0, z));

            vec3 v1 = adj1 - v;
            vec3 v2 = adj2 - v;

            vec3 n1 = cross(v2, v1);

            normal = normalize(n1);
        }
        else {
            vec3 adj1 = get_height(vec3(x, 0, z-1));
            vec3 adj2 = get_height(vec3(x+1, 0, z));
            vec3 adj3 = get_height(vec3(x+1, 0, z+1));
            vec3 adj4 = get_height(vec3(x, 0, z+1));

            vec3 v1 = adj1 - v;
            vec3 v2 = adj2 - v;
            vec3 v3 = adj3 - v;
            vec3 v4 = adj4 - v;

            vec3 n1 = cross(v2, v1);
            vec3 n2 = cross(v3, v2);
            vec3 n3 = cross(v4, v3);

            normal = normalize(n1 + n2 + n3);
        }
    }
    else if (x == widthTexture) {
        if (z == 0) {
            vec3 adj1 = get_height(vec3(x, 0, z+1));
            vec3 adj2 = get_height(vec3(x-1, 0, z));

            vec3 v1 = adj1 - v;
            vec3 v2 = adj2 - v;

            vec3 n1 = cross(v2, v1);

            normal = normalize(n1);
        }
        else if (z == heightTexture) {
            vec3 adj1 = get_height(vec3(x-1, 0, z));
            vec3 adj2 = get_height(vec3(x-1, 0, z-1));
            vec3 adj3 = get_height(vec3(x, 0, z-1));

            vec3 v1 = adj1 - v;
            vec3 v2 = adj2 - v;
            vec3 v3 = adj3 - v;

            vec3 n1 = cross(v2, v1);
            vec3 n2 = cross(v3, v2);

            normal = normalize(n1 + n2);
        }
        else {
            vec3 adj1 = get_height(vec3(x, 0, z+1));
            vec3 adj2 = get_height(vec3(x-1, 0, z));
            vec3 adj3 = get_height(vec3(x-1, 0, z-1));
            vec3 adj4 = get_height(vec3(x, 0, z-1));

            vec3 v1 = adj1 - v;
            vec3 v2 = adj2 - v;
            vec3 v3 = adj3 - v;
            vec3 v4 = adj4 - v;

            vec3 n1 = cross(v2, v1);
            vec3 n2 = cross(v3, v2);
            vec3 n3 = cross(v4, v3);

            normal = normalize(n1 + n2 + n3);
        }
    }
    else {
        if (z == 0) {
            vec3 adj1 = get_height(vec3(x+1, 0, z));
            vec3 adj2 = get_height(vec3(x+1, 0, z+1));
            vec3 adj3 = get_height(vec3(x, 0, z+1));
            vec3 adj4 = get_height(vec3(x-1, 0, z));

            vec3 v1 = adj1 - v;
            vec3 v2 = adj2 - v;
            vec3 v3 = adj3 - v;
            vec3 v4 = adj4 - v;

            vec3 n1 = cross(v2, v1);
            vec3 n2 = cross(v3, v2);
            vec3 n3 = cross(v4, v3);

            normal = normalize(n1 + n2 + n3); 
        }
        else if (z == heightTexture) {
            vec3 adj1 = get_height(vec3(x-1, 0, z));
            vec3 adj2 = get_height(vec3(x-1, 0, z-1));
            vec3 adj3 = get_height(vec3(x, 0, z-1));
            vec3 adj4 = get_height(vec3(x+1, 0, z));

            vec3 v1 = adj1 - v;
            vec3 v2 = adj2 - v;
            vec3 v3 = adj3 - v;
            vec3 v4 = adj4 - v;

            vec3 n1 = cross(v2, v1);
            vec3 n2 = cross(v3, v2);
            vec3 n3 = cross(v4, v3);

            normal = normalize(n1 + n2 + n3); 
        }
        else {
            vec3 adj1 = get_height(vec3(x+1, 0, z));
            vec3 adj2 = get_height(vec3(x+1, 0, z+1));
            vec3 adj3 = get_height(vec3(x, 0, z+1));
            vec3 adj4 = get_height(vec3(x-1, 0, z));
            vec3 adj5 = get_height(vec3(x-1, 0, z));
            vec3 adj6 = get_height(vec3(x, 0, z-1));

            vec3 v1 = adj1 - v;
            vec3 v2 = adj2 - v;
            vec3 v3 = adj3 - v;
            vec3 v4 = adj4 - v;
            vec3 v5 = adj5 - v;
            vec3 v6 = adj6 - v;

            vec3 n1 = cross(v2, v1);
            vec3 n2 = cross(v3, v2);
            vec3 n3 = cross(v4, v3);
            vec3 n4 = cross(v5, v4);
            vec3 n5 = cross(v6, v5);
            vec3 n6 = cross(v1, v6);

            normal = normalize(n1 + n2 + n3 + n4 + n5 + n6); 
        }
    }

    return normal;
}

void main() {
    // get texture value, compute height
    textureCoordinate = vec2(1-position.x/widthTexture, 1-position.z/heightTexture);

    vec4 texture_color = texture(rgbTexture, textureCoordinate);
    vec3 pWorld = position;
    pWorld.y = heightFactor * (0.2126*texture_color.x + 0.7152*texture_color.y + 0.0722*texture_color.z);

    vec3 pNormal = calculate_normal(position);

    mat4 mv = viewingMatrix * modelingMatrix;
    // compute toLight vector vertex coordinate in VCS
    ToLightVector = normalize(vec3(mv * vec4(lightPosition - pWorld, 0)));
    // compute ToCamera vector vertex coordinate in VCS
    ToCameraVector = normalize(vec3(mv * vec4(eye - pWorld, 0)));
    // compute normal vector using also the heights of neighbor vertices
    vertexNormal = normalize(vec3(normalMatrix * vec4(pNormal, 0)));

    // set gl_Position variable correctly to give the transformed vertex position
    gl_Position = projectionMatrix * viewingMatrix * modelingMatrix * vec4(pWorld, 1.0f);

}
