#include "SCaBOliC/Energy/ISQ/Terms/SquaredCurvature/SquaredCurvatureTerm.h"

using namespace SCaBOliC::Energy::ISQ;

SquaredCurvatureTerm::SquaredCurvatureTerm(const InputData &id,
                                           const SpaceHandleInterface* spaceHandle):vm(id.optimizationRegions),
                                                                                    spaceHandle(spaceHandle)
{
    initializeOptimizationData(id,this->vm,this->od);
    configureOptimizationData(id,this->vm,this->od);
}


void SquaredCurvatureTerm::initializeOptimizationData(const InputData& id,
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

void SquaredCurvatureTerm::update(const InputData& id,
                                  const VariableMap& vm,
                                  OptimizationData& od)
{
    CoefficientsComputer cc(id.optimizationRegions.applicationRegion,
                            id.optimizationRegions.trustFRG,
                            id.optimizationRegions.optRegion,
                            this->spaceHandle,
                            id.penalizationMode,
                            id.excludeOptPointsFromAreaComputation);

    double maxCtrb;
    setCoeffs(od,
              maxCtrb,
              id,
              cc,
              vm);
}


void SquaredCurvatureTerm::configureOptimizationData(const InputData& id,
                                                     const VariableMap& vm,
                                                     OptimizationData& od)
{
    CoefficientsComputer cc(id.optimizationRegions.applicationRegion,
                            id.optimizationRegions.trustFRG,
                            id.optimizationRegions.optRegion,
                            this->spaceHandle,
                            id.penalizationMode,
                            id.excludeOptPointsFromAreaComputation);

    double maxCtrb;
    setCoeffs(od,
              maxCtrb,
              id,
              cc,
              vm);

    if(!id.repeatedImprovement)
    {
        this->normalizationFactor = 1.0/maxCtrb;
        this->weight = id.sqTermWeight;

        this->constantFactor = cc.scalingFactor()*this->normalizationFactor;;
        this->constantTerm = cc.constantTerm()*this->normalizationFactor;

        od.localUTM*=this->weight*this->normalizationFactor;
        od.localPTM*=this->weight*this->normalizationFactor;
    }
}

void SquaredCurvatureTerm::setCoeffs(OptimizationData& od,
                                     double& maxCtrb,
                                     const InputData& id,
                                     const CoefficientsComputer& cc,
                                     const VariableMap& vm)
{
    const InputData::OptimizationDigitalRegions& ODR = id.optimizationRegions;

    DigitalSet temp(ODR.domain);
    DIPaCUS::Misc::DigitalBallIntersection DBIOptimization = this->spaceHandle->intersectionComputer(ODR.optRegion);

    const VariableMap::PixelIndexMap &iiv = vm.pim;
    OptimizationData::UnaryTermsMatrix &UTM = od.localUTM;
    OptimizationData::PairwiseTermsMatrix &PTM = od.localPTM;


    maxCtrb=0;
    for(auto yit=ODR.applicationRegion.begin();yit!=ODR.applicationRegion.end();++yit)
    {
        temp.clear();
        DBIOptimization(temp, *yit);

        for (auto xjt = temp.begin(); xjt != temp.end(); ++xjt)
        {
            Index xj = iiv.at(*xjt);

            UTM(1,xj) += cc.retrieve(*yit).xi;
            maxCtrb = fabs(UTM(1,xj))>maxCtrb?fabs(UTM(1,xj)):maxCtrb;

            auto ut = xjt;
            ++ut;
            for(;ut!=temp.end();++ut)
            {
                addCoeff(PTM,maxCtrb,xj,iiv.at(*ut),cc.retrieve(*yit).xi_xj);
            }
        }
    }

    for(auto iti=ODR.optRegion.begin();iti!=ODR.optRegion.end();++iti)
    {
        Index xi = iiv.at(*iti);
        UTM(1,xi) += id.penalizationWeight*cc.unaryPenalization();

        auto itj = iti;
        ++itj;
        for(;itj!=ODR.optRegion.end();++itj)
        {
            Index xj = iiv.at(*itj);
            this->crescentOrder(xi,xj);
            PTM.coeffRef(xi,xj) += id.penalizationWeight*cc.binaryPenalization();
        }
    }

}

void SquaredCurvatureTerm::addCoeff(OptimizationData::PairwiseTermsMatrix& PTM,
                                    double& maxPTM,
                                    Index i1,
                                    Index i2,
                                    Scalar v)
{
    this->crescentOrder(i1,i2);
    PTM.coeffRef(i1,i2) += v;
    maxPTM = fabs(PTM.coeffRef(i1,i2))>maxPTM?fabs(PTM.coeffRef(i1,i2)):maxPTM;
}
