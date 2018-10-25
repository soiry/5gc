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

#include "ns3/mobility-helper.h"
#include "ns3/nr-helper.h"
#include "ns3/point-to-point-ngc-helper.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"

#include <ns3/nr-ff-mac-scheduler.h>
#include <ns3/nr-enb-net-device.h>
#include <ns3/nr-enb-phy.h>
#include <ns3/nr-enb-rrc.h>
#include <ns3/nr-ue-net-device.h>
#include <ns3/nr-ue-phy.h>
#include <ns3/nr-ue-rrc.h>

#include "nr-ffr-simple.h"
#include "nr-simple-spectrum-phy.h"
#include <ns3/nr-common.h>

#include "nr-test-frequency-reuse.h"


using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("NrFrequencyReuseTest");

/**
 * TestSuite
 */

NrFrequencyReuseTestSuite::NrFrequencyReuseTestSuite ()
  : TestSuite ("nr-frequency-reuse", SYSTEM)
{
//  LogLevel logLevel = (LogLevel)(LOG_PREFIX_FUNC | LOG_PREFIX_TIME | LOG_LEVEL_DEBUG);
//  LogComponentEnable ("NrFrequencyReuseTest", logLevel);

  std::vector<bool> availableDlRb;
  std::vector<bool> availableUlRb;
  for (uint32_t i = 0; i < 12; i++)
    {
      availableDlRb.push_back (true);
      availableUlRb.push_back (true);
    }
  for (uint32_t i = 12; i < 25; i++)
    {
      availableDlRb.push_back (false);
      availableUlRb.push_back (false);
    }

  AddTestCase (new NrHardFrTestCase ("DownlinkHardFrPf1", 1, "ns3::PfFfMacScheduler", 25, 25, 0, 12, 0, 12, availableDlRb, availableUlRb), TestCase::QUICK);
  AddTestCase (new NrHardFrTestCase ("DownlinkHardFrPf2", 5, "ns3::PfFfMacScheduler", 25, 25, 0, 12, 0, 12, availableDlRb, availableUlRb), TestCase::QUICK);
  AddTestCase (new NrHardFrTestCase ("DownlinkHardFrPss1", 1, "ns3::PssFfMacScheduler", 25, 25, 0, 12, 0, 12, availableDlRb, availableUlRb), TestCase::QUICK);
  AddTestCase (new NrHardFrTestCase ("DownlinkHardFrPss2", 5, "ns3::PssFfMacScheduler", 25, 25, 0, 12, 0, 12, availableDlRb, availableUlRb), TestCase::QUICK);
  AddTestCase (new NrHardFrTestCase ("DownlinkHardFrCqa1", 1, "ns3::CqaFfMacScheduler", 25, 25, 0, 12, 0, 12, availableDlRb, availableUlRb), TestCase::QUICK);
  AddTestCase (new NrHardFrTestCase ("DownlinkHardFrCqa2", 5, "ns3::CqaFfMacScheduler", 25, 25, 0, 12, 0, 12, availableDlRb, availableUlRb), TestCase::QUICK);
  AddTestCase (new NrHardFrTestCase ("DownlinkHardFrFdTbfq1", 1, "ns3::FdTbfqFfMacScheduler", 25, 25, 0, 12, 0, 12, availableDlRb, availableUlRb), TestCase::QUICK);
  AddTestCase (new NrHardFrTestCase ("DownlinkHardFrFdTbfq2", 5, "ns3::FdTbfqFfMacScheduler", 25, 25, 0, 12, 0, 12, availableDlRb, availableUlRb), TestCase::QUICK);
  AddTestCase (new NrHardFrTestCase ("DownlinkHardFrTdTbfq1", 1, "ns3::TdTbfqFfMacScheduler", 25, 25, 0, 12, 0, 12, availableDlRb, availableUlRb), TestCase::QUICK);
  AddTestCase (new NrHardFrTestCase ("DownlinkHardFrTdTbfq2", 5, "ns3::TdTbfqFfMacScheduler", 25, 25, 0, 12, 0, 12, availableDlRb, availableUlRb), TestCase::QUICK);

  availableDlRb.clear ();
  availableUlRb.clear ();
  for (uint32_t i = 0; i < 6; i++)
    {
      availableDlRb.push_back (true);
      availableUlRb.push_back (true);
    }
  for (uint32_t i = 6; i < 12; i++)
    {
      availableDlRb.push_back (false);
      availableUlRb.push_back (false);
    }
  for (uint32_t i = 12; i < 18; i++)
    {
      availableDlRb.push_back (true);
      availableUlRb.push_back (true);
    }
  for (uint32_t i = 18; i < 25; i++)
    {
      availableDlRb.push_back (false);
      availableUlRb.push_back (false);
    }

  AddTestCase (new NrStrictFrTestCase ("DownlinkStrictFrPf1", 1, "ns3::PfFfMacScheduler", 25, 25, 6, 6, 6, 6, 6, 6, availableDlRb, availableUlRb), TestCase::QUICK);
  AddTestCase (new NrStrictFrTestCase ("DownlinkStrictFrPf2", 5, "ns3::PfFfMacScheduler", 25, 25, 6, 6, 6, 6, 6, 6, availableDlRb, availableUlRb), TestCase::QUICK);
  AddTestCase (new NrStrictFrTestCase ("DownlinkStrictFrPss1", 1, "ns3::PssFfMacScheduler", 25, 25, 6, 6, 6, 6, 6, 6, availableDlRb, availableUlRb), TestCase::QUICK);
  AddTestCase (new NrStrictFrTestCase ("DownlinkStrictFrPss2", 5, "ns3::PssFfMacScheduler", 25, 25, 6, 6, 6, 6, 6, 6, availableDlRb, availableUlRb), TestCase::QUICK);
  AddTestCase (new NrStrictFrTestCase ("DownlinkStrictFrCqa1", 1, "ns3::CqaFfMacScheduler", 25, 25, 6, 6, 6, 6, 6, 6, availableDlRb, availableUlRb), TestCase::QUICK);
  AddTestCase (new NrStrictFrTestCase ("DownlinkStrictFrCqa2", 5, "ns3::CqaFfMacScheduler", 25, 25, 6, 6, 6, 6, 6, 6, availableDlRb, availableUlRb), TestCase::QUICK);
  AddTestCase (new NrStrictFrTestCase ("DownlinkStrictFrCqaFdTbfq1", 1, "ns3::FdTbfqFfMacScheduler", 25, 25, 6, 6, 6, 6, 6, 6, availableDlRb, availableUlRb), TestCase::QUICK);
  AddTestCase (new NrStrictFrTestCase ("DownlinkStrictFrCqaFdTbfq2", 5, "ns3::FdTbfqFfMacScheduler", 25, 25, 6, 6, 6, 6, 6, 6, availableDlRb, availableUlRb), TestCase::QUICK);
  AddTestCase (new NrStrictFrTestCase ("DownlinkStrictFrCqaTdTbfq1", 1, "ns3::TdTbfqFfMacScheduler", 25, 25, 6, 6, 6, 6, 6, 6, availableDlRb, availableUlRb), TestCase::QUICK);
  AddTestCase (new NrStrictFrTestCase ("DownlinkStrictFrCqaTdTbfq2", 5, "ns3::TdTbfqFfMacScheduler", 25, 25, 6, 6, 6, 6, 6, 6, availableDlRb, availableUlRb), TestCase::QUICK);

  AddTestCase (new NrStrictFrAreaTestCase ("NrStrictFrAreaTestCasePf1", "ns3::PfFfMacScheduler"), TestCase::QUICK);
  AddTestCase (new NrStrictFrAreaTestCase ("NrStrictFrAreaTestCasePss1", "ns3::PssFfMacScheduler"), TestCase::QUICK);
  AddTestCase (new NrStrictFrAreaTestCase ("NrStrictFrAreaTestCaseCqa1", "ns3::CqaFfMacScheduler"), TestCase::QUICK);
  AddTestCase (new NrStrictFrAreaTestCase ("NrStrictFrAreaTestCaseFdTbfq1", "ns3::FdTbfqFfMacScheduler"), TestCase::QUICK);
  AddTestCase (new NrStrictFrAreaTestCase ("NrStrictFrAreaTestCaseTdTbfq1", "ns3::TdTbfqFfMacScheduler"), TestCase::QUICK);

  AddTestCase (new NrSoftFrAreaTestCase ("NrSoftFrAreaTestCasePf1", "ns3::PfFfMacScheduler"), TestCase::QUICK);
  AddTestCase (new NrSoftFrAreaTestCase ("NrSoftFrAreaTestCasePss1", "ns3::PssFfMacScheduler"), TestCase::QUICK);
  AddTestCase (new NrSoftFrAreaTestCase ("NrSoftFrAreaTestCaseCqa1", "ns3::CqaFfMacScheduler"), TestCase::QUICK);
  AddTestCase (new NrSoftFrAreaTestCase ("NrSoftFrAreaTestCaseFdTbfq1", "ns3::FdTbfqFfMacScheduler"), TestCase::QUICK);
  AddTestCase (new NrSoftFrAreaTestCase ("NrSoftFrAreaTestCaseTdTbfq1", "ns3::TdTbfqFfMacScheduler"), TestCase::QUICK);

  AddTestCase (new NrSoftFfrAreaTestCase ("NrSoftFfrAreaTestCasePf1", "ns3::PfFfMacScheduler"), TestCase::QUICK);
  AddTestCase (new NrSoftFfrAreaTestCase ("NrSoftFfrAreaTestCasePss1", "ns3::PssFfMacScheduler"), TestCase::QUICK);
  AddTestCase (new NrSoftFfrAreaTestCase ("NrSoftFfrAreaTestCaseCqa1", "ns3::CqaFfMacScheduler"), TestCase::QUICK);
  AddTestCase (new NrSoftFfrAreaTestCase ("NrSoftFfrAreaTestCaseFdTbfq1", "ns3::FdTbfqFfMacScheduler"), TestCase::QUICK);
  AddTestCase (new NrSoftFfrAreaTestCase ("NrSoftFfrAreaTestCaseTdTbfq1", "ns3::TdTbfqFfMacScheduler"), TestCase::QUICK);

  AddTestCase (new NrEnhancedFfrAreaTestCase ("NrEnhancedFfrAreaTestCasePf1", "ns3::PfFfMacScheduler"), TestCase::QUICK);
  AddTestCase (new NrEnhancedFfrAreaTestCase ("NrEnhancedFfrAreaTestCasePss1", "ns3::PssFfMacScheduler"), TestCase::QUICK);
  AddTestCase (new NrEnhancedFfrAreaTestCase ("NrEnhancedFfrAreaTestCaseCqa1", "ns3::CqaFfMacScheduler"), TestCase::QUICK);
  AddTestCase (new NrEnhancedFfrAreaTestCase ("NrEnhancedFfrAreaTestCaseFdTbfq1", "ns3::FdTbfqFfMacScheduler"), TestCase::QUICK);
  AddTestCase (new NrEnhancedFfrAreaTestCase ("NrEnhancedFfrAreaTestCaseTdTbfq1", "ns3::TdTbfqFfMacScheduler"), TestCase::QUICK);

  AddTestCase (new NrDistributedFfrAreaTestCase ("NrDistributedFfrAreaTestCasePf1", "ns3::PfFfMacScheduler"), TestCase::QUICK);
  AddTestCase (new NrDistributedFfrAreaTestCase ("NrDistributedFfrAreaTestCasePss1", "ns3::PssFfMacScheduler"), TestCase::QUICK);
  AddTestCase (new NrDistributedFfrAreaTestCase ("NrDistributedFfrAreaTestCaseCqa1", "ns3::CqaFfMacScheduler"), TestCase::QUICK);
  AddTestCase (new NrDistributedFfrAreaTestCase ("NrDistributedFfrAreaTestCaseFdTbfq1", "ns3::FdTbfqFfMacScheduler"), TestCase::QUICK);
  AddTestCase (new NrDistributedFfrAreaTestCase ("NrDistributedFfrAreaTestCaseTdTbfq1", "ns3::TdTbfqFfMacScheduler"), TestCase::QUICK);
}

