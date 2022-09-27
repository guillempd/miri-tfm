#include "AstronomicalPositioning.h"

#include <imgui.h>

#include <glm/gtc/type_ptr.hpp>

AstronomicalPositioning::AstronomicalPositioning()
    : m_M(10)
    , m_D(10)
    , m_Y(2022)
    , m_h(6)
    , m_m(0)
    , m_s(33)
    , m_JD(2459862.750382)
    , m_lonDeg(2.1686)
    , m_latDeg(41.3874)
    , m_lon(0.0378)
    , m_lat(0.7223)
{
    Compute();
}

void AstronomicalPositioning::Update()
{
    if (ImGui::Begin("Celestial Bodies Positioning"))
    {
        ImGui::InputInt("Month", &m_M);
        ImGui::InputInt("Day", &m_D);
        ImGui::InputInt("Year", &m_Y);
        ImGui::InputInt("Hour", &m_h);
        ImGui::InputInt("Minute", &m_m);
        ImGui::InputInt("Second", &m_s);
        ImGui::InputDouble("Observer Longitude", &m_lonDeg);
        ImGui::InputDouble("Observer Latitude", &m_latDeg);

        Compute();

        ImGui::Text("Julian Date (JD): %f", m_JD);
        ImGui::Separator();
        glm::dvec3 sunEclipticCoordinatesDeg = glm::mod(glm::degrees(m_sunEclipticCoordinates), 360.0);
        ImGui::Text("Sun Ecliptic Coordinates | Longitude: %gd, Latitude: %gd", sunEclipticCoordinatesDeg.x, sunEclipticCoordinatesDeg.y);
        glm::dvec3 sunEquatorialCoordinatesDeg = glm::mod(glm::degrees(m_sunEquatorialCoordinates), 360.0);
        ImGui::Text("Sun Equatorial Coordinates | RA: %gd, Latitude: %gd", sunEquatorialCoordinatesDeg.x, sunEquatorialCoordinatesDeg.y);
        glm::dvec3 sunHorizonCoordinatesDeg = glm::mod(glm::degrees(m_sunHorizonCoordinates), 360.0);
        ImGui::Text("Sun Horizon Coordinates | Az: %gd, Alt: %gd", sunHorizonCoordinatesDeg.x, sunHorizonCoordinatesDeg.y);
        ImGui::Text("Distance | %.6f AU", m_sunEclipticCoordinates.z);
        ImGui::Separator();
        glm::dvec3 moonEclipticCoordinatesDeg = glm::mod(glm::degrees(m_moonEclipticCoordinates), 360.0);
        ImGui::Text("Moon Ecliptic Coordinates | Longitude: %gd, Latitude: %gd", moonEclipticCoordinatesDeg.x, moonEclipticCoordinatesDeg.y);
        glm::dvec3 moonEquatorialCoordinatesDeg = glm::mod(glm::degrees(m_moonEquatorialCoordinates), 360.0);
        ImGui::Text("Moon Equatorial Coordinates | RA: %gd, Latitude: %gd", moonEquatorialCoordinatesDeg.x, moonEquatorialCoordinatesDeg.y);
        glm::dvec3 moonHorizonCoordinatesDeg = glm::mod(glm::degrees(m_moonHorizonCoordinates), 360.0);
        ImGui::Text("Moon Horizon Coordinates | Az: %gd, Alt: %gd", moonHorizonCoordinatesDeg.x, moonHorizonCoordinatesDeg.y);
        ImGui::Text("Distance | %.6f AU", m_moonEclipticCoordinates.z);
        ImGui::Separator();
        ImGui::Text("Phase Angles | Earth: %grad Moon: %grad", m_earthPhaseAngle, m_moonPhaseAngle);
    }
    ImGui::End();
}

// See: Jensen 2001
glm::dvec3 AstronomicalPositioning::SphericalToRectangular(glm::dvec3 spherical)
{
    double lon = spherical.x;
    double lat = spherical.y;
    double r = spherical.z;

    glm::dvec3 rectangular;
    rectangular.x = glm::cos(lat) * glm::cos(lon);
    rectangular.y = glm::cos(lat) * glm::sin(lon);
    rectangular.z = glm::sin(lat);
    rectangular *= r;
    return rectangular;
}

