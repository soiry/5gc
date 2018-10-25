/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2014 Piotr Gawlowicz
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Piotr Gawlowicz <gawlowicz.p@gmail.com>
 *
 */

#include <ns3/simulator.h>
#include <ns3/log.h>
#include <ns3/callback.h>
#include <ns3/config.h>
#include <ns3/string.h>
#include <ns3/double.h>
#include <ns3/enum.h>
#include <ns3/boolean.h>
#include <ns3/pointer.h>
#include <ns3/integer.h>

#include "ns3/mobility-helper.h"
#include "ns3/nr-helper.h"

#include <ns3/nr-ff-mac-scheduler.h>
#include <ns3/nr-enb-net-device.h>
#include <ns3/nr-enb-phy.h>
#include <ns3/nr-enb-rrc.h>
#include <ns3/nr-ue-net-device.h>
#include <ns3/nr-ue-phy.h>
#include <ns3/nr-ue-rrc.h>

#include "nr-ffr-simple.h"
#include <ns3/nr-common.h>

#include "nr-test-uplink-power-control.h"
#include <ns3/nr-rrc-sap.h>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("NrUplinkPowerControlTest");

/**
 * TestSuite
 */

NrUplinkPowerControlTestSuite::NrUplinkPowerControlTestSuite ()
  : TestSuite ("nr-uplink-power-control", SYSTEM)
{
//  LogLevel logLevel = (LogLevel)(LOG_PREFIX_FUNC | LOG_PREFIX_TIME | LOG_LEVEL_DEBUG);
//  LogComponentEnable ("NrUplinkPowerControlTest", logLevel);
  NS_LOG_INFO ("Creating NrUplinkPowerControlTestSuite");

  AddTestCase (new NrUplinkOpenLoopPowerControlTestCase ("OpenLoopTest1"), TestCase::QUICK);
  AddTestCase (new NrUplinkClosedLoopPowerControlAbsoluteModeTestCase ("ClosedLoopAbsoluteModeTest1"), TestCase::QUICK);
  AddTestCase (new NrUplinkClosedLoopPowerControlAccumulatedModeTestCase ("ClosedLoopAccumulatedModeTest1"), TestCase::QUICK);
}

static NrUplinkPowerControlTestSuite nrUplinkPowerControlTestSuite;

/**
 * TestCase Data
 */
void
PuschTxPowerNofitication (NrUplinkPowerControlTestCase *testcase,
                          uint16_t cellId, uint16_t rnti, double txPower)
{
  testcase->PuschTxPowerTrace (cellId, rnti, txPower);
}

void
PucchTxPowerNofitication (NrUplinkPowerControlTestCase *testcase,
                          uint16_t cellId, uint16_t rnti, double txPower)
{
  testcase->PucchTxPowerTrace (cellId, rnti, txPower);
}

void
SrsTxPowerNofitication (NrUplinkPowerControlTestCase *testcase,
                        uint16_t cellId, uint16_t rnti, double txPower)
{
  testcase->SrsTxPowerTrace (cellId, rnti, txPower);
}

NrUplinkPowerControlTestCase::NrUplinkPowerControlTestCase (std::string name)
  : TestCase (name)
{
  NS_LOG_INFO ("Creating NrUplinkPowerControlTestCase");
}

NrUplinkPowerControlTestCase::~NrUplinkPowerControlTestCase ()
{
}

void
NrUplinkPowerControlTestCase::TeleportUe (uint32_t x, uint32_t y,
                                           double expectedPuschTxPower, double expectedPucchTxPower, double expectedSrsTxPower)
{
  NS_LOG_FUNCTION (this);
  NS_LOG_DEBUG ("Teleport UE to : (" << x << ", " << y << ", 0)");

  m_ueMobility->SetPosition (Vector (x, y, 0.0));
  m_teleportTime = Simulator::Now ();

  m_expectedPuschTxPower = expectedPuschTxPower;
  m_expectedPucchTxPower = expectedPucchTxPower;
  m_expectedSrsTxPower = expectedSrsTxPower;
}

void
NrUplinkPowerControlTestCase::SetTpcConfiguration (uint32_t tpc, uint32_t tpcNum,
                                                    double expectedPuschTxPower, double expectedPucchTxPower, double expectedSrsTxPower)
{
  NS_LOG_FUNCTION (this);

  m_teleportTime = Simulator::Now ();

  m_expectedPuschTxPower = expectedPuschTxPower;
  m_expectedPucchTxPower = expectedPucchTxPower;
  m_expectedSrsTxPower = expectedSrsTxPower;

  m_ffrSimple->SetTpc (tpc, tpcNum, m_accumulatedMode);
}

