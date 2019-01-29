#include "SCaBOliC/Energy/ISQ/Terms/Length/LengthTerm.h"

using namespace SCaBOliC::Energy::ISQ;


LengthTerm::LengthTerm(const InputData &id,
                       const SpaceHandleInterface* spaceHandle):vm(id.optimizationRegions),
                                                                spaceHandle(spaceHandle)
{
    initializeOptimizationData(id,this->vm,this->od);
    configureOptimizationData(id,this->vm,this->od);
}


void LengthTerm::initializeOptimizationData(const InputData& id,
                                            const VariableMap& vm,
                                            OptimizationData& od)
{
    od.numVars = vm.numVars;

    od.localUTM = OptimizationData::UnaryTermsMatrix(2,
                                                     od.numVars);
    od.localUTM.setZero();

    od.localPTM = OptimizationData::PairwiseTermsMatrix(od.numVars,
                                                        od.numVars);
    od.localPTM.setZero();
}

void LengthTerm::configureOptimizationData(const InputData& id,
                                           const VariableMap& vm,
                                           OptimizationData& od)
{
    this->constantFactor = 1;
    this->constantTerm = 0;

    double maxCtrb;
    setCoeffs(od,
              maxCtrb,
              id,
              vm);

    this->normalizationFactor = 1.0/maxCtrb;
    this->weight = id.lengthTermWeight;

    od.localUTM*=this->weight*this->normalizationFactor;
    od.localPTM*=this->weight*this->normalizationFactor;

}


void LengthTerm::setCoeffs(OptimizationData& od,
                           double& maxCtrb,
                           const InputData& id,
                           const VariableMap& vm)
{
    typedef InputData::OptimizationDigitalRegions::Point Point;

    const InputData::OptimizationDigitalRegions ODR = id.optimizationRegions;
    int col,row;
    Index xi,yi;

    Point neigh;
    maxCtrb=0;
    for(auto it = ODR.optRegion.begin();it!=ODR.optRegion.end();++it)
    {
        col = (*it)[0];
        row = (*it)[1];

        xi = vm.pim.at(*it);

        for(auto itp=this->spaceHandle->neighBegin();itp!=this->spaceHandle->neighEnd();++itp)
        {
            neigh = *it + *itp;
            if(ODR.trustFRG(neigh))
            {
                od.localUTM(1,xi) += 1;
            }else if(ODR.trustBKG(neigh))
            {
                od.localUTM(0,xi) += 1;
            }else
            {
                yi = vm.pim.at(neigh);

                od.localUTM(1,xi) += 1;
                od.localUTM(1,yi) += 1;

                maxCtrb = od.localUTM(1,yi)>maxCtrb?od.localUTM(1,yi):maxCtrb;
                addCoeff(od.localPTM,maxCtrb,xi,yi,-2);
            }

            maxCtrb = od.localUTM(1,xi)>maxCtrb?od.localUTM(1,xi):maxCtrb;
            maxCtrb = od.localUTM(0,xi)>maxCtrb?od.localUTM(0,xi):maxCtrb;
        }

    }

}

void LengthTerm::addCoeff(OptimizationData::PairwiseTermsMatrix& PTM,
                          double& maxPTM,
                          Index i1,
                          Index i2,
                          Scalar v)
{
    this->crescentOrder(i1,i2);
    PTM.coeffRef(i1,i2) += v;
    maxPTM = fabs(PTM.coeffRef(i1,i2))>maxPTM?fabs(PTM.coeffRef(i1,i2)):maxPTM;
}