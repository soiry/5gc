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

#include <ns3/test.h>

#include <ns3/log.h>
#include <ns3/nstime.h>
#include <ns3/callback.h>
#include <ns3/config.h>
#include <ns3/boolean.h>
#include <ns3/double.h>
#include <ns3/uinteger.h>
#include <ns3/simulator.h>

#include <ns3/node-container.h>
#include <ns3/net-device-container.h>
#include <ns3/ipv4-interface-container.h>

#include <ns3/nr-helper.h>
#include <ns3/point-to-point-ngc-helper.h>
#include <ns3/internet-stack-helper.h>
#include <ns3/point-to-point-helper.h>
#include <ns3/ipv4-address-helper.h>
#include <ns3/ipv4-static-routing-helper.h>
#include <ns3/mobility-helper.h>

#include <ns3/data-rate.h>
#include <ns3/ipv4-static-routing.h>
#include <ns3/position-allocator.h>
#include <ns3/nr-enb-net-device.h>
#include <ns3/nr-enb-phy.h>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("NrHandoverTargetTest");

/**
 * \brief Testing a handover algorithm, verifying that it selects the right
 *        target cell when more than one options available.
 *
 * Part of the `nr-handover-target` test suite.
 *
 * The test case will run a 1-second NR-NGC simulation using the parameters
 * provided to the constructor function.
 *
 * \sa ns3::NrHandoverTargetTestCase
 */
class NrHandoverTargetTestCase : public TestCase
{
public:
  /**
   * \brief Construct a new test case and providing input parameters for the
   *        simulation.
   * \param name the name of the test case, to be displayed in the test result
   * \param uePosition the point in (x, y, z) coordinate where the UE will be
   *                   placed in the simulation
   * \param gridSizeX number of eNodeBs in a row
   * \param gridSizeY number of eNodeBs in a column
   * \param sourceCellId the cell ID of the eNodeB which the UE will be
   *                     initially attached to in the beginning of simulation,
   *                     and also the eNodeB which will "shutdown" in the
   *                     middle of simulation
   * \param targetCellId the cell ID of the expected eNodeB where the UE will
   *                     perform handover to after the "shutdown" of the source
   *                     cell
   * \param handoverAlgorithmType the type of handover algorithm to be used in
   *                              all eNodeBs
   */
  NrHandoverTargetTestCase (std::string name, Vector uePosition,
                             uint8_t gridSizeX, uint8_t gridSizeY,
                             uint16_t sourceCellId, uint16_t targetCellId,
                             std::string handoverAlgorithmType);

  virtual ~NrHandoverTargetTestCase ();

  /**
   * \brief Triggers when an eNodeB starts a handover and then verifies that
   *        the handover has the right source and target cells.
   *
   * The trigger is set up beforehand by connecting to the
   * `NrEnbRrc::HandoverStart` trace source.
   */
  void HandoverStartCallback (std::string context, uint64_t imsi,
                              uint16_t sourceCellId, uint16_t rnti,
                              uint16_t targetCellId);

  /**
   * \brief A trigger that can be scheduled to "shutdown" the cell pointed by
   *        `m_sourceCellId` by reducing its power to 1 dB.
   */
  void CellShutdownCallback ();

private:
  /**
   * \brief Run a simulation of a micro-cell network using the parameters
   *        provided to the constructor function.
   */
  virtual void DoRun ();

  /**
   * \brief Called at the end of simulation and verifies that a handover has
   *        occurred in the simulation.
   */
  virtual void DoTeardown ();

  // simulation parameters
  Vector m_uePosition;
  uint8_t m_gridSizeX;
  uint8_t m_gridSizeY;
  uint16_t m_sourceCellId;
  uint16_t m_targetCellId;
  std::string m_handoverAlgorithmType;

  Ptr<NrEnbNetDevice> m_sourceEnbDev;
  bool m_hasHandoverOccurred;

}; // end of class NrHandoverTargetTestCase


NrHandoverTargetTestCase::NrHandoverTargetTestCase (std::string name, Vector uePosition,
                                                      uint8_t gridSizeX, uint8_t gridSizeY,
                                                      uint16_t sourceCellId, uint16_t targetCellId,
                                                      std::string handoverAlgorithmType)
  : TestCase (name),
    m_uePosition (uePosition),
    m_gridSizeX (gridSizeX),
    m_gridSizeY (gridSizeY),
    m_sourceCellId (sourceCellId),
    m_targetCellId (targetCellId),
    m_handoverAlgorithmType (handoverAlgorithmType),
    m_sourceEnbDev (0),
    m_hasHandoverOccurred (false)
{
  NS_LOG_INFO (this << " name=" << name);

  // SANITY CHECK

  uint16_t nEnb = gridSizeX * gridSizeY;

  if (sourceCellId > nEnb)
    {
      NS_FATAL_ERROR ("Invalid source cell ID " << sourceCellId);
    }

  if (targetCellId > nEnb)
    {
      NS_FATAL_ERROR ("Invalid target cell ID " << targetCellId);
    }
}


