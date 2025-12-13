/* Copyright 2017-2021 Institute for Automation of Complex Power Systems,
 *                     EONERC, RWTH Aachen University
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *********************************************************************************/

#include <DPsim.h>

using namespace DPsim;
using namespace CPS::EMT;
using namespace CPS::EMT::Ph1;

int main(int argc, char *argv[]) {
  // Define simulation scenario
  Real timeStep = 0.0001;
  Real finalTime = 0.2;
  String simName = "EMT_VS_RL1_varStep";
  Logger::setLogDir("logs/" + simName);

  // Nodes
  auto n1 = SimNode::make("n1");
  auto n2 = SimNode::make("n2");

  // Components
  auto vs = VoltageSource::make("vs");
  vs->setParameters(Complex(10, 0), 50);
  auto r1 = Resistor::make("r_1");
  r1->setParameters(5);
  auto r2 = Inductor::make("r_1");
  r2->setParameters(2);

  // Topology
  vs->connect(SimNode::List{SimNode::GND, n1});
  r1->connect(SimNode::List{n1, n2});
  r2->connect(SimNode::List{n2, SimNode::GND});

  // Define system topology
  auto sys = SystemTopology(50, SystemNodeList{n1, n2},
                            SystemComponentList{vs, r1, r2});

  // Logger
  auto logger = DataLogger::make(simName);
  logger->logAttribute("v1", n1->attribute("v"));
  logger->logAttribute("v2", n2->attribute("v"));
  logger->logAttribute("i12", r1->attribute("i_intf"));

  Simulation sim(simName);
  sim.doVarTimestepSimulation(true);
  sim.setMaxMinTimesteps(timeStep * 10, timeStep*0.1);
  sim.setSystem(sys);
  sim.setTimeStep(timeStep);
  sim.setFinalTime(finalTime);
  sim.setDomain(Domain::EMT);
  sim.addLogger(logger);

  sim.run();

  return 0;
}
