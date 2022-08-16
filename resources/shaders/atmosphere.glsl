// Returns the radiance of the Sun, outside the atmosphere.
vec3 GetSunRadiance();

// Returns the sky radiance along the segment from 'camera' to the nearest
// atmosphere boundary in direction 'view_ray', as well as the transmittance
// along this segment.
vec3 GetSolarSkyRadiance(vec3 camera, vec3 view_ray, float shadow_length, vec3 sun_direction, out vec3 transmittance);

// Returns the sky radiance along the segment from 'camera' to 'p', as well as
// the transmittance along this segment.
vec3 GetSolarSkyRadianceToPoint(vec3 camera, vec3 p, float shadow_length, vec3 sun_direction, out vec3 transmittance);

// Returns the sun and sky irradiance received on a surface patch located at 'p'
// and whose normal vector is 'normal'.
vec3 GetSunAndSolarSkyIrradiance(vec3 p, vec3 normal, vec3 sun_direction, out vec3 sky_irradiance);

// Moon equivalents
vec3 GetMoonRadiance();
vec3 GetLunarSkyRadiance(vec3 camera, vec3 view_ray, float shadow_length, vec3 moon_direction, out vec3 transmittance);
vec3 GetLunarSkyRadianceToPoint(vec3 camera, vec3 point, float shadow_length, vec3 moon_direction, out vec3 transmittance);
vec3 GetMoonAndLunarSkyIrradiance(vec3 p, vec3 normal, vec3 moon_direction, out vec3 sky_irradiance);