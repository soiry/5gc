/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013 Budiarto Herman
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
 * Author: Budiarto Herman <budiarto.herman@magister.fi>
 *
 */

#include "nr-test-cell-selection.h"

#include <ns3/simulator.h>
#include <ns3/log.h>
#include <ns3/boolean.h>
#include <ns3/double.h>
#include <ns3/integer.h>

#include <ns3/mobility-helper.h>
#include <ns3/nr-helper.h>
#include <ns3/point-to-point-ngc-helper.h>
#include <ns3/internet-stack-helper.h>
#include <ns3/point-to-point-helper.h>
#include <ns3/ipv4-address-helper.h>
#include <ns3/ipv4-static-routing-helper.h>

#include <ns3/node-container.h>
#include <ns3/net-device-container.h>
#include <ns3/ipv4-interface-container.h>

#include <ns3/nr-ue-net-device.h>
#include <ns3/nr-ue-rrc.h>
#include <ns3/nr-enb-net-device.h>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("NrCellSelectionTest");

/*
 * Test Suite
 */


NrCellSelectionTestSuite::NrCellSelectionTestSuite ()
  : TestSuite ("nr-cell-selection", SYSTEM)
{
  std::vector<NrCellSelectionTestCase::UeSetup_t> w;

  // REAL RRC PROTOCOL

  w.clear ();
  //                                                x     y    csgMember
  //                                                checkPoint     cell1, cell2
  w.push_back (NrCellSelectionTestCase::UeSetup_t (0.0, 0.55, false,
                                                    MilliSeconds (283), 1, 0));
  w.push_back (NrCellSelectionTestCase::UeSetup_t (0.0, 0.45, false,
                                                    MilliSeconds (283), 1, 0));
  w.push_back (NrCellSelectionTestCase::UeSetup_t (0.5, 0.45, false,
                                                    MilliSeconds (363), 1, 3));
  w.push_back (NrCellSelectionTestCase::UeSetup_t (0.5, 0.0,  true,
                                                    MilliSeconds (283), 2, 4));
  w.push_back (NrCellSelectionTestCase::UeSetup_t (1.0, 0.55, true,
                                                    MilliSeconds (283), 3, 0));
  w.push_back (NrCellSelectionTestCase::UeSetup_t (1.0, 0.45, true,
                                                    MilliSeconds (283), 4, 0));

  AddTestCase (new NrCellSelectionTestCase ("NGC, real RRC, RngNum=1",
                                             true, false, 60.0, w, 1),
               //                                        isd       rngrun
               TestCase::QUICK);

  // IDEAL RRC PROTOCOL

  w.clear ();
  //                                                x     y    csgMember
  //                                                checkPoint     cell1, cell2
  w.push_back (NrCellSelectionTestCase::UeSetup_t (0.0, 0.55, false,
                                                    MilliSeconds (266), 1, 0));
  w.push_back (NrCellSelectionTestCase::UeSetup_t (0.0, 0.45, false,
                                                    MilliSeconds (266), 1, 0));
  w.push_back (NrCellSelectionTestCase::UeSetup_t (0.5, 0.45, false,
                                                    MilliSeconds (346), 1, 3));
  w.push_back (NrCellSelectionTestCase::UeSetup_t (0.5, 0.0,  true,
                                                    MilliSeconds (266), 2, 4));
  w.push_back (NrCellSelectionTestCase::UeSetup_t (1.0, 0.55, true,
                                                    MilliSeconds (266), 3, 0));
  w.push_back (NrCellSelectionTestCase::UeSetup_t (1.0, 0.45, true,
                                                    MilliSeconds (266), 4, 0));

  AddTestCase (new NrCellSelectionTestCase ("NGC, ideal RRC, RngNum=1",
                                             true, true, 60.0, w, 1),
               //                                        isd      rngrun
               TestCase::QUICK);

} // end of NrCellSelectionTestSuite::NrCellSelectionTestSuite ()


static NrCellSelectionTestSuite g_nrCellSelectionTestSuite;



/*
 * Test Case
 */


