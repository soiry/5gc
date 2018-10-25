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
#include "ns3/nr-ff-mac-scheduler.h"
#include "ns3/mobility-helper.h"
#include "ns3/nr-helper.h"

#include "nr-ffr-simple.h"
#include "ns3/nr-rrc-sap.h"

#include "nr-test-cqi-generation.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("NrCqiGenerationTest");

void
NrTestDlSchedulingCallback (NrCqiGenerationTestCase *testcase, std::string path,
                             uint32_t frameNo, uint32_t subframeNo, uint16_t rnti,
                             uint8_t mcsTb1, uint16_t sizeTb1, uint8_t mcsTb2, uint16_t sizeTb2)
{
  testcase->DlScheduling (frameNo, subframeNo, rnti, mcsTb1, sizeTb1, mcsTb2, sizeTb2);
}

void
NrTestUlSchedulingCallback (NrCqiGenerationTestCase *testcase, std::string path,
                             uint32_t frameNo, uint32_t subframeNo, uint16_t rnti,
                             uint8_t mcs, uint16_t sizeTb)
{
  testcase->UlScheduling (frameNo, subframeNo, rnti, mcs, sizeTb);
}

void
NrTestDlSchedulingCallback2 (NrCqiGenerationDlPowerControlTestCase *testcase, std::string path,
                              uint32_t frameNo, uint32_t subframeNo, uint16_t rnti,
                              uint8_t mcsTb1, uint16_t sizeTb1, uint8_t mcsTb2, uint16_t sizeTb2)
{
  testcase->DlScheduling (frameNo, subframeNo, rnti, mcsTb1, sizeTb1, mcsTb2, sizeTb2);
}

void
NrTestUlSchedulingCallback2 (NrCqiGenerationDlPowerControlTestCase *testcase, std::string path,
                              uint32_t frameNo, uint32_t subframeNo, uint16_t rnti,
                              uint8_t mcs, uint16_t sizeTb)
{
  testcase->UlScheduling (frameNo, subframeNo, rnti, mcs, sizeTb);
}


/**
 * TestSuite
 */

NrCqiGenerationTestSuite::NrCqiGenerationTestSuite ()
  : TestSuite ("nr-cqi-generation", SYSTEM)
{
//  LogLevel logLevel = (LogLevel)(LOG_PREFIX_FUNC | LOG_PREFIX_TIME | LOG_LEVEL_DEBUG);
//  LogComponentEnable ("NrCqiGenerationTest", logLevel);
  NS_LOG_INFO ("Creating NrCqiGenerationTestSuite");

  AddTestCase (new NrCqiGenerationTestCase ("UsePdcchForCqiGeneration", false, 4, 2), TestCase::QUICK);
  AddTestCase (new NrCqiGenerationTestCase ("UsePdschForCqiGeneration", true, 28, 2), TestCase::QUICK);

  AddTestCase (new NrCqiGenerationDlPowerControlTestCase ("CqiGenerationWithDlPowerControl",
                                                           NrRrcSap::PdschConfigDedicated::dB0, NrRrcSap::PdschConfigDedicated::dB0, 4, 2), TestCase::QUICK);
  AddTestCase (new NrCqiGenerationDlPowerControlTestCase ("CqiGenerationWithDlPowerControl",
                                                           NrRrcSap::PdschConfigDedicated::dB0, NrRrcSap::PdschConfigDedicated::dB_3, 8, 2), TestCase::QUICK);
  AddTestCase (new NrCqiGenerationDlPowerControlTestCase ("CqiGenerationWithDlPowerControl",
                                                           NrRrcSap::PdschConfigDedicated::dB0, NrRrcSap::PdschConfigDedicated::dB_6, 10, 2), TestCase::QUICK);
  AddTestCase (new NrCqiGenerationDlPowerControlTestCase ("CqiGenerationWithDlPowerControl",
                                                           NrRrcSap::PdschConfigDedicated::dB1, NrRrcSap::PdschConfigDedicated::dB_6, 12, 2), TestCase::QUICK);
  AddTestCase (new NrCqiGenerationDlPowerControlTestCase ("CqiGenerationWithDlPowerControl",
                                                           NrRrcSap::PdschConfigDedicated::dB2, NrRrcSap::PdschConfigDedicated::dB_6, 14, 2), TestCase::QUICK);
  AddTestCase (new NrCqiGenerationDlPowerControlTestCase ("CqiGenerationWithDlPowerControl",
                                                           NrRrcSap::PdschConfigDedicated::dB3, NrRrcSap::PdschConfigDedicated::dB_6, 14, 2), TestCase::QUICK);
  AddTestCase (new NrCqiGenerationDlPowerControlTestCase ("CqiGenerationWithDlPowerControl",
                                                           NrRrcSap::PdschConfigDedicated::dB3, NrRrcSap::PdschConfigDedicated::dB0, 8, 2), TestCase::QUICK);
}

