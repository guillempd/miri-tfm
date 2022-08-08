#include "Coordinates.h"

#include <imgui.h>

#include <glm/gtc/type_ptr.hpp>

Coordinates::Coordinates()
    : m_M(1)
    , m_D(1)
    , m_Y(2000)
    , m_h(12)
    , m_m(22)
    , m_s(15)
    , m_JD(2513491.14661)
{}



void GetDegreesMinutesSeconds(float& lon_deg, float& lon_min, float& lon_sec)
{
    lon_min = glm::fract(lon_deg) * 60.0f;
    lon_sec = glm::fract(lon_min) * 60.0f;
    
    lon_deg = glm::floor(lon_deg);
    lon_min = glm::floor(lon_min);
}

void Coordinates::Update()
{
    if (ImGui::Begin("Coordinates Stuff"))
    {
       /* if (ImGui::InputFloat3("Spherical Coordinates", glm::value_ptr(m_sphericalCoordinates)))
        {
            m_rectangularCoordinates = SphericalToRectangular(m_sphericalCoordinates);
        }
        if (ImGui::InputFloat3("Rectangular Coordinates", glm::value_ptr(m_rectangularCoordinates)))
        {

        }*/

        /*ImGui::InputInt("Month", &m_M);
        ImGui::InputInt("Day", &m_D);
        ImGui::InputInt("Year", &m_Y);
        ImGui::InputInt("Hour", &m_h);
        ImGui::InputInt("Minute", &m_m);
        ImGui::InputInt("Second", &m_s);*/

        // double JD = GetJulianDate(m_M, m_D, m_Y, m_h, m_m, m_s);

        //ImGui::Text("JD: %.6f", JD);
        //ImGui::Text("T: %.6f", T);

        ImGui::InputDouble("JD", &m_JD);
        double T = GetJulianCenturies(m_JD);
        PrintSunPosition(T);
        PrintMoonPosition(T);
    }
    ImGui::End();
}

void Coordinates::PrintSunPosition(double T)
{
    glm::vec3 sphericalEclipticSun = GetSunPosition(T);

    float lon_deg_sun = glm::mod(glm::degrees(sphericalEclipticSun.x), 360.0f);
    float lon_min_sun, lon_sec_sun;
    GetDegreesMinutesSeconds(lon_deg_sun, lon_min_sun, lon_sec_sun);

    float lat_deg_sun = glm::mod(glm::degrees(sphericalEclipticSun.y), 360.0f);
    float lat_min_sun, lat_sec_sun;
    GetDegreesMinutesSeconds(lat_deg_sun, lat_min_sun, lat_sec_sun);

    ImGui::Text("Sun Ecliptic Coordinates | Longitud: %gd%g'%g\", Latitud: %gd%g'%g\"", lon_deg_sun, lon_min_sun, lon_sec_sun, lat_deg_sun, lat_min_sun, lat_sec_sun);


    glm::vec3 rectangularEclipticSun = SphericalToRectangular(sphericalEclipticSun);
    glm::vec3 rectangularEquatorialSun = RectangularEclipticToRectangularEquatorial(rectangularEclipticSun, T);
    glm::vec3 sphericalEquatorialSun = RectangularToSpherical(rectangularEquatorialSun);

    float lon_deg_sun_eq = glm::mod(glm::degrees(sphericalEquatorialSun.x) / 15.0f, 24.0f);
    float lon_min_sun_eq, lon_sec_sun_eq;
    GetDegreesMinutesSeconds(lon_deg_sun_eq, lon_min_sun_eq, lon_sec_sun_eq);

    float lat_deg_sun_eq = glm::mod(glm::degrees(sphericalEquatorialSun.y), 360.0f);
    float lat_min_sun_eq, lat_sec_sun_eq;
    GetDegreesMinutesSeconds(lat_deg_sun_eq, lat_min_sun_eq, lat_sec_sun_eq);

    ImGui::Text("Sun Equatorial Coordinates | RA: %gh%gm%gs, Latitud: %gd%g'%g\"", lon_deg_sun_eq, lon_min_sun_eq, lon_sec_sun_eq, lat_deg_sun_eq, lat_min_sun_eq, lat_sec_sun_eq);
}

