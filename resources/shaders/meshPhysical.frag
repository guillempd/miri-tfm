// m_ : Model Space
// w_ : World Space
// v_ : View Space
// t_ : Tangent Space

#ifdef USE_LUMINANCE
#define GetSolarRadiance GetSolarLuminance
#define GetSkyRadiance GetSkyLuminance
#define GetSkyRadianceToPoint GetSkyLuminanceToPoint
#define GetSunAndSkyIrradiance GetSunAndSkyIlluminance
#endif

// NOTE: These can be included
// Forward declarations
vec3 GetSolarRadiance();
vec3 GetSkyRadiance(vec3 camera, vec3 view_ray, float shadow_length, vec3 sun_direction, out vec3 transmittance);
vec3 GetSkyRadianceToPoint(vec3 camera, vec3 point, float shadow_length, vec3 sun_direction, out vec3 transmittance);
vec3 GetSunAndSkyIrradiance(vec3 p, vec3 normal, vec3 sun_direction, out vec3 sky_irradiance);

in vec3 w_Pos;
in vec3 w_Normal;

const float PI = 3.14159265;
const vec3 color = vec3(0.1, 0.1, 0.1);
uniform float exposure;
uniform vec3 white_point;

uniform vec3 w_CameraPos;
uniform vec3 w_EarthCenterPos;
uniform vec3 w_SunDirection;

out vec4 FragColor;

void main()
{
    vec3 sky_irradiance;
    vec3 sun_irradiance = GetSunAndSkyIrradiance(w_Pos - w_EarthCenterPos, w_Normal, w_SunDirection, sky_irradiance);

    vec3 sphere_radiance = color * (1.0 / PI) * (sun_irradiance + sky_irradiance);

    vec3 transmittance;
    vec3 in_scatter = GetSkyRadianceToPoint(w_CameraPos - w_EarthCenterPos, w_Pos - w_EarthCenterPos, 0.0, w_SunDirection, transmittance);

    sphere_radiance = sphere_radiance * transmittance + in_scatter;

    sphere_radiance = pow(vec3(1,1,1) - exp(-sphere_radiance / white_point * exposure), vec3(1.0 / 2.2));

    FragColor = vec4(sphere_radiance, 1.0);
}