NrCellSelectionTestCase::UeSetup_t::UeSetup_t (
  double relPosX, double relPosY, bool isCsgMember, Time checkPoint,
  uint16_t expectedCellId1, uint16_t expectedCellId2)
  : position (Vector (relPosX, relPosY, 0.0)),
    isCsgMember (isCsgMember),
    checkPoint (checkPoint),
    expectedCellId1 (expectedCellId1),
    expectedCellId2 (expectedCellId2)
{
}


NrCellSelectionTestCase::NrCellSelectionTestCase (
  std::string name, bool isNgcMode, bool isIdealRrc,
  double interSiteDistance,
  std::vector<UeSetup_t> ueSetupList, int64_t rngRun)
  : TestCase (name),
    m_isNgcMode (isNgcMode),
    m_isIdealRrc (isIdealRrc),
    m_interSiteDistance (interSiteDistance),
    m_ueSetupList (ueSetupList),
    m_rngRun (rngRun)
{
  NS_LOG_FUNCTION (this << GetName ());
  m_lastState.resize (m_ueSetupList.size (), NrUeRrc::NUM_STATES);
}


NrCellSelectionTestCase::~NrCellSelectionTestCase ()
{
  NS_LOG_FUNCTION (this << GetName ());
}


void
NrCellSelectionTestCase::DoRun ()
{
  NS_LOG_FUNCTION (this << GetName ());

  Config::SetGlobal ("RngRun", IntegerValue (m_rngRun));

  Ptr<NrHelper> nrHelper = CreateObject<NrHelper> ();
  nrHelper->SetAttribute ("PathlossModel",
                           StringValue ("ns3::FriisSpectrumPropagationLossModel"));
  nrHelper->SetAttribute ("UseIdealRrc", BooleanValue (m_isIdealRrc));

  Ptr<PointToPointNgcHelper> ngcHelper;

  if (m_isNgcMode)
    {
      ngcHelper = CreateObject<PointToPointNgcHelper> ();
      nrHelper->SetNgcHelper (ngcHelper);
    }

  /*
   * The topology is the following (the number on the node indicate the cell ID)
   *
   *      [1]        [3]
   *    non-CSG -- non-CSG
   *       |          |
   *       |          | 60 m
   *       |          |
   *      [2]        [4]
   *      CSG ------ CSG
   *           60 m
   */

  // Create Nodes
  NodeContainer enbNodes;
  enbNodes.Create (4);
  NodeContainer ueNodes;
  uint16_t nUe = m_ueSetupList.size ();
  ueNodes.Create (nUe);

  // Assign nodes to position
  Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator> ();
  // eNodeB
  positionAlloc->Add (Vector (                0.0, m_interSiteDistance, 0.0));
  positionAlloc->Add (Vector (                0.0,                 0.0, 0.0));
  positionAlloc->Add (Vector (m_interSiteDistance, m_interSiteDistance, 0.0));
  positionAlloc->Add (Vector (m_interSiteDistance,                 0.0, 0.0));
  // UE
  std::vector<UeSetup_t>::const_iterator itSetup;
  for (itSetup = m_ueSetupList.begin ();
       itSetup != m_ueSetupList.end (); itSetup++)
    {
      Vector uePos (m_interSiteDistance * itSetup->position.x,
                    m_interSiteDistance * itSetup->position.y,
                    m_interSiteDistance * itSetup->position.z);
      NS_LOG_INFO ("UE position " << uePos);
      positionAlloc->Add (uePos);
    }

  MobilityHelper mobility;
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.SetPositionAllocator (positionAlloc);
  mobility.Install (enbNodes);
  mobility.Install (ueNodes);

  // Create Devices and install them in the Nodes (eNB and UE)
  int64_t stream = 1;
  NetDeviceContainer enbDevs;

  // cell ID 1 is a non-CSG cell
  nrHelper->SetEnbDeviceAttribute ("CsgId", UintegerValue (0));
  nrHelper->SetEnbDeviceAttribute ("CsgIndication", BooleanValue (false));
  enbDevs.Add (nrHelper->InstallEnbDevice (enbNodes.Get (0)));

  // cell ID 2 is a CSG cell
  nrHelper->SetEnbDeviceAttribute ("CsgId", UintegerValue (1));
  nrHelper->SetEnbDeviceAttribute ("CsgIndication", BooleanValue (true));
  enbDevs.Add (nrHelper->InstallEnbDevice (enbNodes.Get (1)));

  // cell ID 3 is a non-CSG cell
  nrHelper->SetEnbDeviceAttribute ("CsgId", UintegerValue (0));
  nrHelper->SetEnbDeviceAttribute ("CsgIndication", BooleanValue (false));
  enbDevs.Add (nrHelper->InstallEnbDevice (enbNodes.Get (2)));

  // cell ID 4 is a CSG cell
  nrHelper->SetEnbDeviceAttribute ("CsgId", UintegerValue (1));
  nrHelper->SetEnbDeviceAttribute ("CsgIndication", BooleanValue (true));
  enbDevs.Add (nrHelper->InstallEnbDevice (enbNodes.Get (3)));

  NetDeviceContainer ueDevs;
  Time lastCheckPoint = MilliSeconds (0);
  NS_ASSERT (m_ueSetupList.size () == ueNodes.GetN ());
  NodeContainer::Iterator itNode;
  for (itSetup = m_ueSetupList.begin (), itNode = ueNodes.Begin ();
       itSetup != m_ueSetupList.end () || itNode != ueNodes.End ();
       itSetup++, itNode++)
    {
      if (itSetup->isCsgMember)
        {
          nrHelper->SetUeDeviceAttribute ("CsgId", UintegerValue (1));
        }
      else
        {
          nrHelper->SetUeDeviceAttribute ("CsgId", UintegerValue (0));
        }

      NetDeviceContainer devs = nrHelper->InstallUeDevice (*itNode);
      Ptr<NrUeNetDevice> ueDev = devs.Get (0)->GetObject<NrUeNetDevice> ();
      NS_ASSERT (ueDev != 0);
      ueDevs.Add (devs);
      Simulator::Schedule (itSetup->checkPoint,
                           &NrCellSelectionTestCase::CheckPoint,
                           this, ueDev,
                           itSetup->expectedCellId1, itSetup->expectedCellId2);

      if (lastCheckPoint < itSetup->checkPoint)
        {
          lastCheckPoint = itSetup->checkPoint;
        }
    }

  stream += nrHelper->AssignStreams (enbDevs, stream);
  stream += nrHelper->AssignStreams (ueDevs, stream);

  // Tests
  NS_ASSERT (m_ueSetupList.size () == ueDevs.GetN ());
  NetDeviceContainer::Iterator itDev;
  for (itSetup = m_ueSetupList.begin (), itDev = ueDevs.Begin ();
       itSetup != m_ueSetupList.end () || itDev != ueDevs.End ();
       itSetup++, itDev++)
    {
      Ptr<NrUeNetDevice> ueDev = (*itDev)->GetObject<NrUeNetDevice> ();
    }

  if (m_isNgcMode)
    {
      // Create P-GW node
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

      // Routing of the Internet Host (towards the NR network)
      Ipv4StaticRoutingHelper ipv4RoutingHelper;
      Ptr<Ipv4StaticRouting> remoteHostStaticRouting = ipv4RoutingHelper.GetStaticRouting (remoteHost->GetObject<Ipv4> ());
      remoteHostStaticRouting->AddNetworkRouteTo (Ipv4Address ("7.0.0.0"), Ipv4Mask ("255.0.0.0"), 1);

      // Install the IP stack on the UEs
      internet.Install (ueNodes);
      Ipv4InterfaceContainer ueIpIfaces;
      ueIpIfaces = ngcHelper->AssignUeIpv4Address (NetDeviceContainer (ueDevs));

      // Assign IP address to UEs
      for (uint32_t u = 0; u < ueNodes.GetN (); ++u)
        {
          Ptr<Node> ueNode = ueNodes.Get (u);
          // Set the default gateway for the UE
          Ptr<Ipv4StaticRouting> ueStaticRouting = ipv4RoutingHelper.GetStaticRouting (ueNode->GetObject<Ipv4> ());
          ueStaticRouting->SetDefaultRoute (ngcHelper->GetUeDefaultGatewayAddress (), 1);
        }

    } // end of if (m_isNgcMode)
  else
    {
      NS_FATAL_ERROR ("No support yet for NR-only simulations");
    }

  // Connect to trace sources in UEs
  Config::Connect ("/NodeList/*/DeviceList/*/NrUeRrc/StateTransition",
                   MakeCallback (&NrCellSelectionTestCase::StateTransitionCallback,
                                 this));
  Config::Connect ("/NodeList/*/DeviceList/*/NrUeRrc/InitialCellSelectionEndOk",
                   MakeCallback (&NrCellSelectionTestCase::InitialCellSelectionEndOkCallback,
                                 this));
  Config::Connect ("/NodeList/*/DeviceList/*/NrUeRrc/InitialCellSelectionEndError",
                   MakeCallback (&NrCellSelectionTestCase::InitialCellSelectionEndErrorCallback,
                                 this));
  Config::Connect ("/NodeList/*/DeviceList/*/NrUeRrc/ConnectionEstablished",
                   MakeCallback (&NrCellSelectionTestCase::ConnectionEstablishedCallback,
                                 this));

  // Enable Idle mode cell selection
  nrHelper->Attach (ueDevs);

  // Run simulation
  Simulator::Stop (lastCheckPoint);
  Simulator::Run ();

  NS_LOG_INFO ("Simulation ends");
  Simulator::Destroy ();

} // end of void NrCellSelectionTestCase::DoRun ()


