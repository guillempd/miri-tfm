// Returns the radiance of the Sun, outside the atmosphere.
vec3 GetSolarRadiance();

// Returns the sky radiance along the segment from 'camera' to the nearest
// atmosphere boundary in direction 'view_ray', as well as the transmittance
// along this segment.
vec3 GetSkyRadiance(vec3 camera, vec3 view_ray, float shadow_length, vec3 sun_direction, out vec3 transmittance);

// Returns the sky radiance along the segment from 'camera' to 'p', as well as
// the transmittance along this segment.
vec3 GetSkyRadianceToPoint(vec3 camera, vec3 p, float shadow_length, vec3 sun_direction, out vec3 transmittance);

// Returns the sun and sky irradiance received on a surface patch located at 'p'
// and whose normal vector is 'normal'.
vec3 GetSunAndSkyIrradiance(vec3 p, vec3 normal, vec3 sun_direction, out vec3 sky_irradiance);

// Returns the luminance of the Sun, outside the atmosphere.
vec3 GetSolarLuminance();

// Returns the sky luminance along the segment from 'camera' to the nearest
// atmosphere boundary in direction 'view_ray', as well as the transmittance
// along this segment.
vec3 GetSkyLuminance(vec3 camera, vec3 view_ray, float shadow_length, vec3 sun_direction, out vec3 transmittance);

// Returns the sky luminance along the segment from 'camera' to 'p', as well as
// the transmittance along this segment.
vec3 GetSkyLuminanceToPoint(vec3 camera, vec3 p, float shadow_length, vec3 sun_direction, out vec3 transmittance);

// Returns the sun and sky illuminance received on a surface patch located at
// 'p' and whose normal vector is 'normal'.
vec3 GetSunAndSkyIlluminance(vec3 p, vec3 normal, vec3 sun_direction, out vec3 sky_illuminance);