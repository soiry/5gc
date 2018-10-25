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
 * Author: Nicola Baldo <nbaldo@cttc.es>
 */



#include "ns3/test.h"
#include "ns3/log.h"

#include "ns3/nr-spectrum-value-helper.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("NrTestEarfcn");

class NrEarfcnTestCase : public TestCase
{
public:
  NrEarfcnTestCase (const char* str, uint16_t earfcn, double f);
  virtual ~NrEarfcnTestCase ();

protected:
  uint16_t m_earfcn;
  double m_f;

private:
  virtual void DoRun (void);
};

NrEarfcnTestCase::NrEarfcnTestCase (const char* str, uint16_t earfcn, double f)
  :   TestCase (str),
    m_earfcn (earfcn),
    m_f (f)
{
  NS_LOG_FUNCTION (this << str << earfcn << f);
}

NrEarfcnTestCase::~NrEarfcnTestCase ()
{
}

void 
NrEarfcnTestCase::DoRun (void)
{
  double f = NrSpectrumValueHelper::GetCarrierFrequency (m_earfcn);
  NS_TEST_ASSERT_MSG_EQ_TOL (f, m_f, 0.0000001, "wrong frequency");
}

class NrEarfcnDlTestCase : public NrEarfcnTestCase
{
public:
  NrEarfcnDlTestCase (const char* str, uint16_t earfcn, double f);

private:
  virtual void DoRun (void);
};

NrEarfcnDlTestCase::NrEarfcnDlTestCase (const char* str, uint16_t earfcn, double f)
  : NrEarfcnTestCase (str, earfcn, f)
{
}

void 
NrEarfcnDlTestCase::DoRun (void)
{
//   LogLevel logLevel = (LogLevel)(LOG_PREFIX_FUNC | LOG_PREFIX_TIME | LOG_LEVEL_ALL);
//   LogComponentEnable ("NrSpectrumValueHelper", logLevel);
//   LogComponentEnable ("NrTestEarfcn", logLevel);

  double f = NrSpectrumValueHelper::GetDownlinkCarrierFrequency (m_earfcn);
  NS_TEST_ASSERT_MSG_EQ_TOL (f, m_f, 0.0000001, "wrong frequency");
}


class NrEarfcnUlTestCase : public NrEarfcnTestCase
{
public:
  NrEarfcnUlTestCase (const char* str, uint16_t earfcn, double f);

private:
  virtual void DoRun (void);
};

NrEarfcnUlTestCase::NrEarfcnUlTestCase (const char* str, uint16_t earfcn, double f)
  : NrEarfcnTestCase (str, earfcn, f)
{
}

void 
NrEarfcnUlTestCase::DoRun (void)
{
  double f = NrSpectrumValueHelper::GetUplinkCarrierFrequency (m_earfcn);
  NS_TEST_ASSERT_MSG_EQ_TOL (f, m_f, 0.0000001, "wrong frequency");
}


/**
 * Test the calculation of carrier frequency based on EARFCN
 */
class NrEarfcnTestSuite : public TestSuite
{
public:
  NrEarfcnTestSuite ();
};

static NrEarfcnTestSuite g_nrEarfcnTestSuite;

NrEarfcnTestSuite::NrEarfcnTestSuite ()
  : TestSuite ("nr-earfcn", UNIT)
{
  NS_LOG_FUNCTION (this);

  AddTestCase (new NrEarfcnDlTestCase ("DL EARFCN=500", 500, 2160e6), TestCase::QUICK);
  AddTestCase (new NrEarfcnDlTestCase ("DL EARFCN=1000", 1000, 1970e6), TestCase::QUICK);
  AddTestCase (new NrEarfcnDlTestCase ("DL EARFCN=1301", 1301, 1815.1e6), TestCase::QUICK);
  AddTestCase (new NrEarfcnDlTestCase ("DL EARFCN=7000", 7000, 0.0), TestCase::QUICK);
  AddTestCase (new NrEarfcnDlTestCase ("DL EARFCN=20000", 20000, 0.0), TestCase::QUICK);
  AddTestCase (new NrEarfcnDlTestCase ("DL EARFCN=50000", 50000, 0.0), TestCase::QUICK);

  AddTestCase (new NrEarfcnUlTestCase ("UL EARFCN=18100", 18100, 1930e6), TestCase::QUICK);
  AddTestCase (new NrEarfcnUlTestCase ("UL EARFCN=19000", 19000, 1890e6), TestCase::QUICK);
  AddTestCase (new NrEarfcnUlTestCase ("UL EARFCN=19400", 19400, 1730e6), TestCase::QUICK);
  AddTestCase (new NrEarfcnUlTestCase ("UL EARFCN=10", 10, 0.0), TestCase::QUICK);
  AddTestCase (new NrEarfcnUlTestCase ("UL EARFCN=1000", 1000, 0.0), TestCase::QUICK);
  AddTestCase (new NrEarfcnUlTestCase ("UL EARFCN=50000", 50000, 0.0), TestCase::QUICK);

  AddTestCase (new NrEarfcnTestCase ("EARFCN=500", 500, 2160e6), TestCase::QUICK);
  AddTestCase (new NrEarfcnTestCase ("EARFCN=1000", 1000, 1970e6), TestCase::QUICK);
  AddTestCase (new NrEarfcnTestCase ("EARFCN=1301", 1301, 1815.1e6), TestCase::QUICK);
  AddTestCase (new NrEarfcnTestCase ("EARFCN=8000", 8000, 0.0), TestCase::QUICK);
  AddTestCase (new NrEarfcnTestCase ("EARFCN=50000", 50000, 0.0), TestCase::QUICK);
  AddTestCase (new NrEarfcnTestCase ("EARFCN=18100", 18100, 1930e6), TestCase::QUICK);
  AddTestCase (new NrEarfcnTestCase ("EARFCN=19000", 19000, 1890e6), TestCase::QUICK);
  AddTestCase (new NrEarfcnTestCase ("EARFCN=19400", 19400, 1730e6), TestCase::QUICK);
  AddTestCase (new NrEarfcnTestCase ("EARFCN=50000", 50000, 0.0), TestCase::QUICK);
}
