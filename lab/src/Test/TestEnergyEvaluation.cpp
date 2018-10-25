#include "Test/TestEnergyEvaluation.h"

using namespace SCaBOliC::Lab::Test;

TestEnergyEvaluation::TestEnergyEvaluation(const UserInput& ui)
{
    int estimatingBallRadius=3;
    MockDistribution frgDistribution;
    MockDistribution bkgDistribution;

    MyISQEnergy::ODRFactory odrFactory;


    Image2D image = DGtal::GenericReader<Image2D>::import(ui.imagePath);
    Domain domain = image.domain();

    DigitalSet ds( Domain(domain.lowerBound()-Point(0,0),
                          domain.upperBound()+Point(0,0) )
    );

    DIPaCUS::Representation::ImageAsDigitalSet(ds,image);

    cv::Mat cvImg = cv::imread(ui.imagePath);
    ODRModel odr = odrFactory.createODR(ui.om,
                                        ui.am,
                                        ui.ac,
                                        ui.cm,
                                        3,
                                        ds);

    ISQInputData input (odr,
                        cvImg,
                        estimatingBallRadius,
                        frgDistribution,
                        bkgDistribution,
                        0,
                        1);

    MyISQEnergy energy(input);
    Solution solution(input.optimizationRegions.domain);
    solution.init(energy.numVars());
    solution.labelsVector.setZero();

    if(ui.solverType==QPBOSolverType::Simple)
        energy.solve<QPBOSimpleSolver>(solution);
    else if(ui.solverType==QPBOSolverType::Probe)
        energy.solve<QPBOProbeSolver>(solution);
    else if(ui.solverType==QPBOSolverType::Improve)
        energy.solve<QPBOImproveSolver>(solution);
    else if(ui.solverType==QPBOSolverType::ImproveProbe)
        energy.solve<QPBOIP>(solution);

    Solution::LabelsVector& labelsVector = solution.labelsVector;

    double e1 = energy.sqEnergy(solution.labelsVector);
    double e2 = solution.energyValue;

    std::cout << "E1: " << e1 <<std::endl;
    std::cout << "E2: " << e2 <<std::endl;
}