glm::dvec3 AstronomicalPositioning::RectangularToSpherical(glm::dvec3 rectangular)
{
    double r = glm::length(rectangular);
    double lon = glm::atan(rectangular.y, rectangular.x);
    double lat = glm::asin(rectangular.z / r);

    glm::dvec3 spherical;
    spherical.x = lon;
    spherical.y = lat;
    spherical.z = r;
    return spherical;
}

// See: Jensen 2001
glm::dvec3 AstronomicalPositioning::RectangularEclipticToRectangularEquatorial(glm::dvec3 rectangularEcliptic, double T)
{
    double eps = 0.409093 - 0.000227 * T;
    glm::dvec3 rectangularEquatorial = Rx(eps) * rectangularEcliptic;
    return rectangularEquatorial;
}

glm::dvec3 AstronomicalPositioning::RectangularEquatorialToRectangularHorizon(glm::dvec3 rectangularEquatorial, double T, double T_, double lon, double lat)
{
    double LMST = 4.894961 + 230121.675315 * T_ + lon;
    glm::dmat3 P = Rz(0.01118*T) * Ry(-0.00972*T) * Rz(0.01118*T);
    glm::dmat3 M = Ry(lat - glm::half_pi<float>()) * Rz(-LMST) * P;
    glm::dvec3 rectangularHorizon = M * rectangularEquatorial;
    return rectangularHorizon;
}



void AstronomicalPositioning::Compute()
{
    ComputeT();
    ComputeLonLat();
    ComputeCoordinates();
    ComputePhaseAngles();
}

// TODO: Check if something has to be added to the seconds (see: Jensen 2001 Appendix Time Conversion)
void AstronomicalPositioning::ComputeT()
{
    m_JD = ComputeJulianDate(m_M, m_D, m_Y, m_h, m_m, m_s, 0.0);
    m_T = ComputeJulianCenturies(m_JD);

    double JDp = ComputeJulianDate(m_M, m_D, m_Y, m_h, m_m, m_s, 73.0);
    m_Tp = ComputeJulianCenturies(JDp);
}

void AstronomicalPositioning::ComputeLonLat()
{
    m_lon = glm::radians(m_lonDeg);
    m_lat = glm::radians(m_latDeg);
}

void AstronomicalPositioning::ComputeCoordinates()
{
    m_sunEclipticCoordinates = ComputeSunEclipticCoordinates(m_T);
    m_moonEclipticCoordinates = ComputeMoonEclipticCoordinates(m_T);
    ComputeEquatorialAndHorizonCoordinates(m_sunEclipticCoordinates, m_sunEclipticRectangularCoordinates, m_sunEquatorialCoordinates, m_sunHorizonCoordinates);
    ComputeEquatorialAndHorizonCoordinates(m_moonEclipticCoordinates, m_moonEclipticRectangularCoordinates, m_moonEquatorialCoordinates, m_moonHorizonCoordinates);
}

void AstronomicalPositioning::ComputeEquatorialAndHorizonCoordinates(glm::dvec3 eclipticCoordinates, glm::dvec3& eclipticRectangularCoordinates, glm::dvec3& equatorialCoordinates, glm::dvec3& horizonCoordinates)
{
    eclipticRectangularCoordinates = SphericalToRectangular(eclipticCoordinates);
    glm::dvec3 rectangularEquatorial = RectangularEclipticToRectangularEquatorial(eclipticRectangularCoordinates, m_T);
    equatorialCoordinates = RectangularToSpherical(rectangularEquatorial);
    glm::dvec3 rectangularHorizon = RectangularEquatorialToRectangularHorizon(rectangularEquatorial, m_T, m_Tp, m_lon, m_lat);
    horizonCoordinates = RectangularToSpherical(rectangularHorizon);
}

void AstronomicalPositioning::ComputePhaseAngles()
{
    m_earthPhaseAngle = glm::acos(glm::dot(m_sunEclipticRectangularCoordinates, m_moonEclipticRectangularCoordinates) / (glm::length(m_sunEclipticRectangularCoordinates) * glm::length(m_moonEclipticRectangularCoordinates)));
    m_moonPhaseAngle = glm::pi<float>() - m_earthPhaseAngle;
}

