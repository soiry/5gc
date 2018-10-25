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

#include "nr-ffr-simple.h"
#include <ns3/log.h>
#include "ns3/nr-rrc-sap.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("NrFfrSimple");

NS_OBJECT_ENSURE_REGISTERED (NrFfrSimple);


NrFfrSimple::NrFfrSimple ()
  : m_ffrSapUser (0),
    m_ffrRrcSapUser (0),
    m_dlOffset (0),
    m_dlSubBand (0),
    m_ulOffset (0),
    m_ulSubBand (0),
    m_measId (0),
    m_changePdschConfigDedicated (false),
    m_tpc (1),
    m_tpcNum (0),
    m_acculumatedMode (false)
{
  NS_LOG_FUNCTION (this);
  m_ffrSapProvider = new MemberNrFfrSapProvider<NrFfrSimple> (this);
  m_ffrRrcSapProvider = new MemberNrFfrRrcSapProvider<NrFfrSimple> (this);
}


NrFfrSimple::~NrFfrSimple ()
{
  NS_LOG_FUNCTION (this);
}


void
NrFfrSimple::DoDispose ()
{
  NS_LOG_FUNCTION (this);
  delete m_ffrSapProvider;
  delete m_ffrRrcSapProvider;
}


TypeId
NrFfrSimple::GetTypeId ()
{
  static TypeId tid = TypeId ("ns3::NrFfrSimple")
    .SetParent<NrFfrAlgorithm> ()
    .AddConstructor<NrFfrSimple> ()
    .AddAttribute ("UlSubBandOffset",
                   "Uplink Offset in number of Resource Block Groups",
                   UintegerValue (0),
                   MakeUintegerAccessor (&NrFfrSimple::m_ulOffset),
                   MakeUintegerChecker<uint8_t> ())
    .AddAttribute ("UlSubBandwidth",
                   "Uplink Transmission SubBandwidth Configuration in number of Resource Block Groups",
                   UintegerValue (25),
                   MakeUintegerAccessor (&NrFfrSimple::m_ulSubBand),
                   MakeUintegerChecker<uint8_t> ())
    .AddAttribute ("DlSubBandOffset",
                   "Downlink Offset in number of Resource Block Groups",
                   UintegerValue (0),
                   MakeUintegerAccessor (&NrFfrSimple::m_dlOffset),
                   MakeUintegerChecker<uint8_t> ())
    .AddAttribute ("DlSubBandwidth",
                   "Downlink Transmission SubBandwidth Configuration in number of Resource Block Groups",
                   UintegerValue (12),
                   MakeUintegerAccessor (&NrFfrSimple::m_dlSubBand),
                   MakeUintegerChecker<uint8_t> ())
    .AddTraceSource ("ChangePdschConfigDedicated",
                     "trace fired upon change of PdschConfigDedicated",
                     MakeTraceSourceAccessor (&NrFfrSimple::m_changePdschConfigDedicatedTrace),
                     "ns3::NrFfrSimple::PdschTracedCallback")
  ;
  return tid;
}


void
NrFfrSimple::SetNrFfrSapUser (NrFfrSapUser* s)
{
  NS_LOG_FUNCTION (this << s);
  m_ffrSapUser = s;
}


NrFfrSapProvider*
NrFfrSimple::GetNrFfrSapProvider ()
{
  NS_LOG_FUNCTION (this);
  return m_ffrSapProvider;
}

void
NrFfrSimple::SetNrFfrRrcSapUser (NrFfrRrcSapUser* s)
{
  NS_LOG_FUNCTION (this << s);
  m_ffrRrcSapUser = s;
}


NrFfrRrcSapProvider*
NrFfrSimple::GetNrFfrRrcSapProvider ()
{
  NS_LOG_FUNCTION (this);
  return m_ffrRrcSapProvider;
}


void
NrFfrSimple::DoInitialize ()
{
  NS_LOG_FUNCTION (this);
  NrFfrAlgorithm::DoInitialize ();

  NS_LOG_LOGIC (this << " requesting Event A4 measurements"
                     << " (threshold = 0" << ")");
  NrRrcSap::ReportConfigEutra reportConfig;
  reportConfig.eventId = NrRrcSap::ReportConfigEutra::EVENT_A1;
  reportConfig.threshold1.choice = NrRrcSap::ThresholdEutra::THRESHOLD_RSRQ;
  reportConfig.threshold1.range = 0;
  reportConfig.triggerQuantity = NrRrcSap::ReportConfigEutra::RSRQ;
  reportConfig.reportInterval = NrRrcSap::ReportConfigEutra::MS120;
  m_measId = m_ffrRrcSapUser->AddUeMeasReportConfigForFfr (reportConfig);

  m_pdschConfigDedicated.pa = NrRrcSap::PdschConfigDedicated::dB0;
}

void
NrFfrSimple::Reconfigure ()
{
  NS_LOG_FUNCTION (this);
}

