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
 * Author: Manuel Requena <manuel.requena@cttc.es>
 */

#include "ns3/simulator.h"
#include "ns3/log.h"
#include "ns3/node.h"

#include "ns3/nr-rlc-header.h"
#include "ns3/nr-rlc-am-header.h"
#include "ns3/nr-pdcp-header.h"

#include "nr-test-entities.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("NrTestEntities");

/////////////////////////////////////////////////////////////////////

TypeId
NrTestRrc::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::NrTestRrc")
    .SetParent<Object> ()
    .AddConstructor<NrTestRrc> ()
    ;

  return tid;
}

NrTestRrc::NrTestRrc ()
{
  NS_LOG_FUNCTION (this);

  m_txPdus = 0;
  m_txBytes = 0;
  m_rxPdus = 0;
  m_rxBytes = 0;
  m_txLastTime = Time (0);
  m_rxLastTime = Time (0);;

  m_pdcpSapUser = new NrPdcpSpecificNrPdcpSapUser<NrTestRrc> (this);
//   Simulator::ScheduleNow (&NrTestRrc::Start, this);
}

NrTestRrc::~NrTestRrc ()
{
  NS_LOG_FUNCTION (this);
}

void
NrTestRrc::DoDispose ()
{
  NS_LOG_FUNCTION (this);
  delete m_pdcpSapUser;
}

void
NrTestRrc::SetDevice (Ptr<NetDevice> device)
{
  m_device = device;
}

void
NrTestRrc::SetNrPdcpSapProvider (NrPdcpSapProvider* s)
{
  m_pdcpSapProvider = s;
}

NrPdcpSapUser*
NrTestRrc::GetNrPdcpSapUser (void)
{
  return m_pdcpSapUser;
}


std::string
NrTestRrc::GetDataReceived (void)
{
  NS_LOG_FUNCTION (this);
  return m_receivedData;
}

// Stats
uint32_t
NrTestRrc::GetTxPdus (void)
{
  NS_LOG_FUNCTION (this << m_txPdus);
  return m_txPdus;
}

uint32_t
NrTestRrc::GetTxBytes (void)
{
  NS_LOG_FUNCTION (this << m_txBytes);
  return m_txBytes;
}

uint32_t
NrTestRrc::GetRxPdus (void)
{
  NS_LOG_FUNCTION (this << m_rxPdus);
  return m_rxPdus;
}

uint32_t
NrTestRrc::GetRxBytes (void)
{
  NS_LOG_FUNCTION (this << m_rxBytes);
  return m_rxBytes;
}

Time
NrTestRrc::GetTxLastTime (void)
{
  NS_LOG_FUNCTION (this << m_txLastTime);
  return m_txLastTime;
}

Time
NrTestRrc::GetRxLastTime (void)
{
  NS_LOG_FUNCTION (this << m_rxLastTime);
  return m_rxLastTime;
}


void
NrTestRrc::SetArrivalTime (Time arrivalTime)
{
  NS_LOG_FUNCTION (this << arrivalTime);
  m_arrivalTime = arrivalTime;
}

void
NrTestRrc::SetPduSize (uint32_t pduSize)
{
  NS_LOG_FUNCTION (this << pduSize);
  m_pduSize = pduSize;
}


/**
 * PDCP SAP
 */

void
NrTestRrc::DoReceivePdcpSdu (NrPdcpSapUser::ReceivePdcpSduParameters params)
{
  NS_LOG_FUNCTION (this << params.pdcpSdu->GetSize ());
  Ptr<Packet> p = params.pdcpSdu;
//   NS_LOG_LOGIC ("PDU received = " << (*p));

  uint32_t dataLen = p->GetSize ();
  uint8_t *buf = new uint8_t[dataLen];

  // Stats
  m_rxPdus++;
  m_rxBytes += dataLen;
  m_rxLastTime = Simulator::Now ();

  p->CopyData (buf, dataLen);
  m_receivedData = std::string ((char *)buf, dataLen);

//   NS_LOG_LOGIC (m_receivedData);

  delete [] buf;
}

/**
 * START
 */

void
NrTestRrc::Start ()
{
  NS_LOG_FUNCTION (this);
  NS_ASSERT_MSG (m_arrivalTime != Time (0), "Arrival time must be different from 0");

  // Stats
  m_txPdus++;
  m_txBytes += m_pduSize;
  m_txLastTime = Simulator::Now ();

  NrPdcpSapProvider::TransmitPdcpSduParameters p;
  p.rnti = 1111;
  p.lcid = 222;
  p.pdcpSdu = Create<Packet> (m_pduSize);
  
  bool haveContext = false;
  Ptr<Node> node;
  if (m_device != 0)
    {
      node = m_device->GetNode ();
      if (node != 0)
        {                    
          haveContext = true;
        }
    }
  if (haveContext)
    {
      Simulator::ScheduleWithContext (node->GetId (), Seconds (0), &NrPdcpSapProvider::TransmitPdcpSdu, m_pdcpSapProvider, p);
    }
  else
    {
      Simulator::Schedule (Seconds (0), &NrPdcpSapProvider::TransmitPdcpSdu, m_pdcpSapProvider, p);
    }

  m_nextPdu = Simulator::Schedule (m_arrivalTime, &NrTestRrc::Start, this);
//   Simulator::Run ();
}