double AstronomicalPositioning::ComputeJulianDate(int M, int D, int Y, int h, int m, int s, double deltaT)
{
    int Mp = M;
    int Yp = Y;
    if (M == 1 || M == 2)
    {
        Yp = Y - 1;
        Mp = M + 12;
    }

    double q = (h + (m + (s + deltaT) / 60.0) / 60.0) / 24.0 - 0.5;
    double JDN = 1720997.0 - glm::floor(Yp / 100.0) + glm::floor(Yp / 400.0) + glm::floor(Yp * 365.25) + glm::floor(30.6001 * (Mp + 1.0)) + D;

    double JD = JDN + q;
    return JD;
}

double AstronomicalPositioning::ComputeJulianCenturies(double JD)
{
    double T = (JD - 2451545.0) / 36525.0;
    return T;
}

glm::dvec3 AstronomicalPositioning::ComputeSunEclipticCoordinates(double T)
{
    double M = 6.24 + 628.302 * T;

    double lambda = 4.895048 + 628.331951 * T + (0.033417 - 0.000084 * T) * glm::sin(M) + 0.000351 * glm::sin(2 * M);
    double beta = 0.0;
    double r = 1.000140 - (0.016708 - 0.000042 * T) * glm::cos(M) - 0.000141 * glm::cos(2 * M); // AU

    return glm::dvec3(lambda, beta, r);
}

glm::dvec3 AstronomicalPositioning::ComputeMoonEclipticCoordinates(double T)
{
    double lp = 3.8104 + 8399.7091 * T;
    double m = 6.2300 + 628.3019 * T;
    double f = 1.6280 + 8433.4663 * T;
    double mp = 2.3554 + 8328.6911 * T;
    double d = 5.1985 + 7771.3772 * T;

    double lambda = lp
        + 0.1098 * glm::sin(mp)
        + 0.0222 * glm::sin(2 * d - mp)
        + 0.0115 * glm::sin(2 * d)
        + 0.0037 * glm::sin(2 * mp)
        - 0.0032 * glm::sin(m)
        - 0.0020 * glm::sin(2 * f)
        + 0.0010 * glm::sin(2 * d - 2 * mp)
        + 0.0010 * glm::sin(2 * d - m - mp)
        + 0.0009 * glm::sin(2 * d + mp)
        + 0.0008 * glm::sin(2 * d - m)
        + 0.0007 * glm::sin(mp - m)
        - 0.0006 * glm::sin(d)
        - 0.0005 * glm::sin(m + mp);
    double beta =
        +0.0895 * glm::sin(f)
        + 0.0049 * glm::sin(mp + f)
        + 0.0048 * glm::sin(mp - f)
        + 0.0030 * glm::sin(2 * d - f)
        + 0.0010 * glm::sin(2 * d + f - mp)
        + 0.0008 * glm::sin(2 * d - f - mp)
        + 0.0006 * glm::sin(2 * d + f);
    double pip =
        +0.016593
        + 0.000904 * glm::cos(mp)
        + 0.000166 * glm::cos(2 * d - mp)
        + 0.000137 * glm::cos(2 * d)
        + 0.000049 * glm::cos(2 * mp)
        + 0.000015 * glm::cos(2 * d + mp)
        + 0.000009 * glm::cos(2 * d - m);
    constexpr double au_in_earth_radi = 23455.0; // 1 AU = 23455 earth radi
    double r = (1.0 / pip) / au_in_earth_radi; // AU

    return glm::dvec3(lambda, beta, r);
}

glm::dmat3 AstronomicalPositioning::Rx(double a)
{
    double sa = glm::sin(a);
    double ca = glm::cos(a);
    return glm::dmat3(glm::dvec3(1.0f, 0.0f, 0.0f), glm::dvec3(0.0f, ca, sa), glm::dvec3(0.0f, -sa, ca));
}

glm::dmat3 AstronomicalPositioning::Ry(double a)
{
    double sa = glm::sin(a);
    double ca = glm::cos(a);
    return glm::dmat3(glm::dvec3(ca, 0.0f, -sa), glm::dvec3(0.0f, 1.0f, 0.0f), glm::dvec3(sa, 0.0f, ca));
}

glm::dmat3 AstronomicalPositioning::Rz(double a)
{
    double sa = glm::sin(a);
    double ca = glm::cos(a);
    return glm::dmat3(glm::dvec3(ca, sa, 0.0f), glm::dvec3(-sa, ca, 0.0f), glm::dvec3(0.0f, 0.0f, 1.0f));
}