static NrFrequencyReuseTestSuite nrFrequencyReuseTestSuite;


/**
 * TestCase Data
 */
void
DlDataRxStartNofitication (NrFrTestCase *testcase,
                           Ptr<const SpectrumValue> spectrumValue)
{
  testcase->DlDataRxStart (spectrumValue);
}

void
UlDataRxStartNofitication (NrFrTestCase *testcase,
                           Ptr<const SpectrumValue> spectrumValue)
{
  testcase->UlDataRxStart (spectrumValue);
}

NrFrTestCase::NrFrTestCase (std::string name,
                              uint32_t userNum,uint8_t dlBandwidth,uint8_t ulBandwidth,
                              std::vector<bool> availableDlRb, std::vector<bool> availableUlRb)
  : TestCase ("Test: " + name),
    m_userNum (userNum),
    m_dlBandwidth (dlBandwidth),
    m_ulBandwidth (ulBandwidth),
    m_availableDlRb (availableDlRb),
    m_usedMutedDlRbg (false),
    m_availableUlRb (availableUlRb),
    m_usedMutedUlRbg (false)
{
}

NrFrTestCase::~NrFrTestCase ()
{
}



void
NrFrTestCase::DlDataRxStart (Ptr<const SpectrumValue> spectrumValue)
{
  NS_LOG_DEBUG ("DL DATA Power allocation :");
  Values::const_iterator it;
  uint32_t i = 0;
  for (it = spectrumValue->ConstValuesBegin (); it != spectrumValue->ConstValuesEnd (); it++)
    {
      double power =  (*it) * (m_dlBandwidth * 180000);
      NS_LOG_DEBUG ("RB " << i << " POWER: " << " " << power << " isAvailable: " << m_availableDlRb[i]);

      if (m_availableDlRb[i] == false && power > 0)
        {
          m_usedMutedDlRbg = true;
        }
      i++;
    }
}

void
NrFrTestCase::UlDataRxStart (Ptr<const SpectrumValue> spectrumValue)
{
  NS_LOG_DEBUG ("UL DATA Power allocation :");
  Values::const_iterator it;
  uint32_t i = 0;
  for (it = spectrumValue->ConstValuesBegin (); it != spectrumValue->ConstValuesEnd (); it++)
    {
      double power =  (*it) * (m_ulBandwidth * 180000);
      NS_LOG_DEBUG ("RB " << i << " POWER: " << " " << power << " isAvailable: " << m_availableUlRb[i]);

      if (m_availableUlRb[i] == false && power > 0)
        {
          m_usedMutedUlRbg = true;
        }
      i++;
    }
}


void
NrFrTestCase::DoRun (void)
{

}


NrHardFrTestCase::NrHardFrTestCase (std::string name, uint32_t userNum,
                                      std::string schedulerType,
                                      uint8_t dlBandwidth, uint8_t ulBandwidth,
                                      uint8_t dlSubBandOffset, uint8_t dlSubBandwidth,
                                      uint8_t ulSubBandOffset, uint8_t ulSubBandwidth,
                                      std::vector<bool> availableDlRb, std::vector<bool> availableUlRb)
  : NrFrTestCase (name, userNum, dlBandwidth, ulBandwidth, availableDlRb, availableUlRb),
    m_schedulerType (schedulerType),
    m_dlSubBandOffset (dlSubBandOffset),
    m_dlSubBandwidth (dlSubBandwidth),
    m_ulSubBandOffset (ulSubBandOffset),
    m_ulSubBandwidth (ulSubBandwidth)
{
  NS_LOG_INFO ("Creating NrDownlinkFrTestCase");
}

NrHardFrTestCase::~NrHardFrTestCase ()
{
}

void
NrHardFrTestCase::DoRun (void)
{
  NS_LOG_DEBUG ("NrFrTestCase");

  Config::Reset ();
  Config::SetDefault ("ns3::NrHelper::UseIdealRrc", BooleanValue (false));

  /**
    * Simulation Topology
    */

  Ptr<NrHelper> nrHelper = CreateObject<NrHelper> ();
  nrHelper->SetFfrAlgorithmType ("ns3::NrFrHardAlgorithm");

  nrHelper->SetFfrAlgorithmAttribute ("DlSubBandOffset", UintegerValue (m_dlSubBandOffset));
  nrHelper->SetFfrAlgorithmAttribute ("DlSubBandwidth", UintegerValue (m_dlSubBandwidth));

  nrHelper->SetFfrAlgorithmAttribute ("UlSubBandOffset", UintegerValue (m_ulSubBandOffset));
  nrHelper->SetFfrAlgorithmAttribute ("UlSubBandwidth", UintegerValue (m_ulSubBandwidth));


  // Create Nodes: eNodeB and UE
  NodeContainer enbNodes;
  NodeContainer ueNodes;
  enbNodes.Create (1);
  ueNodes.Create (m_userNum);
  NodeContainer allNodes = NodeContainer ( enbNodes, ueNodes );

  // Install Mobility Model
  MobilityHelper mobility;
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.Install (allNodes);

  // Create Devices and install them in the Nodes (eNB and UE)
  NetDeviceContainer enbDevs;
  NetDeviceContainer ueDevs;
  nrHelper->SetSchedulerType (m_schedulerType);
  enbDevs = nrHelper->InstallEnbDevice (enbNodes);
  ueDevs = nrHelper->InstallUeDevice (ueNodes);

  // Attach a UE to a eNB
  nrHelper->Attach (ueDevs, enbDevs.Get (0));

  // Activate the default EPS bearer
  enum EpsBearer::Qci q = EpsBearer::NGBR_VIDEO_TCP_DEFAULT;
  EpsBearer bearer (q);
  nrHelper->ActivateDataRadioBearer (ueDevs, bearer);

  //Test SpectrumPhy to get signals form DL channel
  Ptr<NrSpectrumPhy> enbDlSpectrumPhy = enbDevs.Get (0)->GetObject<NrEnbNetDevice> ()->GetPhy ()->GetDownlinkSpectrumPhy ()->GetObject<NrSpectrumPhy> ();
  Ptr<SpectrumChannel> dlChannel = enbDlSpectrumPhy->GetChannel ();

  Ptr<NrSimpleSpectrumPhy> testDlSpectrumPhy = CreateObject<NrSimpleSpectrumPhy> ();
  Ptr<NrEnbNetDevice> eNbDev = enbDevs.Get (0)->GetObject<NrEnbNetDevice> ();
  testDlSpectrumPhy->SetRxSpectrumModel (NrSpectrumValueHelper::GetSpectrumModel (eNbDev->GetDlEarfcn (), m_dlBandwidth));
  dlChannel->AddRx (testDlSpectrumPhy);

  testDlSpectrumPhy->TraceConnectWithoutContext ("RxStart",
                                                 MakeBoundCallback (&DlDataRxStartNofitication, this));

  //Test SpectrumPhy to get signals form UL channel
  Ptr<NrSpectrumPhy> ueUlSpectrumPhy = ueDevs.Get (0)->GetObject<NrUeNetDevice> ()->GetPhy ()->GetUplinkSpectrumPhy ()->GetObject<NrSpectrumPhy> ();
  Ptr<SpectrumChannel> ulChannel = ueUlSpectrumPhy->GetChannel ();

  Ptr<NrSimpleSpectrumPhy> testUlSpectrumPhy = CreateObject<NrSimpleSpectrumPhy> ();
  testUlSpectrumPhy->SetRxSpectrumModel (NrSpectrumValueHelper::GetSpectrumModel (eNbDev->GetUlEarfcn (), m_ulBandwidth));
  ulChannel->AddRx (testUlSpectrumPhy);

  testUlSpectrumPhy->TraceConnectWithoutContext ("RxStart",
                                                 MakeBoundCallback (&UlDataRxStartNofitication, this));

  Simulator::Stop (Seconds (0.500));
  Simulator::Run ();

  NS_TEST_ASSERT_MSG_EQ (m_usedMutedDlRbg, false,
                         "Scheduler used DL RBG muted by FFR Algorithm");

  NS_TEST_ASSERT_MSG_EQ (m_usedMutedUlRbg, false,
                         "Scheduler used UL RBG muted by FFR Algorithm");

  Simulator::Destroy ();
}



NrStrictFrTestCase::NrStrictFrTestCase (std::string name, uint32_t userNum,
                                          std::string schedulerType,
                                          uint8_t dlBandwidth, uint8_t ulBandwidth,
                                          uint8_t dlCommonSubBandwidth, uint8_t dlEdgeSubBandOffset, uint8_t dlEdgeSubBandwidth,
                                          uint8_t ulCommonSubBandwidth, uint8_t ulEdgeSubBandOffset, uint8_t ulEdgeSubBandwidth,
                                          std::vector<bool> availableDlRb, std::vector<bool> availableUlRb)
  : NrFrTestCase (name, userNum, dlBandwidth, ulBandwidth, availableDlRb, availableUlRb),
    m_schedulerType (schedulerType),
    m_dlCommonSubBandwidth (dlCommonSubBandwidth),
    m_dlEdgeSubBandOffset (dlEdgeSubBandOffset),
    m_dlEdgeSubBandwidth (dlEdgeSubBandwidth),
    m_ulCommonSubBandwidth (ulCommonSubBandwidth),
    m_ulEdgeSubBandOffset (ulEdgeSubBandOffset),
    m_ulEdgeSubBandwidth (ulEdgeSubBandwidth)
{
  NS_LOG_INFO ("Creating NrFrTestCase");
}

NrStrictFrTestCase::~NrStrictFrTestCase ()
{
}

