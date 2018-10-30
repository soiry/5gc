/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2012 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
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
 * Author: Manuel Requena <manuel.requena@cttc.es> (Based on nr-helper.cc)
 */


#include "ns3/log.h"
#include "ns3/callback.h"
#include "ns3/config.h"
#include "ns3/simple-channel.h"
#include "ns3/error-model.h"

#include "nr-simple-helper.h"
#include "nr-simple-net-device.h"
#include "nr-test-entities.h"


namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("NrSimpleHelper");

NS_OBJECT_ENSURE_REGISTERED (NrSimpleHelper);

NrSimpleHelper::NrSimpleHelper (void)
{
  NS_LOG_FUNCTION (this);
  m_enbDeviceFactory.SetTypeId (NrSimpleNetDevice::GetTypeId ());
  m_ueDeviceFactory.SetTypeId (NrSimpleNetDevice::GetTypeId ());
}

void
NrSimpleHelper::DoInitialize (void)
{
  NS_LOG_FUNCTION (this);

  m_phyChannel = CreateObject<SimpleChannel> ();

  Object::DoInitialize ();
}

NrSimpleHelper::~NrSimpleHelper (void)
{
  NS_LOG_FUNCTION (this);
}

TypeId NrSimpleHelper::GetTypeId (void)
{
  static TypeId
  tid =
    TypeId ("ns3::NrSimpleHelper")
    .SetParent<Object> ()
    .AddConstructor<NrSimpleHelper> ()
    .AddAttribute ("RlcEntity",
                   "Specify which type of RLC will be used. ",
                   EnumValue (RLC_UM),
                   MakeEnumAccessor (&NrSimpleHelper::m_nrRlcEntityType),
                   MakeEnumChecker (RLC_UM, "RlcUm",
                                    RLC_AM, "RlcAm"))
  ;
  return tid;
}

void
NrSimpleHelper::DoDispose ()
{
  NS_LOG_FUNCTION (this);
  m_phyChannel = 0;

  m_enbMac->Dispose ();
  m_enbMac = 0;
  m_ueMac->Dispose ();
  m_ueMac = 0;

  Object::DoDispose ();
}


NetDeviceContainer
NrSimpleHelper::InstallEnbDevice (NodeContainer c)
{
  NS_LOG_FUNCTION (this);
  Initialize ();  // will run DoInitialize () if necessary
  NetDeviceContainer devices;
  for (NodeContainer::Iterator i = c.Begin (); i != c.End (); ++i)
    {
      Ptr<Node> node = *i;
      Ptr<NetDevice> device = InstallSingleEnbDevice (node);
      devices.Add (device);
    }
  return devices;
}

NetDeviceContainer
NrSimpleHelper::InstallUeDevice (NodeContainer c)
{
  NS_LOG_FUNCTION (this);
  NetDeviceContainer devices;
  for (NodeContainer::Iterator i = c.Begin (); i != c.End (); ++i)
    {
      Ptr<Node> node = *i;
      Ptr<NetDevice> device = InstallSingleUeDevice (node);
      devices.Add (device);
    }
  return devices;
}


Ptr<NetDevice>
NrSimpleHelper::InstallSingleEnbDevice (Ptr<Node> n)
{
  NS_LOG_FUNCTION (this);

  m_enbRrc = CreateObject<NrTestRrc> ();
  m_enbPdcp = CreateObject<NrPdcp> ();

  if (m_nrRlcEntityType == RLC_UM)
    {
      m_enbRlc = CreateObject<NrRlcUm> ();
    }
  else // m_nrRlcEntityType == RLC_AM
    {
      m_enbRlc = CreateObject<NrRlcAm> ();
    }

  m_enbRlc->SetRnti (11);
  m_enbRlc->SetLcId (12);

  Ptr<NrSimpleNetDevice> enbDev = m_enbDeviceFactory.Create<NrSimpleNetDevice> ();
  enbDev->SetAddress (Mac48Address::Allocate ());
  enbDev->SetChannel (m_phyChannel);

  n->AddDevice (enbDev);

  m_enbMac = CreateObject<NrTestMac> ();
  m_enbMac->SetDevice (enbDev);

  m_enbRrc->SetDevice (enbDev);

  enbDev->SetReceiveCallback (MakeCallback (&NrTestMac::Receive, m_enbMac));

  // Connect SAPs: RRC <-> PDCP <-> RLC <-> MAC

  m_enbRrc->SetNrPdcpSapProvider (m_enbPdcp->GetNrPdcpSapProvider ());
  m_enbPdcp->SetNrPdcpSapUser (m_enbRrc->GetNrPdcpSapUser ());

  m_enbPdcp->SetNrRlcSapProvider (m_enbRlc->GetNrRlcSapProvider ());
  m_enbRlc->SetNrRlcSapUser (m_enbPdcp->GetNrRlcSapUser ());

  m_enbRlc->SetNrMacSapProvider (m_enbMac->GetNrMacSapProvider ());
  m_enbMac->SetNrMacSapUser (m_enbRlc->GetNrMacSapUser ());

  return enbDev;
}

