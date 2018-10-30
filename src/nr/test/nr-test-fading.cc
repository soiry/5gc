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

#include "ns3/simulator.h"

#include "ns3/log.h"

#include "ns3/spectrum-test.h"

#include "ns3/nr-phy-tag.h"
#include "ns3/nr-test-ue-phy.h"
#include "ns3/nr-chunk-processor.h"

#include "ns3/nr-test-fading.h"
#include <ns3/buildings-propagation-loss-model.h>
#include <ns3/node-container.h>
#include <ns3/mobility-helper.h>
#include <ns3/nr-helper.h>
#include <ns3/single-model-spectrum-channel.h>
#include "ns3/string.h"
#include "ns3/double.h"
#include <ns3/building.h>
#include <ns3/enum.h>
#include <ns3/net-device-container.h>
#include <ns3/nr-ue-net-device.h>
#include <ns3/nr-enb-net-device.h>
#include <ns3/nr-ue-rrc.h>
#include <ns3/nr-helper.h>
#include <ns3/nr-enb-phy.h>
#include <ns3/nr-ue-phy.h>
#include "nr-test-sinr-chunk-processor.h"

// #include <ns3/nr-trace-fading-loss-model.h>
// #include <ns3/spectrum-value.h>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("NrFadingTest");

/**
* Test 1.1 Fading compound test
*/

/**
* This TestSuite tests the fading model...
*/



NrFadingTestSuite::NrFadingTestSuite ()
: TestSuite ("nr-fading-model", SYSTEM)
{
  
  
  // -------------- COMPOUND TESTS ----------------------------------
  
  LogComponentEnable ("NrFadingTest", LOG_LEVEL_ALL);
  
  // NS_LOG_INFO ("Creating NrDownlinkSinrTestSuite");
  
  Ptr<NrHelper> nrHelper = CreateObject<NrHelper> ();
  
  nrHelper->SetAttribute ("PathlossModel", StringValue ("ns3::BuildingsPropagationLossModel"));
  
  // Create Nodes: eNodeB, home eNB, UE and home UE (UE attached to HeNB)
  NodeContainer enbNodes;
  NodeContainer henbNodes;
  NodeContainer ueNodes;
  NodeContainer hueNodes;
  enbNodes.Create (1);
  henbNodes.Create (2);
  ueNodes.Create (5);
  hueNodes.Create (3);
  
  // Install Mobility Model
  MobilityHelper mobility;
  mobility.SetMobilityModel ("ns3::BuildingsMobilityModel");
  mobility.Install (enbNodes);
  mobility.Install (henbNodes);
  mobility.Install (ueNodes);
  mobility.Install (hueNodes);
  
  NetDeviceContainer enbDevs;
  NetDeviceContainer henbDevs;
  NetDeviceContainer ueDevs;
  NetDeviceContainer hueDevs;
  enbDevs = nrHelper->InstallEnbDevice (enbNodes);
  ueDevs = nrHelper->InstallUeDevice (ueNodes);
  henbDevs = nrHelper->InstallEnbDevice (henbNodes);
  hueDevs = nrHelper->InstallUeDevice (hueNodes);
  
  
  
  nrHelper->Attach (ueDevs, enbDevs.Get (0));
  nrHelper->Attach (hueDevs, henbDevs.Get (0));
  
  // Test #1 Okumura Hata Model (150 < freq < 1500 MHz) (Macro<->UE)
  
  double distance = 2000;
  double hm = 1;
  double hb = 30;
//   double freq = 869e6; // E_UTRA BAND #5 see table 5.5-1 of 36.101
  Ptr<BuildingsMobilityModel> mm1 = enbNodes.Get (0)->GetObject<BuildingsMobilityModel> ();
  mm1->SetPosition (Vector (0.0, 0.0, hb));
  
  Ptr<BuildingsMobilityModel> mm2 = ueNodes.Get (0)->GetObject<BuildingsMobilityModel> ();
  mm2->SetPosition (Vector (distance, 0.0, hm));
  
  AddTestCase (new NrFadingTestCase (mm1, mm2, 137.93, "OH Urban Large city"), TestCase::QUICK);
    
  
}

