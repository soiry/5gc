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
 *         Nicola Baldo <nbaldo@cttc.es>
 *         Marco Miozzo <mmiozzo@cttc.es>
 *              adapt nr-test-interference.cc to nr-ue-measurements.cc
 *         Budiarto Herman <budiarto.herman@magister.fi>
 */

#ifndef NR_TEST_UE_MEASUREMENTS_H
#define NR_TEST_UE_MEASUREMENTS_H

#include <ns3/test.h>
#include <ns3/nr-rrc-sap.h>
#include <ns3/nstime.h>
#include <list>
#include <set>
#include <vector>

namespace ns3 {

class MobilityModel;

}

using namespace ns3;


// ===== NR-UE-MEASUREMENTS TEST SUITE ==================================== //


/**
 * Test that UE Measurements (see 36.214) calculation works fine in a
 * multi-cell interference scenario.
 */
class NrUeMeasurementsTestSuite : public TestSuite
{
public:
  NrUeMeasurementsTestSuite ();
};


class NrUeMeasurementsTestCase : public TestCase
{
public:
  NrUeMeasurementsTestCase (std::string name, double d1, double d2, double rsrpDbmUe1, double rsrpDbmUe2, double rsrqDbUe1, double rsrqDbUe2);
  virtual ~NrUeMeasurementsTestCase ();

  void ReportUeMeasurements (uint16_t rnti, uint16_t cellId, double rsrp, double rsrq, bool servingCell);

  void RecvMeasurementReport (uint64_t imsi, uint16_t cellId, uint16_t rnti, NrRrcSap::MeasurementReport meas);

private:
  virtual void DoRun (void);

  double m_d1;
  double m_d2;
  double m_rsrpDbmUeServingCell;
  double m_rsrpDbmUeNeighborCell;
  double m_rsrqDbUeServingCell;
  double m_rsrqDbUeNeighborCell;

};



// ===== NR-UE-MEASUREMENTS-PIECEWISE-1 TEST SUITE ======================== //


/**
 * \brief Test suite for generating calls to UE measurements test case
 *        ns3::NrUeMeasurementsPiecewiseTestCase1.
 */
class NrUeMeasurementsPiecewiseTestSuite1 : public TestSuite
{
public:
  NrUeMeasurementsPiecewiseTestSuite1 ();
};


/**
 * \brief Testing UE measurements in NR with simulation of 1 eNodeB and 1 UE in
 *        piecewise configuration and 120 ms report interval.
 */
class NrUeMeasurementsPiecewiseTestCase1 : public TestCase
{
public:
  NrUeMeasurementsPiecewiseTestCase1 (std::string name,
                                       NrRrcSap::ReportConfigEutra config,
                                       std::vector<Time> expectedTime,
                                       std::vector<uint8_t> expectedRsrp);

  virtual ~NrUeMeasurementsPiecewiseTestCase1 ();

  /**
   * \brief Triggers when eNodeB receives measurement report from UE, then
   *        perform verification on it.
   *
   * The trigger is set up beforehand by connecting to the
   * `NrUeRrc::RecvMeasurementReport` trace source.
   *
   * Verification consists of checking whether the report carries the right
   * value of RSRP or not, and whether it occurs at the expected time or not.
   */
  void RecvMeasurementReportCallback (std::string context, uint64_t imsi,
                                      uint16_t cellId, uint16_t rnti,
                                      NrRrcSap::MeasurementReport report);

private:
  /**
   * \brief Setup the simulation with the intended UE measurement reporting
   *        configuration, run it, and connect the
   *        `RecvMeasurementReportCallback` function to the
   *        `NrUeRrc::RecvMeasurementReport` trace source.
   */
  virtual void DoRun ();

  /**
   * \brief Runs at the end of the simulation, verifying that all expected
   *        measurement reports have been examined.
   */
  virtual void DoTeardown ();

  void TeleportVeryNear ();
  void TeleportNear ();
  void TeleportFar ();
  void TeleportVeryFar ();

  /**
   * \brief The active report triggering configuration.
   */
  NrRrcSap::ReportConfigEutra m_config;