void
NrStrictFrTestCase::DoRun (void)
{
  NS_LOG_DEBUG ("NrFrTestCase");

  Config::Reset ();
  Config::SetDefault ("ns3::NrHelper::UseIdealRrc", BooleanValue (false));

  /**
    * Simulation Topology
    */

  Ptr<NrHelper> nrHelper = CreateObject<NrHelper> ();
  nrHelper->SetFfrAlgorithmType ("ns3::NrFrStrictAlgorithm");

  nrHelper->SetFfrAlgorithmAttribute ("DlCommonSubBandwidth", UintegerValue (m_dlCommonSubBandwidth));
  nrHelper->SetFfrAlgorithmAttribute ("DlEdgeSubBandOffset", UintegerValue (m_dlEdgeSubBandOffset));
  nrHelper->SetFfrAlgorithmAttribute ("DlEdgeSubBandwidth", UintegerValue (m_dlEdgeSubBandwidth));

  nrHelper->SetFfrAlgorithmAttribute ("UlCommonSubBandwidth", UintegerValue (m_ulCommonSubBandwidth));
  nrHelper->SetFfrAlgorithmAttribute ("UlEdgeSubBandOffset", UintegerValue (m_ulEdgeSubBandOffset));
  nrHelper->SetFfrAlgorithmAttribute ("UlEdgeSubBandwidth", UintegerValue (m_ulEdgeSubBandwidth));

  // Create Nodes: eNodeB and UE
  NodeContainer enbNodes;
  NodeContainer ueNodes;
  enbNodes.Create (1);
  ueNodes.Create (m_userNum);
  NodeContainer allNodes = NodeContainer ( enbNodes, ueNodes );

  // Install Mobility Model
  MobilityHelper mobility;
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.Install (allNodes);

  // Create Devices and install them in the Nodes (eNB and UE)
  NetDeviceContainer enbDevs;
  NetDeviceContainer ueDevs;
  nrHelper->SetSchedulerType (m_schedulerType);
  enbDevs = nrHelper->InstallEnbDevice (enbNodes);
  ueDevs = nrHelper->InstallUeDevice (ueNodes);

  // Attach a UE to a eNB
  nrHelper->Attach (ueDevs, enbDevs.Get (0));

  // Activate the default EPS bearer
  enum EpsBearer::Qci q = EpsBearer::NGBR_VIDEO_TCP_DEFAULT;
  EpsBearer bearer (q);
  nrHelper->ActivateDataRadioBearer (ueDevs, bearer);

  //Test SpectrumPhy to get signals form DL channel
  Ptr<NrSpectrumPhy> enbDlSpectrumPhy = enbDevs.Get (0)->GetObject<NrEnbNetDevice> ()->GetPhy ()->GetDownlinkSpectrumPhy ()->GetObject<NrSpectrumPhy> ();
  Ptr<SpectrumChannel> dlChannel = enbDlSpectrumPhy->GetChannel ();

  Ptr<NrSimpleSpectrumPhy> testDlSpectrumPhy = CreateObject<NrSimpleSpectrumPhy> ();
  Ptr<NrEnbNetDevice> eNbDev = enbDevs.Get (0)->GetObject<NrEnbNetDevice> ();
  testDlSpectrumPhy->SetRxSpectrumModel (NrSpectrumValueHelper::GetSpectrumModel (eNbDev->GetDlEarfcn (), m_dlBandwidth));
  dlChannel->AddRx (testDlSpectrumPhy);

  testDlSpectrumPhy->TraceConnectWithoutContext ("RxStart",
                                                 MakeBoundCallback (&DlDataRxStartNofitication, this));

  //Test SpectrumPhy to get signals form UL channel
  Ptr<NrSpectrumPhy> ueUlSpectrumPhy = ueDevs.Get (0)->GetObject<NrUeNetDevice> ()->GetPhy ()->GetUplinkSpectrumPhy ()->GetObject<NrSpectrumPhy> ();
  Ptr<SpectrumChannel> ulChannel = ueUlSpectrumPhy->GetChannel ();

  Ptr<NrSimpleSpectrumPhy> testUlSpectrumPhy = CreateObject<NrSimpleSpectrumPhy> ();
  testUlSpectrumPhy->SetRxSpectrumModel (NrSpectrumValueHelper::GetSpectrumModel (eNbDev->GetUlEarfcn (), m_ulBandwidth));
  ulChannel->AddRx (testUlSpectrumPhy);

  testUlSpectrumPhy->TraceConnectWithoutContext ("RxStart",
                                                 MakeBoundCallback (&UlDataRxStartNofitication, this));

  Simulator::Stop (Seconds (0.500));
  Simulator::Run ();

  NS_TEST_ASSERT_MSG_EQ (m_usedMutedDlRbg, false,
                         "Scheduler used DL RBG muted by FFR Algorithm");

  NS_TEST_ASSERT_MSG_EQ (m_usedMutedUlRbg, false,
                         "Scheduler used UL RBG muted by FFR Algorithm");

  Simulator::Destroy ();
}

void
DlDataRxStartNofiticationArea (NrFrAreaTestCase *testcase,
                               Ptr<const SpectrumValue> spectrumValue)
{
  testcase->DlDataRxStart (spectrumValue);
}

void
UlDataRxStartNofiticationArea (NrFrAreaTestCase *testcase,
                               Ptr<const SpectrumValue> spectrumValue)
{
  testcase->UlDataRxStart (spectrumValue);
}

NrFrAreaTestCase::NrFrAreaTestCase (std::string name, std::string schedulerType)
  : TestCase ("Test: " + name),
    m_schedulerType (schedulerType)
{
  m_dlBandwidth = 25;
  m_ulBandwidth = 25;
  m_usedWrongDlRbg = false;
  m_usedWrongUlRbg = false;
}
NrFrAreaTestCase::~NrFrAreaTestCase ()
{
}

void
NrFrAreaTestCase::DlDataRxStart (Ptr<const SpectrumValue> spectrumValue)
{
  //need time to report new UE measurements, and wait because of filtering
  if ( (Simulator::Now () - m_teleportTime ) < MilliSeconds (400))
    {
      return;
    }

  NS_LOG_DEBUG ("DL DATA Power allocation :");
  Values::const_iterator it;
  uint32_t i = 0;
  for (it = spectrumValue->ConstValuesBegin (); it != spectrumValue->ConstValuesEnd (); it++)
    {
      double power =  (*it) * (m_dlBandwidth * 180000);
      NS_LOG_DEBUG ("RB " << i << " POWER: " << " " << power);
      NS_LOG_DEBUG ("RB " << i << " POWER: " << " " << power
                          << " Available: " << m_expectedDlRb[i] << " Expected Power: " << m_expectedDlPower );

      if (m_expectedDlRb[i] == false && power > 0)
        {
          m_usedWrongDlRbg = true;
        }
      else if (m_expectedDlRb[i] == true && power > 0)
        {
          NS_TEST_ASSERT_MSG_EQ_TOL (power, m_expectedDlPower, 0.1,
                                     "Wrong Data Channel DL Power level");
        }
      i++;
    }
}

void
NrFrAreaTestCase::UlDataRxStart (Ptr<const SpectrumValue> spectrumValue)
{
  //need time to report new UE measurements, and wait because of filtering
  if ( (Simulator::Now () - m_teleportTime ) < MilliSeconds (400))
    {
      return;
    }

  NS_LOG_DEBUG ("UL DATA Power allocation :");
  Values::const_iterator it;
  uint32_t i = 0;
  for (it = spectrumValue->ConstValuesBegin (); it != spectrumValue->ConstValuesEnd (); it++)
    {
      double power =  (*it) * (m_ulBandwidth * 180000);
      NS_LOG_DEBUG ("RB " << i << " POWER: " << power << " expectedUlPower: " << m_expectedUlPower);
      if (m_expectedUlRb[i] == false && power > 0)
        {
          m_usedWrongUlRbg = true;
        }
      else if (m_expectedUlRb[i] == true && power > 0)
        {
          NS_TEST_ASSERT_MSG_EQ_TOL (power, m_expectedUlPower, 0.01,
                                     "Wrong Data Channel UL Power level" << Simulator::Now ().GetSeconds ());
        }
      i++;
    }
}

void
NrFrAreaTestCase::SimpleTeleportUe (uint32_t x, uint32_t y)
{
  NS_LOG_FUNCTION (this);
  NS_LOG_DEBUG ("Teleport UE to : (" << x << ", " << y << ", 0)");
  m_teleportTime = Simulator::Now ();
  m_ueMobility->SetPosition (Vector (x, y, 0.0));
}

void
NrFrAreaTestCase::TeleportUe (uint32_t x, uint32_t y, double expectedPower,
                               std::vector<bool> expectedDlRb)
{
  NS_LOG_FUNCTION (this);
  NS_LOG_DEBUG ("Teleport UE to : (" << x << ", " << y << ", 0)");
  m_teleportTime = Simulator::Now ();
  m_ueMobility->SetPosition (Vector (x, y, 0.0));
  m_expectedDlPower = expectedPower;
  m_expectedDlRb = expectedDlRb;
}

void
NrFrAreaTestCase::TeleportUe2 (Ptr<Node> ueNode, uint32_t x, uint32_t y, double expectedPower,
                                std::vector<bool> expectedDlRb)
{
  NS_LOG_FUNCTION (this);
  NS_LOG_DEBUG ("Teleport UE to : (" << x << ", " << y << ", 0)");

  Ptr<MobilityModel> ueMobility = ueNode->GetObject<MobilityModel> ();
  ueMobility->SetPosition (Vector (x, y, 0.0));
  m_teleportTime = Simulator::Now ();
  m_expectedDlPower = expectedPower;
  m_expectedDlRb = expectedDlRb;
}

void
NrFrAreaTestCase::SetDlExpectedValues (double expectedDlPower, std::vector<bool> expectedDlRb)
{
  NS_LOG_FUNCTION (this);
  m_expectedDlPower = expectedDlPower;
  m_expectedDlRb = expectedDlRb;
}

void
NrFrAreaTestCase::SetUlExpectedValues (double expectedUlPower, std::vector<bool> expectedUlRb)
{
  NS_LOG_FUNCTION (this);
  m_expectedUlPower = expectedUlPower;
  m_expectedUlRb = expectedUlRb;
}

void
NrFrAreaTestCase::DoRun (void)
{
}

NrStrictFrAreaTestCase::NrStrictFrAreaTestCase (std::string name, std::string schedulerType)
  : NrFrAreaTestCase (name, schedulerType)
{
  NS_LOG_INFO ("Creating NrFrTestCase");
}

NrStrictFrAreaTestCase::~NrStrictFrAreaTestCase ()
{
}

