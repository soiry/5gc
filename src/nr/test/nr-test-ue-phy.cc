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

#include "ns3/log.h"
#include "nr-test-ue-phy.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("NrTestUePhy");
 
NS_OBJECT_ENSURE_REGISTERED (NrTestUePhy);

NrTestUePhy::NrTestUePhy ()
{
  NS_LOG_FUNCTION (this);
  NS_FATAL_ERROR ("This constructor should not be called");
}

NrTestUePhy::NrTestUePhy (Ptr<NrSpectrumPhy> dlPhy, Ptr<NrSpectrumPhy> ulPhy)
  : NrPhy (dlPhy, ulPhy)
{
  NS_LOG_FUNCTION (this);
}

NrTestUePhy::~NrTestUePhy ()
{
}

void
NrTestUePhy::DoDispose ()
{
  NS_LOG_FUNCTION (this);

  NrPhy::DoDispose ();
}

TypeId
NrTestUePhy::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::NrTestUePhy")
    .SetParent<NrPhy> ()
    .AddConstructor<NrTestUePhy> ()
  ;
  return tid;
}

void
NrTestUePhy::DoSendMacPdu (Ptr<Packet> p)
{
  NS_LOG_FUNCTION (this);
}

Ptr<SpectrumValue>
NrTestUePhy::CreateTxPowerSpectralDensity ()
{
  NS_LOG_FUNCTION (this);
  Ptr<SpectrumValue> psd;

  return psd;
}

void
NrTestUePhy::GenerateCtrlCqiReport (const SpectrumValue& sinr)
{
  NS_LOG_FUNCTION (this);

  // Store calculated SINR, it will be retrieved at the end of the test
  m_sinr = sinr;
}

void
NrTestUePhy::GenerateDataCqiReport (const SpectrumValue& sinr)
{
  NS_LOG_FUNCTION (this);
  
  // Store calculated SINR, it will be retrieved at the end of the test
  m_sinr = sinr;
}

void
NrTestUePhy::ReportRsReceivedPower (const SpectrumValue& power)
{
  NS_LOG_FUNCTION (this);
  // Not used by the NrTestUePhy
}

void
NrTestUePhy::ReportInterference (const SpectrumValue& interf)
{
  NS_LOG_FUNCTION (this);
  // Not used by the NrTestUePhy
}

void
NrTestUePhy::ReceiveNrControlMessage (Ptr<NrControlMessage> msg)
{
  NS_LOG_FUNCTION (this << msg);
}

SpectrumValue
NrTestUePhy::GetSinr ()
{
  NS_LOG_FUNCTION (this);

  return m_sinr;
}


} // namespace ns3