static NrCqiGenerationTestSuite nrCqiGenerationTestSuite;


NrCqiGenerationTestCase::NrCqiGenerationTestCase (std::string name, bool usePdcchForCqiGeneration,
                                                    uint16_t dlMcs, uint16_t ulMcs)
  : TestCase ("Downlink Power Control: " + name),
    m_dlMcs (dlMcs),
    m_ulMcs (ulMcs)
{
  m_usePdcchForCqiGeneration = usePdcchForCqiGeneration;
  NS_LOG_INFO ("Creating NrCqiGenerationTestCase");
}

NrCqiGenerationTestCase::~NrCqiGenerationTestCase ()
{
}

void
NrCqiGenerationTestCase::DlScheduling (uint32_t frameNo, uint32_t subframeNo, uint16_t rnti,
                                        uint8_t mcsTb1, uint16_t sizeTb1, uint8_t mcsTb2, uint16_t sizeTb2)
{
  // need to allow for RRC connection establishment + CQI feedback reception
  if (Simulator::Now () > MilliSeconds (35))
    {
//	  NS_LOG_UNCOND("DL MSC: " << (uint32_t)mcsTb1 << " expected DL MCS: " << (uint32_t)m_dlMcs);
      NS_TEST_ASSERT_MSG_EQ ((uint32_t)mcsTb1, (uint32_t)m_dlMcs, "Wrong DL MCS ");
    }
}

void
NrCqiGenerationTestCase::UlScheduling (uint32_t frameNo, uint32_t subframeNo, uint16_t rnti,
                                        uint8_t mcs, uint16_t sizeTb)
{
  // need to allow for RRC connection establishment + SRS transmission
  if (Simulator::Now () > MilliSeconds (50))
    {
//	  NS_LOG_UNCOND("UL MSC: " << (uint32_t)mcs << " expected UL MCS: " << (uint32_t)m_ulMcs);
      NS_TEST_ASSERT_MSG_EQ ((uint32_t)mcs, (uint32_t)m_ulMcs, "Wrong UL MCS");
    }
}