void Coordinates::PrintMoonPosition(double T)
{
    glm::vec3 sphericalEclipticMoon = GetMoonPosition(T);

    float lon_deg_moon = glm::mod(glm::degrees(sphericalEclipticMoon.x), 360.0f);
    float lon_min_moon, lon_sec_moon;
    GetDegreesMinutesSeconds(lon_deg_moon, lon_min_moon, lon_sec_moon);

    float lat_deg_moon = glm::mod(glm::degrees(sphericalEclipticMoon.y), 360.0f);
    float lat_min_moon, lat_sec_moon;
    GetDegreesMinutesSeconds(lat_deg_moon, lat_min_moon, lat_sec_moon);

    ImGui::Text("Moon Ecliptic Coordinates | Longitud: %gd%g'%g\", Latitud: %gd%g'%g\"", lon_deg_moon, lon_min_moon, lon_sec_moon, lat_deg_moon, lat_min_moon, lat_sec_moon);

    glm::vec3 rectangularEclipticMoon = SphericalToRectangular(sphericalEclipticMoon);
    glm::vec3 rectangularEquatorialMoon = RectangularEclipticToRectangularEquatorial(rectangularEclipticMoon, T);
    glm::vec3 sphericalEquatorialMoon = RectangularToSpherical(rectangularEquatorialMoon);

    float lon_deg_moon_eq = glm::mod(glm::degrees(sphericalEquatorialMoon.x) / 15.0f, 24.0f);
    float lon_min_moon_eq, lon_sec_moon_eq;
    GetDegreesMinutesSeconds(lon_deg_moon_eq, lon_min_moon_eq, lon_sec_moon_eq);

    float lat_deg_moon_eq = glm::mod(glm::degrees(sphericalEquatorialMoon.y), 360.0f);
    float lat_min_moon_eq, lat_sec_moon_eq;
    GetDegreesMinutesSeconds(lat_deg_moon_eq, lat_min_moon_eq, lat_sec_moon_eq);

    ImGui::Text("Moon Equatorial Coordinates | RA: %gh%gm%gs, Latitud: %gd%g'%g\"", lon_deg_moon_eq, lon_min_moon_eq, lon_sec_moon_eq, lat_deg_moon_eq, lat_min_moon_eq, lat_sec_moon_eq);
}

// See: Jensen 2001
glm::vec3 Coordinates::SphericalToRectangular(glm::vec3 spherical)
{
    float lon = spherical.x;
    float lat = spherical.y;
    float r = spherical.z;

    glm::vec3 rectangular;
    rectangular.x = glm::cos(lat) * glm::cos(lon);
    rectangular.y = glm::cos(lat) * glm::sin(lon);
    rectangular.z = glm::sin(lat);
    rectangular *= r;
    return rectangular;
}

glm::vec3 Coordinates::RectangularToSpherical(glm::vec3 rectangular)
{
    float r = glm::length(rectangular);
    float lon = glm::atan(rectangular.y, rectangular.x);
    float lat = glm::asin(rectangular.z / r);

    glm::vec3 spherical;
    spherical.x = lon;
    spherical.y = lat;
    spherical.z = r;
    return spherical;
}

// See: Jensen 2001
glm::vec3 Coordinates::RectangularEclipticToRectangularEquatorial(glm::vec3 rectangularEcliptic, double T)
{
    double eps = 0.409093 - 0.000227 * T;
    glm::vec3 rectangularEquatorial = Rx(eps) * rectangularEcliptic;
    return rectangularEquatorial;
}

//glm::vec3 Coordinates::RectangularEquatorialToRectangularHorizon(glm::vec3 rectangularEquatorial)
//{
//    float lat, lon, LMST; // TODO: Compute
//    glm::mat3 P; // TODO: Compute
//    glm::mat3 M = Ry(lat - glm::half_pi<float>()) * Rz(-LMST) * P;
//    glm::vec3 rectangularHorizon = M * rectangularEquatorial;
//    return rectangularHorizon;
//}

