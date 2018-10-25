/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2011 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
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
 * Author: Marco Miozzo <marco.miozzo@cttc.es>
 */

#include <ns3/object.h>
#include <ns3/spectrum-interference.h>
#include <ns3/spectrum-error-model.h>
#include <ns3/log.h>
#include <ns3/test.h>
#include <ns3/simulator.h>
#include <ns3/packet.h>
#include <ns3/ptr.h>
#include <iostream>
#include <cmath>
#include <ns3/radio-bearer-stats-calculator.h>
#include <ns3/mobility-building-info.h>
#include <ns3/hybrid-buildings-propagation-loss-model.h>
#include <ns3/eps-bearer.h>
#include <ns3/node-container.h>
#include <ns3/mobility-helper.h>
#include <ns3/net-device-container.h>
#include <ns3/nr-ue-net-device.h>
#include <ns3/nr-enb-net-device.h>
#include <ns3/nr-ue-rrc.h>
#include <ns3/nr-helper.h>
#include <ns3/string.h>
#include <ns3/double.h>
#include <ns3/nr-enb-phy.h>
#include <ns3/nr-ue-phy.h>
#include <ns3/config.h>
#include <ns3/boolean.h>
#include <ns3/enum.h>
#include <ns3/unused.h>
#include <ns3/nr-ff-mac-scheduler.h>
#include <ns3/buildings-helper.h>

#include "nr-test-harq.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("LenaTestHarq");

LenaTestHarqSuite::LenaTestHarqSuite ()
  : TestSuite ("nr-harq", SYSTEM)
{
  NS_LOG_INFO ("creating LenaTestHarqTestCase");


  // Tests on DL/UL Data channels (PDSCH, PUSCH)
  // MCS 0 TB size of 66 bytes SINR -9.91 dB expected throughput 31822 bytes/s
  // TBLER 1st tx 1.0
  // TBLER 2nd tx 0.074
  AddTestCase (new LenaHarqTestCase (2, 2400, 66, 0.12, 31822), TestCase::QUICK);

  // Tests on DL/UL Data channels (PDSCH, PUSCH)
  // MCS 10 TB size of 472 bytes SINR 0.3 dB expected throughput 209964 bytes/s
  // TBLER 1st tx 1.0
  // TBLER 2nd tx 0.248
  AddTestCase (new LenaHarqTestCase (1, 770, 472, 0.06, 209964), TestCase::QUICK);



}

static LenaTestHarqSuite lenaTestHarqSuite;

std::string
LenaHarqTestCase::BuildNameString (uint16_t nUser, uint16_t dist, uint16_t tbSize)
{
  std::ostringstream oss;
  oss << nUser << " UEs, distance " << dist << " m, TB size " << tbSize;
  return oss.str ();
}

LenaHarqTestCase::LenaHarqTestCase (uint16_t nUser, uint16_t dist, uint16_t tbSize, double amcBer, double thrRef)
  : TestCase (BuildNameString (nUser, dist, tbSize)),
    m_nUser (nUser),
    m_dist (dist),
    m_amcBer (amcBer),
    m_throughputRef (thrRef)
{
}

LenaHarqTestCase::~LenaHarqTestCase ()
{
}