void
NrUplinkPowerControlTestCase::PuschTxPowerTrace (uint16_t cellId, uint16_t rnti, double txPower)
{
  NS_LOG_FUNCTION (this);
  NS_LOG_DEBUG ("PuschTxPower : CellId: " << cellId << " RNTI: " << rnti << " PuschTxPower: " << txPower);
  //wait because of RSRP filtering
  if ( (Simulator::Now () - m_teleportTime ) < MilliSeconds (50))
    {
      return;
    }
  NS_TEST_ASSERT_MSG_EQ_TOL (txPower, m_expectedPuschTxPower, 0.01, "Wrong Pusch Tx Power");
}

void
NrUplinkPowerControlTestCase::PucchTxPowerTrace (uint16_t cellId, uint16_t rnti, double txPower)
{
  NS_LOG_FUNCTION (this);
  NS_LOG_DEBUG ("PucchTxPower : CellId: " << cellId << " RNTI: " << rnti << " PuschTxPower: " << txPower);
  //wait because of RSRP filtering
  if ( (Simulator::Now () - m_teleportTime ) < MilliSeconds (50))
    {
      return;
    }

  NS_TEST_ASSERT_MSG_EQ_TOL (txPower, m_expectedPucchTxPower, 0.01, "Wrong Pucch Tx Power");
}

void
NrUplinkPowerControlTestCase::SrsTxPowerTrace (uint16_t cellId, uint16_t rnti, double txPower)
{
  NS_LOG_FUNCTION (this);
  NS_LOG_DEBUG ("SrsTxPower : CellId: " << cellId << " RNTI: " << rnti << " PuschTxPower: " << txPower);
  //wait because of RSRP filtering
  if ( (Simulator::Now () - m_teleportTime ) < MilliSeconds (50))
    {
      return;
    }
  NS_TEST_ASSERT_MSG_EQ_TOL (txPower, m_expectedSrsTxPower, 0.01, "Wrong Srs Tx Power");
}

void
NrUplinkPowerControlTestCase::DoRun (void)
{
}


NrUplinkOpenLoopPowerControlTestCase::NrUplinkOpenLoopPowerControlTestCase (std::string name)
  : NrUplinkPowerControlTestCase ("Uplink Open Loop Power Control: " + name)
{
  NS_LOG_INFO ("Creating NrUplinkPowerControlTestCase");
}

NrUplinkOpenLoopPowerControlTestCase::~NrUplinkOpenLoopPowerControlTestCase ()
{
}

