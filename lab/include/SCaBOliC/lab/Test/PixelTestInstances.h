#ifndef SCABOLIC_TESTINSTANCES_H
#define SCABOLIC_TESTINSTANCES_H

#include <SCaBOliC/Optimization/solver/QPBOSolverType.h>
#include <SCaBOliC/Core/ODRPixels.h>
#include <SCaBOliC/lab/Utils/Generator.h>
#include <SCaBOliC/lab/model/UserInput.h>

namespace SCaBOliC
{
    namespace Lab
    {
        namespace Test
        {
            class PixelTestInstances
            {
            public:
                typedef SCaBOliC::Optimization::QPBOSolverType QPBOSolverType;
                typedef SCaBOliC::Core::ODRModel::OptimizationMode OptimizationMode;
                typedef SCaBOliC::Core::ODRModel::ApplicationMode ApplicationMode;
                typedef SCaBOliC::Core::ODRModel::ApplicationCenter ApplicationCenter;
                typedef SCaBOliC::Core::ODRModel::CountingMode CountingMode;
                typedef SCaBOliC::Core::ODRModel::LevelDefinition LevelDefinition;

                typedef SCaBOliC::Lab::Model::UserInput UserInput;

            private:
                typedef SCaBOliC::Lab::Utils::Generator<6> MyGenerator;
            public:
                PixelTestInstances(std::string imagePath);

                UserInput next(bool& success);

            private:
                static MyGenerator::Index indexLims[6];
                static QPBOSolverType vectorOfSolver[4];
                static OptimizationMode vectorOfOM[2];
                static ApplicationMode vectorOfAM[2];
                static LevelDefinition vectorOfLD[2];
                static bool vectorOfOptInAppl[2];
                static bool vectorOfInversion[2];

                MyGenerator gen;
                std::string imagePath;
                MyGenerator::Index currSequence[6];
            };
        }
    }
}

#endif //SCABOLIC_TESTINSTANCES_H
