#ifndef SCABOLIC_ODRDISTANCE_H
#define SCABOLIC_ODRDISTANCE_H

#include <DGtal/helpers/StdDefs.h>
#include <DGtal/images/ImageContainerBySTLVector.h>
#include <DGtal/geometry/volumes/distance/DistanceTransformation.h>

#include "DIPaCUS/components/SetOperations.h"
#include "DIPaCUS/components/Morphology.h"
#include "DIPaCUS/components/Neighborhood.h"
#include "DIPaCUS/derivates/Misc.h"

#include <geoc/api/gridCurve/Length.hpp>
#include <geoc/api/gridCurve/Curvature.hpp>

#include "SCaBOliC/Core/model/ODRModel.h"
#include "SCaBOliC/Core/interface/ODRInterface.h"
#include "SCaBOliC/Core/ODRUtils.h"
#include "PixelSpaceHandle.h"

namespace SCaBOliC
{
    namespace Core
    {
        class ODRPixels: public SCaBOliC::Core::ODRInterface
        {
        public:
            typedef DGtal::Z2i::Point Point;
            typedef DGtal::Z2i::Domain Domain;
            typedef DGtal::Z2i::DigitalSet DigitalSet;
            typedef DGtal::DistanceTransformation<DGtal::Z2i::Space, DigitalSet, DGtal::Z2i::L2Metric> DTL2;

            typedef ODRModel::ApplicationMode ApplicationMode;
            typedef ODRModel::NeighborhoodType NeighborhoodType;
            typedef ODRModel::LevelDefinition  LevelDefinition;

            typedef DIPaCUS::Neighborhood::FourNeighborhoodPredicate FourNeighborhood;
            typedef DIPaCUS::Neighborhood::EightNeighborhoodPredicate EightNeighborhood;

        public:
            ODRPixels(double radius,
                      double gridStep,
                      const double levels,
                      LevelDefinition ld,
                      const NeighborhoodType nt,
                      const double optBand);


            ODRModel createODR(ApplicationMode appMode,
                               const DigitalSet& original,
                               bool optRegionInApplication=false,
                               const DigitalSet& pixelMask=ODRInterface::mockPixelMask) const;

            const SpaceHandleInterface* handle() const{return &spaceHandle;};

        private:
            DTL2 interiorDistanceTransform(const Domain& domain, const DigitalSet& original) const;

            DTL2 exteriorDistanceTransform(const Domain& domain, const DigitalSet& original) const;

            DigitalSet level(const DTL2& dtL2, int lessThan, int greaterThan=0) const;

            DigitalSet getBoundary(const Domain& domain, const DigitalSet& ds) const;

            DigitalSet omOriginalBoundary(const Domain& domain,
                                          const DigitalSet& original) const;

            DigitalSet omDilationBoundary(const DTL2& dtL2) const;

            DigitalSet amOriginalBoundary(const Domain& domain,
                                          const DigitalSet& original) const;


            DigitalSet amAroundBoundary(const DTL2& interiorTransform,
                                        const DTL2& exteriorTransform,
                                        const unsigned int radius,
                                        const LevelDefinition ld,
                                        int length) const;

            DigitalSet amLevel(const DTL2& distanceTransform,
                               const unsigned int radius,
                               const LevelDefinition ld,
                               double levelNum) const;

            double curveLength(const Domain& domain, const Curve& curve) const;
            std::unordered_map<Point,double> curvatureMap(const Domain& domain, const Curve& curve, const DigitalSet& interior) const;


        private:
            double levels;
            double optBand;
            LevelDefinition ld;
            NeighborhoodType nt;

            PixelSpaceHandle spaceHandle;
        };
    }
}

#endif //SCABOLIC_ODRDISTANCE_H