void
NrStrictFrAreaTestCase::DoRun (void)
{
  NS_LOG_DEBUG ("NrStrictFrAreaTestCase");

  Config::Reset ();
  Config::SetDefault ("ns3::NrHelper::UseIdealRrc", BooleanValue (true));
  Config::SetDefault ("ns3::NrSpectrumPhy::CtrlErrorModelEnabled", BooleanValue (false));
  Config::SetDefault ("ns3::NrSpectrumPhy::DataErrorModelEnabled", BooleanValue (false));

  double eNbTxPower = 30;
  Config::SetDefault ("ns3::NrEnbPhy::TxPower", DoubleValue (eNbTxPower));
  Config::SetDefault ("ns3::NrUePhy::TxPower", DoubleValue (10.0));
  Config::SetDefault ("ns3::NrUePhy::EnableUplinkPowerControl", BooleanValue (true));

  Config::SetDefault ("ns3::NrUePowerControl::ClosedLoop", BooleanValue (true));
  Config::SetDefault ("ns3::NrUePowerControl::AccumulationEnabled", BooleanValue (false));

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
   *  eNB1            UE1                                      eNB2
   *    |              |                                         |
   *    x ------------ x ------------------------ x ------------ x----UE2
   *         200 m               600 m                  200 m      20 m
   *
   */

  Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator> ();
  positionAlloc->Add (Vector (0.0, 0.0, 0.0));   // eNB1
  positionAlloc->Add (Vector (1000, 0.0, 0.0)); // eNB2
  positionAlloc->Add (Vector (0.0, 0.0, 0.0));  // UE1
  positionAlloc->Add (Vector (1020, 0.0, 0.0));  // UE2
  MobilityHelper mobility;
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.SetPositionAllocator (positionAlloc);
  mobility.Install (allNodes);
  m_ueMobility = ueNodes1.Get (0)->GetObject<MobilityModel> ();

  // Disable layer-3 filtering
  Config::SetDefault ("ns3::NrEnbRrc::RsrpFilterCoefficient",
                      UintegerValue (0));
  Config::SetDefault ("ns3::NrEnbRrc::RsrqFilterCoefficient",
                      UintegerValue (0));

  // Create Devices and install them in the Nodes (eNB and UE)
  NetDeviceContainer enbDevs;
  NetDeviceContainer ueDevs1;
  NetDeviceContainer ueDevs2;
  nrHelper->SetSchedulerType (m_schedulerType);

  nrHelper->SetFfrAlgorithmType ("ns3::NrFrStrictAlgorithm");
  nrHelper->SetFfrAlgorithmAttribute ("RsrqThreshold", UintegerValue (25));
  nrHelper->SetFfrAlgorithmAttribute ("CenterPowerOffset",
                                       UintegerValue (NrRrcSap::PdschConfigDedicated::dB0));
  nrHelper->SetFfrAlgorithmAttribute ("EdgePowerOffset",
                                       UintegerValue (NrRrcSap::PdschConfigDedicated::dB3));

  nrHelper->SetFfrAlgorithmAttribute ("DlCommonSubBandwidth", UintegerValue (6));
  nrHelper->SetFfrAlgorithmAttribute ("DlEdgeSubBandOffset", UintegerValue (6));
  nrHelper->SetFfrAlgorithmAttribute ("DlEdgeSubBandwidth", UintegerValue (6));

  nrHelper->SetFfrAlgorithmAttribute ("UlCommonSubBandwidth", UintegerValue (6));
  nrHelper->SetFfrAlgorithmAttribute ("UlEdgeSubBandOffset", UintegerValue (6));
  nrHelper->SetFfrAlgorithmAttribute ("UlEdgeSubBandwidth", UintegerValue (6));
  enbDevs.Add (nrHelper->InstallEnbDevice (enbNodes.Get (0)));

  nrHelper->SetFfrAlgorithmType ("ns3::NrFrNoOpAlgorithm");
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

  //Test SpectrumPhy to get signals form DL channel
  Ptr<NrSpectrumPhy> enbDlSpectrumPhy = enbDevs.Get (0)->GetObject<NrEnbNetDevice> ()->GetPhy ()->GetDownlinkSpectrumPhy ()->GetObject<NrSpectrumPhy> ();
  Ptr<SpectrumChannel> dlChannel = enbDlSpectrumPhy->GetChannel ();

  Ptr<NrSimpleSpectrumPhy> testDlSpectrumPhy = CreateObject<NrSimpleSpectrumPhy> ();
  Ptr<NrEnbNetDevice> eNbDev = enbDevs.Get (0)->GetObject<NrEnbNetDevice> ();
  testDlSpectrumPhy->SetRxSpectrumModel (NrSpectrumValueHelper::GetSpectrumModel (eNbDev->GetDlEarfcn (), m_dlBandwidth));
  dlChannel->AddRx (testDlSpectrumPhy);

  testDlSpectrumPhy->SetCellId (1);

  testDlSpectrumPhy->TraceConnectWithoutContext ("RxStart",
                                                 MakeBoundCallback (&DlDataRxStartNofiticationArea, this));

  //Test SpectrumPhy to get signals form UL channel
  Ptr<NrSpectrumPhy> ueUlSpectrumPhy = ueDevs1.Get (0)->GetObject<NrUeNetDevice> ()->GetPhy ()->GetUplinkSpectrumPhy ()->GetObject<NrSpectrumPhy> ();
  Ptr<SpectrumChannel> ulChannel = ueUlSpectrumPhy->GetChannel ();

  Ptr<NrSimpleSpectrumPhy> testUlSpectrumPhy = CreateObject<NrSimpleSpectrumPhy> ();
  testUlSpectrumPhy->SetRxSpectrumModel (NrSpectrumValueHelper::GetSpectrumModel (eNbDev->GetUlEarfcn (), m_ulBandwidth));
  ulChannel->AddRx (testUlSpectrumPhy);

  testUlSpectrumPhy->SetCellId (1);

  testUlSpectrumPhy->TraceConnectWithoutContext ("RxStart",
                                                 MakeBoundCallback (&UlDataRxStartNofiticationArea, this));

  std::vector<bool> expectedDlRbCenterArea;
  expectedDlRbCenterArea.resize (m_dlBandwidth, false);
  std::vector<bool> expectedUlRbCenterArea;
  expectedUlRbCenterArea.resize (m_ulBandwidth, false);
  for (uint32_t i = 0; i < 6; i++)
    {
      expectedDlRbCenterArea[i] = true;
      expectedUlRbCenterArea[i] = true;
    }

  std::vector<bool> expectedDlRbEdgeArea;
  expectedDlRbEdgeArea.resize (m_dlBandwidth, false);
  std::vector<bool> expectedUlRbEdgeArea;
  expectedUlRbEdgeArea.resize (m_ulBandwidth, false);
  for (uint32_t i = 12; i < 18; i++)
    {
      expectedDlRbEdgeArea[i] = true;
      expectedUlRbEdgeArea[i] = true;
    }

  Simulator::Schedule (MilliSeconds (1),
                       &NrFrAreaTestCase::TeleportUe, this, 200, 0, 1, expectedDlRbCenterArea );
  Simulator::Schedule (MilliSeconds (1),
                       &NrFrAreaTestCase::SetUlExpectedValues, this, 0.0150543, expectedUlRbCenterArea );

  Simulator::Schedule (MilliSeconds (501),
                       &NrFrAreaTestCase::TeleportUe, this, 800, 0, 2, expectedDlRbEdgeArea );
  Simulator::Schedule (MilliSeconds (501),
                       &NrFrAreaTestCase::SetUlExpectedValues, this, 0.199526, expectedUlRbEdgeArea );

  Simulator::Schedule (MilliSeconds (1001),
                       &NrFrAreaTestCase::TeleportUe, this, 200, 0, 1, expectedDlRbCenterArea );
  Simulator::Schedule (MilliSeconds (1001),
                       &NrFrAreaTestCase::SetUlExpectedValues, this, 0.0150543, expectedUlRbCenterArea );

  Simulator::Stop (Seconds (1.500));
  Simulator::Run ();

  NS_TEST_ASSERT_MSG_EQ (m_usedWrongDlRbg, false,
                         "Scheduler used DL RB muted by FFR Algorithm");
  NS_TEST_ASSERT_MSG_EQ (m_usedWrongUlRbg, false,
                         "Scheduler used UL RB muted by FFR Algorithm");

  Simulator::Destroy ();
}

NrSoftFrAreaTestCase::NrSoftFrAreaTestCase (std::string name, std::string schedulerType)
  : NrFrAreaTestCase (name, schedulerType)
{
  NS_LOG_INFO ("Creating NrSoftFrAreaTestCase");
}

NrSoftFrAreaTestCase::~NrSoftFrAreaTestCase ()
{
}

