#include <DGtal/io/boards/Board2D.h>
#include "SCaBOliC/Core/ODRPixels.h"

using namespace SCaBOliC::Core;

DIPaCUS::Morphology::StructuringElement ODRPixels::dilationSE = DIPaCUS::Morphology::StructuringElement::RECT;
DIPaCUS::Morphology::StructuringElement ODRPixels::erosionSE = DIPaCUS::Morphology::StructuringElement::RECT;


ODRPixels::Point ODRPixels::neighborhoodFilter[5] = {ODRPixels::Point(0,1),
                                                     ODRPixels::Point(1,0),
                                                     ODRPixels::Point(-1,0),
                                                     ODRPixels::Point(0,-1),
                                                     ODRPixels::Point(0,0)};

ODRPixels::DigitalSet ODRPixels::omOriginalBoundary(const DigitalSet& original)
{
    DigitalSet originalBoundary(original.domain());
    EightNeighborhood en(originalBoundary,original);

    return originalBoundary;
}

ODRPixels::DigitalSet ODRPixels::omDilationBoundary(const DigitalSet& original)
{
    DigitalSet dilated(original.domain());
    DigitalSet dilatedBoundary(original.domain());

    DIPaCUS::Morphology::Dilate(dilated,
                                original,
                                dilationSE,
                                1);

    EightNeighborhood en(dilatedBoundary,dilated);

    return dilatedBoundary;
}

ODRPixels::DigitalSet ODRPixels::omFullDomain(const Domain& originalDomain)
{
    DigitalSet fullDomain(originalDomain);
    fullDomain.insert(originalDomain.begin(),originalDomain.end());

    return fullDomain;
}

ODRPixels::DigitalSet ODRPixels::amOriginalBoundary(const DigitalSet& original)
{
    return omOriginalBoundary(original);
}


ODRPixels::DigitalSet ODRPixels::amFullDomain(const Domain& applicationDomain)
{
    return omFullDomain(applicationDomain);
}

ODRPixels::DigitalSet ODRPixels::amAroundBoundary(const DigitalSet& original,
                                                    const DigitalSet& optRegion,
                                                    int length)
{
    DigitalSet internRegion = amInternRange(original,optRegion,length);
    DigitalSet externRegion = amExternRange(original,optRegion,length);

    for(auto it=optRegion.begin();it!=optRegion.end();++it)
    {
        internRegion.erase(*it);
        externRegion.erase(*it);
    }

    DigitalSet aroundBoundary(original.domain());

    aroundBoundary.insert(externRegion.begin(),externRegion.end());
    aroundBoundary.insert(internRegion.begin(),internRegion.end());

    return aroundBoundary;
}

ODRPixels::DigitalSet ODRPixels::amInternRange(const DigitalSet& original, const DigitalSet& optRegion, int length)
{
    DigitalSet originalPlusOptRegion(original.domain());
    originalPlusOptRegion.insert(original.begin(),original.end());
    originalPlusOptRegion.insert(optRegion.begin(),optRegion.end());

    DigitalSet eroded (originalPlusOptRegion.domain());
    DIPaCUS::Morphology::Erode(eroded,originalPlusOptRegion,erosionSE,length+1);

    DigitalSet internRegion(originalPlusOptRegion.domain());
    DIPaCUS::SetOperations::SetDifference(internRegion,originalPlusOptRegion,eroded);

    return internRegion;
}

ODRPixels::DigitalSet ODRPixels::amExternRange(const DigitalSet& original, const DigitalSet& optRegion, int length)
{
    DigitalSet originalPlusOptRegion(original.domain());
    originalPlusOptRegion.insert(original.begin(),original.end());
    originalPlusOptRegion.insert(optRegion.begin(),optRegion.end());

    DigitalSet dilated (originalPlusOptRegion.domain());
    DIPaCUS::Morphology::Dilate(dilated,originalPlusOptRegion,dilationSE,length);

    DigitalSet externRegion(originalPlusOptRegion.domain());
    DIPaCUS::SetOperations::SetDifference(externRegion,dilated,originalPlusOptRegion);

    return externRegion;
}

