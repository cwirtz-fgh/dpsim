/* Copyright 2017-2021 Institute for Automation of Complex Power Systems,
 *                     EONERC, RWTH Aachen University
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *********************************************************************************/

#include "../Examples.h"
#include <DPsim.h>

using namespace DPsim;
using namespace CPS;
using namespace CPS::CIM;

// General grid parameters
Real VnomMV = 24e3;
Real VnomHV = 230e3;
Real nomFreq = 50;
Real ratio = VnomMV / VnomHV;
Real nomOmega = nomFreq * 2 * PI;

Examples::Grids::CIGREHVAmerican::LineParameters lineCIGREHV;

// HV line parameters referred to MV side
Real lineLength = 50;
Real lineResistance =
    lineCIGREHV.lineResistancePerKm * lineLength * std::pow(ratio, 2);
Real lineInductance =
    lineCIGREHV.lineReactancePerKm * lineLength * std::pow(ratio, 2) / nomOmega;
Real lineCapacitance = lineCIGREHV.lineSusceptancePerKm * lineLength /
                       std::pow(ratio, 2) / nomOmega;
Real lineConductance =
    8e-2; //change to allow bigger time steps and to stabilize simulation (8e-2 used for 10us)

// Breaker to trigger fault between the two lines
Real BreakerOpen = 1e9;
Real BreakerClosed = 0.001;

//Synchronous generators
Real setPointActivePower = 300e6;
Real setPointVoltage = 1.05 * VnomMV;

//Transformers
Real voltageHVSide = 230e3;
Real voltageMVSide = 24e3;
Real trafoResistance = 1;
Real trafoInductance = 0.1;
Real trafoPower = 5e6;
//Real ratio = voltageHVSide / voltageMVSide;

//Loads
Real PloadL5 = 300e6;
Real QloadL5 = 0;

Real PloadL6 = 300e6;
Real QloadL6 = 0;

Real PloadL8 = 300e6;
Real QloadL8 = 0;

//Simulation parameters
String simName = "EMT_3Ph_Switch_Test";
Real finalTime = 0.4;
Real timeStep = 10e-5;
Real startTimeFault = 0.2;

