#pragma once

#include <glm/glm.hpp>

// TODO: Take into account deltaT
class AstronomicalPositioning
{
public:
    AstronomicalPositioning();
    ~AstronomicalPositioning() = default;
    void Update();
    glm::dvec3 GetSunHorizonCoordinates() { return m_sunHorizonCoordinates; }
    glm::dvec3 GetMoonHorizonCoordinates() { return m_moonHorizonCoordinates; }
    double GetMoonPhaseAngle() { return m_moonPhaseAngle; }
    double GetEarthPhaseAngle() { return m_earthPhaseAngle; }
    double GetLon() { return m_lon; }
    double GetLat() { return m_lat; }
    double GetT() { return m_T; }
private:
    void Compute();
    void ComputeJulianDate();
    void ComputeT();
    void ComputeLonLat();
    void ComputeCoordinates();
    void ComputePhaseAngles();
    void ComputeEquatorialAndHorizonCoordinates(glm::dvec3 eclipticCoordinates, glm::dvec3& eclipticRectangularCoordinates, glm::dvec3& equatorialCoordinates, glm::dvec3& horizonCoordinates);
private:
    static double ComputeJulianDate(int M, int D, int Y, int h, int m, int s, double deltaT); // JD
    static double ComputeJulianCenturies(double JD); // T
    static glm::dvec3 ComputeSunEclipticCoordinates(double T);
    static glm::dvec3 ComputeMoonEclipticCoordinates(double T);
    static glm::dvec3 SphericalToRectangular(glm::dvec3 spherical);
    static glm::dvec3 RectangularToSpherical(glm::dvec3 rectangular);
    static glm::dvec3 RectangularEclipticToRectangularEquatorial(glm::dvec3 rectangularEcliptic, double T);
    static glm::dvec3 RectangularEquatorialToRectangularHorizon(glm::dvec3 rectangularEquatorial, double T, double T_, double lat, double lon);
    static glm::dmat3 Rx(double a);
    static glm::dmat3 Ry(double a);
    static glm::dmat3 Rz(double a);
private:
    int m_M;
    int m_D;
    int m_Y;
    int m_h;
    int m_m;
    int m_s;
    double m_T; // TODO: Add m_Tp
    double m_JD;
    double m_lon;
    double m_lat;
    double m_lonDeg;
    double m_latDeg;
    glm::dvec3 m_sunEclipticCoordinates;
    glm::dvec3 m_sunEclipticRectangularCoordinates;
    glm::dvec3 m_sunEquatorialCoordinates;
    glm::dvec3 m_sunHorizonCoordinates;
    glm::dvec3 m_moonEclipticCoordinates;
    glm::dvec3 m_moonEclipticRectangularCoordinates;
    glm::dvec3 m_moonEquatorialCoordinates;
    glm::dvec3 m_moonHorizonCoordinates;
    double m_earthPhaseAngle;
    double m_moonPhaseAngle;
};