Ptr<NetDevice>
NrSimpleHelper::InstallSingleUeDevice (Ptr<Node> n)
{
  NS_LOG_FUNCTION (this);

  m_ueRrc = CreateObject<NrTestRrc> ();
  m_uePdcp = CreateObject<NrPdcp> ();

  if (m_nrRlcEntityType == RLC_UM)
    {
      m_ueRlc = CreateObject<NrRlcUm> ();
    }
  else // m_nrRlcEntityType == RLC_AM
    {
      m_ueRlc = CreateObject<NrRlcAm> ();
    }

  m_ueRlc->SetRnti (21);
  m_ueRlc->SetLcId (22);

  Ptr<NrSimpleNetDevice> ueDev = m_ueDeviceFactory.Create<NrSimpleNetDevice> ();
  ueDev->SetAddress (Mac48Address::Allocate ());
  ueDev->SetChannel (m_phyChannel);

  n->AddDevice (ueDev);

  m_ueMac = CreateObject<NrTestMac> ();
  m_ueMac->SetDevice (ueDev);

  ueDev->SetReceiveCallback (MakeCallback (&NrTestMac::Receive, m_ueMac));

  // Connect SAPs: RRC <-> PDCP <-> RLC <-> MAC

  m_ueRrc->SetNrPdcpSapProvider (m_uePdcp->GetNrPdcpSapProvider ());
  m_uePdcp->SetNrPdcpSapUser (m_ueRrc->GetNrPdcpSapUser ());

  m_uePdcp->SetNrRlcSapProvider (m_ueRlc->GetNrRlcSapProvider ());
  m_ueRlc->SetNrRlcSapUser (m_uePdcp->GetNrRlcSapUser ());

  m_ueRlc->SetNrMacSapProvider (m_ueMac->GetNrMacSapProvider ());
  m_ueMac->SetNrMacSapUser (m_ueRlc->GetNrMacSapUser ());

  return ueDev;
}


void
NrSimpleHelper::EnableLogComponents (void)
{
  LogLevel level = (LogLevel) (LOG_LEVEL_ALL | LOG_PREFIX_TIME | LOG_PREFIX_NODE | LOG_PREFIX_FUNC);

  LogComponentEnable ("Config", level);
  LogComponentEnable ("NrSimpleHelper", level);
  LogComponentEnable ("NrTestEntities", level);
  LogComponentEnable ("NrPdcp", level);
  LogComponentEnable ("NrRlc", level);
  LogComponentEnable ("NrRlcUm", level);
  LogComponentEnable ("NrRlcAm", level);
  LogComponentEnable ("NrSimpleNetDevice", level);
  LogComponentEnable ("SimpleNetDevice", level);
  LogComponentEnable ("SimpleChannel", level);
}

void
NrSimpleHelper::EnableTraces (void)
{
//   EnableMacTraces ();
  EnableRlcTraces ();
  EnablePdcpTraces ();
}

void
NrSimpleHelper::EnableRlcTraces (void)
{
  EnableDlRlcTraces ();
  EnableUlRlcTraces ();
}


void
NrSimpleHelperDlTxPduCallback (Ptr<NrRadioBearerStatsCalculator> rlcStats, std::string path,
                 uint16_t rnti, uint8_t lcid, uint32_t packetSize)
{
  NS_LOG_FUNCTION (rlcStats << path << rnti << (uint16_t)lcid << packetSize);
  uint64_t imsi = 111;
  uint16_t cellId = 222;
  rlcStats->DlTxPdu (cellId, imsi, rnti, lcid, packetSize);
}