glm::vec3 Coordinates::GetSunPosition(double T)
{
    double M = 6.24 + 628.302 * T;

    double lambda = 4.895048 + 628.331951 * T + (0.033417 - 0.000084 * T) * glm::sin(M) + 0.000351 * glm::sin(2*M);
    double beta = 0.0;
    double r = 1.000140 - (0.016708 - 0.000042 * T) * glm::cos(M) - 0.000141 * glm::cos(2*M);
    
    glm::vec3 sphericalEcliptic = glm::vec3(lambda, beta, r);
    return sphericalEcliptic;
}

glm::vec3 Coordinates::GetMoonPosition(double T)
{
    double lp = 3.8104 + 8399.7091 * T;
    double m  = 6.2300 +  628.3019 * T;
    double f  = 1.6280 + 8433.4663 * T;
    double mp = 2.3554 + 8328.6911 * T;
    double d  = 5.1985 + 7771.3772 * T;

    double lambda = lp
        +0.1098 * glm::sin(mp)
        +0.0222 * glm::sin(2*d - mp)
        +0.0115 * glm::sin(2*d)
        +0.0037 * glm::sin(2*mp)
        -0.0032 * glm::sin(m)
        -0.0020 * glm::sin(2*f)
        +0.0010 * glm::sin(2*d - 2*mp)
        +0.0010 * glm::sin(2*d - m - mp)
        +0.0009 * glm::sin(2*d + mp)
        +0.0008 * glm::sin(2*d - m)
        +0.0007 * glm::sin(mp - m)
        -0.0006 * glm::sin(d)
        -0.0005 * glm::sin(m + mp);
    double beta = 
        +0.0895 * glm::sin(f)
        +0.0049 * glm::sin(mp + f)
        +0.0048 * glm::sin(mp - f)
        +0.0030 * glm::sin(2*d - f)
        +0.0010 * glm::sin(2*d + f - mp)
        +0.0008 * glm::sin(2*d - f - mp)
        +0.0006 * glm::sin(2*d + f);
    double r = 1.0;

    glm::vec3 sphericalEcliptic = glm::vec3(lambda, beta, r);
    return sphericalEcliptic;
}

glm::mat3 Coordinates::Rx(float theta)
{
    float stheta = glm::sin(theta);
    float ctheta = glm::cos(theta);
    return glm::mat3(glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, ctheta, stheta), glm::vec3(0.0f, -stheta, ctheta));
}

glm::mat3 Coordinates::Ry(float theta)
{
    float stheta = glm::sin(theta);
    float ctheta = glm::cos(theta);
    return glm::mat3(glm::vec3(ctheta, 0.0f, -stheta), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(stheta, 0.0f, ctheta));
}

glm::mat3 Coordinates::Rz(float theta)
{
    float stheta = glm::sin(theta);
    float ctheta = glm::cos(theta);
    return glm::mat3(glm::vec3(ctheta, stheta, 0.0f), glm::vec3(-stheta, ctheta, 0.0), glm::vec3(0.0f, 0.0f, 1.0f));
}

// TODO: Check if something has to be added to the seconds (see: Jensen 2001 Appendix Time Conversion)
double Coordinates::GetJulianDate(int M, int D, int Y, int h, int m, int s)
{
    int Mp = M;
    int Yp = Y;
    if (M == 1 || M == 2)
    {
        Yp = Y - 1;
        Mp = M + 12;
    }

    double q = (h + (m + s / 60.0) / 60.0) / 24.0;
    double JDN = 1720996.5 - glm::floor(Yp / 100.0) + glm::floor(Yp / 400.0) + glm::floor(Yp * 365.25) + glm::floor(30.6001 * (Mp + 1.0)) + D;

    double JD = JDN + q;
    return JD;
}

double Coordinates::GetJulianCenturies(double JD)
{
    double T = (JD - 2451545.0) / 36525.0;
    return T;
}
