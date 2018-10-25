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

#ifndef NR_TEST_RLC_UM_TRANSMITTER_H
#define NR_TEST_RLC_UM_TRANSMITTER_H

#include "ns3/test.h"

namespace ns3 {

class NrTestRrc;
class NrTestMac;
class NrTestPdcp;

}

using namespace ns3;

/**
 * TestSuite 4.1.1 RLC UM: Only transmitter
 */
class NrRlcUmTransmitterTestSuite : public TestSuite
{
  public:
    NrRlcUmTransmitterTestSuite ();
};

class NrRlcUmTransmitterTestCase : public TestCase
{
  public:
    NrRlcUmTransmitterTestCase (std::string name);
    NrRlcUmTransmitterTestCase ();
    virtual ~NrRlcUmTransmitterTestCase ();

    void CheckDataReceived (Time time, std::string shouldReceived, std::string assertMsg);

  protected:
    virtual void DoRun (void);

    Ptr<NrTestPdcp> txPdcp;
    Ptr<NrRlc> txRlc;
    Ptr<NrTestMac> txMac;

  private:
    void DoCheckDataReceived (std::string shouldReceived, std::string assertMsg);

};

/**
 * Test 4.1.1.1 One SDU, One PDU
 */
class NrRlcUmTransmitterOneSduTestCase : public NrRlcUmTransmitterTestCase
{
  public:
    NrRlcUmTransmitterOneSduTestCase (std::string name);
    NrRlcUmTransmitterOneSduTestCase ();
    virtual ~NrRlcUmTransmitterOneSduTestCase ();

  private:
    virtual void DoRun (void);

};

/**
 * Test 4.1.1.2 Segmentation (One SDU => n PDUs)
 */
class NrRlcUmTransmitterSegmentationTestCase : public NrRlcUmTransmitterTestCase
{
  public:
    NrRlcUmTransmitterSegmentationTestCase (std::string name);
    NrRlcUmTransmitterSegmentationTestCase ();
    virtual ~NrRlcUmTransmitterSegmentationTestCase ();

  private:
    virtual void DoRun (void);

};

/**
 * Test 4.1.1.3 Concatenation (n SDUs => One PDU)
 */
class NrRlcUmTransmitterConcatenationTestCase : public NrRlcUmTransmitterTestCase
{
  public:
    NrRlcUmTransmitterConcatenationTestCase (std::string name);
    NrRlcUmTransmitterConcatenationTestCase ();
    virtual ~NrRlcUmTransmitterConcatenationTestCase ();

  private:
    virtual void DoRun (void);

};

/**
 * Test 4.1.1.4 Report Buffer Status (test primitive parameters)
 */
class NrRlcUmTransmitterReportBufferStatusTestCase : public NrRlcUmTransmitterTestCase
{
  public:
    NrRlcUmTransmitterReportBufferStatusTestCase (std::string name);
    NrRlcUmTransmitterReportBufferStatusTestCase ();
    virtual ~NrRlcUmTransmitterReportBufferStatusTestCase ();

  private:
    virtual void DoRun (void);

};

#endif /* NR_TEST_RLC_UM_TRANSMITTER_H */
