//Type->vertexShader

#version 450 core

in layout(location=0) vec3 ver;

void main() {
    gl_Position = vec4(ver, 1);
}


//Type->fragmentShader

#version 450 core

out vec4 color;

struct PlanesColor {
    vec3 side[6];
};

uniform PlanesColor pc;

vec4 getPlaneColorOfVertex(PlanesColor self, int primitiveID) {
    return vec4(self.side[primitiveID/2], 1);
}

void main() {
    // color = vec4(.7, .7, 0, 1);   
    color = getPlaneColorOfVertex(pc, gl_PrimitiveID);
}