void
LenaHarqTestCase::DoRun (void)
{

  Config::SetDefault ("ns3::NrAmc::Ber", DoubleValue (m_amcBer));
  Config::SetDefault ("ns3::NrAmc::AmcModel", EnumValue (NrAmc::PiroEW2010));
  Config::SetDefault ("ns3::NrSpectrumPhy::CtrlErrorModelEnabled", BooleanValue (false));
  Config::SetDefault ("ns3::NrSpectrumPhy::DataErrorModelEnabled", BooleanValue (true));
  Config::SetDefault ("ns3::NrHelper::UseIdealRrc", BooleanValue (true));
  //Disable Uplink Power Control
  Config::SetDefault ("ns3::NrUePhy::EnableUplinkPowerControl", BooleanValue (false));

//   Config::SetDefault ("ns3::RrFfMacScheduler::HarqEnabled", BooleanValue (false));
//   LogComponentEnable ("NrEnbRrc", LOG_LEVEL_ALL);
//   LogComponentEnable ("NrUeRrc", LOG_LEVEL_ALL);
//   LogComponentEnable ("NrEnbMac", LOG_LEVEL_ALL);
//   LogComponentEnable ("NrUeMac", LOG_LEVEL_ALL);
//   LogComponentEnable ("NrRlc", LOG_LEVEL_ALL);
//
//   LogComponentEnable ("NrPhy", LOG_LEVEL_ALL);
//   LogComponentEnable ("NrEnbPhy", LOG_LEVEL_ALL);
//   LogComponentEnable ("NrUePhy", LOG_LEVEL_ALL);

//   LogComponentEnable ("NrSpectrumPhy", LOG_LEVEL_ALL);
//   LogComponentEnable ("NrInterference", LOG_LEVEL_ALL);
//   LogComponentEnable ("NrChunkProcessor", LOG_LEVEL_ALL);
//
//   LogComponentEnable ("NrPropagationLossModel", LOG_LEVEL_ALL);
//   LogComponentEnable ("LossModel", LOG_LEVEL_ALL);
//   LogComponentEnable ("ShadowingLossModel", LOG_LEVEL_ALL);
//   LogComponentEnable ("PenetrationLossModel", LOG_LEVEL_ALL);
//   LogComponentEnable ("MultipathLossModel", LOG_LEVEL_ALL);
//   LogComponentEnable ("PathLossModel", LOG_LEVEL_ALL);
//
//   LogComponentEnable ("NrNetDevice", LOG_LEVEL_ALL);
//   LogComponentEnable ("NrUeNetDevice", LOG_LEVEL_ALL);
//   LogComponentEnable ("NrEnbNetDevice", LOG_LEVEL_ALL);

//   LogComponentEnable ("RrFfMacScheduler", LOG_LEVEL_ALL);
//   LogComponentEnable ("LenaHelper", LOG_LEVEL_ALL);
//   LogComponentEnable ("RlcStatsCalculator", LOG_LEVEL_ALL);


//   LogComponentEnable ("NrSpectrumPhy", LOG_LEVEL_ALL);
//   LogComponentEnable ("NrEnbMac", LOG_LEVEL_ALL);
//   LogComponentEnable ("NrEnbPhy", LOG_LEVEL_ALL);
//   LogComponentEnable ("NrUePhy", LOG_LEVEL_ALL);
//   LogComponentEnable ("RrFfMacScheduler", LOG_LEVEL_ALL);
//   LogComponentEnable ("LenaHelper", LOG_LEVEL_ALL);
//   LogComponentEnable ("BuildingsPropagationLossModel", LOG_LEVEL_ALL);
//   LogComponentEnable ("NrMiErrorModel", LOG_LEVEL_ALL);
//   LogComponentEnable ("NrAmc", LOG_LEVEL_ALL);
//
//   LogComponentDisableAll (LOG_LEVEL_ALL);

//  LogComponentEnable ("LenaTestHarq", LOG_LEVEL_ALL);


  /**
   * Initialize Simulation Scenario: 1 eNB and m_nUser UEs
   */

  Ptr<NrHelper> lena = CreateObject<NrHelper> ();

  // Create Nodes: eNodeB and UE
  NodeContainer enbNodes;
  NodeContainer ueNodes;
  enbNodes.Create (1);
  ueNodes.Create (m_nUser);

  // Install Mobility Model
  MobilityHelper mobility;
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.Install (enbNodes);
  BuildingsHelper::Install (enbNodes);
  
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.Install (ueNodes);
  BuildingsHelper::Install (ueNodes);

  // remove random shadowing component
  lena->SetAttribute ("PathlossModel", StringValue ("ns3::HybridBuildingsPropagationLossModel"));
  lena->SetPathlossModelAttribute ("ShadowSigmaOutdoor", DoubleValue (0.0));
  lena->SetPathlossModelAttribute ("ShadowSigmaIndoor", DoubleValue (0.0));
  lena->SetPathlossModelAttribute ("ShadowSigmaExtWalls", DoubleValue (0.0));

  // Create Devices and install them in the Nodes (eNB and UE)
  NetDeviceContainer enbDevs;
  NetDeviceContainer ueDevs;
  lena->SetSchedulerType ("ns3::RrFfMacScheduler");
  lena->SetSchedulerAttribute ("UlCqiFilter", EnumValue (FfMacScheduler::PUSCH_UL_CQI));

  enbDevs = lena->InstallEnbDevice (enbNodes);
  ueDevs = lena->InstallUeDevice (ueNodes);

  // Attach a UE to a eNB
  lena->Attach (ueDevs, enbDevs.Get (0));

  // Activate an EPS bearer
  enum EpsBearer::Qci q = EpsBearer::GBR_CONV_VOICE;
  EpsBearer bearer (q);
  lena->ActivateDataRadioBearer (ueDevs, bearer);


  Ptr<NrEnbNetDevice> nrEnbDev = enbDevs.Get (0)->GetObject<NrEnbNetDevice> ();
  Ptr<NrEnbPhy> enbPhy = nrEnbDev->GetPhy ();
  enbPhy->SetAttribute ("TxPower", DoubleValue (43.0));
  enbPhy->SetAttribute ("NoiseFigure", DoubleValue (5.0));
  // place the HeNB over the default rooftop level (20 mt.)
  Ptr<MobilityModel> mm = enbNodes.Get (0)->GetObject<MobilityModel> ();
  mm->SetPosition (Vector (0.0, 0.0, 30.0));

  // Set UEs' position and power
  for (int i = 0; i < m_nUser; i++)
    {
      Ptr<MobilityModel> mm = ueNodes.Get (i)->GetObject<MobilityModel> ();
      mm->SetPosition (Vector (m_dist, 0.0, 1.0));
      Ptr<NrUeNetDevice> nrUeDev = ueDevs.Get (i)->GetObject<NrUeNetDevice> ();
      Ptr<NrUePhy> uePhy = nrUeDev->GetPhy ();
      uePhy->SetAttribute ("TxPower", DoubleValue (23.0));
      uePhy->SetAttribute ("NoiseFigure", DoubleValue (9.0));
    }


  double statsStartTime = 0.050; // need to allow for RRC connection establishment + SRS 
  double statsDuration = 2.0;
  Simulator::Stop (Seconds (statsStartTime + statsDuration - 0.0001));

  lena->EnableRlcTraces ();
  Ptr<RadioBearerStatsCalculator> rlcStats = lena->GetRlcStats ();
  rlcStats->SetAttribute ("StartTime", TimeValue (Seconds (statsStartTime)));
  rlcStats->SetAttribute ("EpochDuration", TimeValue (Seconds (statsDuration)));

  // for debugging purposes
  lena->EnableMacTraces ();

  Simulator::Run ();

  /**
   * Check that the assignation is done in a RR fashion
   */
  NS_LOG_INFO ("\tTest on downlink data shared channels (PDSCH)");
  NS_LOG_INFO ("Test with " << m_nUser << " user(s) at distance " << m_dist << " expected Thr " << m_throughputRef);
  for (int i = 0; i < m_nUser; i++)
    {
      // get the imsi
      uint64_t imsi = ueDevs.Get (i)->GetObject<NrUeNetDevice> ()->GetImsi ();
      uint8_t lcId = 3;
      double txed = rlcStats->GetDlTxData (imsi, lcId);
      double rxed = rlcStats->GetDlRxData (imsi, lcId);
      double tolerance = 0.1;

      NS_LOG_INFO (" User " << i << " imsi " << imsi << " bytes rxed/t " << rxed/statsDuration << " txed/t " << txed/statsDuration << " thr Ref " << m_throughputRef << " Err " << (std::abs (txed/statsDuration - m_throughputRef)) / m_throughputRef);

      NS_TEST_ASSERT_MSG_EQ_TOL (txed/statsDuration, m_throughputRef, m_throughputRef * tolerance, " Unexpected Throughput!");
      NS_TEST_ASSERT_MSG_EQ_TOL (rxed/statsDuration, m_throughputRef, m_throughputRef * tolerance, " Unexpected Throughput!");
    }


  Simulator::Destroy ();
}