void
NrSoftFrAreaTestCase::DoRun (void)
{
  NS_LOG_DEBUG ("NrSoftFrAreaTestCase");

  Config::Reset ();
  Config::SetDefault ("ns3::NrHelper::UseIdealRrc", BooleanValue (true));
  Config::SetDefault ("ns3::NrSpectrumPhy::CtrlErrorModelEnabled", BooleanValue (false));
  Config::SetDefault ("ns3::NrSpectrumPhy::DataErrorModelEnabled", BooleanValue (false));

  double eNbTxPower = 30;
  Config::SetDefault ("ns3::NrEnbPhy::TxPower", DoubleValue (eNbTxPower));
  Config::SetDefault ("ns3::NrUePhy::TxPower", DoubleValue (10.0));
  Config::SetDefault ("ns3::NrUePhy::EnableUplinkPowerControl", BooleanValue (true));

  Config::SetDefault ("ns3::NrUePowerControl::ClosedLoop", BooleanValue (true));
  Config::SetDefault ("ns3::NrUePowerControl::AccumulationEnabled", BooleanValue (false));

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
   *  eNB1            UE1                                      eNB2
   *    |              |                                         |
   *    x ------------ x ------------------------ x ------------ x----UE2
   *         200 m               600 m                  200 m      20 m
   *
   */

  Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator> ();
  positionAlloc->Add (Vector (0.0, 0.0, 0.0));   // eNB1
  positionAlloc->Add (Vector (1000, 0.0, 0.0)); // eNB2
  positionAlloc->Add (Vector (0.0, 0.0, 0.0));  // UE1
  positionAlloc->Add (Vector (1020, 0.0, 0.0));  // UE2
  MobilityHelper mobility;
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.SetPositionAllocator (positionAlloc);
  mobility.Install (allNodes);
  m_ueMobility = ueNodes1.Get (0)->GetObject<MobilityModel> ();

  // Disable layer-3 filtering
  Config::SetDefault ("ns3::NrEnbRrc::RsrpFilterCoefficient",
                      UintegerValue (0));
  Config::SetDefault ("ns3::NrEnbRrc::RsrqFilterCoefficient",
                      UintegerValue (0));

  // Create Devices and install them in the Nodes (eNB and UE)
  NetDeviceContainer enbDevs;
  NetDeviceContainer ueDevs1;
  NetDeviceContainer ueDevs2;
  nrHelper->SetSchedulerType (m_schedulerType);

  nrHelper->SetFfrAlgorithmType ("ns3::NrFrSoftAlgorithm");
  nrHelper->SetFfrAlgorithmAttribute ("AllowCenterUeUseEdgeSubBand", BooleanValue (false));
  nrHelper->SetFfrAlgorithmAttribute ("RsrqThreshold", UintegerValue (25));
  nrHelper->SetFfrAlgorithmAttribute ("CenterPowerOffset",
                                       UintegerValue (NrRrcSap::PdschConfigDedicated::dB0));
  nrHelper->SetFfrAlgorithmAttribute ("EdgePowerOffset",
                                       UintegerValue (NrRrcSap::PdschConfigDedicated::dB3));

  nrHelper->SetFfrAlgorithmAttribute ("DlEdgeSubBandOffset", UintegerValue (8));
  nrHelper->SetFfrAlgorithmAttribute ("DlEdgeSubBandwidth", UintegerValue (8));
  nrHelper->SetFfrAlgorithmAttribute ("UlEdgeSubBandOffset", UintegerValue (8));
  nrHelper->SetFfrAlgorithmAttribute ("UlEdgeSubBandwidth", UintegerValue (8));
  enbDevs.Add (nrHelper->InstallEnbDevice (enbNodes.Get (0)));

  nrHelper->SetFfrAlgorithmType ("ns3::NrFrNoOpAlgorithm");
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

  //Test SpectrumPhy to get signals form DL channel
  Ptr<NrSpectrumPhy> enbDlSpectrumPhy = enbDevs.Get (0)->GetObject<NrEnbNetDevice> ()->GetPhy ()->GetDownlinkSpectrumPhy ()->GetObject<NrSpectrumPhy> ();
  Ptr<SpectrumChannel> dlChannel = enbDlSpectrumPhy->GetChannel ();

  Ptr<NrSimpleSpectrumPhy> testDlSpectrumPhy = CreateObject<NrSimpleSpectrumPhy> ();
  Ptr<NrEnbNetDevice> eNbDev = enbDevs.Get (0)->GetObject<NrEnbNetDevice> ();
  testDlSpectrumPhy->SetRxSpectrumModel (NrSpectrumValueHelper::GetSpectrumModel (eNbDev->GetDlEarfcn (), m_dlBandwidth));
  dlChannel->AddRx (testDlSpectrumPhy);

  testDlSpectrumPhy->SetCellId (1);

  testDlSpectrumPhy->TraceConnectWithoutContext ("RxStart",
                                                 MakeBoundCallback (&DlDataRxStartNofiticationArea, this));

  //Test SpectrumPhy to get signals form UL channel
  Ptr<NrSpectrumPhy> ueUlSpectrumPhy = ueDevs1.Get (0)->GetObject<NrUeNetDevice> ()->GetPhy ()->GetUplinkSpectrumPhy ()->GetObject<NrSpectrumPhy> ();
  Ptr<SpectrumChannel> ulChannel = ueUlSpectrumPhy->GetChannel ();

  Ptr<NrSimpleSpectrumPhy> testUlSpectrumPhy = CreateObject<NrSimpleSpectrumPhy> ();
  testUlSpectrumPhy->SetRxSpectrumModel (NrSpectrumValueHelper::GetSpectrumModel (eNbDev->GetUlEarfcn (), m_ulBandwidth));
  ulChannel->AddRx (testUlSpectrumPhy);

  testUlSpectrumPhy->SetCellId (1);

  testUlSpectrumPhy->TraceConnectWithoutContext ("RxStart",
                                                 MakeBoundCallback (&UlDataRxStartNofiticationArea, this));


  std::vector<bool> expectedDlRbCenterArea;
  expectedDlRbCenterArea.resize (m_dlBandwidth, false);
  std::vector<bool> expectedUlRbCenterArea;
  expectedUlRbCenterArea.resize (m_ulBandwidth, false);
  for (uint32_t i = 0; i < 8; i++)
    {
      expectedDlRbCenterArea[i] = true;
      expectedUlRbCenterArea[i] = true;
    }
  for (uint32_t i = 16; i < 25; i++)
    {
      expectedDlRbCenterArea[i] = true;
    }

  std::vector<bool> expectedDlRbEdgeArea;
  expectedDlRbEdgeArea.resize (m_dlBandwidth, false);
  std::vector<bool> expectedUlRbEdgeArea;
  expectedUlRbEdgeArea.resize (m_ulBandwidth, false);
  for (uint32_t i = 8; i < 16; i++)
    {
      expectedDlRbEdgeArea[i] = true;
      expectedUlRbEdgeArea[i] = true;
    }

  Simulator::Schedule (MilliSeconds (1),
                       &NrFrAreaTestCase::TeleportUe, this, 200, 0, 1, expectedDlRbCenterArea );
  Simulator::Schedule (MilliSeconds (1),
                       &NrFrAreaTestCase::SetUlExpectedValues, this, 0.0150543, expectedUlRbCenterArea );

  Simulator::Schedule (MilliSeconds (501),
                       &NrFrAreaTestCase::TeleportUe, this, 800, 0, 2, expectedDlRbEdgeArea );
  Simulator::Schedule (MilliSeconds (501),
                       &NrFrAreaTestCase::SetUlExpectedValues, this, 0.199526, expectedUlRbEdgeArea );

  Simulator::Schedule (MilliSeconds (1001),
                       &NrFrAreaTestCase::TeleportUe, this, 200, 0, 1, expectedDlRbCenterArea );
  Simulator::Schedule (MilliSeconds (1001),
                       &NrFrAreaTestCase::SetUlExpectedValues, this, 0.0150543, expectedUlRbCenterArea );

  Simulator::Stop (Seconds (1.500));
  Simulator::Run ();

#if 0
  NS_TEST_ASSERT_MSG_EQ (m_usedWrongDlRbg, false,
                         "Scheduler used DL RBG muted by FFR Algorithm");

#endif
  NS_TEST_ASSERT_MSG_EQ (m_usedWrongUlRbg, false,
                         "Scheduler used UL RB muted by FFR Algorithm");

  Simulator::Destroy ();
}

NrSoftFfrAreaTestCase::NrSoftFfrAreaTestCase (std::string name, std::string schedulerType)
  : NrFrAreaTestCase (name, schedulerType)
{
  NS_LOG_INFO ("Creating NrSoftFfrAreaTestCase");
}

NrSoftFfrAreaTestCase::~NrSoftFfrAreaTestCase ()
{
}

void
NrSoftFfrAreaTestCase::DoRun (void)
{
  NS_LOG_DEBUG ("NrSoftFfrAreaTestCase");

  Config::Reset ();
  Config::SetDefault ("ns3::NrHelper::UseIdealRrc", BooleanValue (true));
  Config::SetDefault ("ns3::NrSpectrumPhy::CtrlErrorModelEnabled", BooleanValue (false));
  Config::SetDefault ("ns3::NrSpectrumPhy::DataErrorModelEnabled", BooleanValue (false));

  double eNbTxPower = 30;
  Config::SetDefault ("ns3::NrEnbPhy::TxPower", DoubleValue (eNbTxPower));
  Config::SetDefault ("ns3::NrUePhy::TxPower", DoubleValue (10.0));
  Config::SetDefault ("ns3::NrUePhy::EnableUplinkPowerControl", BooleanValue (true));

  Config::SetDefault ("ns3::NrUePowerControl::ClosedLoop", BooleanValue (true));
  Config::SetDefault ("ns3::NrUePowerControl::AccumulationEnabled", BooleanValue (false));

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
   *  eNB1            UE1                                      eNB2
   *    |              |                                         |
   *    x ------------ x ------------------------ x ------------ x----UE2
   *         200 m               600 m                  200 m      20 m
   *
   */

  Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator> ();
  positionAlloc->Add (Vector (0.0, 0.0, 0.0));   // eNB1
  positionAlloc->Add (Vector (1000, 0.0, 0.0)); // eNB2
  positionAlloc->Add (Vector (0.0, 0.0, 0.0));  // UE1
  positionAlloc->Add (Vector (1020, 0.0, 0.0));  // UE2
  MobilityHelper mobility;
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.SetPositionAllocator (positionAlloc);
  mobility.Install (allNodes);
  m_ueMobility = ueNodes1.Get (0)->GetObject<MobilityModel> ();

  // Disable layer-3 filtering
  Config::SetDefault ("ns3::NrEnbRrc::RsrpFilterCoefficient",
                      UintegerValue (0));
  Config::SetDefault ("ns3::NrEnbRrc::RsrqFilterCoefficient",
                      UintegerValue (0));

  // Create Devices and install them in the Nodes (eNB and UE)
  NetDeviceContainer enbDevs;
  NetDeviceContainer ueDevs1;
  NetDeviceContainer ueDevs2;
  nrHelper->SetSchedulerType (m_schedulerType);

  nrHelper->SetFfrAlgorithmType ("ns3::NrFfrSoftAlgorithm");
  nrHelper->SetFfrAlgorithmAttribute ("CenterRsrqThreshold", UintegerValue (28));
  nrHelper->SetFfrAlgorithmAttribute ("EdgeRsrqThreshold", UintegerValue (18));
  nrHelper->SetFfrAlgorithmAttribute ("CenterAreaPowerOffset",
                                       UintegerValue (NrRrcSap::PdschConfigDedicated::dB_3));
  nrHelper->SetFfrAlgorithmAttribute ("MediumAreaPowerOffset",
                                       UintegerValue (NrRrcSap::PdschConfigDedicated::dB0));
  nrHelper->SetFfrAlgorithmAttribute ("EdgeAreaPowerOffset",
                                       UintegerValue (NrRrcSap::PdschConfigDedicated::dB3));

  nrHelper->SetFfrAlgorithmAttribute ("UlCommonSubBandwidth", UintegerValue (6));
  nrHelper->SetFfrAlgorithmAttribute ("DlCommonSubBandwidth", UintegerValue (6));

  nrHelper->SetFfrAlgorithmAttribute ("DlEdgeSubBandOffset", UintegerValue (6));
  nrHelper->SetFfrAlgorithmAttribute ("DlEdgeSubBandwidth", UintegerValue (6));
  nrHelper->SetFfrAlgorithmAttribute ("UlEdgeSubBandOffset", UintegerValue (6));
  nrHelper->SetFfrAlgorithmAttribute ("UlEdgeSubBandwidth", UintegerValue (6));
  enbDevs.Add (nrHelper->InstallEnbDevice (enbNodes.Get (0)));

  nrHelper->SetFfrAlgorithmType ("ns3::NrFrNoOpAlgorithm");
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

  //Test SpectrumPhy to get signals form DL channel
  Ptr<NrSpectrumPhy> enbDlSpectrumPhy = enbDevs.Get (0)->GetObject<NrEnbNetDevice> ()->GetPhy ()->GetDownlinkSpectrumPhy ()->GetObject<NrSpectrumPhy> ();
  Ptr<SpectrumChannel> dlChannel = enbDlSpectrumPhy->GetChannel ();

  Ptr<NrSimpleSpectrumPhy> testDlSpectrumPhy = CreateObject<NrSimpleSpectrumPhy> ();
  Ptr<NrEnbNetDevice> eNbDev = enbDevs.Get (0)->GetObject<NrEnbNetDevice> ();
  testDlSpectrumPhy->SetRxSpectrumModel (NrSpectrumValueHelper::GetSpectrumModel (eNbDev->GetDlEarfcn (), m_dlBandwidth));
  dlChannel->AddRx (testDlSpectrumPhy);

  testDlSpectrumPhy->SetCellId (1);

  testDlSpectrumPhy->TraceConnectWithoutContext ("RxStart",
                                                 MakeBoundCallback (&DlDataRxStartNofiticationArea, this));

  //Test SpectrumPhy to get signals form UL channel
  Ptr<NrSpectrumPhy> ueUlSpectrumPhy = ueDevs1.Get (0)->GetObject<NrUeNetDevice> ()->GetPhy ()->GetUplinkSpectrumPhy ()->GetObject<NrSpectrumPhy> ();
  Ptr<SpectrumChannel> ulChannel = ueUlSpectrumPhy->GetChannel ();

  Ptr<NrSimpleSpectrumPhy> testUlSpectrumPhy = CreateObject<NrSimpleSpectrumPhy> ();
  testUlSpectrumPhy->SetRxSpectrumModel (NrSpectrumValueHelper::GetSpectrumModel (eNbDev->GetUlEarfcn (), m_ulBandwidth));
  ulChannel->AddRx (testUlSpectrumPhy);

  testUlSpectrumPhy->SetCellId (1);

  testUlSpectrumPhy->TraceConnectWithoutContext ("RxStart",
                                                 MakeBoundCallback (&UlDataRxStartNofiticationArea, this));

  double expectedDlPowerCenterArea = 0.5;
  std::vector<bool> expectedDlRbCenterArea;
  expectedDlRbCenterArea.resize (m_dlBandwidth, false);
  std::vector<bool> expectedUlRbCenterArea;
  expectedUlRbCenterArea.resize (m_ulBandwidth, false);
  for (uint32_t i = 6; i < 12; i++)
    {
      expectedDlRbCenterArea[i] = true;
      expectedUlRbCenterArea[i] = true;
    }
  for (uint32_t i = 18; i < 25; i++)
    {
      expectedDlRbCenterArea[i] = true;
      expectedUlRbCenterArea[i] = true;
    }

  double expectedDlPowerMiddleArea = 1.0;
  std::vector<bool> expectedDlRbMiddleArea;
  expectedDlRbMiddleArea.resize (m_dlBandwidth, false);
  std::vector<bool> expectedUlRbMiddleArea;
  expectedUlRbMiddleArea.resize (m_ulBandwidth, false);
  for (uint32_t i = 0; i < 6; i++)
    {
      expectedDlRbMiddleArea[i] = true;
      expectedUlRbMiddleArea[i] = true;
    }

  double expectedDlPowerEdgeArea = 2.0;
  std::vector<bool> expectedDlRbEdgeArea;
  expectedDlRbEdgeArea.resize (m_dlBandwidth, false);
  std::vector<bool> expectedUlRbEdgeArea;
  expectedUlRbEdgeArea.resize (m_ulBandwidth, false);
  for (uint32_t i = 12; i < 18; i++)
    {
      expectedDlRbEdgeArea[i] = true;
      expectedUlRbEdgeArea[i] = true;
    }

  Simulator::Schedule (MilliSeconds (1),
                       &NrFrAreaTestCase::TeleportUe, this, 200, 0,
                       expectedDlPowerCenterArea, expectedDlRbCenterArea );
  Simulator::Schedule (MilliSeconds (1),
                       &NrFrAreaTestCase::SetUlExpectedValues, this, 0.0150543, expectedUlRbCenterArea );

  Simulator::Schedule (MilliSeconds (501),
                       &NrFrAreaTestCase::TeleportUe, this, 600, 0,
                       expectedDlPowerMiddleArea, expectedDlRbMiddleArea );
  Simulator::Schedule (MilliSeconds (501),
                       &NrFrAreaTestCase::SetUlExpectedValues, this, 0.135489, expectedUlRbMiddleArea );

  Simulator::Schedule (MilliSeconds (1001),
                       &NrFrAreaTestCase::TeleportUe, this, 800, 0,
                       expectedDlPowerEdgeArea, expectedDlRbEdgeArea );
  Simulator::Schedule (MilliSeconds (1001),
                       &NrFrAreaTestCase::SetUlExpectedValues, this, 0.199526, expectedUlRbEdgeArea );

  Simulator::Schedule (MilliSeconds (1501),
                       &NrFrAreaTestCase::TeleportUe, this, 600, 0,
                       expectedDlPowerMiddleArea, expectedDlRbMiddleArea );
  Simulator::Schedule (MilliSeconds (1501),
                       &NrFrAreaTestCase::SetUlExpectedValues, this, 0.135489, expectedUlRbMiddleArea );

  Simulator::Schedule (MilliSeconds (2001),
                       &NrFrAreaTestCase::TeleportUe, this, 200, 0,
                       expectedDlPowerCenterArea, expectedDlRbCenterArea );
  Simulator::Schedule (MilliSeconds (2001),
                       &NrFrAreaTestCase::SetUlExpectedValues, this, 0.0150543, expectedUlRbCenterArea );

  Simulator::Stop (Seconds (2.500));
  Simulator::Run ();

  NS_TEST_ASSERT_MSG_EQ (m_usedWrongDlRbg, false,
                         "Scheduler used DL RBG muted by FFR Algorithm");

  NS_TEST_ASSERT_MSG_EQ (m_usedWrongUlRbg, false,
                         "Scheduler used UL RB muted by FFR Algorithm");

  Simulator::Destroy ();
}


