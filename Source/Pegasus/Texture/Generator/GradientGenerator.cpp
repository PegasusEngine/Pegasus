/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	GradientGenerator.cpp
//! \author	Karolyn Boulanger
//! \date	11th May 2014
//! \brief	Texture generator that renders a gradient

#include "Pegasus/Texture/Generator/GradientGenerator.h"
#include "Pegasus/Math/Plane.h"
#include "Pegasus/Texture/Shared/TextureEventDefs.h"

namespace Pegasus {
namespace Texture {


BEGIN_IMPLEMENT_PROPERTIES(GradientGenerator)
    IMPLEMENT_PROPERTY(GradientGenerator, Color0)
    IMPLEMENT_PROPERTY(GradientGenerator, Color1)
    IMPLEMENT_PROPERTY(GradientGenerator, Point0)
    IMPLEMENT_PROPERTY(GradientGenerator, Point1)
END_IMPLEMENT_PROPERTIES(GradientGenerator)

//----------------------------------------------------------------------------------------

void GradientGenerator::InitProperties()
{
    BEGIN_INIT_PROPERTIES(GradientGenerator)
        INIT_PROPERTY(Color0)
        INIT_PROPERTY(Color1)
        INIT_PROPERTY(Point0)
        INIT_PROPERTY(Point1)
    END_INIT_PROPERTIES()
}

//----------------------------------------------------------------------------------------

void GradientGenerator::GenerateData()
{
    PEGASUS_EVENT_DISPATCH(this, TextureNodeGenerationEvent, TextureNodeGenerationEvent::BEGIN);

    //! \todo Use a simpler syntax
    Graph::NodeDataRef dataRef = GetData();
    TextureData * data = static_cast<TextureData *>(&(*dataRef));
    PG_ASSERT(data != nullptr);

    // Conversion of the color parameters to floating point numbers
    const Math::ColorRGBA color0F(ToColorRGBA(GetColor0()));
    const Math::ColorRGBA color1F(ToColorRGBA(GetColor1()));
    const Math::ColorRGBA colorDiffF(color1F - color0F);

    const TextureConfiguration & configuration = GetConfiguration();
    const unsigned int width = configuration.GetWidth();
    const float widthRcp = 1.0f / static_cast<float>(width);
    const unsigned int height = configuration.GetHeight();
    const float heightRcp = 1.0f / static_cast<float>(height);
    const unsigned int depth = configuration.GetDepth();
    const float depthRcp = 1.0f / static_cast<float>(depth);
    const unsigned int numBytesPerPixel = configuration.GetNumBytesPerPixel();
    const unsigned int numLayers = configuration.GetNumLayers();

    // To calculate the gradient, we consider two parallel planes,
    // the first one for which all points use color0, and the second one for color1.
    // For each pixel of the texture, we will calculate the distance from the two planes,
    // and derive a lerp factor from them. That factor is then used to linearly interpolate
    // the input colors.
    const Math::Vec3 point0(GetPoint0());
    Math::Vec3 point1(GetPoint1());
    if (point0 == point1)
    {
        point1.x = point0.x + PEG_PLANE_NORMAL_EPSILON;
    }
    Math::Vec3 planeNormal(point1 - point0);
    const float planeNormalLengthRcp = RcpLength(planeNormal);
    planeNormal *= planeNormalLengthRcp;
    const Math::Plane plane0(planeNormal, point0);

    Math::PUInt32 * layerData32;
    unsigned int layer, x, y, z;
    Math::Vec3 currentPoint;
    float distance0, lerpFactor;
    Math::ColorRGBA colorF;
    Math::Color8RGBA color;

    // For each layer
    for (layer = 0; layer < numLayers; ++layer)
    {
        switch (numBytesPerPixel)
        {
            case 4:
                {
                    layerData32 = reinterpret_cast<Math::PUInt32 *>(data->GetLayerImageData(layer));

                    // For each pixel, compute the coordinates in normalized space
                    for (z = 0; z < depth; ++z)
                    {
                        currentPoint.z = (static_cast<float>(z) + 0.5f) * depthRcp;
                        for (y = 0; y < height; ++y)
                        {
                            currentPoint.y = (static_cast<float>(y) + 0.5f) * heightRcp;
                            for (x = 0; x < width; ++x)
                            {
                                currentPoint.x = (static_cast<float>(x) + 0.5f) * widthRcp;

                                // Compute the distance from the first plane
                                // (no need to compute the distance from the second plane,
                                //  as we know they are parallel and we know the distance between them)
                                distance0 = plane0.DistanceOfPoint(currentPoint);

                                // Scale the distance from the first plane (so a point in the second plane
                                // has a distance of 0 from the first plane) to obtain a lerp factor.
                                // Clamp the result to clamp the gradient.
                                lerpFactor = Math::Saturate(distance0 * planeNormalLengthRcp);

                                // Apply linear interpolation to the color
                                colorF = color0F + lerpFactor * colorDiffF;

                                // Convert the color to 8 bits
                                color = colorF;

                                // Store the pixel
                                *layerData32++ = color.rgba32;
                            }
                        }
                    }
                }
                break;
                
            default:
                PG_FAILSTR("Unsupported number of bytes per pixel (%d) for GradientGenerator", numBytesPerPixel);
        }
    }
    PEGASUS_EVENT_DISPATCH(this, TextureNodeGenerationEvent, TextureNodeGenerationEvent::END_SUCCESS);
}


}   // namespace Texture
}   // namespace Pegasus