void
NrCqiGenerationTestCase::DoRun (void)
{
  NS_LOG_DEBUG ("NrCqiGenerationTestCase");

  Config::Reset ();
  Config::SetDefault ("ns3::NrHelper::UseIdealRrc", BooleanValue (true));
  Config::SetDefault ("ns3::NrHelper::UsePdschForCqiGeneration", BooleanValue (m_usePdcchForCqiGeneration));

  Config::SetDefault ("ns3::NrSpectrumPhy::CtrlErrorModelEnabled", BooleanValue (true));
  Config::SetDefault ("ns3::NrSpectrumPhy::DataErrorModelEnabled", BooleanValue (true));

  Ptr<NrHelper> nrHelper = CreateObject<NrHelper> ();

  // Create Nodes: eNodeB and UE
  NodeContainer enbNodes;
  NodeContainer ueNodes1;
  NodeContainer ueNodes2;
  enbNodes.Create (2);
  ueNodes1.Create (1);
  ueNodes2.Create (1);
  NodeContainer allNodes = NodeContainer ( enbNodes, ueNodes1, ueNodes2);

  /*
   * The topology is the following:
   *
   *  eNB1                        UE1 UE2                        eNB2
   *    |                            |                            |
   *    x -------------------------- x -------------------------- x
   *                  500 m                       500 m
   *
   */

  Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator> ();
  positionAlloc->Add (Vector (0.0, 0.0, 0.0));   // eNB1
  positionAlloc->Add (Vector (1000, 0.0, 0.0)); // eNB2
  positionAlloc->Add (Vector (500.0, 0.0, 0.0));  // UE1
  positionAlloc->Add (Vector (500, 0.0, 0.0));  // UE2
  MobilityHelper mobility;
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.SetPositionAllocator (positionAlloc);
  mobility.Install (allNodes);

  // Create Devices and install them in the Nodes (eNB and UE)
  NetDeviceContainer enbDevs;
  NetDeviceContainer ueDevs1;
  NetDeviceContainer ueDevs2;
  nrHelper->SetSchedulerType ("ns3::PfFfMacScheduler");
  nrHelper->SetSchedulerAttribute ("UlCqiFilter", EnumValue (FfMacScheduler::PUSCH_UL_CQI));

  nrHelper->SetFfrAlgorithmType ("ns3::NrFrHardAlgorithm");

  nrHelper->SetFfrAlgorithmAttribute ("DlSubBandOffset", UintegerValue (0));
  nrHelper->SetFfrAlgorithmAttribute ("DlSubBandwidth", UintegerValue (12));
  nrHelper->SetFfrAlgorithmAttribute ("UlSubBandOffset", UintegerValue (0));
  nrHelper->SetFfrAlgorithmAttribute ("UlSubBandwidth", UintegerValue (25));
  enbDevs.Add (nrHelper->InstallEnbDevice (enbNodes.Get (0)));

  nrHelper->SetFfrAlgorithmAttribute ("DlSubBandOffset", UintegerValue (12));
  nrHelper->SetFfrAlgorithmAttribute ("DlSubBandwidth", UintegerValue (12));
  nrHelper->SetFfrAlgorithmAttribute ("UlSubBandOffset", UintegerValue (0));
  nrHelper->SetFfrAlgorithmAttribute ("UlSubBandwidth", UintegerValue (25));
  enbDevs.Add (nrHelper->InstallEnbDevice (enbNodes.Get (1)));

  ueDevs1 = nrHelper->InstallUeDevice (ueNodes1);
  ueDevs2 = nrHelper->InstallUeDevice (ueNodes2);

  // Attach a UE to a eNB
  nrHelper->Attach (ueDevs1, enbDevs.Get (0));
  nrHelper->Attach (ueDevs2, enbDevs.Get (1));

  // Activate an EPS bearer
  enum EpsBearer::Qci q = EpsBearer::GBR_CONV_VOICE;
  EpsBearer bearer (q);
  nrHelper->ActivateDataRadioBearer (ueDevs1, bearer);
  nrHelper->ActivateDataRadioBearer (ueDevs2, bearer);

  Config::Connect ("/NodeList/0/DeviceList/0/NrEnbMac/DlScheduling",
                   MakeBoundCallback (&NrTestDlSchedulingCallback, this));

  Config::Connect ("/NodeList/0/DeviceList/0/NrEnbMac/UlScheduling",
                   MakeBoundCallback (&NrTestUlSchedulingCallback, this));

  Config::Connect ("/NodeList/1/DeviceList/0/NrEnbMac/DlScheduling",
                   MakeBoundCallback (&NrTestDlSchedulingCallback, this));

  Config::Connect ("/NodeList/1/DeviceList/0/NrEnbMac/UlScheduling",
                   MakeBoundCallback (&NrTestUlSchedulingCallback, this));

  Simulator::Stop (Seconds (1.100));
  Simulator::Run ();

  Simulator::Destroy ();
}

NrCqiGenerationDlPowerControlTestCase::NrCqiGenerationDlPowerControlTestCase (std::string name,
                                                                                uint8_t cell0Pa, uint8_t cell1Pa, uint16_t dlMcs, uint16_t ulMcs)
  : TestCase ("Downlink Power Control: " + name),
    m_cell0Pa (cell0Pa),
    m_cell1Pa (cell1Pa),
    m_dlMcs (dlMcs),
    m_ulMcs (ulMcs)
{
  NS_LOG_INFO ("Creating NrCqiGenerationTestCase");
}

NrCqiGenerationDlPowerControlTestCase::~NrCqiGenerationDlPowerControlTestCase ()
{
}

void
NrCqiGenerationDlPowerControlTestCase::DlScheduling (uint32_t frameNo, uint32_t subframeNo, uint16_t rnti,
                                                      uint8_t mcsTb1, uint16_t sizeTb1, uint8_t mcsTb2, uint16_t sizeTb2)
{
  // need to allow for RRC connection establishment + CQI feedback reception
  if (Simulator::Now () > MilliSeconds (500))
    {
//	  NS_LOG_UNCOND("DL MSC: " << (uint32_t)mcsTb1 << " expected DL MCS: " << (uint32_t)m_dlMcs);
      NS_TEST_ASSERT_MSG_EQ ((uint32_t)mcsTb1, (uint32_t)m_dlMcs, "Wrong DL MCS ");
    }
}

void
NrCqiGenerationDlPowerControlTestCase::UlScheduling (uint32_t frameNo, uint32_t subframeNo, uint16_t rnti,
                                                      uint8_t mcs, uint16_t sizeTb)
{
  // need to allow for RRC connection establishment + SRS transmission
  if (Simulator::Now () > MilliSeconds (500))
    {
//	  NS_LOG_UNCOND("UL MSC: " << (uint32_t)mcs << " expected UL MCS: " << (uint32_t)m_ulMcs);
      NS_TEST_ASSERT_MSG_EQ ((uint32_t)mcs, (uint32_t)m_ulMcs, "Wrong UL MCS");
    }
}