NrHandoverTargetTestCase::~NrHandoverTargetTestCase ()
{
  NS_LOG_FUNCTION (this);
}


void
NrHandoverTargetTestCase::HandoverStartCallback (std::string context, uint64_t imsi,
                                                  uint16_t sourceCellId, uint16_t rnti,
                                                  uint16_t targetCellId)
{
  NS_LOG_FUNCTION (this << context << imsi << sourceCellId << rnti << targetCellId);

  uint64_t timeNowMs = Simulator::Now ().GetMilliSeconds ();
  NS_TEST_ASSERT_MSG_GT (timeNowMs, 500,
                         "Handover occured but too early");
  NS_TEST_ASSERT_MSG_EQ (sourceCellId, m_sourceCellId,
                         "Handover occured but with wrong source cell");
  NS_TEST_ASSERT_MSG_EQ (targetCellId, m_targetCellId,
                         "Handover occured but with wrong target cell");
  m_hasHandoverOccurred = true;
}


void
NrHandoverTargetTestCase::CellShutdownCallback ()
{
  NS_LOG_FUNCTION (this);

  if (m_sourceEnbDev != 0)
    {
      // set the Tx power to 1 dBm
      NS_ASSERT (m_sourceEnbDev->GetCellId () == m_sourceCellId);
      NS_LOG_INFO ("Shutting down cell " << m_sourceCellId);
      Ptr<NrEnbPhy> phy = m_sourceEnbDev->GetPhy ();
      phy->SetTxPower (1);
    }
}


void
NrHandoverTargetTestCase::DoRun ()
{
  NS_LOG_INFO (this << " " << GetName ());

  Config::SetDefault ("ns3::NrEnbPhy::TxPower", DoubleValue (38)); // micro cell
  Config::SetDefault ("ns3::NrSpectrumPhy::CtrlErrorModelEnabled",
                      BooleanValue (false)); // disable control channel error model

  Ptr<NrHelper> nrHelper = CreateObject<NrHelper> ();
  Ptr<PointToPointNgcHelper> ngcHelper = CreateObject<PointToPointNgcHelper> ();
  nrHelper->SetNgcHelper (ngcHelper);
  nrHelper->SetAttribute ("PathlossModel",
                           StringValue ("ns3::FriisSpectrumPropagationLossModel"));
  nrHelper->SetAttribute ("UseIdealRrc", BooleanValue (true));

  if (m_handoverAlgorithmType == "ns3::NrA2A4RsrqHandoverAlgorithm")
    {
      nrHelper->SetHandoverAlgorithmType ("ns3::NrA2A4RsrqHandoverAlgorithm");
      nrHelper->SetHandoverAlgorithmAttribute ("ServingCellThreshold",
                                                UintegerValue (30));
      nrHelper->SetHandoverAlgorithmAttribute ("NeighbourCellOffset",
                                                UintegerValue (1));
    }
  else if (m_handoverAlgorithmType == "ns3::NrA3RsrpHandoverAlgorithm")
    {
      nrHelper->SetHandoverAlgorithmType ("ns3::NrA3RsrpHandoverAlgorithm");
      nrHelper->SetHandoverAlgorithmAttribute ("Hysteresis",
                                                DoubleValue (1.5));
      nrHelper->SetHandoverAlgorithmAttribute ("TimeToTrigger",
                                                TimeValue (MilliSeconds (128)));
    }
  else
    {
      NS_FATAL_ERROR ("Unknown handover algorithm " << m_handoverAlgorithmType);
    }

  // Create Nodes: eNodeB and UE
  NodeContainer enbNodes;
  NodeContainer ueNodes;
  enbNodes.Create (m_gridSizeX * m_gridSizeY);
  ueNodes.Create (1);

  /*
   * The size of the grid is determined by m_gridSizeX and m_gridSizeY. The
   * following figure is the topology when m_gridSizeX = 4 and m_gridSizeY = 3.
   *
   *                  9 -- 10 -- 11 -- 12
   *                  |     |     |     |
   *                  |     |     |     |
   *                  5 --- 6 --- 7 --- 8
   *                  |     |     |     |
   *                  |     |     |     |
   *   (0, 0, 0) ---> 1 --- 2 --- 3 --- 4
   *
   * The grid starts at (0, 0, 0) point on the bottom left corner. The distance
   * between two adjacent eNodeBs is 130 m.
   */

  // Set up eNodeB position
  MobilityHelper enbMobility;
  enbMobility.SetPositionAllocator ("ns3::GridPositionAllocator",
                                    "MinX", DoubleValue (0.0),
                                    "MinY", DoubleValue (0.0),
                                    "DeltaX", DoubleValue (130.0),
                                    "DeltaY", DoubleValue (130.0),
                                    "GridWidth", UintegerValue (m_gridSizeX),
                                    "LayoutType", StringValue ("RowFirst"));
  enbMobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  enbMobility.Install (enbNodes);

  // Setup UE position
  Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator> ();
  positionAlloc->Add (m_uePosition);
  MobilityHelper ueMobility;
  ueMobility.SetPositionAllocator (positionAlloc);
  ueMobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  ueMobility.Install (ueNodes);

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

  // Create Devices and install them in the Nodes (eNB and UE)
  NetDeviceContainer enbDevs;
  NetDeviceContainer ueDevs;
  enbDevs = nrHelper->InstallEnbDevice (enbNodes);
  ueDevs = nrHelper->InstallUeDevice (ueNodes);

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

  // Add X2 interface
  nrHelper->AddX2Interface (enbNodes);

  // Connect to trace sources in all eNodeB
  Config::Connect ("/NodeList/*/DeviceList/*/NrEnbRrc/HandoverStart",
                   MakeCallback (&NrHandoverTargetTestCase::HandoverStartCallback,
                                 this));

  // Get the source eNodeB
  Ptr<NetDevice> sourceEnb = enbDevs.Get (m_sourceCellId - 1);
  m_sourceEnbDev = sourceEnb->GetObject<NrEnbNetDevice> ();
  NS_ASSERT (m_sourceEnbDev != 0);
  NS_ASSERT (m_sourceEnbDev->GetCellId () == m_sourceCellId);

  // Attach UE to the source eNodeB
  nrHelper->Attach (ueDevs.Get (0), sourceEnb);

  // Schedule a "shutdown" of the source eNodeB
  Simulator::Schedule (Seconds (0.5),
                       &NrHandoverTargetTestCase::CellShutdownCallback, this);

  // Run simulation
  Simulator::Stop (Seconds (1));
  Simulator::Run ();
  Simulator::Destroy ();

} // end of void NrX2HandoverTargetTestCase::DoRun ()