void
NrTestRrc::Stop ()
{
  NS_LOG_FUNCTION (this);
  m_nextPdu.Cancel ();
}

void
NrTestRrc::SendData (Time at, std::string dataToSend)
{
  NS_LOG_FUNCTION (this << at << dataToSend.length () << dataToSend);

  // Stats
  m_txPdus++;
  m_txBytes += dataToSend.length ();

  NrPdcpSapProvider::TransmitPdcpSduParameters p;
  p.rnti = 1111;
  p.lcid = 222;

  NS_LOG_LOGIC ("Data(" << dataToSend.length () << ") = " << dataToSend.data ());
  p.pdcpSdu = Create<Packet> ((uint8_t *) dataToSend.data (), dataToSend.length ());

  NS_LOG_LOGIC ("Packet(" << p.pdcpSdu->GetSize () << ")");
  Simulator::Schedule (at, &NrPdcpSapProvider::TransmitPdcpSdu, m_pdcpSapProvider, p);
}

/////////////////////////////////////////////////////////////////////

TypeId
NrTestPdcp::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::NrTestPdcp")
    .SetParent<Object> ()
    .AddConstructor<NrTestPdcp> ()
    ;

  return tid;
}

NrTestPdcp::NrTestPdcp ()
{
  NS_LOG_FUNCTION (this);
  m_rlcSapUser = new NrRlcSpecificNrRlcSapUser<NrTestPdcp> (this);
  Simulator::ScheduleNow (&NrTestPdcp::Start, this);
}

NrTestPdcp::~NrTestPdcp ()
{
  NS_LOG_FUNCTION (this);
}

void
NrTestPdcp::DoDispose ()
{
  NS_LOG_FUNCTION (this);
  delete m_rlcSapUser;
}

void
NrTestPdcp::SetNrRlcSapProvider (NrRlcSapProvider* s)
{
  m_rlcSapProvider = s;
}

NrRlcSapUser*
NrTestPdcp::GetNrRlcSapUser (void)
{
  return m_rlcSapUser;
}


std::string
NrTestPdcp::GetDataReceived (void)
{
  NS_LOG_FUNCTION (this);

  return m_receivedData;
}


/**
 * RLC SAP
 */

void
NrTestPdcp::DoReceivePdcpPdu (Ptr<Packet> p)
{
  NS_LOG_FUNCTION (this << p->GetSize ());
  NS_LOG_LOGIC ("Data = " << (*p));

  uint32_t dataLen = p->GetSize ();
  uint8_t *buf = new uint8_t[dataLen];
  p->CopyData (buf, dataLen);
  m_receivedData = std::string ((char *)buf, dataLen);

  NS_LOG_LOGIC (m_receivedData);

  delete [] buf;
}

/**
 * START
 */

void
NrTestPdcp::Start ()
{
  NS_LOG_FUNCTION (this);
}

void
NrTestPdcp::SendData (Time time, std::string dataToSend)
{
  NS_LOG_FUNCTION (this << time << dataToSend.length () << dataToSend);

  NrRlcSapProvider::TransmitPdcpPduParameters p;
  p.rnti = 1111;
  p.lcid = 222;

  NS_LOG_LOGIC ("Data(" << dataToSend.length () << ") = " << dataToSend.data ());
  p.pdcpPdu = Create<Packet> ((uint8_t *) dataToSend.data (), dataToSend.length ());

  NS_LOG_LOGIC ("Packet(" << p.pdcpPdu->GetSize () << ")");
  Simulator::Schedule (time, &NrRlcSapProvider::TransmitPdcpPdu, m_rlcSapProvider, p);
}

/////////////////////////////////////////////////////////////////////

TypeId
NrTestMac::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::NrTestMac")
    .SetParent<Object> ()
    .AddConstructor<NrTestMac> ()
    ;

  return tid;
}

