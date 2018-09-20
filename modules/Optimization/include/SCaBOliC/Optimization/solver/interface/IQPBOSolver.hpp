#include "SCaBOliC/Optimization/solver/interface/IQPBOSolver.h"

using namespace SCaBOliC::Optimization;

template <typename Unary, typename Graph, typename Labels>
IQPBOSolver<Unary,Graph,Labels>::IQPBOSolver(Scalar &energyValue,
                                             int &unlabelled,
                                             const Unary &U,
                                             const Graph &G,
                                             Labels &labels,
                                             int max_num_iterations)
{
    numVariables = U.cols();
    mapping = (int*) malloc(sizeof(int)*this->numVariables);
    for(int i=0;i<numVariables;++i) mapping[i] = 2*i;

    qpbo = std::unique_ptr<QPBO<Scalar>>( new QPBO<Scalar>(numVariables, G.nonZeros()) );

    qpbo->AddNode(numVariables);
    for(int i=0;i<numVariables;++i)
    {
        qpbo->AddUnaryTerm( i,U(0,i),U(1,i) );
    }

    for( int j=0;j<numVariables;++j )
    {
        for (typename Graph::InnerIterator it(
                G, static_cast<typename Graph::Index>(j)); it; ++it)
        {
            Index i1 = it.row();
            Index i2 = it.col();

            qpbo->AddPairwiseTerm(
                    static_cast<typename QPBO<Scalar>::NodeId>(i1),
                    static_cast<typename QPBO<Scalar>::NodeId>(i2),
                    0,
                    0,
                    0,
                    it.value()
            );
        }
    }

}

template <typename Unary, typename Graph, typename Labels>
void IQPBOSolver<Unary,Graph,Labels>::fillLabels(int& unlabelled,Labels& labels)
{
    Labels originalLabels = labels;

    unlabelled=0;
    for (Index j = 0; j < numVariables; ++j)
    {
        int xi = qpbo->GetLabel( mapping[j]/2.0 );
        if(mapping[j]%2==0)
            labels[j] = xi;
        else
            labels[j] = (1+xi)%2;
        if (xi < 0)
        {
            labels[j] = originalLabels[j];
            unlabelled+= 1;
        }
    }
}