void
NrUplinkOpenLoopPowerControlTestCase::DoRun (void)
{
  Config::Reset ();
  Config::SetDefault ("ns3::NrHelper::UseIdealRrc", BooleanValue (false));

  double eNbTxPower = 30;
  Config::SetDefault ("ns3::NrEnbPhy::TxPower", DoubleValue (eNbTxPower));
  Config::SetDefault ("ns3::NrUePhy::TxPower", DoubleValue (10.0));
  Config::SetDefault ("ns3::NrUePhy::EnableUplinkPowerControl", BooleanValue (true));

  Config::SetDefault ("ns3::NrUePowerControl::ClosedLoop", BooleanValue (false));
  Config::SetDefault ("ns3::NrUePowerControl::AccumulationEnabled", BooleanValue (false));
  Config::SetDefault ("ns3::NrUePowerControl::PoNominalPusch", IntegerValue (-90));
  Config::SetDefault ("ns3::NrUePowerControl::PsrsOffset", IntegerValue (9));

  Ptr<NrHelper> nrHelper = CreateObject<NrHelper> ();

  uint8_t bandwidth = 25;
  double d1 = 0;

  // Create Nodes: eNodeB and UE
  NodeContainer enbNodes;
  NodeContainer ueNodes;
  enbNodes.Create (1);
  ueNodes.Create (1);
  NodeContainer allNodes = NodeContainer ( enbNodes, ueNodes);

/*   the topology is the following:
 *
 *   eNB1-------------------------UE
 *                  d1
 */

  // Install Mobility Model
  Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator> ();
  positionAlloc->Add (Vector (0.0, 0.0, 0.0));                  // eNB1
  positionAlloc->Add (Vector (d1, 0.0, 0.0));           // UE1

  MobilityHelper mobility;
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.SetPositionAllocator (positionAlloc);
  mobility.Install (allNodes);
  m_ueMobility = ueNodes.Get (0)->GetObject<MobilityModel> ();

  // Create Devices and install them in the Nodes (eNB and UE)
  NetDeviceContainer enbDevs;
  NetDeviceContainer ueDevs;
  nrHelper->SetSchedulerType ("ns3::PfFfMacScheduler");

  nrHelper->SetEnbDeviceAttribute ("DlBandwidth", UintegerValue (bandwidth));
  nrHelper->SetEnbDeviceAttribute ("UlBandwidth", UintegerValue (bandwidth));

  enbDevs = nrHelper->InstallEnbDevice (enbNodes);
  ueDevs = nrHelper->InstallUeDevice (ueNodes);

  Ptr<NrUePhy> uePhy = DynamicCast<NrUePhy>( ueDevs.Get (0)->GetObject<NrUeNetDevice> ()->GetPhy () );
  m_ueUpc = uePhy->GetUplinkPowerControl ();

  m_ueUpc->TraceConnectWithoutContext ("ReportPuschTxPower",
                                       MakeBoundCallback (&PuschTxPowerNofitication, this));
  m_ueUpc->TraceConnectWithoutContext ("ReportPucchTxPower",
                                       MakeBoundCallback (&PucchTxPowerNofitication, this));
  m_ueUpc->TraceConnectWithoutContext ("ReportSrsTxPower",
                                       MakeBoundCallback (&SrsTxPowerNofitication, this));

  // Attach a UE to a eNB
  nrHelper->Attach (ueDevs, enbDevs.Get (0));

  // Activate a data radio bearer
  enum EpsBearer::Qci q = EpsBearer::GBR_CONV_VOICE;
  EpsBearer bearer (q);
  nrHelper->ActivateDataRadioBearer (ueDevs, bearer);

  //Changing UE position
  Simulator::Schedule (MilliSeconds (0),
                       &NrUplinkPowerControlTestCase::TeleportUe, this, 0, 0, -40, -40, -40);
  Simulator::Schedule (MilliSeconds (200),
                       &NrUplinkPowerControlTestCase::TeleportUe, this, 200, 0, 8.9745, 8.9745, 11.9745);
  Simulator::Schedule (MilliSeconds (300),
                       &NrUplinkPowerControlTestCase::TeleportUe, this, 400, 0, 14.9951, 14.9951, 17.9951 );
  Simulator::Schedule (MilliSeconds (400),
                       &NrUplinkPowerControlTestCase::TeleportUe, this, 600, 0, 18.5169, 18.5169, 21.5169 );
  Simulator::Schedule (MilliSeconds (500),
                       &NrUplinkPowerControlTestCase::TeleportUe, this, 800, 0, 21.0157, 21.0157, 23 );
  Simulator::Schedule (MilliSeconds (600),
                       &NrUplinkPowerControlTestCase::TeleportUe, this, 1000, 0, 22.9539, 22.9539, 23 );
  Simulator::Schedule (MilliSeconds (700),
                       &NrUplinkPowerControlTestCase::TeleportUe, this, 1200, 0, 23, 10, 23 );
  Simulator::Schedule (MilliSeconds (800),
                       &NrUplinkPowerControlTestCase::TeleportUe, this, 400, 0, 14.9951, 14.9951, 17.9951 );
  Simulator::Schedule (MilliSeconds (900),
                       &NrUplinkPowerControlTestCase::TeleportUe, this, 800, 0, 21.0157, 21.0157, 23 );
  Simulator::Schedule (MilliSeconds (1000),
                       &NrUplinkPowerControlTestCase::TeleportUe, this, 0, 0, -40, -40, -40 );
  Simulator::Schedule (MilliSeconds (1100),
                       &NrUplinkPowerControlTestCase::TeleportUe, this, 100, 0, 2.9539, 2.9539, 5.9539 );
  Simulator::Stop (Seconds (1.200));
  Simulator::Run ();

  Simulator::Destroy ();
}

NrUplinkClosedLoopPowerControlAbsoluteModeTestCase::NrUplinkClosedLoopPowerControlAbsoluteModeTestCase (std::string name)
  : NrUplinkPowerControlTestCase ("Uplink Closed Loop Power Control: " + name)
{
  NS_LOG_INFO ("Creating NrUplinkClosedLoopPowerControlAbsoluteModeTestCase");
}

