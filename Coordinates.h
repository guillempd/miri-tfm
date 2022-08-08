#pragma once

#include <glm/glm.hpp>

// TODO: Use doubles for these calculations (JD, T and others), output angles might be in floats
class Coordinates
{
public:
    Coordinates();
    ~Coordinates() = default;
    void Update();
    static void PrintSunPosition(double T);
    static void PrintMoonPosition(double T);
    static glm::vec3 SphericalToRectangular(glm::vec3 spherical);
    static glm::vec3 RectangularToSpherical(glm::vec3 rectangular);
    static glm::vec3 RectangularEclipticToRectangularEquatorial(glm::vec3 rectangularEcliptic, double T);
    static glm::vec3 RectangularEquatorialToRectangularHorizon(glm::vec3 rectangularEquatorial, double T);
    static glm::vec3 GetSunPosition(double T);
    static glm::vec3 GetMoonPosition(double T);
    static glm::mat3 Rx(float theta); // TODO: Implement
    static glm::mat3 Ry(float theta); // TODO: Implement
    static glm::mat3 Rz(float theta); // TODO: Implement
    static double GetJulianDate(int M, int D, int Y, int h, int m, int s); // JD
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
};