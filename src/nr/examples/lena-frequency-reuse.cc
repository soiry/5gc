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

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/mobility-module.h"
#include "ns3/nr-module.h"
#include "ns3/config-store.h"
#include <ns3/buildings-helper.h>
#include <ns3/spectrum-module.h>
#include <ns3/log.h>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("LenaFrequencyReuse");

void
PrintGnuplottableUeListToFile (std::string filename)
{
  std::ofstream outFile;
  outFile.open (filename.c_str (), std::ios_base::out | std::ios_base::trunc);
  if (!outFile.is_open ())
    {
      NS_LOG_ERROR ("Can't open file " << filename);
      return;
    }
  for (NodeList::Iterator it = NodeList::Begin (); it != NodeList::End (); ++it)
    {
      Ptr<Node> node = *it;
      int nDevs = node->GetNDevices ();
      for (int j = 0; j < nDevs; j++)
        {
          Ptr<NrUeNetDevice> uedev = node->GetDevice (j)->GetObject <NrUeNetDevice> ();
          if (uedev)
            {
              Vector pos = node->GetObject<MobilityModel> ()->GetPosition ();
              outFile << "set label \"" << uedev->GetImsi ()
                      << "\" at " << pos.x << "," << pos.y << " left font \"Helvetica,4\" textcolor rgb \"grey\" front point pt 1 ps 0.3 lc rgb \"grey\" offset 0,0"
                      << std::endl;
            }
        }
    }
}

void
PrintGnuplottableEnbListToFile (std::string filename)
{
  std::ofstream outFile;
  outFile.open (filename.c_str (), std::ios_base::out | std::ios_base::trunc);
  if (!outFile.is_open ())
    {
      NS_LOG_ERROR ("Can't open file " << filename);
      return;
    }
  for (NodeList::Iterator it = NodeList::Begin (); it != NodeList::End (); ++it)
    {
      Ptr<Node> node = *it;
      int nDevs = node->GetNDevices ();
      for (int j = 0; j < nDevs; j++)
        {
          Ptr<NrEnbNetDevice> enbdev = node->GetDevice (j)->GetObject <NrEnbNetDevice> ();
          if (enbdev)
            {
              Vector pos = node->GetObject<MobilityModel> ()->GetPosition ();
              outFile << "set label \"" << enbdev->GetCellId ()
                      << "\" at " << pos.x << "," << pos.y
                      << " left font \"Helvetica,4\" textcolor rgb \"white\" front  point pt 2 ps 0.3 lc rgb \"white\" offset 0,0"
                      << std::endl;
            }
        }
    }
}

