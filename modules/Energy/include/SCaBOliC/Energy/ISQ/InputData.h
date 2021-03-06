#ifndef SCABOLIC_ENERGY_MODEL_H
#define SCABOLIC_ENERGY_MODEL_H

#include <eigen3/Eigen/Sparse>
#include <opencv2/core/mat.hpp>

#include "SCaBOliC/Core/model/ODRModel.h"
#include "SCaBOliC/Energy/ISQ/Terms/Data/IProbabilityDistribution.h"

namespace SCaBOliC
{
    namespace Energy
    {
        namespace ISQ
        {
            class InputData
            {
            public:
                typedef double Scalar;
                typedef unsigned int Index;

                typedef SCaBOliC::Core::ODRModel OptimizationDigitalRegions;
                typedef IProbabilityDistribution<Scalar> MyProbabilityDistribution;

                typedef DGtal::Z2i::Domain Domain;
                typedef DGtal::Z2i::Point Point;

                typedef cv::Mat cvColorImage;
                typedef cv::Vec3b cvColorType;

                enum PenalizationMode{No_Penalization,Penalize_Ones,Penalize_Zeros};

            public:
                InputData(const OptimizationDigitalRegions& ODR,
                          const cvColorImage& image,
                          const MyProbabilityDistribution& fgDistr,
                          const MyProbabilityDistribution& bgDistr,
                          bool excludeOptPointsFromAreaComputation,
                          bool uniformPerimeter,
                          double dataTermWeight=1.0,
                          double sqTermWeight=1.0,
                          double lengthTermWeight=1.0,
                          double innerBallCoeff=1.0,
                          double outerBallCoeff=1.0,
                          Point translation=Point(0,0)):optimizationRegions(ODR),
                                                        image(image),
                                                        fgDistr(fgDistr),
                                                        bgDistr(bgDistr),
                                                        excludeOptPointsFromAreaComputation(excludeOptPointsFromAreaComputation),
                                                        uniformPerimeter(uniformPerimeter),
                                                        dataTermWeight(dataTermWeight),
                                                        sqTermWeight(sqTermWeight),
                                                        lengthTermWeight(lengthTermWeight),
                                                        innerBallCoeff(innerBallCoeff),
                                                        outerBallCoeff(outerBallCoeff),
                                                        translation(translation){}

            public:
                const OptimizationDigitalRegions optimizationRegions;

                const double dataTermWeight;
                const double sqTermWeight;
                const double lengthTermWeight;

                const double innerBallCoeff,outerBallCoeff;

                const MyProbabilityDistribution& fgDistr;
                const MyProbabilityDistribution& bgDistr;

                const cvColorImage& image;
                const Point translation;

                const bool excludeOptPointsFromAreaComputation;
                const bool uniformPerimeter;

            };
        }
    }
}
#endif //SCABOLIC_ENERGY_MODEL_H
