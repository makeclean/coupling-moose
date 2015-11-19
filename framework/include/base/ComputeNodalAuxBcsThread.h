/****************************************************************/
/*               DO NOT MODIFY THIS HEADER                      */
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*           (c) 2010 Battelle Energy Alliance, LLC             */
/*                   ALL RIGHTS RESERVED                        */
/*                                                              */
/*          Prepared by Battelle Energy Alliance, LLC           */
/*            Under Contract No. DE-AC07-05ID14517              */
/*            With the U. S. Department of Energy               */
/*                                                              */
/*            See COPYRIGHT for full restrictions               */
/****************************************************************/

#ifndef COMPUTENODALAUXBCSTHREAD_H
#define COMPUTENODALAUXBCSTHREAD_H

// MOOSE includes
#include "ThreadedNodeLoop.h"

class ComputeNodalAuxBcsThread : public ThreadedNodeLoop<ConstBndNodeRange, ConstBndNodeRange::const_iterator>
{
public:
  ComputeNodalAuxBcsThread(FEProblem & fe_problem, AuxiliarySystem & sys, const MooseObjectStorage<AuxKernel> & storage);

  // Splitting Constructor
  ComputeNodalAuxBcsThread(ComputeNodalAuxBcsThread & x, Threads::split split);

  virtual void onNode(ConstBndNodeRange::const_iterator & node_it);

  void join(const ComputeNodalAuxBcsThread & /*y*/);

protected:
  AuxiliarySystem & _aux_sys;

  /// Storage object containing active AuxKernel objects
  const MooseObjectStorage<AuxKernel> & _storage;
};

#endif //COMPUTENODALAUXBCSTHREAD_H