void
NrCellSelectionTestCase::CheckPoint (Ptr<NrUeNetDevice> ueDev,
                                      uint16_t expectedCellId1,
                                      uint16_t expectedCellId2)
{
  uint16_t actualCellId = ueDev->GetRrc ()->GetCellId ();

  if (expectedCellId2 == 0)
    {
      NS_TEST_ASSERT_MSG_EQ (actualCellId, expectedCellId1,
                             "IMSI " << ueDev->GetImsi ()
                                     << " has attached to an unexpected cell");
    }
  else
    {
      bool pass = (actualCellId == expectedCellId1) ||
        (actualCellId == expectedCellId2);
      NS_TEST_ASSERT_MSG_EQ (pass, true,
                             "IMSI " << ueDev->GetImsi ()
                                     << " has attached to an unexpected cell"
                                     << " (actual: " << actualCellId << ","
                                     << " expected: " << expectedCellId1
                                     << " or " << expectedCellId2 << ")");
    }

  if (expectedCellId1 > 0)
    {
      NS_TEST_ASSERT_MSG_EQ (m_lastState.at (ueDev->GetImsi () - 1),
                             NrUeRrc::CONNECTED_NORMALLY,
                             "UE " << ueDev->GetImsi ()
                                   << " is not at CONNECTED_NORMALLY state");
    }
}


void
NrCellSelectionTestCase::StateTransitionCallback (
  std::string context, uint64_t imsi, uint16_t cellId, uint16_t rnti,
  NrUeRrc::State oldState, NrUeRrc::State newState)
{
  NS_LOG_FUNCTION (this << imsi << cellId << rnti << oldState << newState);
  m_lastState.at (imsi - 1) = newState;
}


void
NrCellSelectionTestCase::InitialCellSelectionEndOkCallback (
  std::string context, uint64_t imsi, uint16_t cellId)
{
  NS_LOG_FUNCTION (this << imsi << cellId);
}


void
NrCellSelectionTestCase::InitialCellSelectionEndErrorCallback (
  std::string context, uint64_t imsi, uint16_t cellId)
{
  NS_LOG_FUNCTION (this << imsi << cellId);
}


void
NrCellSelectionTestCase::ConnectionEstablishedCallback (
  std::string context, uint64_t imsi, uint16_t cellId, uint16_t rnti)
{
  NS_LOG_FUNCTION (this << imsi << cellId << rnti);
}
