#version 330

// Model-Space coordinates
in vec3 position;
in vec3 normal;

struct LightSource {
    vec3 position;
    vec3 rgbIntensity;
};
uniform LightSource light;

uniform mat4 ModelView;
uniform mat4 Perspective;

// Remember, this is transpose(inverse(ModelView)).  Normals should be
// transformed using this matrix instead of the ModelView matrix.
uniform mat3 NormalMatrix;

struct Material {
    vec3 kd;
    vec3 ks;
    float shininess;
};
uniform Material material;

// Ambient light intensity for each RGB component.
uniform vec3 ambientIntensity;

out vec3 vcolour;

vec3 lighting(vec3 vertPosition, vec3 vertNormal) {
    // Direction from vertex to light source.
    vec3 l = normalize(light.position - vertPosition);

    float n_dot_l = max(dot(vertNormal, l), 0.0);

    vec3 diffuse;
    diffuse = material.kd * n_dot_l;

    vec3 v = normalize(-vertPosition);
    vec3 r = reflect(-l, vertNormal);
    float r_dot_v = max(dot(r, v), 0.0);
    vec3 specular = material.ks * pow(r_dot_v, material.shininess);

    vec3 ambient = ambientIntensity * material.kd;
    vec3 result = ambient + light.rgbIntensity * (diffuse + specular);
    return result;
}

void main() {
	vec4 pos4 = vec4(position, 1.0);

	vcolour = lighting((ModelView * pos4).xyz, normalize(NormalMatrix * normal));
	
	gl_Position = Perspective * ModelView * pos4;
}
