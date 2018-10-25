/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2011, 2012 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
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
 * Author: Manuel Requena <manuel.requena@cttc.es>
 *         Nicola Baldo <nbaldo@cttc.es>
 */

#include "ns3/simulator.h"
#include "ns3/log.h"
#include "ns3/string.h"
#include "ns3/double.h"
#include "ns3/enum.h"
#include "ns3/boolean.h"
#include "ns3/test.h"
#include "ns3/mobility-helper.h"
#include "ns3/nr-helper.h"

#include "ns3/nr-ue-phy.h"
#include "ns3/nr-ue-net-device.h"
#include "ns3/nr-enb-phy.h"
#include "ns3/nr-enb-net-device.h"
#include "ns3/nr-ff-mac-scheduler.h"

#include "ns3/nr-global-pathloss-database.h"

#include <ns3/nr-chunk-processor.h>


using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("NrAntennaTest");


class NrEnbAntennaTestCase : public TestCase
{
public:
  static std::string BuildNameString (double orientationDegrees, double beamwidthDegrees, double x, double y);
  NrEnbAntennaTestCase (double orientationDegrees, double beamwidthDegrees, double x, double y, double antennaGainDb);
  NrEnbAntennaTestCase ();
  virtual ~NrEnbAntennaTestCase ();

private:
  virtual void DoRun (void);

  double m_orientationDegrees;
  double m_beamwidthDegrees;
  double m_x;
  double m_y;
  double m_antennaGainDb;
};




std::string NrEnbAntennaTestCase::BuildNameString (double orientationDegrees, double beamwidthDegrees, double x, double y)
{
  std::ostringstream oss;
  oss <<  "o=" << orientationDegrees
      << ", bw=" << beamwidthDegrees  
      << ", x=" << x 
      << ", y=" << y;
  return oss.str ();
}


NrEnbAntennaTestCase::NrEnbAntennaTestCase (double orientationDegrees, double beamwidthDegrees, double x, double y, double antennaGainDb)
  : TestCase (BuildNameString (orientationDegrees, beamwidthDegrees, x, y)),
    m_orientationDegrees (orientationDegrees),
    m_beamwidthDegrees (beamwidthDegrees),
    m_x (x),
    m_y (y),
    m_antennaGainDb (antennaGainDb)
{
  NS_LOG_FUNCTION (this);
}

NrEnbAntennaTestCase::~NrEnbAntennaTestCase ()
{
}