  /**
   * \brief The list of expected time when measurement reports are received by
   *        eNodeB.
   */
  std::vector<Time> m_expectedTime;

  /**
   * \brief The list of expected values of RSRP (in 3GPP range unit) from the
   *        measurement reports received.
   */
  std::vector<uint8_t> m_expectedRsrp;

  /**
   * \brief Pointer to the element of `m_expectedTime` which is expected to
   *        occur next in the simulation.
   */
  std::vector<Time>::iterator m_itExpectedTime;

  /**
   * \brief Pointer to the element of `m_expectedRsrp` which is expected to
   *        occur next in the simulation.
   */
  std::vector<uint8_t>::iterator m_itExpectedRsrp;

  /**
   * \brief The measurement identity being tested. Measurement reports with
   *        different measurement identity (e.g. from handover algorithm) will
   *        be ignored.
   */
  uint8_t m_expectedMeasId;

  Ptr<MobilityModel> m_ueMobility;

}; // end of class NrUeMeasurementsPiecewiseTestCase1



// ===== NR-UE-MEASUREMENTS-PIECEWISE-2 TEST SUITE ======================== //


/**
 * \brief Test suite for generating calls to UE measurements test case
 *        ns3::NrUeMeasurementsPiecewiseTestCase2.
 */
class NrUeMeasurementsPiecewiseTestSuite2 : public TestSuite
{
public:
  NrUeMeasurementsPiecewiseTestSuite2 ();
};


/**
 * \brief Testing UE measurements in NR with simulation of 2 eNodeB and 1 UE in
 *        piecewise configuration and 240 ms report interval.
 */
class NrUeMeasurementsPiecewiseTestCase2 : public TestCase
{
public:
  NrUeMeasurementsPiecewiseTestCase2 (std::string name,
                                       NrRrcSap::ReportConfigEutra config,
                                       std::vector<Time> expectedTime,
                                       std::vector<uint8_t> expectedRsrp);

  virtual ~NrUeMeasurementsPiecewiseTestCase2 ();

  /**
   * \brief Triggers when eNodeB receives measurement report from UE, then
   *        perform verification on it.
   *
   * The trigger is set up beforehand by connecting to the
   * `NrUeRrc::RecvMeasurementReport` trace source.
   *
   * Verification consists of checking whether the report carries the right
   * value of RSRP or not, and whether it occurs at the expected time or not.
   */
  void RecvMeasurementReportCallback (std::string context, uint64_t imsi,
                                      uint16_t cellId, uint16_t rnti,
                                      NrRrcSap::MeasurementReport report);

private:
  /**
   * \brief Setup the simulation with the intended UE measurement reporting
   *        configuration, run it, and connect the
   *        `RecvMeasurementReportCallback` function to the
   *        `NrUeRrc::RecvMeasurementReport` trace source.
   */
  virtual void DoRun ();

  /**
   * \brief Runs at the end of the simulation, verifying that all expected
   *        measurement reports have been examined.
   */
  virtual void DoTeardown ();

  void TeleportVeryNear ();
  void TeleportNear ();
  void TeleportFar ();
  void TeleportVeryFar ();

  /**
   * \brief The active report triggering configuration.
   */
  NrRrcSap::ReportConfigEutra m_config;

  /**
   * \brief The list of expected time when measurement reports are received by
   *        eNodeB.
   */
  std::vector<Time> m_expectedTime;

  /**
   * \brief The list of expected values of RSRP (in 3GPP range unit) from the
   *        measurement reports received.
   */
  std::vector<uint8_t> m_expectedRsrp;

  /**
   * \brief Pointer to the element of `m_expectedTime` which is expected to
   *        occur next in the simulation.
   */
  std::vector<Time>::iterator m_itExpectedTime;

  /**
   * \brief Pointer to the element of `m_expectedRsrp` which is expected to
   *        occur next in the simulation.
   */
  std::vector<uint8_t>::iterator m_itExpectedRsrp;

  /**
   * \brief The measurement identity being tested. Measurement reports with
   *        different measurement identity (e.g. from handover algorithm) will
   *        be ignored.
   */
  uint8_t m_expectedMeasId;