void
NrFfrSimple::ChangePdschConfigDedicated (bool change)
{
  m_changePdschConfigDedicated = change;
}

void
NrFfrSimple::SetPdschConfigDedicated (NrRrcSap::PdschConfigDedicated pdschConfigDedicated)
{
  m_pdschConfigDedicated = pdschConfigDedicated;
}

void
NrFfrSimple::SetTpc (uint32_t tpc, uint32_t num, bool acculumatedMode)
{
  m_tpc = tpc;
  m_tpcNum = num;
  m_acculumatedMode = acculumatedMode;
}

std::vector <bool>
NrFfrSimple::DoGetAvailableDlRbg ()
{
  NS_LOG_FUNCTION (this);

  if (m_dlRbgMap.empty ())
    {
      int rbgSize = GetRbgSize (m_dlBandwidth);
      m_dlRbgMap.resize (m_dlBandwidth / rbgSize, true);

      for (uint8_t i = m_dlOffset; i < (m_dlOffset + m_dlSubBand); i++)
        {
          m_dlRbgMap[i] = false;

        }
    }

  return m_dlRbgMap;
}

bool
NrFfrSimple::DoIsDlRbgAvailableForUe (int i, uint16_t rnti)
{
  NS_LOG_FUNCTION (this);
  return true;
}

std::vector <bool>
NrFfrSimple::DoGetAvailableUlRbg ()
{
  NS_LOG_FUNCTION (this);

  if (m_ulRbgMap.empty ())
    {
      m_ulRbgMap.resize (m_ulBandwidth, true);

      for (uint8_t i = m_ulOffset; i < (m_ulOffset + m_ulSubBand); i++)
        {
          m_ulRbgMap[i] = false;
        }
    }

  return m_ulRbgMap;
}

bool
NrFfrSimple::DoIsUlRbgAvailableForUe (int i, uint16_t rnti)
{
  NS_LOG_FUNCTION (this);
  return true;
}

void
NrFfrSimple::DoReportDlCqiInfo (const struct FfMacSchedSapProvider::SchedDlCqiInfoReqParameters& params)
{
  NS_LOG_FUNCTION (this);
}

void
NrFfrSimple::DoReportUlCqiInfo (const struct FfMacSchedSapProvider::SchedUlCqiInfoReqParameters& params)
{
  NS_LOG_FUNCTION (this);
}

void
NrFfrSimple::DoReportUlCqiInfo (std::map <uint16_t, std::vector <double> > ulCqiMap)
{
  NS_LOG_FUNCTION (this);
}

uint8_t
NrFfrSimple::DoGetTpc (uint16_t rnti)
{
  NS_LOG_FUNCTION (this);

  if (m_acculumatedMode)
    {
      if (m_tpcNum > 0)
        {
          m_tpcNum--;
          return m_tpc;
        }
      else
        {
          return 1;
        }
    }
  else
    {
      return m_tpc;
    }

  return 1; // 1 is mapped to 0 for Accumulated mode, and to -1 in Absolute mode TS36.213 Table 5.1.1.1-2
}

uint8_t
NrFfrSimple::DoGetMinContinuousUlBandwidth ()
{
  NS_LOG_FUNCTION (this);
  return m_ulBandwidth;
}

void
NrFfrSimple::DoReportUeMeas (uint16_t rnti,
                              NrRrcSap::MeasResults measResults)
{
  NS_LOG_FUNCTION (this << rnti << (uint16_t) measResults.measId);

  std::map<uint16_t, NrRrcSap::PdschConfigDedicated>::iterator it;

  it = m_ues.find (rnti);

  if (it == m_ues.end ())
    {
      NrRrcSap::PdschConfigDedicated pdschConfigDedicated;
      pdschConfigDedicated.pa = NrRrcSap::PdschConfigDedicated::dB0;
      m_ues.insert (std::pair<uint16_t, NrRrcSap::PdschConfigDedicated> (rnti,
                                                                          pdschConfigDedicated));
    }

  if (m_changePdschConfigDedicated)
    {
      UpdatePdschConfigDedicated ();
    }
}

void
NrFfrSimple::UpdatePdschConfigDedicated ()
{
  NS_LOG_FUNCTION (this);

  std::map<uint16_t, NrRrcSap::PdschConfigDedicated>::iterator it;
  for (it = m_ues.begin (); it != m_ues.end (); it++)
    {
      if (it->second.pa != m_pdschConfigDedicated.pa)
        {
          m_changePdschConfigDedicatedTrace (it->first, m_pdschConfigDedicated.pa);
          NrRrcSap::PdschConfigDedicated pdschConfigDedicated = m_pdschConfigDedicated;
          m_ffrRrcSapUser->SetPdschConfigDedicated (it->first, pdschConfigDedicated );
        }
    }
}

void
NrFfrSimple::DoRecvLoadInformation (NgcX2Sap::LoadInformationParams params)
{
  NS_LOG_FUNCTION (this);
}

} // end of namespace ns3
