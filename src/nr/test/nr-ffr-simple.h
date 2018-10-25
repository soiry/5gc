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

#ifndef NR_FFR_SIMPLE_H
#define NR_FFR_SIMPLE_H

#include <ns3/nr-ffr-algorithm.h>
#include <ns3/nr-ffr-sap.h>
#include <ns3/nr-ffr-rrc-sap.h>
#include <ns3/nr-rrc-sap.h>

#include <ns3/traced-callback.h>
#include <map>

namespace ns3 {


/**
 * \brief Simple Frequency Reuse algorithm implementation which uses only 1 sub-band.
 *                Used to test Downlink Power Allocation. When Simple FR receives UE measurements
 *                it immediately call functions to change PdschConfigDedicated (i.e. P_A) value for
 *                this UE.
 */
class NrFfrSimple : public NrFfrAlgorithm
{
public:
  /**
   * \brief Creates a trivial ffr algorithm instance.
   */
  NrFfrSimple ();

  virtual ~NrFfrSimple ();

  // inherited from Object
  static TypeId GetTypeId ();

  void ChangePdschConfigDedicated (bool change);
  void SetPdschConfigDedicated (NrRrcSap::PdschConfigDedicated pdschConfigDedicated);

  void SetTpc (uint32_t tpc, uint32_t num, bool acculumatedMode);

  // inherited from NrFfrAlgorithm
  virtual void SetNrFfrSapUser (NrFfrSapUser* s);
  virtual NrFfrSapProvider* GetNrFfrSapProvider ();

  virtual void SetNrFfrRrcSapUser (NrFfrRrcSapUser* s);
  virtual NrFfrRrcSapProvider* GetNrFfrRrcSapProvider ();

  // let the forwarder class access the protected and private members
  friend class MemberNrFfrSapProvider<NrFfrSimple>;
  friend class MemberNrFfrRrcSapProvider<NrFfrSimple>;

  /**
   * TracedCallback signature for change of PdschConfigDedicated.
   *
   * \param [in] rnti
   * \param [in] pdschPa PdschConfiDedicated.pa
   */
  typedef void (* PdschTracedCallback)(uint16_t rnti, uint8_t pdschPa);

protected:
  // inherited from Object
  virtual void DoInitialize ();
  virtual void DoDispose ();

  virtual void Reconfigure ();

  // FFR SAP PROVIDER IMPLEMENTATION
  virtual std::vector <bool> DoGetAvailableDlRbg ();
  virtual bool DoIsDlRbgAvailableForUe (int i, uint16_t rnti);
  virtual std::vector <bool> DoGetAvailableUlRbg ();
  virtual bool DoIsUlRbgAvailableForUe (int i, uint16_t rnti);
  virtual void DoReportDlCqiInfo (const struct FfMacSchedSapProvider::SchedDlCqiInfoReqParameters& params);
  virtual void DoReportUlCqiInfo (const struct FfMacSchedSapProvider::SchedUlCqiInfoReqParameters& params);
  virtual void DoReportUlCqiInfo ( std::map <uint16_t, std::vector <double> > ulCqiMap );
  virtual uint8_t DoGetTpc (uint16_t rnti);
  virtual uint8_t DoGetMinContinuousUlBandwidth ();

  // FFR SAP RRC PROVIDER IMPLEMENTATION
  virtual void DoReportUeMeas (uint16_t rnti, NrRrcSap::MeasResults measResults);
  virtual void DoRecvLoadInformation (NgcX2Sap::LoadInformationParams params);

private:

  void UpdatePdschConfigDedicated ();

  // FFR SAP
  NrFfrSapUser* m_ffrSapUser;
  NrFfrSapProvider* m_ffrSapProvider;

  // FFR RRF SAP
  NrFfrRrcSapUser* m_ffrRrcSapUser;
  NrFfrRrcSapProvider* m_ffrRrcSapProvider;

  uint8_t m_dlOffset;
  uint8_t m_dlSubBand;

  uint8_t m_ulOffset;
  uint8_t m_ulSubBand;

  std::vector <bool> m_dlRbgMap;
  std::vector <bool> m_ulRbgMap;

  std::map <uint16_t, NrRrcSap::PdschConfigDedicated> m_ues;

  // The expected measurement identity
  uint8_t m_measId;

  bool m_changePdschConfigDedicated;

  NrRrcSap::PdschConfigDedicated m_pdschConfigDedicated;

  TracedCallback<uint16_t, uint8_t> m_changePdschConfigDedicatedTrace;


  //Uplink Power Control
  uint32_t m_tpc;
  uint32_t m_tpcNum;
  bool m_acculumatedMode;

}; // end of class NrFfrSimple


} // end of namespace ns3


#endif /* NR_FFR_SIMPLE_H */
