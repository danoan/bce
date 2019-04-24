#include "SCaBOliC/Core/ODRUtils.h"

using namespace SCaBOliC::Core;

template<typename TNeighborhood>
ODRUtils::DigitalSet ODRUtils::omOriginalBoundary(const DigitalSet& original)
{
    DigitalSet originalBoundary(original.domain());
    DIPaCUS::Misc::digitalBoundary<TNeighborhood>(originalBoundary,original);

    return originalBoundary;
}

template<typename TNeighborhood>
ODRUtils::DigitalSet ODRUtils::omDilationBoundary(const DigitalSet& original,
                                                  const StructuringElement::Type& st)
{
    DigitalSet dilated(original.domain());
    DigitalSet dilatedBoundary(original.domain());

    DIPaCUS::Morphology::dilate(dilated,
                                original,
                                StructuringElement(st,1));

    DIPaCUS::Misc::digitalBoundary<TNeighborhood>(dilatedBoundary,dilated);

    return dilatedBoundary;
}

template<typename TNeighborhood>
ODRUtils::DigitalSet ODRUtils::amOriginalBoundary(const DigitalSet& original)
{
    return omOriginalBoundary<TNeighborhood>(original);
}