static NrFadingTestSuite nrFadingTestSuite;


/**
* TestCase
*/

NrFadingTestCase::NrFadingTestCase (Ptr<BuildingsMobilityModel> m1, Ptr<BuildingsMobilityModel> m2, double refValue, std::string name)
: TestCase ("FADING calculation: " + name),
m_node1 (m1),
m_node2 (m2),
m_lossRef (refValue)
{
}

NrFadingTestCase::~NrFadingTestCase ()
{
}

void
NrFadingTestCase::DoRun (void)
{
  //   LogLevel logLevel = (LogLevel)(LOG_PREFIX_FUNC | LOG_PREFIX_TIME | LOG_LEVEL_ALL);
  
  //   LogComponentEnable ("NrEnbRrc", logLevel);
  //   LogComponentEnable ("NrUeRrc", logLevel);
  //   LogComponentEnable ("NrEnbMac", logLevel);
  //   LogComponentEnable ("NrUeMac", logLevel);
  //   LogComponentEnable ("NrRlc", logLevel);
  //   LogComponentEnable ("RrPacketScheduler", logLevel);
  // 
  //   LogComponentEnable ("NrPhy", logLevel);
  //   LogComponentEnable ("NrEnbPhy", logLevel);
  //   LogComponentEnable ("NrUePhy", logLevel);
  // 
  //   LogComponentEnable ("NrSpectrumPhy", logLevel);
  //   LogComponentEnable ("NrInterference", logLevel);
  //   LogComponentEnable ("NrChunkProcessor", logLevel);
  // 
  //   LogComponentEnable ("NrPropagationLossModel", logLevel);
  //   LogComponentEnable ("LossModel", logLevel);
  //   LogComponentEnable ("ShadowingLossModel", logLevel);
  //   LogComponentEnable ("PenetrationLossModel", logLevel);
  //   LogComponentEnable ("MultipathLossModel", logLevel);
  //   LogComponentEnable ("PathLossModel", logLevel);
  // 
  //   LogComponentEnable ("NrNetDevice", logLevel);
  //   LogComponentEnable ("NrUeNetDevice", logLevel);
  //   LogComponentEnable ("NrEnbNetDevice", logLevel);
  
  LogComponentEnable ("NrTraceFadingLossModel", LOG_LEVEL_ALL);
//   LogComponentEnable ("NrTraceFadingLossModel", LOG_LEVEL_ALL);
//   LogComponentEnable ("BuildingsPropagationLossModel", LOG_LEVEL_ALL);
  NS_LOG_INFO ("Testing " << GetName());
  
  
  m_fadingModule = CreateObject<NrTraceFadingLossModel> ();
  
  m_fadingModule->SetAttribute("TraceFilename", StringValue("../../../src/nr/model/fading-traces/fading_trace_EPA_3kmph.fad"));
  //m_fadingModule->SetAttribute("WindowSize", TimeValue(Seconds (0.003)));
  
  m_fadingModule->CreateFadingChannelRealization (m_node1, m_node2);
  
//   Ptr<SpectrumModel> sm;
//   
//   Bands bands;
//   BandInfo bi;
//   
//   bi.fl = 2.400e9;
//   bi.fc = 2.410e9;
//   bi.fh = 2.420e9;
//   bands.push_back (bi);
//   
//   bi.fl = 2.420e9;
//   bi.fc = 2.431e9;
//   bi.fh = 2.442e9;
//   bands.push_back (bi);
//   
//   sm = Create<SpectrumModel> (bands);
//   
//   /**
//   * TX signal #1: Power Spectral Density (W/Hz) of the signal  = [0 0] dBm and BW = [20 22] MHz
//   */
//   Ptr<SpectrumValue> inPsd1 = Create<SpectrumValue> (sm);
//   (*inPsd1)[0] = 1.;
//   (*inPsd1)[1] = 1.;
//   Ptr<SpectrumValue> outPsd1 = Create<SpectrumValue> (sm);
//   outPsd1 = m_fadingModule->CalcRxPowerSpectralDensity (inPsd1, m_node1, m_node2);
//   
//   NS_LOG_INFO ("A ver " << (*outPsd1)[0] << " " << (*outPsd1)[1]);
  double samplingInterval = 0.001;
  double time = 0.0;
  while (time<0.010)
    {
      Time t = Seconds (time);
      Simulator::Schedule(t, &NrFadingTestCase::GetFadingSample, this);
      time += samplingInterval;
    }
  Simulator::Stop (Seconds (10.1));
  Simulator::Run ();
  Simulator::Destroy ();
//   double loss = m_downlinkPropagationLossModel->GetLoss (m_node1, m_node2);
  time = 0.0;
  int rbNum = 2;
  std::vector<double> sum (rbNum);
  std::vector<double> sumSquared (rbNum);
  for (int i = 0; i < rbNum; i++)
    {
      sum.at (i) = 0.;
      sumSquared.at (i) = 0.;
    }
  for (std::vector<SpectrumValue>::size_type i = 0; i != m_fadingSamples.size (); i++)
  {
    NS_LOG_INFO ("Sample time " << time << " : " << m_fadingSamples.at(i)[0] << " " << m_fadingSamples.at(i)[1]);
    time += samplingInterval;
    for (int j = 0; j < rbNum; j++)
    {
      sum.at (j) += m_fadingSamples.at(i)[j];
      sumSquared.at (j) += (m_fadingSamples.at(i)[j]*m_fadingSamples.at(i)[j]);
    }
  }
//   NS_LOG_INFO ("Calculated loss: " << loss);
  NS_LOG_INFO ("Theoretical loss: " << m_lossRef);
  for (int i = 0; i < rbNum; i++)
    {
      double mean = sum.at (i)/m_fadingSamples.size ();
      double sigma = sqrt(sumSquared.at (i)/m_fadingSamples.size () - (mean*mean));
      NS_LOG_INFO (" Mean " << mean << " sigma " << sigma);
    }
  
  //   NS_TEST_ASSERT_MSG_EQ_TOL(loss, m_lossRef, 0.1, "Wrong loss !");
}