NrTestMac::NrTestMac ()
{
  NS_LOG_FUNCTION (this);
  m_device = 0;
  m_macSapProvider = new EnbMacMemberNrMacSapProvider<NrTestMac> (this);
  m_macSapUser = 0;
  m_macLoopback = 0;
  m_pdcpHeaderPresent = false;
  m_rlcHeaderType = UM_RLC_HEADER;
  m_txOpportunityMode = MANUAL_MODE;
  m_txOppTime = Seconds (0.001);
  m_txOppSize = 0;

  m_txPdus = 0;
  m_txBytes = 0;
  m_rxPdus = 0;
  m_rxBytes = 0;

//   m_cmacSapProvider = new EnbMacMemberNrEnbCmacSapProvider (this);
//   m_schedSapUser = new EnbMacMemberFfMacSchedSapUser (this);
//   m_cschedSapUser = new EnbMacMemberFfMacCschedSapUser (this);
//   m_enbPhySapUser = new EnbMacMemberNrEnbPhySapUser (this);
}

NrTestMac::~NrTestMac ()
{
  NS_LOG_FUNCTION (this);
}

void
NrTestMac::DoDispose ()
{
  NS_LOG_FUNCTION (this);
  delete m_macSapProvider;
//   delete m_cmacSapProvider;
//   delete m_schedSapUser;
//   delete m_cschedSapUser;
//   delete m_enbPhySapUser;

  m_device = 0;
}

void
NrTestMac::SetDevice (Ptr<NetDevice> device)
{
  m_device = device;
}

void
NrTestMac::SetNrMacSapUser (NrMacSapUser* s)
{
  m_macSapUser = s;
}

NrMacSapProvider*
NrTestMac::GetNrMacSapProvider (void)
{
  return m_macSapProvider;
}

void
NrTestMac::SetNrMacLoopback (Ptr<NrTestMac> s)
{
  m_macLoopback = s;
}

std::string
NrTestMac::GetDataReceived (void)
{
  NS_LOG_FUNCTION (this);
  return m_receivedData;
}

// Stats
uint32_t
NrTestMac::GetTxPdus (void)
{
  NS_LOG_FUNCTION (this << m_txPdus);
  return m_txPdus;
}

uint32_t
NrTestMac::GetTxBytes (void)
{
  NS_LOG_FUNCTION (this << m_txBytes);
  return m_txBytes;
}

uint32_t
NrTestMac::GetRxPdus (void)
{
  NS_LOG_FUNCTION (this << m_rxPdus);
  return m_rxPdus;
}

uint32_t
NrTestMac::GetRxBytes (void)
{
  NS_LOG_FUNCTION (this << m_rxBytes);
  return m_rxBytes;
}


void
NrTestMac::SendTxOpportunity (Time time, uint32_t bytes)
{
  NS_LOG_FUNCTION (this << time << bytes);
  bool haveContext = false;
  Ptr<Node> node;
  if (m_device != 0)
    {
      node = m_device->GetNode ();
      if (node != 0)
        {                    
          haveContext = true;
        }
    }
  if (haveContext)
    {
      Simulator::ScheduleWithContext (node->GetId (), time, &NrMacSapUser::NotifyTxOpportunity, m_macSapUser, bytes, 0, 0);
    }
  else
    {
      Simulator::Schedule (time, &NrMacSapUser::NotifyTxOpportunity, m_macSapUser, bytes, 0, 0);
    }
    
  if (m_txOpportunityMode == RANDOM_MODE)
  {
    if (m_txOppTime != Seconds (0))
    {
      Simulator::Schedule (m_txOppTime, &NrTestMac::SendTxOpportunity, this, m_txOppTime, m_txOppSize);
    }
  }
}

void
NrTestMac::SetPdcpHeaderPresent (bool present)
{
  NS_LOG_FUNCTION (this << present);
  m_pdcpHeaderPresent = present;
}

void
NrTestMac::SetRlcHeaderType (uint8_t rlcHeaderType)
{
  NS_LOG_FUNCTION (this << rlcHeaderType);
  m_rlcHeaderType = rlcHeaderType;
}

void
NrTestMac::SetTxOpportunityMode (uint8_t mode)
{
  NS_LOG_FUNCTION (this << (uint32_t)mode);
  m_txOpportunityMode = mode;

  if (m_txOpportunityMode == RANDOM_MODE)
    {
      if (m_txOppTime != Seconds (0.0))
        {
          SendTxOpportunity (m_txOppTime, m_txOppSize);
        }
    }
}

void
NrTestMac::SetTxOppTime (Time txOppTime)
{
  NS_LOG_FUNCTION (this << txOppTime);
  m_txOppTime = txOppTime;
}

void
NrTestMac::SetTxOppSize (uint32_t txOppSize)
{
  NS_LOG_FUNCTION (this << txOppSize);
  m_txOppSize = txOppSize;
}


/**
 * MAC SAP
 */

