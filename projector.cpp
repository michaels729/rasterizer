#include "projector.h"
#include "geomath.h"
#include <algorithm>

Projector::Projector(const CameraSpace &cameraSpace, const ScreenSpace &screenSpace,
                     const NDCSpace &ndcSpace, const RasterSpace &rasterSpace)
    : cameraSpace(cameraSpace), screenSpace(screenSpace), ndcSpace(ndcSpace), rasterSpace(rasterSpace)
{

}

std::vector<glm::vec3> Projector::project(Primitive &primitive, Film &film) const
{
    std::vector<glm::vec3> rasterPixels;

    std::vector<glm::vec3> objectVertices = primitive.getVertices();
    std::vector<glm::vec3> objectColors = primitive.getColors();
    for (int i = 0; i < objectVertices.size(); i += 3) {
        const glm::vec3 &v0 = objectVertices[i];
        const glm::vec3 &v1 = objectVertices[i+1];
        const glm::vec3 &v2 = objectVertices[i+2];

        const glm::vec3 v0Raster = convertToRaster(v0);
        const glm::vec3 v1Raster = convertToRaster(v1);
        const glm::vec3 v2Raster = convertToRaster(v2);

        float xMinRaster = std::min(v0Raster.x, v1Raster.x, v2Raster.x);
        float yMinRaster = std::min(v0Raster.y, v1Raster.y, v2Raster.y);
        float xMaxRaster = std::max(v0Raster.x, v1Raster.x, v2Raster.x);
        float yMaxRaster = std::max(v0Raster.y, v1Raster.y, v2Raster.y);

        const glm::vec3 &c0 = objectColors[i];
        const glm::vec3 &c1 = objectColors[i+1];
        const glm::vec3 &c2 = objectColors[i+2];

        float area = edgeFunction(v0, v1, v2);
        for (int x = xMinRaster; x < xMaxRaster; ++x) {
            for (int y = yMinRaster; y < yMaxRaster; ++y) {
                glm::vec3 pixel(x + 0.5f, y + 0.5f, 0.0f);
                float w0 = edgeFunction(v1Raster, v2Raster, pixel);
                float w1 = edgeFunction(v2Raster, v0Raster, pixel);
                float w2 = edgeFunction(v0Raster, v1Raster, pixel);

                if (w0 >= 0.0f && w1 >= 0.0f && w2 >= 0.0f) {
                    float lambda0 = w0 / area;
                    float lambda1 = w1 / area;
                    float lambda2 = w2 / area;

                    float r = lambda0 * c0.r + lambda1 * c1.r + lambda2 * c2.r;
                    float g = lambda0 * c0.g + lambda1 * c1.g + lambda2 * c2.g;
                    float b = lambda0 * c0.b + lambda1 * c1.b + lambda2 * c2.b;

                    film.write(x, y, r, g, b);
                }
            }
        }
    }
    return rasterPixels;
}

glm::vec3 Projector::convertToRaster(const glm::vec3 &worldCoordinates) const {
    glm::vec3 cameraSpaceCoords = cameraSpace.getCoordinates(worldCoordinates);
    glm::vec2 screenSpaceCoords = screenSpace.getCoordinates(cameraSpaceCoords);
    glm::vec2 ndcSpaceCoords = ndcSpace.getCoordinates(screenSpaceCoords);
    return rasterSpace.getCoordinates(ndcSpaceCoords, cameraSpaceCoords.z);
}