NrEnhancedFfrAreaTestCase::NrEnhancedFfrAreaTestCase (std::string name, std::string schedulerType)
  : NrFrAreaTestCase (name, schedulerType)
{
  NS_LOG_INFO ("Creating NrEnhancedFfrAreaTestCase");
}

NrEnhancedFfrAreaTestCase::~NrEnhancedFfrAreaTestCase ()
{
}

void
NrEnhancedFfrAreaTestCase::DoRun (void)
{
  NS_LOG_DEBUG ("NrEnhancedFfrAreaTestCase");

  Config::Reset ();
  Config::SetDefault ("ns3::NrHelper::UseIdealRrc", BooleanValue (true));
  Config::SetDefault ("ns3::NrHelper::UsePdschForCqiGeneration", BooleanValue (true));
  Config::SetDefault ("ns3::NrSpectrumPhy::CtrlErrorModelEnabled", BooleanValue (false));
  Config::SetDefault ("ns3::NrSpectrumPhy::DataErrorModelEnabled", BooleanValue (false));

  double eNbTxPower = 30;
  Config::SetDefault ("ns3::NrEnbPhy::TxPower", DoubleValue (eNbTxPower));
  Config::SetDefault ("ns3::NrUePhy::TxPower", DoubleValue (10.0));
  Config::SetDefault ("ns3::NrUePhy::EnableUplinkPowerControl", BooleanValue (true));

  Config::SetDefault ("ns3::NrUePowerControl::ClosedLoop", BooleanValue (true));
  Config::SetDefault ("ns3::NrUePowerControl::AccumulationEnabled", BooleanValue (false));


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
   *  eNB1            UE1                                      eNB2
   *    |              |                                         |
   *    x ------------ x ------------------------ x ------------ x----UE2
   *         200 m               600 m                  200 m      20 m
   *
   */

  Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator> ();
  positionAlloc->Add (Vector (0.0, 0.0, 0.0));   // eNB1
  positionAlloc->Add (Vector (1000, 0.0, 0.0)); // eNB2
  positionAlloc->Add (Vector (0.0, 0.0, 0.0));  // UE1
  positionAlloc->Add (Vector (1020, 0.0, 0.0));  // UE2
  MobilityHelper mobility;
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.SetPositionAllocator (positionAlloc);
  mobility.Install (allNodes);
  m_ueMobility = ueNodes1.Get (0)->GetObject<MobilityModel> ();

  // Disable layer-3 filtering
  Config::SetDefault ("ns3::NrEnbRrc::RsrpFilterCoefficient",
                      UintegerValue (0));
  Config::SetDefault ("ns3::NrEnbRrc::RsrqFilterCoefficient",
                      UintegerValue (0));

  // Create Devices and install them in the Nodes (eNB and UE)
  NetDeviceContainer enbDevs;
  NetDeviceContainer ueDevs1;
  NetDeviceContainer ueDevs2;
  nrHelper->SetSchedulerType (m_schedulerType);
  nrHelper->SetSchedulerAttribute ("HarqEnabled",  BooleanValue (true));

  nrHelper->SetEnbDeviceAttribute ("DlBandwidth", UintegerValue (m_dlBandwidth));
  nrHelper->SetEnbDeviceAttribute ("UlBandwidth", UintegerValue (m_ulBandwidth));

  nrHelper->SetFfrAlgorithmType ("ns3::NrFfrEnhancedAlgorithm");
  nrHelper->SetFfrAlgorithmAttribute ("RsrqThreshold", UintegerValue (25));
  nrHelper->SetFfrAlgorithmAttribute ("DlCqiThreshold", UintegerValue (10));
  nrHelper->SetFfrAlgorithmAttribute ("UlCqiThreshold", UintegerValue (15));
  nrHelper->SetFfrAlgorithmAttribute ("CenterAreaPowerOffset",
                                       UintegerValue (NrRrcSap::PdschConfigDedicated::dB_6));
  nrHelper->SetFfrAlgorithmAttribute ("EdgeAreaPowerOffset",
                                       UintegerValue (NrRrcSap::PdschConfigDedicated::dB0));

  nrHelper->SetFfrAlgorithmAttribute ("UlSubBandOffset", UintegerValue (0));
  nrHelper->SetFfrAlgorithmAttribute ("UlReuse3SubBandwidth", UintegerValue (4));
  nrHelper->SetFfrAlgorithmAttribute ("UlReuse1SubBandwidth", UintegerValue (4));

  nrHelper->SetFfrAlgorithmAttribute ("DlSubBandOffset", UintegerValue (0));
  nrHelper->SetFfrAlgorithmAttribute ("DlReuse3SubBandwidth", UintegerValue (4));
  nrHelper->SetFfrAlgorithmAttribute ("DlReuse1SubBandwidth", UintegerValue (4));

  enbDevs.Add (nrHelper->InstallEnbDevice (enbNodes.Get (0)));

  nrHelper->SetFfrAlgorithmType ("ns3::NrFrNoOpAlgorithm");
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

  //Test SpectrumPhy to get signals form DL channel
  Ptr<NrSpectrumPhy> enbDlSpectrumPhy = enbDevs.Get (0)->GetObject<NrEnbNetDevice> ()->GetPhy ()->GetDownlinkSpectrumPhy ()->GetObject<NrSpectrumPhy> ();
  Ptr<SpectrumChannel> dlChannel = enbDlSpectrumPhy->GetChannel ();

  Ptr<NrSimpleSpectrumPhy> testDlSpectrumPhy = CreateObject<NrSimpleSpectrumPhy> ();
  Ptr<NrEnbNetDevice> eNbDev = enbDevs.Get (0)->GetObject<NrEnbNetDevice> ();
  testDlSpectrumPhy->SetRxSpectrumModel (NrSpectrumValueHelper::GetSpectrumModel (eNbDev->GetDlEarfcn (), m_dlBandwidth));
  dlChannel->AddRx (testDlSpectrumPhy);

  testDlSpectrumPhy->SetCellId (1);

  testDlSpectrumPhy->TraceConnectWithoutContext ("RxStart",
                                                 MakeBoundCallback (&DlDataRxStartNofiticationArea, this));

  //Test SpectrumPhy to get signals form UL channel
  Ptr<NrSpectrumPhy> ueUlSpectrumPhy = ueDevs1.Get (0)->GetObject<NrUeNetDevice> ()->GetPhy ()->GetUplinkSpectrumPhy ()->GetObject<NrSpectrumPhy> ();
  Ptr<SpectrumChannel> ulChannel = ueUlSpectrumPhy->GetChannel ();

  Ptr<NrSimpleSpectrumPhy> testUlSpectrumPhy = CreateObject<NrSimpleSpectrumPhy> ();
  testUlSpectrumPhy->SetRxSpectrumModel (NrSpectrumValueHelper::GetSpectrumModel (eNbDev->GetUlEarfcn (), m_ulBandwidth));
  ulChannel->AddRx (testUlSpectrumPhy);

  testUlSpectrumPhy->SetCellId (1);

  testUlSpectrumPhy->TraceConnectWithoutContext ("RxStart",
                                                 MakeBoundCallback (&UlDataRxStartNofiticationArea, this));

  double expectedDlPowerCenterArea = 0.251189;
  std::vector<bool> expectedDlRbCenterArea;
  expectedDlRbCenterArea.resize (m_dlBandwidth, false);
  std::vector<bool> expectedUlRbCenterArea;
  expectedUlRbCenterArea.resize (m_ulBandwidth, false);
  for (uint32_t i = 0; i < 8; i++)
    {
      expectedDlRbCenterArea[i] = true;
      expectedUlRbCenterArea[i] = true;
    }
  for (uint32_t i = 12; i < 16; i++)
    {
      expectedDlRbCenterArea[i] = true;
      expectedUlRbCenterArea[i] = true;
    }
  for (uint32_t i = 20; i < 24; i++)
    {
      expectedDlRbCenterArea[i] = true;
      expectedUlRbCenterArea[i] = true;
    }

  double expectedDlPowerMiddleArea = 0.251189;
  std::vector<bool> expectedDlRbMiddleArea;
  expectedDlRbMiddleArea.resize (m_dlBandwidth, false);
  std::vector<bool> expectedUlRbMiddleArea;
  expectedUlRbMiddleArea.resize (m_ulBandwidth, false);
  for (uint32_t i = 4; i < 8; i++)
    {
      expectedDlRbMiddleArea[i] = true;
      expectedUlRbMiddleArea[i] = true;
    }

  double expectedDlPowerEdgeArea = 1.0;
  std::vector<bool> expectedDlRbEdgeArea;
  expectedDlRbEdgeArea.resize (m_dlBandwidth, false);
  std::vector<bool> expectedUlRbEdgeArea;
  expectedUlRbEdgeArea.resize (m_ulBandwidth, false);
  for (uint32_t i = 0; i < 4; i++)
    {
      expectedDlRbEdgeArea[i] = true;
      expectedUlRbEdgeArea[i] = true;
    }

  Simulator::Schedule (MilliSeconds (1),
                       &NrEnhancedFfrAreaTestCase::TeleportUe, this, 100, 0,
                       expectedDlPowerCenterArea, expectedDlRbCenterArea );
  Simulator::Schedule (MilliSeconds (1),
                       &NrFrAreaTestCase::SetUlExpectedValues, this, 0.00250905, expectedUlRbCenterArea );

  Simulator::Schedule (MilliSeconds (501),
                       &NrEnhancedFfrAreaTestCase::TeleportUe, this, 300, 0,
                       expectedDlPowerMiddleArea, expectedDlRbMiddleArea );
  Simulator::Schedule (MilliSeconds (501),
                       &NrFrAreaTestCase::SetUlExpectedValues, this, 0.0225815, expectedUlRbMiddleArea );

  Simulator::Schedule (MilliSeconds (1001),
                       &NrEnhancedFfrAreaTestCase::TeleportUe, this, 600, 0,
                       expectedDlPowerEdgeArea, expectedDlRbEdgeArea );
  Simulator::Schedule (MilliSeconds (1001),
                       &NrFrAreaTestCase::SetUlExpectedValues, this, 0.0903259, expectedUlRbEdgeArea );

  Simulator::Schedule (MilliSeconds (1501),
                       &NrEnhancedFfrAreaTestCase::TeleportUe, this, 100, 0,
                       expectedDlPowerCenterArea, expectedDlRbCenterArea );
  Simulator::Schedule (MilliSeconds (1501),
                       &NrFrAreaTestCase::SetUlExpectedValues, this, 0.00250905, expectedUlRbCenterArea );

  Simulator::Schedule (MilliSeconds (2001),
                       &NrEnhancedFfrAreaTestCase::TeleportUe, this, 300, 0,
                       expectedDlPowerMiddleArea, expectedDlRbMiddleArea );
  Simulator::Schedule (MilliSeconds (2001),
                       &NrFrAreaTestCase::SetUlExpectedValues, this, 0.0225815, expectedUlRbCenterArea );

  Simulator::Stop (Seconds (2.500));
  Simulator::Run ();

  NS_TEST_ASSERT_MSG_EQ (m_usedWrongDlRbg, false,
                         "Scheduler used DL RBG muted by FFR Algorithm");

  NS_TEST_ASSERT_MSG_EQ (m_usedWrongUlRbg, false,
                         "Scheduler used UL RB muted by FFR Algorithm");

  Simulator::Destroy ();
}