void
NrTestMac::DoTransmitPdu (NrMacSapProvider::TransmitPduParameters params)
{
  NS_LOG_FUNCTION (this << params.pdu->GetSize ());

  m_txPdus++;
  m_txBytes += params.pdu->GetSize ();

  if (m_device)
    {
      m_device->Send (params.pdu, m_device->GetBroadcast (), 0);
    }
  else if (m_macLoopback)
    {
      Simulator::Schedule (Seconds (0.1), &NrMacSapUser::ReceivePdu,
                           m_macLoopback->m_macSapUser, params.pdu);
    }
  else
    {
      NrPdcpHeader pdcpHeader;

      if (m_rlcHeaderType == AM_RLC_HEADER)
        {
          // Remove AM RLC header
          NrRlcAmHeader rlcAmHeader;
          params.pdu->RemoveHeader (rlcAmHeader);
          NS_LOG_LOGIC ("AM RLC header: " << rlcAmHeader);
        }
      else // if (m_rlcHeaderType == UM_RLC_HEADER)
        {
          // Remove UM RLC header
          NrRlcHeader rlcHeader;
          params.pdu->RemoveHeader (rlcHeader);
          NS_LOG_LOGIC ("UM RLC header: " << rlcHeader);
        }

      // Remove PDCP header, if present
      if (m_pdcpHeaderPresent)
        {
          params.pdu->RemoveHeader (pdcpHeader);
          NS_LOG_LOGIC ("PDCP header: " << pdcpHeader);
        }

      // Copy data to a string
      uint32_t dataLen = params.pdu->GetSize ();
      uint8_t *buf = new uint8_t[dataLen];
      params.pdu->CopyData (buf, dataLen);
      m_receivedData = std::string ((char *)buf, dataLen);

      NS_LOG_LOGIC ("Data (" << dataLen << ") = " << m_receivedData);
      delete [] buf;
    }
}

void
NrTestMac::DoReportBufferStatus (NrMacSapProvider::ReportBufferStatusParameters params)
{
  NS_LOG_FUNCTION (this << params.txQueueSize << params.retxQueueSize << params.statusPduSize);

  if (m_txOpportunityMode == AUTOMATIC_MODE)
    {
      // cancel all previously scheduled TxOpps
      for (std::list<EventId>::iterator it = m_nextTxOppList.begin ();
           it != m_nextTxOppList.end ();
           ++it)
        {          
          it->Cancel ();
        }
      m_nextTxOppList.clear ();

      int32_t size = params.statusPduSize + params.txQueueSize  + params.retxQueueSize;
      Time time = m_txOppTime;
      while (size > 0)
        {
          EventId e = Simulator::Schedule (time, 
                                           &NrMacSapUser::NotifyTxOpportunity,
                                           m_macSapUser, m_txOppSize, 0, 0);
          m_nextTxOppList.push_back (e);
          size -= m_txOppSize;
          time += m_txOppTime;
        }
    }
}


bool
NrTestMac::Receive (Ptr<NetDevice> nd, Ptr<const Packet> p, uint16_t protocol, const Address& addr)
{
  NS_LOG_FUNCTION (this << addr << protocol << p->GetSize ());

  m_rxPdus++;
  m_rxBytes += p->GetSize ();

  Ptr<Packet> packet = p->Copy ();
  m_macSapUser->ReceivePdu (packet);
  return true;
}








NS_OBJECT_ENSURE_REGISTERED (NgcTestRrc);

NgcTestRrc::NgcTestRrc ()
  : m_n2SapProvider (0)
{
  NS_LOG_FUNCTION (this);
  m_n2SapUser = new MemberNgcEnbN2SapUser<NgcTestRrc> (this);
}


NgcTestRrc::~NgcTestRrc ()
{
  NS_LOG_FUNCTION (this);
}


void
NgcTestRrc::DoDispose ()
{
  NS_LOG_FUNCTION (this);
  delete m_n2SapUser;
}

TypeId
NgcTestRrc::GetTypeId (void)
{
  NS_LOG_FUNCTION ("NgcTestRrc::GetTypeId");
  static TypeId tid = TypeId ("ns3::NgcTestRrc")
    .SetParent<Object> ()
    .AddConstructor<NgcTestRrc> ()
  ;
  return tid;
}
void 
NgcTestRrc::SetN2SapProvider (NgcEnbN2SapProvider * s)
{
  m_n2SapProvider = s;
}

  
NgcEnbN2SapUser* 
NgcTestRrc::GetN2SapUser ()
{
  return m_n2SapUser;
}

void 
NgcTestRrc::DoDataRadioBearerSetupRequest (NgcEnbN2SapUser::DataRadioBearerSetupRequestParameters request)
{

}
  
void 
NgcTestRrc::DoPathSwitchRequestAcknowledge (NgcEnbN2SapUser::PathSwitchRequestAcknowledgeParameters params)
{

}


} // namespace ns3