void
NrFadingTestCase::GetFadingSample ()
{
  Ptr<SpectrumModel> sm;
  
  Bands bands;
  BandInfo bi;
  
  bi.fl = 2.400e9;
  bi.fc = 2.410e9;
  bi.fh = 2.420e9;
  bands.push_back (bi);
  
  bi.fl = 2.420e9;
  bi.fc = 2.431e9;
  bi.fh = 2.442e9;
  bands.push_back (bi);
  
  sm = Create<SpectrumModel> (bands);
  
  /**
  * TX signal #1: Power Spectral Density (W/Hz) of the signal  = [0 0] dBm and BW = [20 22] MHz
  */
  Ptr<SpectrumValue> inPsd1 = Create<SpectrumValue> (sm);
  (*inPsd1)[0] = 1.;
  (*inPsd1)[1] = 1.;
  Ptr<SpectrumValue> outPsd1 = Create<SpectrumValue> (sm);
  outPsd1 = m_fadingModule->CalcRxPowerSpectralDensity (inPsd1, m_node1, m_node2);
  (*outPsd1)[0] = (10 * std::log10 (180000*(*outPsd1)[0])) - (10 * std::log10 (180000*(*inPsd1)[0]));
  (*outPsd1)[1] = (10 * std::log10 (180000*(*outPsd1)[1])) - (10 * std::log10 (180000*(*inPsd1)[1]));
  m_fadingSamples.push_back ((*outPsd1));
}
