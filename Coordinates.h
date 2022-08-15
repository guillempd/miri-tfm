#pragma once

#include <glm/glm.hpp>

// TODO: Compute radi correctly (check units)
// TODO: Take into account deltaT
class Coordinates
{
public:
    Coordinates();
    ~Coordinates() = default;
    void Update();
    static void PrintJulianDate(double JD);
    static void PrintSunPosition(double T, double T_, double lon, double lat);
    static void PrintMoonPosition(double T, double T_, double lon, double lat);
    static glm::vec3 SphericalToRectangular(glm::vec3 spherical);
    static glm::vec3 RectangularToSpherical(glm::vec3 rectangular);
    static glm::vec3 RectangularEclipticToRectangularEquatorial(glm::vec3 rectangularEcliptic, double T);
    static glm::vec3 RectangularEquatorialToRectangularHorizon(glm::vec3 rectangularEquatorial, double T, double T_, double lat, double lon);
    static glm::vec3 GetSunPosition(double T);
    static glm::vec3 GetMoonPosition(double T);
    static glm::mat3 Rx(float theta);
    static glm::mat3 Ry(float theta);
    static glm::mat3 Rz(float theta);
    static double GetJulianDate(int M, int D, int Y, int h, int m, int s, double deltaT); // JD
    static double GetJulianCenturies(double JD); // T
private:
    glm::vec3 m_sphericalCoordinates;
    glm::vec3 m_rectangularCoordinates;
    int m_M;
    int m_D;
    int m_Y;
    int m_h;
    int m_m;
    int m_s;
    double m_JD;
    double m_lon;
    double m_lat;
};