NrUplinkClosedLoopPowerControlAbsoluteModeTestCase::~NrUplinkClosedLoopPowerControlAbsoluteModeTestCase ()
{
}

void
NrUplinkClosedLoopPowerControlAbsoluteModeTestCase::DoRun (void)
{
  Config::Reset ();
  Config::SetDefault ("ns3::NrHelper::UseIdealRrc", BooleanValue (false));

  double eNbTxPower = 30;
  Config::SetDefault ("ns3::NrEnbPhy::TxPower", DoubleValue (eNbTxPower));
  Config::SetDefault ("ns3::NrUePhy::TxPower", DoubleValue (10.0));
  Config::SetDefault ("ns3::NrUePhy::EnableUplinkPowerControl", BooleanValue (true));

  Config::SetDefault ("ns3::NrUePowerControl::ClosedLoop", BooleanValue (true));
  Config::SetDefault ("ns3::NrUePowerControl::AccumulationEnabled", BooleanValue (false));
  Config::SetDefault ("ns3::NrUePowerControl::PoNominalPusch", IntegerValue (-90));
  Config::SetDefault ("ns3::NrUePowerControl::PsrsOffset", IntegerValue (9));

  Ptr<NrHelper> nrHelper = CreateObject<NrHelper> ();
  nrHelper->SetFfrAlgorithmType ("ns3::NrFfrSimple");

  uint8_t bandwidth = 25;
  double d1 = 100;

  // Create Nodes: eNodeB and UE
  NodeContainer enbNodes;
  NodeContainer ueNodes;
  enbNodes.Create (1);
  ueNodes.Create (1);
  NodeContainer allNodes = NodeContainer ( enbNodes, ueNodes);

/*   the topology is the following:
 *
 *   eNB1-------------------------UE
 *                  d1
 */

  // Install Mobility Model
  Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator> ();
  positionAlloc->Add (Vector (0.0, 0.0, 0.0));                  // eNB1
  positionAlloc->Add (Vector (d1, 0.0, 0.0));           // UE1

  MobilityHelper mobility;
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.SetPositionAllocator (positionAlloc);
  mobility.Install (allNodes);
  m_ueMobility = ueNodes.Get (0)->GetObject<MobilityModel> ();

  // Create Devices and install them in the Nodes (eNB and UE)
  NetDeviceContainer enbDevs;
  NetDeviceContainer ueDevs;
  nrHelper->SetSchedulerType ("ns3::PfFfMacScheduler");

  nrHelper->SetEnbDeviceAttribute ("DlBandwidth", UintegerValue (bandwidth));
  nrHelper->SetEnbDeviceAttribute ("UlBandwidth", UintegerValue (bandwidth));

  enbDevs = nrHelper->InstallEnbDevice (enbNodes);
  ueDevs = nrHelper->InstallUeDevice (ueNodes);

  Ptr<NrUePhy> uePhy = DynamicCast<NrUePhy>( ueDevs.Get (0)->GetObject<NrUeNetDevice> ()->GetPhy () );
  m_ueUpc = uePhy->GetUplinkPowerControl ();

  m_ueUpc->TraceConnectWithoutContext ("ReportPuschTxPower",
                                       MakeBoundCallback (&PuschTxPowerNofitication, this));
  m_ueUpc->TraceConnectWithoutContext ("ReportPucchTxPower",
                                       MakeBoundCallback (&PucchTxPowerNofitication, this));
  m_ueUpc->TraceConnectWithoutContext ("ReportSrsTxPower",
                                       MakeBoundCallback (&SrsTxPowerNofitication, this));

  // Attach a UE to a eNB
  nrHelper->Attach (ueDevs, enbDevs.Get (0));

  // Activate a data radio bearer
  enum EpsBearer::Qci q = EpsBearer::GBR_CONV_VOICE;
  EpsBearer bearer (q);
  nrHelper->ActivateDataRadioBearer (ueDevs, bearer);

  PointerValue tmp;
  enbDevs.Get (0)->GetAttribute ("NrFfrAlgorithm", tmp);
  m_ffrSimple = DynamicCast<NrFfrSimple>(tmp.GetObject ());
  m_accumulatedMode = false;

  //Changing TPC value
  Simulator::Schedule (MilliSeconds (0),
                       &NrUplinkPowerControlTestCase::SetTpcConfiguration, this, 1, 0, 1.9539, 1.9539, 4.9539);
  Simulator::Schedule (MilliSeconds (100),
                       &NrUplinkPowerControlTestCase::SetTpcConfiguration, this, 0, 0, -1.0461, -1.0461, 1.9539);
  Simulator::Schedule (MilliSeconds (200),
                       &NrUplinkPowerControlTestCase::SetTpcConfiguration, this, 2, 0, 3.9539, 3.9539, 6.9539);
  Simulator::Schedule (MilliSeconds (300),
                       &NrUplinkPowerControlTestCase::SetTpcConfiguration, this, 3, 0, 6.9539, 6.9539, 9.9539);
  Simulator::Schedule (MilliSeconds (400),
                       &NrUplinkPowerControlTestCase::SetTpcConfiguration, this, 0, 0, -1.0461, -1.0461, 1.9539);
  Simulator::Schedule (MilliSeconds (500),
                       &NrUplinkPowerControlTestCase::SetTpcConfiguration, this, 1, 0, 1.9539, 1.9539, 4.9539);
  Simulator::Schedule (MilliSeconds (600),
                       &NrUplinkPowerControlTestCase::SetTpcConfiguration, this, 3, 0, 6.9539, 6.9539, 9.9539);
  Simulator::Schedule (MilliSeconds (800),
                       &NrUplinkPowerControlTestCase::SetTpcConfiguration, this, 2, 0, 3.9539, 3.9539, 6.9539);
  Simulator::Stop (Seconds (1.000));
  Simulator::Run ();

  Simulator::Destroy ();
}

