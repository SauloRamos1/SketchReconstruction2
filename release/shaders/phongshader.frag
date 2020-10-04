#version 150

in vec4 color;
in vec3 normal;
in vec4 vert;

in vec2 texCoords;
in float depth;

out vec4 out_Color;

uniform mat4 lightViewMatrix;
uniform mat4 viewMatrix;
uniform float ka;
uniform float kd;
uniform float ks;
uniform float shininess;

uniform bool has_texture;
uniform sampler2D model_texture;

void main(void)
{
    vec4 model_color = color;
    if (has_texture)
    {
        model_color = texture(model_texture, texCoords);
    }

    vec3 lightDirection = (viewMatrix * inverse(lightViewMatrix) * vec4(0.0, 0.0, 1.0, 0.0)).xyz;
    lightDirection = normalize(lightDirection);

    vec3 lightReflection = reflect(-lightDirection, normal);

    vec3 eyeDirection = normalize(-vert.xyz);

    vec4 ambientLight = model_color * ka;
    vec4 diffuseLight = model_color * kd * max(dot(lightDirection, normal),0.0);
    vec4 specularLight = vec4(vec3(ks), 1.0) *  max(pow(dot(lightReflection, eyeDirection), shininess),0.0);

    out_Color = vec4(ambientLight.xyz + diffuseLight.xyz + specularLight.xyz, model_color.w);
}