int main() {
  // ----- Dynamic simulation ------
  Logger::setLogDir("logs/" + simName);

  // Nodes
  //auto n1 = SimNode<Real>::make("n1", PhaseType::ABC);
  //auto n2 = SimNode<Real>::make("n2", PhaseType::ABC);
  //auto n3 = SimNode<Real>::make("n3", PhaseType::ABC);
  auto n4 = SimNode<Real>::make("n4", PhaseType::ABC);
  auto n5 = SimNode<Real>::make("n5", PhaseType::ABC);
  auto n6 = SimNode<Real>::make("n6", PhaseType::ABC);
  auto n7 = SimNode<Real>::make("n7", PhaseType::ABC);
  auto n8 = SimNode<Real>::make("n8", PhaseType::ABC);
  auto n9 = SimNode<Real>::make("n9", PhaseType::ABC);

  // Components
  //Grid bus as Slack
  auto extnet1 =
      EMT::Ph3::NetworkInjection::make("Slack4", Logger::Level::debug);
  extnet1->setParameters(Math::singlePhaseVariableToThreePhase(voltageHVSide));
  auto extnet2 =
      EMT::Ph3::NetworkInjection::make("Slack7", Logger::Level::debug);
  extnet2->setParameters(Math::singlePhaseVariableToThreePhase(voltageHVSide));
  auto extnet3 =
      EMT::Ph3::NetworkInjection::make("Slack9", Logger::Level::debug);
  extnet3->setParameters(Math::singlePhaseVariableToThreePhase(voltageHVSide));

  // Lines
  auto line45 = EMT::Ph3::PiLine::make("PiLine45", Logger::Level::debug);
  line45->setParameters(
      Math::singlePhaseParameterToThreePhase(lineResistance),
      Math::singlePhaseParameterToThreePhase(lineInductance),
      Math::singlePhaseParameterToThreePhase(lineCapacitance),
      Math::singlePhaseParameterToThreePhase(lineConductance));

  auto line46 = EMT::Ph3::PiLine::make("line46", Logger::Level::debug);
  line46->setParameters(
      Math::singlePhaseParameterToThreePhase(lineResistance),
      Math::singlePhaseParameterToThreePhase(lineInductance),
      Math::singlePhaseParameterToThreePhase(lineCapacitance),
      Math::singlePhaseParameterToThreePhase(lineConductance));

  auto line57 = EMT::Ph3::PiLine::make("line57", Logger::Level::debug);
  line57->setParameters(
      Math::singlePhaseParameterToThreePhase(lineResistance),
      Math::singlePhaseParameterToThreePhase(lineInductance),
      Math::singlePhaseParameterToThreePhase(lineCapacitance),
      Math::singlePhaseParameterToThreePhase(lineConductance));

  auto line69 = EMT::Ph3::PiLine::make("line69", Logger::Level::debug);
  line69->setParameters(
      Math::singlePhaseParameterToThreePhase(lineResistance),
      Math::singlePhaseParameterToThreePhase(lineInductance),
      Math::singlePhaseParameterToThreePhase(lineCapacitance),
      Math::singlePhaseParameterToThreePhase(lineConductance));

  auto line78 = EMT::Ph3::PiLine::make("line78", Logger::Level::debug);
  line78->setParameters(
      Math::singlePhaseParameterToThreePhase(lineResistance),
      Math::singlePhaseParameterToThreePhase(lineInductance),
      Math::singlePhaseParameterToThreePhase(lineCapacitance),
      Math::singlePhaseParameterToThreePhase(lineConductance));

  auto line89 = EMT::Ph3::PiLine::make("line89", Logger::Level::debug);
  line89->setParameters(
      Math::singlePhaseParameterToThreePhase(lineResistance),
      Math::singlePhaseParameterToThreePhase(lineInductance),
      Math::singlePhaseParameterToThreePhase(lineCapacitance),
      Math::singlePhaseParameterToThreePhase(lineConductance));

  auto loadRX5 = EMT::Ph3::RXLoad::make("r_5", Logger::Level::debug);
  loadRX5->setParameters(Math::singlePhaseParameterToThreePhase(PloadL5),
                         Math::singlePhaseParameterToThreePhase(QloadL5),
                         voltageHVSide);
  auto loadRX6 = EMT::Ph3::RXLoad::make("r_6", Logger::Level::debug);
  loadRX6->setParameters(Math::singlePhaseParameterToThreePhase(PloadL6),
                         Math::singlePhaseParameterToThreePhase(QloadL6),
                         voltageHVSide);
  auto loadRX8 = EMT::Ph3::RXLoad::make("r_8", Logger::Level::debug);
  loadRX8->setParameters(Math::singlePhaseParameterToThreePhase(PloadL8),
                         Math::singlePhaseParameterToThreePhase(QloadL8),
                         voltageHVSide);

  //Breaker
  auto fault = CPS::EMT::Ph3::Switch::make("Br_fault", Logger::Level::debug);
  fault->setParameters(Math::singlePhaseParameterToThreePhase(BreakerOpen),
                       Math::singlePhaseParameterToThreePhase(BreakerClosed));
  fault->openSwitch();

  // Topology
  extnet1->connect({n4});
  extnet2->connect({n7});
  extnet3->connect({n9});
  line45->connect({n4, n5});
  line46->connect({n4, n6});
  line57->connect({n5, n7});
  line69->connect({n6, n9});
  line78->connect({n7, n8});
  line89->connect({n8, n9});
  loadRX5->connect({n5});
  loadRX6->connect({n6});
  loadRX8->connect({n8});
  fault->connect({n5, EMT::SimNode::GND});
  auto system =
    SystemTopology(nomFreq, SystemNodeList{n4, n5, n6, n7, n8, n9},
                   SystemComponentList{extnet1, extnet2, extnet3, line45,
                                       line46, line57, line69, line78, line89,
                                       loadRX5, loadRX6, loadRX8, fault});

  // Logging
  auto logger = DataLogger::make(simName);
  logger->logAttribute("v4", n4->attribute("v"));
  logger->logAttribute("v5", n5->attribute("v"));
  logger->logAttribute("v6", n6->attribute("v"));
  logger->logAttribute("v7", n7->attribute("v"));
  logger->logAttribute("v8", n8->attribute("v"));
  logger->logAttribute("v9", n9->attribute("v"));

  // Events
  auto sw1 = SwitchEvent3Ph::make(startTimeFault, fault, true);

  // Simulation
  Simulation sim(simName, Logger::Level::debug);
  sim.setSystem(system);
  sim.setTimeStep(timeStep);
  sim.setFinalTime(finalTime);
  sim.setDomain(Domain::EMT);
  //sim.doSplitSubnets();
  sim.addLogger(logger);
  sim.addEvent(sw1);
  sim.doSystemMatrixRecomputation(true);
  sim.run();
}