void
NrEnbAntennaTestCase::DoRun (void)
{
  Config::Reset ();
  Config::SetDefault ("ns3::NrSpectrumPhy::CtrlErrorModelEnabled", BooleanValue (false));
  Config::SetDefault ("ns3::NrSpectrumPhy::DataErrorModelEnabled", BooleanValue (false));
  Config::SetDefault ("ns3::NrHelper::UseIdealRrc", BooleanValue (true));

  //Disable Uplink Power Control
  Config::SetDefault ("ns3::NrUePhy::EnableUplinkPowerControl", BooleanValue (false));

  Ptr<NrHelper> nrHelper = CreateObject<NrHelper> ();

  // use 0dB Pathloss, since we are testing only the antenna gain
  nrHelper->SetAttribute ("PathlossModel", StringValue ("ns3::ConstantSpectrumPropagationLossModel"));
  nrHelper->SetPathlossModelAttribute ("Loss", DoubleValue (0.0));

  // Create Nodes: eNodeB and UE
  NodeContainer enbNodes;
  NodeContainer ueNodes;
  enbNodes.Create (1);
  ueNodes.Create (1);
  NodeContainer allNodes = NodeContainer ( enbNodes, ueNodes );

  // Install Mobility Model
  Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator> ();
  positionAlloc->Add (Vector (0.0, 0.0, 0.0));  // eNB
  positionAlloc->Add (Vector (m_x, m_y, 0.0));  // UE
  MobilityHelper mobility;
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.SetPositionAllocator (positionAlloc);
  mobility.Install (allNodes);

  // Create Devices and install them in the Nodes (eNB and UE)
  NetDeviceContainer enbDevs;
  NetDeviceContainer ueDevs;
  nrHelper->SetSchedulerType ("ns3::RrFfMacScheduler");
  nrHelper->SetSchedulerAttribute ("UlCqiFilter", EnumValue (FfMacScheduler::PUSCH_UL_CQI));
  nrHelper->SetEnbAntennaModelType ("ns3::CosineAntennaModel");
  nrHelper->SetEnbAntennaModelAttribute ("Orientation", DoubleValue (m_orientationDegrees));
  nrHelper->SetEnbAntennaModelAttribute ("Beamwidth",   DoubleValue (m_beamwidthDegrees));
  nrHelper->SetEnbAntennaModelAttribute ("MaxGain",     DoubleValue (0.0));

  enbDevs = nrHelper->InstallEnbDevice (enbNodes);
  ueDevs = nrHelper->InstallUeDevice (ueNodes);

  // Attach a UE to a eNB
  nrHelper->Attach (ueDevs, enbDevs.Get (0));

  // Activate the default EPS bearer
  enum EpsBearer::Qci q = EpsBearer::NGBR_VIDEO_TCP_DEFAULT;
  EpsBearer bearer (q);
  nrHelper->ActivateDataRadioBearer (ueDevs, bearer);

  // Use testing chunk processor in the PHY layer
  // It will be used to test that the SNR is as intended
  Ptr<NrPhy> uePhy = ueDevs.Get (0)->GetObject<NrUeNetDevice> ()->GetPhy ()->GetObject<NrPhy> ();
  Ptr<NrChunkProcessor> testDlSinr = Create<NrChunkProcessor> ();
  NrSpectrumValueCatcher dlSinrCatcher;
  testDlSinr->AddCallback (MakeCallback (&NrSpectrumValueCatcher::ReportValue, &dlSinrCatcher));
  uePhy->GetDownlinkSpectrumPhy ()->AddDataSinrChunkProcessor (testDlSinr);

  Ptr<NrPhy> enbphy = enbDevs.Get (0)->GetObject<NrEnbNetDevice> ()->GetPhy ()->GetObject<NrPhy> ();
  Ptr<NrChunkProcessor> testUlSinr = Create<NrChunkProcessor> ();
  NrSpectrumValueCatcher ulSinrCatcher;
  testUlSinr->AddCallback (MakeCallback (&NrSpectrumValueCatcher::ReportValue, &ulSinrCatcher));
  enbphy->GetUplinkSpectrumPhy ()->AddDataSinrChunkProcessor (testUlSinr);


  // keep track of all path loss values in two centralized objects
  DownlinkNrGlobalPathlossDatabase dlPathlossDb;
  UplinkNrGlobalPathlossDatabase ulPathlossDb;
  // we rely on the fact that NrHelper creates the DL channel object first, then the UL channel object,
  // hence the former will have index 0 and the latter 1
  Config::Connect ("/ChannelList/0/PathLoss",
                   MakeCallback (&DownlinkNrGlobalPathlossDatabase::UpdatePathloss, &dlPathlossDb));
  Config::Connect ("/ChannelList/1/PathLoss",
                    MakeCallback (&UplinkNrGlobalPathlossDatabase::UpdatePathloss, &ulPathlossDb)); 

  Simulator::Stop (Seconds (0.035));
  Simulator::Run ();

  const double enbTxPowerDbm = 30; // default eNB TX power over whole bandwidth
  const double ueTxPowerDbm  = 10; // default UE TX power over whole bandwidth
  const double ktDbm = -174;    // reference NR noise PSD
  const double noisePowerDbm = ktDbm + 10 * std::log10 (25 * 180000); // corresponds to kT*bandwidth in linear units
  const double ueNoiseFigureDb = 9.0; // default UE noise figure
  const double enbNoiseFigureDb = 5.0; // default eNB noise figure
  double tolerance = (m_antennaGainDb != 0) ? std::abs (m_antennaGainDb) * 0.001 : 0.001;

  // first test with SINR from NrChunkProcessor
  // this can only be done for not-too-bad SINR otherwise the measurement won't be available
  double expectedSinrDl = enbTxPowerDbm + m_antennaGainDb - noisePowerDbm + ueNoiseFigureDb;
  if (expectedSinrDl > 0)
    {
      double calculatedSinrDbDl = -INFINITY;
      if (dlSinrCatcher.GetValue () != 0)
        {
          calculatedSinrDbDl = 10.0 * std::log10 (dlSinrCatcher.GetValue ()->operator[] (0));
        }      
      // remember that propagation loss is 0dB
      double calculatedAntennaGainDbDl = - (enbTxPowerDbm - calculatedSinrDbDl - noisePowerDbm - ueNoiseFigureDb);      
      NS_TEST_ASSERT_MSG_EQ_TOL (calculatedAntennaGainDbDl, m_antennaGainDb, tolerance, "Wrong DL antenna gain!");
    }
  double expectedSinrUl = ueTxPowerDbm + m_antennaGainDb - noisePowerDbm + enbNoiseFigureDb;
  if (expectedSinrUl > 0)
    {      
      double calculatedSinrDbUl = -INFINITY;
      if (ulSinrCatcher.GetValue () != 0)
        {
          calculatedSinrDbUl = 10.0 * std::log10 (ulSinrCatcher.GetValue ()->operator[] (0));
        }  
      double calculatedAntennaGainDbUl = - (ueTxPowerDbm - calculatedSinrDbUl - noisePowerDbm - enbNoiseFigureDb);
      NS_TEST_ASSERT_MSG_EQ_TOL (calculatedAntennaGainDbUl, m_antennaGainDb, tolerance, "Wrong UL antenna gain!");
    }


  // repeat the same tests with the NrGlobalPathlossDatabases
  double measuredLossDl = dlPathlossDb.GetPathloss (1, 1);
  NS_TEST_ASSERT_MSG_EQ_TOL (measuredLossDl, -m_antennaGainDb, tolerance, "Wrong DL loss!");
  double measuredLossUl = ulPathlossDb.GetPathloss (1, 1);
  NS_TEST_ASSERT_MSG_EQ_TOL (measuredLossUl, -m_antennaGainDb, tolerance, "Wrong UL loss!");

  
  Simulator::Destroy ();
}


