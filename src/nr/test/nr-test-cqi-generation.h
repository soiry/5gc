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

#ifndef NR_TEST_CQI_GENERATION_H
#define NR_TEST_CQI_GENERATION_H

#include "ns3/test.h"

using namespace ns3;

class NrCqiGenerationTestSuite : public TestSuite
{
public:
  NrCqiGenerationTestSuite ();
};

class NrCqiGenerationTestCase : public TestCase
{
public:
  NrCqiGenerationTestCase (std::string name, bool usePdcchForCqiGeneration,
                            uint16_t dlMcs, uint16_t ulMcs);
  virtual ~NrCqiGenerationTestCase ();

  void DlScheduling (uint32_t frameNo, uint32_t subframeNo, uint16_t rnti,
                     uint8_t mcsTb1, uint16_t sizeTb1, uint8_t mcsTb2, uint16_t sizeTb2);

  void UlScheduling (uint32_t frameNo, uint32_t subframeNo, uint16_t rnti,
                     uint8_t mcs, uint16_t sizeTb);

private:
  virtual void DoRun (void);

  bool m_usePdcchForCqiGeneration;
  uint16_t m_dlMcs;
  uint16_t m_ulMcs;

};

class NrCqiGenerationDlPowerControlTestCase : public TestCase
{
public:
  NrCqiGenerationDlPowerControlTestCase (std::string name, uint8_t cell0Pa, uint8_t cell1Pa,
                                          uint16_t dlMcs, uint16_t ulMcs);
  virtual ~NrCqiGenerationDlPowerControlTestCase ();

  void DlScheduling (uint32_t frameNo, uint32_t subframeNo, uint16_t rnti,
                     uint8_t mcsTb1, uint16_t sizeTb1, uint8_t mcsTb2, uint16_t sizeTb2);

  void UlScheduling (uint32_t frameNo, uint32_t subframeNo, uint16_t rnti,
                     uint8_t mcs, uint16_t sizeTb);

private:
  virtual void DoRun (void);

  uint8_t m_cell0Pa;
  uint8_t m_cell1Pa;

  uint16_t m_dlMcs;
  uint16_t m_ulMcs;

};

#endif /* NR_TEST_CQI_GENERATION_H */