NrDistributedFfrAreaTestCase::NrDistributedFfrAreaTestCase (std::string name, std::string schedulerType)
  : NrFrAreaTestCase (name, schedulerType)
{
  NS_LOG_INFO ("Creating NrDistributedFfrAreaTestCase");
}

NrDistributedFfrAreaTestCase::~NrDistributedFfrAreaTestCase ()
{
}

void
NrDistributedFfrAreaTestCase::DoRun (void)
{
  NS_LOG_DEBUG ("NrDistributedFfrAreaTestCase");

  Config::Reset ();
  Config::SetDefault ("ns3::NrHelper::UseIdealRrc", BooleanValue (true));
  Config::SetDefault ("ns3::NrHelper::UsePdschForCqiGeneration", BooleanValue (true));
  Config::SetDefault ("ns3::NrSpectrumPhy::CtrlErrorModelEnabled", BooleanValue (false));
  Config::SetDefault ("ns3::NrSpectrumPhy::DataErrorModelEnabled", BooleanValue (false));

  double eNbTxPower = 30;
  Config::SetDefault ("ns3::NrEnbPhy::TxPower", DoubleValue (eNbTxPower));
  Config::SetDefault ("ns3::NrUePhy::TxPower", DoubleValue (10.0));
  Config::SetDefault ("ns3::NrUePhy::EnableUplinkPowerControl", BooleanValue (true));

  Config::SetDefault ("ns3::NrUePowerControl::ClosedLoop", BooleanValue (true));
  Config::SetDefault ("ns3::NrUePowerControl::AccumulationEnabled", BooleanValue (false));

  // Disable layer-3 filtering
  Config::SetDefault ("ns3::NrEnbRrc::RsrpFilterCoefficient",
                      UintegerValue (0));
  Config::SetDefault ("ns3::NrEnbRrc::RsrqFilterCoefficient",
                      UintegerValue (0));

  uint8_t bandwidth = 25;

  Ptr<NrHelper> nrHelper = CreateObject<NrHelper> ();
  Ptr<PointToPointNgcHelper> ngcHelper = CreateObject<PointToPointNgcHelper> ();
  nrHelper->SetNgcHelper (ngcHelper);
  nrHelper->SetHandoverAlgorithmType ("ns3::NoOpHandoverAlgorithm"); // disable automatic handover

  Ptr<Node> upf = ngcHelper->GetUpfNode ();

  // Create a single RemoteHost
  NodeContainer remoteHostContainer;
  remoteHostContainer.Create (1);
  Ptr<Node> remoteHost = remoteHostContainer.Get (0);
  InternetStackHelper internet;
  internet.Install (remoteHostContainer);

  // Create the Internet
  PointToPointHelper p2ph;
  p2ph.SetDeviceAttribute ("DataRate", DataRateValue (DataRate ("100Gb/s")));
  p2ph.SetDeviceAttribute ("Mtu", UintegerValue (1500));
  p2ph.SetChannelAttribute ("Delay", TimeValue (Seconds (0.010)));
  NetDeviceContainer internetDevices = p2ph.Install (upf, remoteHost);
  Ipv4AddressHelper ipv4h;
  ipv4h.SetBase ("1.0.0.0", "255.0.0.0");
  Ipv4InterfaceContainer internetIpIfaces = ipv4h.Assign (internetDevices);
  Ipv4Address remoteHostAddr = internetIpIfaces.GetAddress (1);

  // Routing of the Internet Host (towards the NR network)
  Ipv4StaticRoutingHelper ipv4RoutingHelper;
  Ptr<Ipv4StaticRouting> remoteHostStaticRouting = ipv4RoutingHelper.GetStaticRouting (remoteHost->GetObject<Ipv4> ());
  // interface 0 is localhost, 1 is the p2p device
  remoteHostStaticRouting->AddNetworkRouteTo (Ipv4Address ("7.0.0.0"), Ipv4Mask ("255.0.0.0"), 1);

  // Create Nodes: eNodeB and UE
  NodeContainer enbNodes;
  NodeContainer ueNodes1;
  NodeContainer ueNodes2;
  enbNodes.Create (2);
  ueNodes1.Create (2);
  ueNodes2.Create (1);
  NodeContainer ueNodes = NodeContainer ( ueNodes1, ueNodes2);
  NodeContainer allNodes = NodeContainer ( enbNodes, ueNodes1, ueNodes2);

  /*
   * The topology is the following:
   *
   *  eNB1            UE1                        UE2            eNB2
   *    |              |                          |              |
   *    x ------------ x ------------------------ x ------------ x
   *         200 m               600 m                  200 m
   *
   */

  // Install Mobility Model
  Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator> ();
  positionAlloc->Add (Vector (0.0, 0.0, 0.0));                  // eNB1
  positionAlloc->Add (Vector (1000,  0.0, 0.0));                // eNB2

  positionAlloc->Add (Vector (200, 0.0, 0.0));                  // UE1
  positionAlloc->Add (Vector (200, 0.0, 0.0));                  // UE1
  positionAlloc->Add (Vector (800, 0.0, 0.0));                  // UE2

  MobilityHelper mobility;
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.SetPositionAllocator (positionAlloc);
  mobility.Install (allNodes);
  m_ueMobility = ueNodes2.Get (0)->GetObject<MobilityModel> ();

  // Create Devices and install them in the Nodes (eNB and UE)
  NetDeviceContainer enbDevs;
  NetDeviceContainer ueDevs1;
  NetDeviceContainer ueDevs2;
  nrHelper->SetSchedulerType (m_schedulerType);

  nrHelper->SetEnbDeviceAttribute ("DlBandwidth", UintegerValue (bandwidth));
  nrHelper->SetEnbDeviceAttribute ("UlBandwidth", UintegerValue (bandwidth));

  nrHelper->SetFfrAlgorithmType ("ns3::NrFfrDistributedAlgorithm");
  nrHelper->SetFfrAlgorithmAttribute ("CalculationInterval", TimeValue (MilliSeconds (10)));
  nrHelper->SetFfrAlgorithmAttribute ("RsrqThreshold", UintegerValue (25));
  nrHelper->SetFfrAlgorithmAttribute ("RsrpDifferenceThreshold", UintegerValue (5));
  nrHelper->SetFfrAlgorithmAttribute ("EdgeRbNum", UintegerValue (6));
  nrHelper->SetFfrAlgorithmAttribute ("CenterPowerOffset",
                                       UintegerValue (NrRrcSap::PdschConfigDedicated::dB0));
  nrHelper->SetFfrAlgorithmAttribute ("EdgePowerOffset",
                                       UintegerValue (NrRrcSap::PdschConfigDedicated::dB3));

  enbDevs = nrHelper->InstallEnbDevice (enbNodes);
  ueDevs1 = nrHelper->InstallUeDevice (ueNodes1);
  ueDevs2 = nrHelper->InstallUeDevice (ueNodes2);

  NetDeviceContainer ueNrDevs;
  ueNrDevs.Add (ueDevs1);
  ueNrDevs.Add (ueDevs2);

  // Add X2 inteface
  nrHelper->AddX2Interface (enbNodes);

  // Install the IP stack on the UEs
  internet.Install (ueNodes);
  Ipv4InterfaceContainer ueIpIfaces;
  ueIpIfaces = ngcHelper->AssignUeIpv4Address (NetDeviceContainer (ueNrDevs));
  // Assign IP address to UEs, and install applications
  for (uint32_t u = 0; u < ueNodes.GetN (); ++u)
    {
      Ptr<Node> ueNode = ueNodes.Get (u);
      // Set the default gateway for the UE
      Ptr<Ipv4StaticRouting> ueStaticRouting = ipv4RoutingHelper.GetStaticRouting (ueNode->GetObject<Ipv4> ());
      ueStaticRouting->SetDefaultRoute (ngcHelper->GetUeDefaultGatewayAddress (), 1);
    }

  // Attach a UE to a eNB
  nrHelper->Attach (ueDevs1, enbDevs.Get (0));
  nrHelper->Attach (ueDevs2, enbDevs.Get (1));


  // Install and start applications on UEs and remote host
  uint16_t dlPort = 10000;
  uint16_t ulPort = 20000;

  // randomize a bit start times to avoid simulation artifacts
  // (e.g., buffer overflows due to packet transmissions happening
  // exactly at the same time)
  Ptr<UniformRandomVariable> startTimeSeconds = CreateObject<UniformRandomVariable> ();
  startTimeSeconds->SetAttribute ("Min", DoubleValue (0));
  startTimeSeconds->SetAttribute ("Max", DoubleValue (0.010));

  for (uint32_t u = 0; u < ueNodes.GetN (); ++u)
    {
      Ptr<Node> ue = ueNodes.Get (u);
      // Set the default gateway for the UE
      Ptr<Ipv4StaticRouting> ueStaticRouting = ipv4RoutingHelper.GetStaticRouting (ue->GetObject<Ipv4> ());
      ueStaticRouting->SetDefaultRoute (ngcHelper->GetUeDefaultGatewayAddress (), 1);

      for (uint32_t b = 0; b < 1; ++b)
        {
          ++dlPort;
          ++ulPort;

          ApplicationContainer clientApps;
          ApplicationContainer serverApps;

          NS_LOG_LOGIC ("installing UDP DL app for UE " << u);
          UdpClientHelper dlClientHelper (ueIpIfaces.GetAddress (u), dlPort);
          dlClientHelper.SetAttribute ("MaxPackets", UintegerValue (1000000));
          dlClientHelper.SetAttribute ("Interval", TimeValue (MilliSeconds (1.0)));
          clientApps.Add (dlClientHelper.Install (remoteHost));
          PacketSinkHelper dlPacketSinkHelper ("ns3::UdpSocketFactory",
                                               InetSocketAddress (Ipv4Address::GetAny (), dlPort));
          serverApps.Add (dlPacketSinkHelper.Install (ue));

          NS_LOG_LOGIC ("installing UDP UL app for UE " << u);
          UdpClientHelper ulClientHelper (remoteHostAddr, ulPort);
          ulClientHelper.SetAttribute ("MaxPackets", UintegerValue (1000000));
          ulClientHelper.SetAttribute ("Interval", TimeValue (MilliSeconds (1.0)));
          clientApps.Add (ulClientHelper.Install (ue));
          PacketSinkHelper ulPacketSinkHelper ("ns3::UdpSocketFactory",
                                               InetSocketAddress (Ipv4Address::GetAny (), ulPort));
          serverApps.Add (ulPacketSinkHelper.Install (remoteHost));

          Ptr<NgcTft> tft = Create<NgcTft> ();
          NgcTft::PacketFilter dlpf;
          dlpf.localPortStart = dlPort;
          dlpf.localPortEnd = dlPort;
          tft->Add (dlpf);
          NgcTft::PacketFilter ulpf;
          ulpf.remotePortStart = ulPort;
          ulpf.remotePortEnd = ulPort;
          tft->Add (ulpf);
          EpsBearer bearer (EpsBearer::NGBR_VIDEO_TCP_DEFAULT);
          nrHelper->ActivateDedicatedEpsBearer (ueNrDevs.Get (u), bearer, tft);

          Time startTime = Seconds (startTimeSeconds->GetValue ());
          serverApps.Start (startTime);
          clientApps.Start (startTime);
        }
    }

  //Test SpectrumPhy to get signals form DL channel
  Ptr<NrSpectrumPhy> enbDlSpectrumPhy = enbDevs.Get (0)->GetObject<NrEnbNetDevice> ()->GetPhy ()->GetDownlinkSpectrumPhy ()->GetObject<NrSpectrumPhy> ();
  Ptr<SpectrumChannel> dlChannel = enbDlSpectrumPhy->GetChannel ();

  Ptr<NrSimpleSpectrumPhy> testDlSpectrumPhy = CreateObject<NrSimpleSpectrumPhy> ();
  Ptr<NrEnbNetDevice> eNbDev = enbDevs.Get (0)->GetObject<NrEnbNetDevice> ();
  testDlSpectrumPhy->SetRxSpectrumModel (NrSpectrumValueHelper::GetSpectrumModel (eNbDev->GetDlEarfcn (), m_dlBandwidth));
  dlChannel->AddRx (testDlSpectrumPhy);

  testDlSpectrumPhy->SetCellId (2);

  testDlSpectrumPhy->TraceConnectWithoutContext ("RxStart",
                                                 MakeBoundCallback (&DlDataRxStartNofiticationArea, this));

  //Test SpectrumPhy to get signals form UL channel
  Ptr<NrSpectrumPhy> ueUlSpectrumPhy = ueDevs1.Get (0)->GetObject<NrUeNetDevice> ()->GetPhy ()->GetUplinkSpectrumPhy ()->GetObject<NrSpectrumPhy> ();
  Ptr<SpectrumChannel> ulChannel = ueUlSpectrumPhy->GetChannel ();

  Ptr<NrSimpleSpectrumPhy> testUlSpectrumPhy = CreateObject<NrSimpleSpectrumPhy> ();
  testUlSpectrumPhy->SetRxSpectrumModel (NrSpectrumValueHelper::GetSpectrumModel (eNbDev->GetUlEarfcn (), m_ulBandwidth));
  ulChannel->AddRx (testUlSpectrumPhy);

  testUlSpectrumPhy->SetCellId (2);

  testUlSpectrumPhy->TraceConnectWithoutContext ("RxStart",
                                                 MakeBoundCallback (&UlDataRxStartNofiticationArea, this));

  double expectedDlPowerCenterArea = 1.0;
  std::vector<bool> expectedDlRbCenterArea;
  expectedDlRbCenterArea.resize (m_dlBandwidth, false);
  std::vector<bool> expectedUlRbCenterArea;
  expectedUlRbCenterArea.resize (m_ulBandwidth, false);
  for (uint32_t i = 0; i < m_dlBandwidth; i++)
    {
      expectedDlRbCenterArea[i] = true;
      expectedUlRbCenterArea[i] = true;
    }

  double expectedDlPowerEdgeArea = 2.0;
  std::vector<bool> expectedDlRbEdgeArea;
  expectedDlRbEdgeArea.resize (m_dlBandwidth, false);
  std::vector<bool> expectedUlRbEdgeArea;
  expectedUlRbEdgeArea.resize (m_ulBandwidth, false);
  for (uint32_t i = 0; i < 6; i++)
    {
      expectedDlRbEdgeArea[i] = true;
      expectedUlRbEdgeArea[i] = true;
    }

  std::vector<bool> expectedDlRbEdgeArea2;
  expectedDlRbEdgeArea2.resize (m_dlBandwidth, false);
  std::vector<bool> expectedUlRbEdgeArea2;
  expectedUlRbEdgeArea2.resize (m_dlBandwidth, false);
  for (uint32_t i = 6; i < 12; i++)
    {
      expectedDlRbEdgeArea2[i] = true;
      expectedUlRbEdgeArea2[i] = true;
    }


  Simulator::Schedule (MilliSeconds (1),
                       &NrFrAreaTestCase::TeleportUe, this, 800, 0,
                       expectedDlPowerCenterArea, expectedDlRbCenterArea );
  Simulator::Schedule (MilliSeconds (1),
                       &NrFrAreaTestCase::SetUlExpectedValues, this, 0.0225815, expectedUlRbCenterArea );

  Simulator::Schedule (MilliSeconds (501),
                       &NrFrAreaTestCase::TeleportUe, this, 400, 0,
                       expectedDlPowerEdgeArea, expectedDlRbEdgeArea );
  Simulator::Schedule (MilliSeconds (501),
                       &NrFrAreaTestCase::SetUlExpectedValues, this, 0.135489, expectedUlRbEdgeArea );

  Simulator::Schedule (MilliSeconds (1001),
                       &NrFrAreaTestCase::TeleportUe2, this, ueNodes1.Get (0), 600, 0,
                       expectedDlPowerEdgeArea, expectedDlRbEdgeArea2 );
  Simulator::Schedule (MilliSeconds (1001),
                       &NrFrAreaTestCase::SetUlExpectedValues, this, 0.135489, expectedUlRbEdgeArea2 );

  Simulator::Schedule (MilliSeconds (1501),
                       &NrFrAreaTestCase::TeleportUe2, this, ueNodes1.Get (0), 200, 0,
                       expectedDlPowerEdgeArea, expectedDlRbEdgeArea );
  Simulator::Schedule (MilliSeconds (1501),
                       &NrFrAreaTestCase::SetUlExpectedValues, this, 0.135489, expectedUlRbEdgeArea );

  Simulator::Schedule (MilliSeconds (2001),
                       &NrFrAreaTestCase::TeleportUe, this, 800, 0,
                       expectedDlPowerCenterArea, expectedDlRbCenterArea );
  Simulator::Schedule (MilliSeconds (2001),
                       &NrFrAreaTestCase::SetUlExpectedValues, this, 0.0225815, expectedUlRbCenterArea );

  Simulator::Stop (Seconds (2.500));
  Simulator::Run ();

#if 0
  NS_TEST_ASSERT_MSG_EQ (m_usedWrongDlRbg, false,
                         "Scheduler used DL RBG muted by FFR Algorithm");
#endif

  NS_TEST_ASSERT_MSG_EQ (m_usedWrongUlRbg, false,
                         "Scheduler used UL RB muted by FFR Algorithm");

  Simulator::Destroy ();

}
