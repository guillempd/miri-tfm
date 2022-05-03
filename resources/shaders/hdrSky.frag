in vec3 view_ray;
out vec4 FragColor;
uniform sampler2D hdrImage;

// TODO: Check this code extracted from learnopengl.com
const vec2 invAtan = vec2(0.1591, 0.3183);
vec2 SampleSphericalMap(vec3 v)
{
    vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
    uv *= invAtan;
    uv += 0.5;
    return uv;
}

void main()
{
    vec3 direction = normalize(view_ray);
    // Sample with this direction the hdrImage
    FragColor = texture(hdrImage, SampleSphericalMap(direction));
    FragColor = FragColor / (1.0 + FragColor); // Tonemapping
    FragColor.a = 1.0;
}