void
NrSimpleHelperDlRxPduCallback (Ptr<NrRadioBearerStatsCalculator> rlcStats, std::string path,
                 uint16_t rnti, uint8_t lcid, uint32_t packetSize, uint64_t delay)
{
  NS_LOG_FUNCTION (rlcStats << path << rnti << (uint16_t)lcid << packetSize << delay);
  uint64_t imsi = 333;
  uint16_t cellId = 555;
  rlcStats->DlRxPdu (cellId, imsi, rnti, lcid, packetSize, delay);
}

void
NrSimpleHelper::EnableDlRlcTraces (void)
{
  NS_LOG_FUNCTION_NOARGS ();

                //   Config::Connect ("/NodeList/*/DeviceList/*/NrRlc/TxPDU",
                //                    MakeBoundCallback (&NrSimpleHelperDlTxPduCallback, m_rlcStats));
                //   Config::Connect ("/NodeList/*/DeviceList/*/NrRlc/RxPDU",
                //                    MakeBoundCallback (&NrSimpleHelperDlRxPduCallback, m_rlcStats));
}

void
NrSimpleHelperUlTxPduCallback (Ptr<NrRadioBearerStatsCalculator> rlcStats, std::string path,
                 uint16_t rnti, uint8_t lcid, uint32_t packetSize)
{
  NS_LOG_FUNCTION (rlcStats << path << rnti << (uint16_t)lcid << packetSize);
  uint64_t imsi = 1111;
  uint16_t cellId = 555;
  rlcStats->UlTxPdu (cellId, imsi, rnti, lcid, packetSize);
}

void
NrSimpleHelperUlRxPduCallback (Ptr<NrRadioBearerStatsCalculator> rlcStats, std::string path,
                 uint16_t rnti, uint8_t lcid, uint32_t packetSize, uint64_t delay)
{
  NS_LOG_FUNCTION (rlcStats << path << rnti << (uint16_t)lcid << packetSize << delay);
  uint64_t imsi = 444;
  uint16_t cellId = 555;
  rlcStats->UlRxPdu (cellId, imsi, rnti, lcid, packetSize, delay);
}


void
NrSimpleHelper::EnableUlRlcTraces (void)
{
  NS_LOG_FUNCTION_NOARGS ();

                  //   Config::Connect ("/NodeList/*/DeviceList/*/NrRlc/TxPDU",
                  //                    MakeBoundCallback (&NrSimpleHelperUlTxPduCallback, m_rlcStats));
                  //   Config::Connect ("/NodeList/*/DeviceList/*/NrRlc/RxPDU",
                  //                    MakeBoundCallback (&NrSimpleHelperUlRxPduCallback, m_rlcStats));
}


void
NrSimpleHelper::EnablePdcpTraces (void)
{
  EnableDlPdcpTraces ();
  EnableUlPdcpTraces ();
}

void
NrSimpleHelper::EnableDlPdcpTraces (void)
{
  NS_LOG_FUNCTION_NOARGS ();

                  //   Config::Connect ("/NodeList/*/DeviceList/*/NrPdcp/TxPDU",
                  //                    MakeBoundCallback (&NrSimpleHelperDlTxPduCallback, m_pdcpStats));
                  //   Config::Connect ("/NodeList/*/DeviceList/*/NrPdcp/RxPDU",
                  //                    MakeBoundCallback (&NrSimpleHelperDlRxPduCallback, m_pdcpStats));
}

void
NrSimpleHelper::EnableUlPdcpTraces (void)
{
  NS_LOG_FUNCTION_NOARGS ();

                  //   Config::Connect ("/NodeList/*/DeviceList/*/NrPdcp/TxPDU",
                  //                    MakeBoundCallback (&NrSimpleHelperUlTxPduCallback, m_pdcpStats));
                  //   Config::Connect ("/NodeList/*/DeviceList/*/NrPdcp/RxPDU",
                  //                    MakeBoundCallback (&NrSimpleHelperUlRxPduCallback, m_pdcpStats));
}


} // namespace ns3