NrUplinkClosedLoopPowerControlAccumulatedModeTestCase::NrUplinkClosedLoopPowerControlAccumulatedModeTestCase (std::string name)
  : NrUplinkPowerControlTestCase ("Uplink Closed Loop Power Control: " + name)
{
  NS_LOG_INFO ("Creating NrUplinkClosedLoopPowerControlAccumulatedModeTestCase");
}

NrUplinkClosedLoopPowerControlAccumulatedModeTestCase::~NrUplinkClosedLoopPowerControlAccumulatedModeTestCase ()
{
}

void
NrUplinkClosedLoopPowerControlAccumulatedModeTestCase::DoRun (void)
{
  Config::Reset ();
  Config::SetDefault ("ns3::NrHelper::UseIdealRrc", BooleanValue (false));

  double eNbTxPower = 30;
  Config::SetDefault ("ns3::NrEnbPhy::TxPower", DoubleValue (eNbTxPower));
  Config::SetDefault ("ns3::NrUePhy::TxPower", DoubleValue (10.0));
  Config::SetDefault ("ns3::NrUePhy::EnableUplinkPowerControl", BooleanValue (true));

  Config::SetDefault ("ns3::NrUePowerControl::ClosedLoop", BooleanValue (true));
  Config::SetDefault ("ns3::NrUePowerControl::AccumulationEnabled", BooleanValue (true));
  Config::SetDefault ("ns3::NrUePowerControl::PoNominalPusch", IntegerValue (-90));
  Config::SetDefault ("ns3::NrUePowerControl::PsrsOffset", IntegerValue (9));

  Ptr<NrHelper> nrHelper = CreateObject<NrHelper> ();
  nrHelper->SetFfrAlgorithmType ("ns3::NrFfrSimple");

  uint8_t bandwidth = 25;
  double d1 = 10;

  // Create Nodes: eNodeB and UE
  NodeContainer enbNodes;
  NodeContainer ueNodes;
  enbNodes.Create (1);
  ueNodes.Create (1);
  NodeContainer allNodes = NodeContainer ( enbNodes, ueNodes);

/*   the topology is the following:
 *
 *   eNB1-------------------------UE
 *                  d1
 */

  // Install Mobility Model
  Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator> ();
  positionAlloc->Add (Vector (0.0, 0.0, 0.0));                  // eNB1
  positionAlloc->Add (Vector (d1, 0.0, 0.0));           // UE1

  MobilityHelper mobility;
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.SetPositionAllocator (positionAlloc);
  mobility.Install (allNodes);
  m_ueMobility = ueNodes.Get (0)->GetObject<MobilityModel> ();

  // Create Devices and install them in the Nodes (eNB and UE)
  NetDeviceContainer enbDevs;
  NetDeviceContainer ueDevs;
  nrHelper->SetSchedulerType ("ns3::PfFfMacScheduler");

  nrHelper->SetEnbDeviceAttribute ("DlBandwidth", UintegerValue (bandwidth));
  nrHelper->SetEnbDeviceAttribute ("UlBandwidth", UintegerValue (bandwidth));

  enbDevs = nrHelper->InstallEnbDevice (enbNodes);
  ueDevs = nrHelper->InstallUeDevice (ueNodes);

  Ptr<NrUePhy> uePhy = DynamicCast<NrUePhy>( ueDevs.Get (0)->GetObject<NrUeNetDevice> ()->GetPhy () );
  m_ueUpc = uePhy->GetUplinkPowerControl ();

  m_ueUpc->TraceConnectWithoutContext ("ReportPuschTxPower",
                                       MakeBoundCallback (&PuschTxPowerNofitication, this));
  m_ueUpc->TraceConnectWithoutContext ("ReportPucchTxPower",
                                       MakeBoundCallback (&PucchTxPowerNofitication, this));
  m_ueUpc->TraceConnectWithoutContext ("ReportSrsTxPower",
                                       MakeBoundCallback (&SrsTxPowerNofitication, this));

  // Attach a UE to a eNB
  nrHelper->Attach (ueDevs, enbDevs.Get (0));

  // Activate a data radio bearer
  enum EpsBearer::Qci q = EpsBearer::GBR_CONV_VOICE;
  EpsBearer bearer (q);
  nrHelper->ActivateDataRadioBearer (ueDevs, bearer);

  PointerValue tmp;
  enbDevs.Get (0)->GetAttribute ("NrFfrAlgorithm", tmp);
  m_ffrSimple = DynamicCast<NrFfrSimple>(tmp.GetObject ());
  m_accumulatedMode = true;

  //Changing TPC value
  Simulator::Schedule (MilliSeconds (0),
                       &NrUplinkPowerControlTestCase::SetTpcConfiguration, this, 1, 0, -17.0461, -17.0461, -14.0461);
  Simulator::Schedule (MilliSeconds (100),
                       &NrUplinkPowerControlTestCase::SetTpcConfiguration, this, 0, 20, -37.0461, -37.0461, -34.0461);
  Simulator::Schedule (MilliSeconds (200),
                       &NrUplinkPowerControlTestCase::SetTpcConfiguration, this, 0, 20, -40, 10, -37.0461);
  Simulator::Schedule (MilliSeconds (300),
                       &NrUplinkPowerControlTestCase::SetTpcConfiguration, this, 2, 1, -39.0461, -39.0461, -36.0461);
  Simulator::Schedule (MilliSeconds (400),
                       &NrUplinkPowerControlTestCase::SetTpcConfiguration, this, 3, 10, -9.0461, -9.0461, -6.0461);
  Simulator::Schedule (MilliSeconds (500),
                       &NrUplinkPowerControlTestCase::SetTpcConfiguration, this, 2, 15, 5.9539, 5.9539, 8.9539);
  Simulator::Schedule (MilliSeconds (600),
                       &NrUplinkPowerControlTestCase::SetTpcConfiguration, this, 3, 1, 8.9539, 8.9539, 11.9539);
  Simulator::Schedule (MilliSeconds (700),
                       &NrUplinkPowerControlTestCase::SetTpcConfiguration, this, 2, 10, 18.9539, 18.9539, 21.9539);
  Simulator::Schedule (MilliSeconds (800),
                       &NrUplinkPowerControlTestCase::SetTpcConfiguration, this, 2, 20, 23, 23, 23);
  Simulator::Schedule (MilliSeconds (900),
                       &NrUplinkPowerControlTestCase::SetTpcConfiguration, this, 0, 1, 22.9539, 22.9539, 23);
  Simulator::Schedule (MilliSeconds (1000),
                       &NrUplinkPowerControlTestCase::SetTpcConfiguration, this, 0, 20, 2.9539, 2.9539, 5.9539);
  Simulator::Schedule (MilliSeconds (1100),
                       &NrUplinkPowerControlTestCase::SetTpcConfiguration, this, 2, 5, 7.9539, 7.9539, 10.9539);
  Simulator::Stop (Seconds (1.200));
  Simulator::Run ();

  Simulator::Destroy ();
}