void
NrHandoverTargetTestCase::DoTeardown ()
{
  NS_LOG_FUNCTION (this);
  NS_TEST_ASSERT_MSG_EQ (m_hasHandoverOccurred, true, "Handover did not occur");
}



/**
 * \brief Test suite ``nr-handover-target``, verifying that handover
 *        algorithms are able to select the right target cell.
 *
 * Handover algorithm tested in this test suite:
 * - A2-A4-RSRQ handover algorithm (ns3::NrA2A4RsrqHandoverAlgorithm)
 * - Strongest cell handover algorithm (ns3::NrA3RsrpHandoverAlgorithm)
 */
class NrHandoverTargetTestSuite : public TestSuite
{
public:
  NrHandoverTargetTestSuite ();
};


NrHandoverTargetTestSuite::NrHandoverTargetTestSuite ()
  : TestSuite ("nr-handover-target", SYSTEM)
{
  // LogComponentEnable ("NrHandoverTargetTest", LOG_PREFIX_ALL);
  // LogComponentEnable ("NrHandoverTargetTest", LOG_LEVEL_ALL);
  // LogComponentEnable ("NrA2A4RsrqHandoverAlgorithm", LOG_PREFIX_ALL);
  // LogComponentEnable ("NrA2A4RsrqHandoverAlgorithm", LOG_LEVEL_ALL);
  // LogComponentEnable ("NrA3RsrpHandoverAlgorithm", LOG_PREFIX_ALL);
  // LogComponentEnable ("NrA3RsrpHandoverAlgorithm", LOG_LEVEL_ALL);

  /*
   *    3 --- 4
   *    |     |
   *    |o    |
   *    1 --- 2   o = UE
   */
  AddTestCase (new NrHandoverTargetTestCase ("4 cells and A2-A4-RSRQ algorithm",
                                              Vector (20, 40, 0), 2, 2, 1, 3,
                                              "ns3::NrA2A4RsrqHandoverAlgorithm"),
               TestCase::QUICK);
  AddTestCase (new NrHandoverTargetTestCase ("4 cells and strongest cell algorithm",
                                              Vector (20, 40, 0), 2, 2, 1, 3,
                                              "ns3::NrA3RsrpHandoverAlgorithm"),
               TestCase::QUICK);

  /*
   *    4 --- 5 --- 6
   *    |     |o    |
   *    |     |     |
   *    1 --- 2 --- 3   o = UE
   */
  AddTestCase (new NrHandoverTargetTestCase ("6 cells and A2-A4-RSRQ algorithm",
                                              Vector (150, 90, 0), 3, 2, 5, 2,
                                              "ns3::NrA2A4RsrqHandoverAlgorithm"),
               TestCase::EXTENSIVE);
  AddTestCase (new NrHandoverTargetTestCase ("6 cells and strongest cell algorithm",
                                              Vector (150, 90, 0), 3, 2, 5, 2,
                                              "ns3::NrA3RsrpHandoverAlgorithm"),
               TestCase::EXTENSIVE);

} // end of NrHandoverTargetTestSuite ()


static NrHandoverTargetTestSuite g_nrHandoverTargetTestSuiteInstance;