void
NrCqiGenerationDlPowerControlTestCase::DoRun (void)
{
  NS_LOG_DEBUG ("NrCqiGenerationTestCase");

  Config::Reset ();
  Config::SetDefault ("ns3::NrHelper::UseIdealRrc", BooleanValue (true));
  Config::SetDefault ("ns3::NrHelper::UsePdschForCqiGeneration", BooleanValue (true));

  Config::SetDefault ("ns3::NrSpectrumPhy::CtrlErrorModelEnabled", BooleanValue (true));
  Config::SetDefault ("ns3::NrSpectrumPhy::DataErrorModelEnabled", BooleanValue (true));

  Ptr<NrHelper> nrHelper = CreateObject<NrHelper> ();
  nrHelper->SetFfrAlgorithmType ("ns3::NrFfrSimple");

  // Create Nodes: eNodeB and UE
  NodeContainer enbNodes;
  NodeContainer ueNodes1;
  NodeContainer ueNodes2;
  enbNodes.Create (2);
  ueNodes1.Create (1);
  ueNodes2.Create (1);
  NodeContainer allNodes = NodeContainer ( enbNodes, ueNodes1, ueNodes2);

  /*
   * The topology is the following:
   *
   *  eNB1                        UE1 UE2                        eNB2
   *    |                            |                            |
   *    x -------------------------- x -------------------------- x
   *                  500 m                       500 m
   *
   */

  Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator> ();
  positionAlloc->Add (Vector (0.0, 0.0, 0.0));   // eNB1
  positionAlloc->Add (Vector (1000, 0.0, 0.0)); // eNB2
  positionAlloc->Add (Vector (500.0, 0.0, 0.0));  // UE1
  positionAlloc->Add (Vector (500, 0.0, 0.0));  // UE2
  MobilityHelper mobility;
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.SetPositionAllocator (positionAlloc);
  mobility.Install (allNodes);

  // Create Devices and install them in the Nodes (eNB and UE)
  NetDeviceContainer enbDevs;
  NetDeviceContainer ueDevs1;
  NetDeviceContainer ueDevs2;
  nrHelper->SetSchedulerType ("ns3::PfFfMacScheduler");
  nrHelper->SetSchedulerAttribute ("UlCqiFilter", EnumValue (FfMacScheduler::PUSCH_UL_CQI));
  enbDevs = nrHelper->InstallEnbDevice (enbNodes);
  ueDevs1 = nrHelper->InstallUeDevice (ueNodes1);
  ueDevs2 = nrHelper->InstallUeDevice (ueNodes2);

  // Attach a UE to a eNB
  nrHelper->Attach (ueDevs1, enbDevs.Get (0));
  nrHelper->Attach (ueDevs2, enbDevs.Get (1));

  // Activate an EPS bearer
  enum EpsBearer::Qci q = EpsBearer::GBR_CONV_VOICE;
  EpsBearer bearer (q);
  nrHelper->ActivateDataRadioBearer (ueDevs1, bearer);
  nrHelper->ActivateDataRadioBearer (ueDevs2, bearer);

  PointerValue tmp;
  enbDevs.Get (0)->GetAttribute ("NrFfrAlgorithm", tmp);
  Ptr<NrFfrSimple> simpleFfrAlgorithmEnb0 = DynamicCast<NrFfrSimple>(tmp.GetObject ());
  simpleFfrAlgorithmEnb0->ChangePdschConfigDedicated (true);

  NrRrcSap::PdschConfigDedicated pdschConfigDedicatedEnb0;
  pdschConfigDedicatedEnb0.pa = m_cell0Pa;
  simpleFfrAlgorithmEnb0->SetPdschConfigDedicated (pdschConfigDedicatedEnb0);

  enbDevs.Get (1)->GetAttribute ("NrFfrAlgorithm", tmp);
  Ptr<NrFfrSimple> simpleFfrAlgorithmEnb1 = DynamicCast<NrFfrSimple>(tmp.GetObject ());
  simpleFfrAlgorithmEnb1->ChangePdschConfigDedicated (true);

  NrRrcSap::PdschConfigDedicated pdschConfigDedicatedEnb1;
  pdschConfigDedicatedEnb1.pa = m_cell1Pa;
  simpleFfrAlgorithmEnb1->SetPdschConfigDedicated (pdschConfigDedicatedEnb1);


  Config::Connect ("/NodeList/0/DeviceList/0/NrEnbMac/DlScheduling",
                   MakeBoundCallback (&NrTestDlSchedulingCallback2, this));

  Config::Connect ("/NodeList/0/DeviceList/0/NrEnbMac/UlScheduling",
                   MakeBoundCallback (&NrTestUlSchedulingCallback2, this));

  Simulator::Stop (Seconds (1.100));
  Simulator::Run ();

  Simulator::Destroy ();
}