int main (int argc, char *argv[])
{
  Config::SetDefault ("ns3::NrSpectrumPhy::CtrlErrorModelEnabled", BooleanValue (true));
  Config::SetDefault ("ns3::NrSpectrumPhy::DataErrorModelEnabled", BooleanValue (true));
  Config::SetDefault ("ns3::NrHelper::UseIdealRrc", BooleanValue (true));
  Config::SetDefault ("ns3::NrHelper::UsePdschForCqiGeneration", BooleanValue (true));

  //Uplink Power Control
  Config::SetDefault ("ns3::NrUePhy::EnableUplinkPowerControl", BooleanValue (true));
  Config::SetDefault ("ns3::NrUePowerControl::ClosedLoop", BooleanValue (true));
  Config::SetDefault ("ns3::NrUePowerControl::AccumulationEnabled", BooleanValue (false));

  uint32_t runId = 3;
  uint16_t numberOfRandomUes = 0;
  double simTime = 2.500;
  bool generateSpectrumTrace = false;
  bool generateRem = false;
  int32_t remRbId = -1;
  uint8_t bandwidth = 25;
  double distance = 1000;
  Box macroUeBox = Box (-distance * 0.5, distance * 1.5, -distance * 0.5, distance * 1.5, 1.5, 1.5);

  // Command line arguments
  CommandLine cmd;
  cmd.AddValue ("numberOfUes", "Number of random UEs", numberOfRandomUes);
  cmd.AddValue ("simTime", "Total duration of the simulation (in seconds)", simTime);
  cmd.AddValue ("generateSpectrumTrace", "if true, will generate a Spectrum Analyzer trace", generateSpectrumTrace);
  cmd.AddValue ("generateRem", "if true, will generate a REM and then abort the simulation", generateRem);
  cmd.AddValue ("remRbId", "Resource Block Id, for which REM will be generated,"
                "default value is -1, what means REM will be averaged from all RBs", remRbId);
  cmd.AddValue ("runId", "runId", runId);
  cmd.Parse (argc, argv);

  RngSeedManager::SetSeed (1);
  RngSeedManager::SetRun (runId);

  Ptr<NrHelper> nrHelper = CreateObject<NrHelper> ();

  // Create Nodes: eNodeB and UE
  NodeContainer enbNodes;
  NodeContainer centerUeNodes;
  NodeContainer edgeUeNodes;
  NodeContainer randomUeNodes;
  enbNodes.Create (3);
  centerUeNodes.Create (3);
  edgeUeNodes.Create (3);
  randomUeNodes.Create (numberOfRandomUes);


/*   the topology is the following:
 *                 eNB3
 *                /     \
 *               /       \
 *              /         \
 *             /           \
 *   distance /             \ distance
 *           /      UEs      \
 *          /                 \
 *         /                   \
 *        /                     \
 *       /                       \
 *   eNB1-------------------------eNB2
 *                  distance
 */

  // Install Mobility Model
  Ptr<ListPositionAllocator> enbPositionAlloc = CreateObject<ListPositionAllocator> ();
  enbPositionAlloc->Add (Vector (0.0, 0.0, 0.0));                       // eNB1
  enbPositionAlloc->Add (Vector (distance,  0.0, 0.0));                 // eNB2
  enbPositionAlloc->Add (Vector (distance * 0.5, distance * 0.866, 0.0));   // eNB3
  MobilityHelper mobility;
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.SetPositionAllocator (enbPositionAlloc);
  mobility.Install (enbNodes);


  Ptr<ListPositionAllocator> edgeUePositionAlloc = CreateObject<ListPositionAllocator> ();
  edgeUePositionAlloc->Add (Vector (distance * 0.5, distance * 0.28867, 0.0));  // edgeUE1
  edgeUePositionAlloc->Add (Vector (distance * 0.5, distance * 0.28867, 0.0));  // edgeUE2
  edgeUePositionAlloc->Add (Vector (distance * 0.5, distance * 0.28867, 0.0));  // edgeUE3
  mobility.SetPositionAllocator (edgeUePositionAlloc);
  mobility.Install (edgeUeNodes);


  Ptr<ListPositionAllocator> centerUePositionAlloc = CreateObject<ListPositionAllocator> ();
  centerUePositionAlloc->Add (Vector (0.0, 0.0, 0.0));                                      // centerUE1
  centerUePositionAlloc->Add (Vector (distance,  0.0, 0.0));                            // centerUE2
  centerUePositionAlloc->Add (Vector (distance * 0.5, distance * 0.866, 0.0));      // centerUE3
  mobility.SetPositionAllocator (centerUePositionAlloc);
  mobility.Install (centerUeNodes);


  Ptr<RandomBoxPositionAllocator> randomUePositionAlloc = CreateObject<RandomBoxPositionAllocator> ();
  Ptr<UniformRandomVariable> xVal = CreateObject<UniformRandomVariable> ();
  xVal->SetAttribute ("Min", DoubleValue (macroUeBox.xMin));
  xVal->SetAttribute ("Max", DoubleValue (macroUeBox.xMax));
  randomUePositionAlloc->SetAttribute ("X", PointerValue (xVal));
  Ptr<UniformRandomVariable> yVal = CreateObject<UniformRandomVariable> ();
  yVal->SetAttribute ("Min", DoubleValue (macroUeBox.yMin));
  yVal->SetAttribute ("Max", DoubleValue (macroUeBox.yMax));
  randomUePositionAlloc->SetAttribute ("Y", PointerValue (yVal));
  Ptr<UniformRandomVariable> zVal = CreateObject<UniformRandomVariable> ();
  zVal->SetAttribute ("Min", DoubleValue (macroUeBox.zMin));
  zVal->SetAttribute ("Max", DoubleValue (macroUeBox.zMax));
  randomUePositionAlloc->SetAttribute ("Z", PointerValue (zVal));
  mobility.SetPositionAllocator (randomUePositionAlloc);
  mobility.Install (randomUeNodes);

  // Create Devices and install them in the Nodes (eNB and UE)
  NetDeviceContainer enbDevs;
  NetDeviceContainer edgeUeDevs;
  NetDeviceContainer centerUeDevs;
  NetDeviceContainer randomUeDevs;
  nrHelper->SetSchedulerType ("ns3::NrPfFfMacScheduler");
  nrHelper->SetSchedulerAttribute ("UlCqiFilter", EnumValue (NrFfMacScheduler::PUSCH_UL_CQI));
  nrHelper->SetEnbDeviceAttribute ("DlBandwidth", UintegerValue (bandwidth));
  nrHelper->SetEnbDeviceAttribute ("UlBandwidth", UintegerValue (bandwidth));

  std::string frAlgorithmType = nrHelper->GetFfrAlgorithmType ();
  NS_LOG_DEBUG ("FrAlgorithmType: " << frAlgorithmType);

  if (frAlgorithmType == "ns3::NrFrHardAlgorithm")
    {

      //Nothing to configure here in automatic mode

    }
  else if (frAlgorithmType == "ns3::NrFrStrictAlgorithm")
    {

      nrHelper->SetFfrAlgorithmAttribute ("RsrqThreshold", UintegerValue (32));
      nrHelper->SetFfrAlgorithmAttribute ("CenterPowerOffset",
                                           UintegerValue (NrRrcSap::PdschConfigDedicated::dB_6));
      nrHelper->SetFfrAlgorithmAttribute ("EdgePowerOffset",
                                           UintegerValue (NrRrcSap::PdschConfigDedicated::dB3));
      nrHelper->SetFfrAlgorithmAttribute ("CenterAreaTpc", UintegerValue (0));
      nrHelper->SetFfrAlgorithmAttribute ("EdgeAreaTpc", UintegerValue (3));

      //ns3::NrFrStrictAlgorithm works with Absolute Mode Uplink Power Control
      Config::SetDefault ("ns3::NrUePowerControl::AccumulationEnabled", BooleanValue (false));

    }
  else if (frAlgorithmType == "ns3::NrFrSoftAlgorithm")
    {

      nrHelper->SetFfrAlgorithmAttribute ("AllowCenterUeUseEdgeSubBand", BooleanValue (true));
      nrHelper->SetFfrAlgorithmAttribute ("RsrqThreshold", UintegerValue (25));
      nrHelper->SetFfrAlgorithmAttribute ("CenterPowerOffset",
                                           UintegerValue (NrRrcSap::PdschConfigDedicated::dB_6));
      nrHelper->SetFfrAlgorithmAttribute ("EdgePowerOffset",
                                           UintegerValue (NrRrcSap::PdschConfigDedicated::dB3));
      nrHelper->SetFfrAlgorithmAttribute ("CenterAreaTpc", UintegerValue (0));
      nrHelper->SetFfrAlgorithmAttribute ("EdgeAreaTpc", UintegerValue (3));

      //ns3::NrFrSoftAlgorithm works with Absolute Mode Uplink Power Control
      Config::SetDefault ("ns3::NrUePowerControl::AccumulationEnabled", BooleanValue (false));

    }
  else if (frAlgorithmType == "ns3::NrFfrSoftAlgorithm")
    {

      nrHelper->SetFfrAlgorithmAttribute ("CenterRsrqThreshold", UintegerValue (30));
      nrHelper->SetFfrAlgorithmAttribute ("EdgeRsrqThreshold", UintegerValue (25));
      nrHelper->SetFfrAlgorithmAttribute ("CenterAreaPowerOffset",
                                           UintegerValue (NrRrcSap::PdschConfigDedicated::dB_6));
      nrHelper->SetFfrAlgorithmAttribute ("MediumAreaPowerOffset",
                                           UintegerValue (NrRrcSap::PdschConfigDedicated::dB_1dot77));
      nrHelper->SetFfrAlgorithmAttribute ("EdgeAreaPowerOffset",
                                           UintegerValue (NrRrcSap::PdschConfigDedicated::dB3));
      nrHelper->SetFfrAlgorithmAttribute ("CenterAreaTpc", UintegerValue (1));
      nrHelper->SetFfrAlgorithmAttribute ("MediumAreaTpc", UintegerValue (2));
      nrHelper->SetFfrAlgorithmAttribute ("EdgeAreaTpc", UintegerValue (3));

      //ns3::NrFfrSoftAlgorithm works with Absolute Mode Uplink Power Control
      Config::SetDefault ("ns3::NrUePowerControl::AccumulationEnabled", BooleanValue (false));

    }
  else if (frAlgorithmType == "ns3::NrFfrEnhancedAlgorithm")
    {

      nrHelper->SetFfrAlgorithmAttribute ("RsrqThreshold", UintegerValue (25));
      nrHelper->SetFfrAlgorithmAttribute ("DlCqiThreshold", UintegerValue (10));
      nrHelper->SetFfrAlgorithmAttribute ("UlCqiThreshold", UintegerValue (10));
      nrHelper->SetFfrAlgorithmAttribute ("CenterAreaPowerOffset",
                                           UintegerValue (NrRrcSap::PdschConfigDedicated::dB_6));
      nrHelper->SetFfrAlgorithmAttribute ("EdgeAreaPowerOffset",
                                           UintegerValue (NrRrcSap::PdschConfigDedicated::dB3));
      nrHelper->SetFfrAlgorithmAttribute ("CenterAreaTpc", UintegerValue (0));
      nrHelper->SetFfrAlgorithmAttribute ("EdgeAreaTpc", UintegerValue (3));

      //ns3::NrFfrEnhancedAlgorithm works with Absolute Mode Uplink Power Control
      Config::SetDefault ("ns3::NrUePowerControl::AccumulationEnabled", BooleanValue (false));

    }
  else if (frAlgorithmType == "ns3::NrFfrDistributedAlgorithm")
    {

      NS_FATAL_ERROR ("ns3::NrFfrDistributedAlgorithm not supported in this example. Please run lena-distributed-ffr");

    }
  else
    {
      nrHelper->SetFfrAlgorithmType ("ns3::NrFrNoOpAlgorithm");
    }

  nrHelper->SetFfrAlgorithmAttribute ("FrCellTypeId", UintegerValue (1));
  enbDevs.Add (nrHelper->InstallEnbDevice (enbNodes.Get (0)));

  nrHelper->SetFfrAlgorithmAttribute ("FrCellTypeId", UintegerValue (2));
  enbDevs.Add (nrHelper->InstallEnbDevice (enbNodes.Get (1)));

  nrHelper->SetFfrAlgorithmAttribute ("FrCellTypeId", UintegerValue (3));
  enbDevs.Add (nrHelper->InstallEnbDevice (enbNodes.Get (2)));

  //FR algorithm reconfiguration if needed
  PointerValue tmp;
  enbDevs.Get (0)->GetAttribute ("NrFfrAlgorithm", tmp);
  Ptr<NrFfrAlgorithm> ffrAlgorithm = DynamicCast<NrFfrAlgorithm> (tmp.GetObject ());
  ffrAlgorithm->SetAttribute ("FrCellTypeId", UintegerValue (1));


  //Install Ue Device
  edgeUeDevs = nrHelper->InstallUeDevice (edgeUeNodes);
  centerUeDevs = nrHelper->InstallUeDevice (centerUeNodes);
  randomUeDevs = nrHelper->InstallUeDevice (randomUeNodes);

  // Attach edge UEs to eNbs
  for (uint32_t i = 0; i < edgeUeDevs.GetN (); i++)
    {
      nrHelper->Attach (edgeUeDevs.Get (i), enbDevs.Get (i));
    }
  // Attach center UEs to eNbs
  for (uint32_t i = 0; i < centerUeDevs.GetN (); i++)
    {
      nrHelper->Attach (centerUeDevs.Get (i), enbDevs.Get (i));
    }

  // Attach UE to a eNB
  nrHelper->AttachToClosestEnb (randomUeDevs, enbDevs);

  // Activate a data radio bearer
  enum EpsBearer::Qci q = EpsBearer::GBR_CONV_VOICE;
  EpsBearer bearer (q);
  nrHelper->ActivateDataRadioBearer (edgeUeDevs, bearer);
  nrHelper->ActivateDataRadioBearer (centerUeDevs, bearer);
  nrHelper->ActivateDataRadioBearer (randomUeDevs, bearer);

  //Spectrum analyzer
  NodeContainer spectrumAnalyzerNodes;
  spectrumAnalyzerNodes.Create (1);
  SpectrumAnalyzerHelper spectrumAnalyzerHelper;

  if (generateSpectrumTrace)
    {
      Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator> ();
      //position of Spectrum Analyzer
//	  positionAlloc->Add (Vector (0.0, 0.0, 0.0));                              // eNB1
//	  positionAlloc->Add (Vector (distance,  0.0, 0.0));                        // eNB2
      positionAlloc->Add (Vector (distance * 0.5, distance * 0.866, 0.0));          // eNB3

      MobilityHelper mobility;
      mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
      mobility.SetPositionAllocator (positionAlloc);
      mobility.Install (spectrumAnalyzerNodes);

      Ptr<NrSpectrumPhy> enbDlSpectrumPhy = enbDevs.Get (0)->GetObject<NrEnbNetDevice> ()->GetPhy ()->GetDownlinkSpectrumPhy ()->GetObject<NrSpectrumPhy> ();
      Ptr<SpectrumChannel> dlChannel = enbDlSpectrumPhy->GetChannel ();

      spectrumAnalyzerHelper.SetChannel (dlChannel);
      Ptr<SpectrumModel> sm = NrSpectrumValueHelper::GetSpectrumModel (100, bandwidth);
      spectrumAnalyzerHelper.SetRxSpectrumModel (sm);
      spectrumAnalyzerHelper.SetPhyAttribute ("Resolution", TimeValue (MicroSeconds (10)));
      spectrumAnalyzerHelper.SetPhyAttribute ("NoisePowerSpectralDensity", DoubleValue (1e-15));     // -120 dBm/Hz
      spectrumAnalyzerHelper.EnableAsciiAll ("spectrum-analyzer-output");
      spectrumAnalyzerHelper.Install (spectrumAnalyzerNodes);
    }

  Ptr<NrRadioEnvironmentMapHelper> remHelper;
  if (generateRem)
    {
      PrintGnuplottableEnbListToFile ("enbs.txt");
      PrintGnuplottableUeListToFile ("ues.txt");

      remHelper = CreateObject<NrRadioEnvironmentMapHelper> ();
      remHelper->SetAttribute ("ChannelPath", StringValue ("/ChannelList/0"));
      remHelper->SetAttribute ("OutputFile", StringValue ("lena-frequency-reuse.rem"));
      remHelper->SetAttribute ("XMin", DoubleValue (macroUeBox.xMin));
      remHelper->SetAttribute ("XMax", DoubleValue (macroUeBox.xMax));
      remHelper->SetAttribute ("YMin", DoubleValue (macroUeBox.yMin));
      remHelper->SetAttribute ("YMax", DoubleValue (macroUeBox.yMax));
      remHelper->SetAttribute ("Z", DoubleValue (1.5));
      remHelper->SetAttribute ("XRes", UintegerValue (500));
      remHelper->SetAttribute ("YRes", UintegerValue (500));
      if (remRbId >= 0)
        {
          remHelper->SetAttribute ("UseDataChannel", BooleanValue (true));
          remHelper->SetAttribute ("RbId", IntegerValue (remRbId));
        }

      remHelper->Install ();
      // simulation will stop right after the REM has been generated
    }
  else
    {
      Simulator::Stop (Seconds (simTime));
    }

  Simulator::Run ();
  Simulator::Destroy ();
  return 0;
}