  Ptr<MobilityModel> m_ueMobility;

}; // end of class NrUeMeasurementsPiecewiseTestCase2



// ===== NR-UE-MEASUREMENTS-HANDOVER TEST SUITE =========================== //


/**
 * \brief Test suite for generating calls to UE measurements test case
 *        ns3::NrUeMeasurementsHandoverTestCase.
 */
class NrUeMeasurementsHandoverTestSuite : public TestSuite
{
public:
  NrUeMeasurementsHandoverTestSuite ();
};


/**
 * \brief Testing UE measurements in NR with simulation of 2 eNodeB and 1 UE in
 *        a handover configuration.
 *
 * The simulation will run for the specified duration, while the handover
 * command will be issued exactly at the middle of simulation.
 */
class NrUeMeasurementsHandoverTestCase : public TestCase
{
public:
  NrUeMeasurementsHandoverTestCase (std::string name,
                                     std::list<NrRrcSap::ReportConfigEutra> sourceConfigList,
                                     std::list<NrRrcSap::ReportConfigEutra> targetConfigList,
                                     std::vector<Time> expectedTime,
                                     std::vector<uint8_t> expectedRsrp,
                                     Time duration);

  virtual ~NrUeMeasurementsHandoverTestCase ();

  /**
   * \brief Triggers when either one of the eNodeBs receives measurement report
   *        from UE, then perform verification on it.
   *
   * The trigger is set up beforehand by connecting to the
   * `NrUeRrc::RecvMeasurementReport` trace source.
   *
   * Verification consists of checking whether the report carries the right
   * value of RSRP or not, and whether it occurs at the expected time or not.
   */
  void RecvMeasurementReportCallback (std::string context, uint64_t imsi,
                                      uint16_t cellId, uint16_t rnti,
                                      NrRrcSap::MeasurementReport report);

private:
  /**
   * \brief Setup the simulation with the intended UE measurement reporting
   *        configuration, run it, and connect the
   *        `RecvMeasurementReportCallback` function to the
   *        `NrUeRrc::RecvMeasurementReport` trace source.
   */
  virtual void DoRun ();

  /**
   * \brief Runs at the end of the simulation, verifying that all expected
   *        measurement reports have been examined.
   */
  virtual void DoTeardown ();

  /**
   * \brief The list of active report triggering configuration for the source
   *        eNodeB.
   */
  std::list<NrRrcSap::ReportConfigEutra> m_sourceConfigList;

  /**
   * \brief The list of active report triggering configuration for the target
   *        eNodeB.
   */
  std::list<NrRrcSap::ReportConfigEutra> m_targetConfigList;

  /**
   * \brief The list of expected time when measurement reports are received by
   *        eNodeB.
   */
  std::vector<Time> m_expectedTime;

  /**
   * \brief The list of expected values of RSRP (in 3GPP range unit) from the
   *        measurement reports received.
   */
  std::vector<uint8_t> m_expectedRsrp;

  /**
   * \brief Pointer to the element of `m_expectedTime` which is expected to
   *        occur next in the simulation.
   */
  std::vector<Time>::iterator m_itExpectedTime;

  /**
   * \brief Pointer to the element of `m_expectedRsrp` which is expected to
   *        occur next in the simulation.
   */
  std::vector<uint8_t>::iterator m_itExpectedRsrp;

  /**
   * \brief Duration of simulation.
   */
  Time m_duration;

  /**
   * \brief The list of measurement identities being tested in the source cell.
   *        Measurement reports with different measurement identity (e.g. from
   *        handover algorithm and ANR) will be ignored.
   */
  std::set<uint8_t> m_expectedSourceCellMeasId;

  /**
   * \brief The list of measurement identities being tested in the target cell.
   *        Measurement reports with different measurement identity (e.g. from
   *        handover algorithm and ANR) will be ignored.
   */
  std::set<uint8_t> m_expectedTargetCellMeasId;

}; // end of class NrUeMeasurementsHandoverTestCase

#endif /* NR_TEST_UE_MEASUREMENTS_H */