ODRPixels::DigitalSet ODRPixels::isolatedPoints(const DigitalSet& original, const DigitalSet& optRegion)
{
    DigitalSet dilated(original.domain());

    DIPaCUS::Morphology::Dilate(dilated,
                                original,
                                dilationSE,
                                1);
    
    DigitalSet tempDS(original.domain());
    DigitalSet isolatedDS(original.domain());
    DIPaCUS::SetOperations::SetDifference(tempDS,dilated,original);
    DIPaCUS::SetOperations::SetDifference(isolatedDS,tempDS,optRegion);
    
    return isolatedDS;
    
}

ODRModel ODRPixels::createODR (OptimizationMode optMode,
                               ApplicationMode appMode,unsigned int radius,
                               const DigitalSet& original) const
{
    typedef DIPaCUS::Misc::DigitalBoundary<DIPaCUS::Neighborhood::EightNeighborhoodPredicate<DigitalSet>> EightNeighborhood;

    Domain domain(original.domain().lowerBound() - 2*Point(radius,radius),
                  original.domain().upperBound() + 2*Point(radius,radius));

    Domain applicationDomain(original.domain().lowerBound(),
                             original.domain().upperBound());



    DigitalSet originalInLargerDomain (domain);
    originalInLargerDomain.insert(original.begin(),original.end());


    DigitalSet optRegion(domain);
    DigitalSet applicationRegion(applicationDomain);


    switch (optMode) {
        case OptimizationMode::OM_OriginalBoundary: {
            optRegion = omOriginalBoundary(original);
            break;
        }
        case OptimizationMode::OM_DilationBoundary: {
            optRegion = omDilationBoundary(original);
            break;
        }
    }


    switch (appMode) {
        case ApplicationMode::AM_OptimizationBoundary: {
            DigitalSet temp = amOriginalBoundary(optRegion);
            applicationRegion.insert(temp.begin(),temp.end());
            break;
        }
        case ApplicationMode::AM_FullDomain: {
            DigitalSet temp = amFullDomain(applicationDomain);
            applicationRegion.insert(temp.begin(),temp.end());
            break;
        }
        case ApplicationMode::AM_AroundBoundary: {
            DigitalSet temp = amAroundBoundary(original,optRegion,radius);
            applicationRegion.insert(temp.begin(),temp.end());
            break;
        }
        case ApplicationMode::AM_InverseAroundBoundary: {
            DigitalSet temp = amAroundBoundary(original,optRegion,radius);
            applicationRegion.insert(temp.begin(),temp.end());
            break;
        }
        case ApplicationMode::AM_InternRange:{
            DigitalSet temp = amInternRange(original,optRegion,1);
            applicationRegion.insert(temp.begin(),temp.end());
            //for(auto it=optRegion.begin();it!=optRegion.end();++it) applicationRegion.erase(*it);
            break;
        }
        case ApplicationMode::AM_ExternRange:{
            DigitalSet temp = amExternRange(original,optRegion,radius);
            applicationRegion.insert(temp.begin(),temp.end());
            break;
        }
    }
    
    DigitalSet extendedOriginal(original.domain());
    extendedOriginal.insert(original.begin(),original.end());
    extendedOriginal.insert(optRegion.begin(),optRegion.end());

    DigitalSet trustFRG(domain);
    DIPaCUS::SetOperations::SetDifference(trustFRG, extendedOriginal, optRegion);
    
    if(optMode==OptimizationMode::OM_DilationBoundary)
    {
        DigitalSet isolatedDS = isolatedPoints(original,optRegion);
        trustFRG+=isolatedDS;
    }

    DigitalSet trustBKG(domain);
    DigitalSet tempp(domain);

    tempp += trustFRG;
    tempp += optRegion;

    trustBKG.assignFromComplement(tempp);


    if(appMode==ApplicationMode::AM_InverseAroundBoundary)
    {
        DigitalSet swap = trustFRG;
        trustFRG = trustBKG;
        trustBKG = swap;
    }

    return ODRModel(domain,
                                      original,
                                      optRegion,
                                      trustFRG,
                                      trustBKG,
                                      applicationRegion);
}

DIPaCUS::Misc::DigitalBallIntersection ODRPixels::intersectionComputer(unsigned int radius,
                                                                       const DigitalSet &toIntersect) const
{
    return DIPaCUS::Misc::DigitalBallIntersection(radius,toIntersect);
}