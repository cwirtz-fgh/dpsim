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

void simPiLine3ph() {
  Real timeStep = 0.00005;
  Real finalTime = 1;
  String simName = "EMT_PiLine_Component_3ph";
  Logger::setLogDir("logs/" + simName);

  // Nodes
  auto n1 = SimNode::make("n1", PhaseType::ABC);
  auto n2 = SimNode::make("n2", PhaseType::ABC);
  auto vn1 = SimNode::make("vn1", PhaseType::ABC);

  // Components
  auto vs = Ph3::VoltageSource::make("v_1", Logger::Level::debug);
  vs->setParameters(
      CPS::Math::singlePhaseVariableToThreePhase(CPS::Math::polar(100000, 0)),
      50);

  // Parametrization of components
  Real resistance = 5;
  Real inductance = 0.16;
  Real capacitance = 1.0e-6;
  Real conductance = 1e-6;

  auto line = Ph3::PiLine::make("Line", Logger::Level::debug);
  line->setParameters(CPS::Math::singlePhaseParameterToThreePhase(resistance),
                      CPS::Math::singlePhaseParameterToThreePhase(inductance),
                      CPS::Math::singlePhaseParameterToThreePhase(capacitance),
                      CPS::Math::singlePhaseParameterToThreePhase(conductance));

  auto load = Ph3::Resistor::make("R_load", Logger::Level::debug);
  load->setParameters(CPS::Math::singlePhaseParameterToThreePhase(10000));

  // Topology
  vs->connect({SimNode::GND, n1});
  line->connect({n1, n2});
  load->connect({n2, SimNode::GND});

  auto sys = SystemTopology(50, SystemNodeList{n1, n2},
                            SystemComponentList{vs, line, load});

  // Logging
  auto logger = DataLogger::make(simName);
  logger->logAttribute("v1", n1->attribute("v"));
  logger->logAttribute("v2", n2->attribute("v"));
  logger->logAttribute("iline", line->attribute("i_intf"));

  Simulation sim(simName, Logger::Level::debug);
  sim.setSystem(sys);
  sim.setDomain(Domain::EMT);
  sim.setVariableTimeStep(true);
  sim.setTimeStep(timeStep);
  sim.setFinalTime(finalTime);
  sim.addLogger(logger);

  sim.run();
}

void simPiLineDiakoptics3ph() {
  Real timeStep = 0.00005;
  Real finalTime = 0.1;
  String simName = "EMT_PiLine_Diakoptics_3ph";
  Logger::setLogDir("logs/" + simName);

  // Nodes
  auto n1 = SimNode::make("n1", PhaseType::ABC);
  auto n2 = SimNode::make("n2", PhaseType::ABC);

  // Components
  auto vs = Ph3::VoltageSource::make("v_1", Logger::Level::debug);
  vs->setParameters(
      CPS::Math::singlePhaseVariableToThreePhase(CPS::Math::polar(100000, 0)),
      50);

  // Parametrization of components
  Real resistance = 5;
  Real inductance = 0.16;
  Real capacitance = 1.0e-6;
  Real conductance = 1e-6;

  auto line = Ph3::PiLine::make("Line", Logger::Level::debug);
  line->setParameters(CPS::Math::singlePhaseParameterToThreePhase(resistance),
                      CPS::Math::singlePhaseParameterToThreePhase(inductance),
                      CPS::Math::singlePhaseParameterToThreePhase(capacitance),
                      CPS::Math::singlePhaseParameterToThreePhase(conductance));

  auto load = Ph3::Resistor::make("R_load", Logger::Level::debug);
  load->setParameters(CPS::Math::singlePhaseParameterToThreePhase(10000));

  // Topology
  vs->connect({SimNode::GND, n1});
  line->connect({n1, n2});
  load->connect({n2, SimNode::GND});

  auto sys =
      SystemTopology(50, SystemNodeList{n1, n2}, SystemComponentList{vs, load});
  sys.addTearComponent(line);

  // Logging
  auto logger = DataLogger::make(simName);
  logger->logAttribute("v1", n1->attribute("v"));
  logger->logAttribute("v2", n2->attribute("v"));
  logger->logAttribute("iline", line->attribute("i_intf"));

  Simulation sim(simName);
  sim.setSystem(sys);
  sim.setDomain(Domain::EMT);
  sim.setTearingComponents(sys.mTearComponents);
  sim.setTimeStep(timeStep);
  sim.setFinalTime(finalTime);
  sim.addLogger(logger);

  sim.run();
}

int main(int argc, char *argv[]) {
  // Test for variable Timestep: normal piline -> then diakoptics
  // new solver?
  simPiLine3ph();
  //simPiLineDiakoptics3ph();
}