class NrAntennaTestSuite : public TestSuite
{
public:
  NrAntennaTestSuite ();
};


NrAntennaTestSuite::NrAntennaTestSuite ()
  : TestSuite ("nr-antenna", SYSTEM)
{
  NS_LOG_FUNCTION (this);

  //                                      orientation beamwidth     x            y         gain 
  AddTestCase (new NrEnbAntennaTestCase (       0.0,     90.0,    1.0,          0.0,       0.0), TestCase::QUICK);
  AddTestCase (new NrEnbAntennaTestCase (       0.0,     90.0,    1.0,          1.0,      -3.0), TestCase::QUICK);
  AddTestCase (new NrEnbAntennaTestCase (       0.0,     90.0,    1.0,         -1.0,      -3.0), TestCase::QUICK);
  AddTestCase (new NrEnbAntennaTestCase (       0.0,     90.0,   -1.0,         -1.0,   -36.396), TestCase::QUICK);
  AddTestCase (new NrEnbAntennaTestCase (       0.0,     90.0,   -1.0,         -0.0,   -1414.6), TestCase::QUICK);
  AddTestCase (new NrEnbAntennaTestCase (       0.0,     90.0,   -1.0,          1.0,   -36.396), TestCase::QUICK);
  AddTestCase (new NrEnbAntennaTestCase (      45.0,     90.0,    1.0,          1.0,       0.0), TestCase::QUICK);
  AddTestCase (new NrEnbAntennaTestCase (     -45.0,     90.0,    1.0,         -1.0,       0.0), TestCase::QUICK);
  AddTestCase (new NrEnbAntennaTestCase (      90.0,     90.0,    1.0,          1.0,      -3.0), TestCase::QUICK);
  AddTestCase (new NrEnbAntennaTestCase (     -90.0,     90.0,    1.0,         -1.0,      -3.0), TestCase::QUICK); 

  AddTestCase (new NrEnbAntennaTestCase (       0.0,    120.0,    1.0,          0.0,       0.0), TestCase::QUICK);
  AddTestCase (new NrEnbAntennaTestCase (       0.0,    120.0,    0.5,  sin(M_PI/3),      -3.0), TestCase::QUICK);
  AddTestCase (new NrEnbAntennaTestCase (       0.0,    120.0,    0.5, -sin(M_PI/3),      -3.0), TestCase::QUICK);
  AddTestCase (new NrEnbAntennaTestCase (       0.0,    120.0,   -1.0,         -2.0,   -13.410), TestCase::QUICK);
  AddTestCase (new NrEnbAntennaTestCase (       0.0,    120.0,   -1.0,          1.0,   -20.034), TestCase::QUICK);
  AddTestCase (new NrEnbAntennaTestCase (      60.0,    120.0,    0.5,  sin(M_PI/3),       0.0), TestCase::QUICK);
  AddTestCase (new NrEnbAntennaTestCase (     -60.0,    120.0,    0.5, -sin(M_PI/3),       0.0), TestCase::QUICK);
  AddTestCase (new NrEnbAntennaTestCase (     -60.0,    120.0,    0.5, -sin(M_PI/3),       0.0), TestCase::QUICK);
  AddTestCase (new NrEnbAntennaTestCase (    -120.0,    120.0,   -0.5, -sin(M_PI/3),       0.0), TestCase::QUICK);
  AddTestCase (new NrEnbAntennaTestCase (    -120.0,    120.0,    0.5, -sin(M_PI/3),      -3.0), TestCase::QUICK);
  AddTestCase (new NrEnbAntennaTestCase (    -120.0,    120.0,     -1,            0,      -3.0), TestCase::QUICK);
  AddTestCase (new NrEnbAntennaTestCase (    -120.0,    120.0,     -1,            2,   -15.578), TestCase::QUICK);
  AddTestCase (new NrEnbAntennaTestCase (    -120.0,    120.0,      1,            0,   -14.457), TestCase::QUICK);
  AddTestCase (new NrEnbAntennaTestCase (    -120.0,    120.0,      1,            2,   -73.154), TestCase::QUICK);
  AddTestCase (new NrEnbAntennaTestCase (    -120.0,    120.0,      1,         -0.1,   -12.754), TestCase::QUICK);


}

static NrAntennaTestSuite nrAntennaTestSuite;
