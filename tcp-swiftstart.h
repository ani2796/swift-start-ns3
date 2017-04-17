/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2017 NITK Surathkal
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
 * Authors: Anirudh Sriram <ADD-EMAIL>
 *          Prajwal Kailas <ADD-EMAIL>
 *          David Joseph Attokaren <davidjattokaren@gmail.com>
 */

/*
 * NOTE: This code is an adaptation of the Swift Start paper written by BBN Technologies.
 * Paper link: www.icir.org/mallman/pubs/PRAKS02/PRAKS02.ps (which is also included in this repository).
 */
#ifndef TCP_SWIFTSTART_H
#define TCP_SWIFTSTART_H

#include "ns3/tcp-congestion-ops.h"
#include "ns3/timer.h"
#include "ns3/object.h"
#include "ns3/sequence-number.h"
#include "ns3/traced-value.h"
#include "ns3/tcp-socket-base.h"

namespace ns3 {

class Packet;
class TcpHeader;
class Time;
class EventId;

/**
 * \ingroup congestionOps
 *
 * \brief An implementation of TCP Swift Start.
 *
 * Westwood and Westwood+ employ the AIAD (Additive Increase/Adaptive Decrease) 
 * congestion control paradigm. When a congestion episode happens, 
 * instead of halving the cwnd, these protocols try to estimate the network's
 * bandwidth and use the estimated value to adjust the cwnd. 
 * While Westwood performs the bandwidth sampling every ACK reception, 
 * Westwood+ samples the bandwidth every RTT.
 *
 * The two main methods in the implementation are the CountAck (const TCPHeader&)
 * and the EstimateBW (int, const, Time). The CountAck method calculates
 * the number of acknowledged segments on the receipt of an ACK.
 * The EstimateBW estimates the bandwidth based on the value returned by CountAck
 * and the sampling interval (last ACK inter-arrival time for Westwood and last RTT for Westwood+).
 */
class TcpSwiftStart : public TcpNewReno
{
public:
  /**
   * \brief Get the type ID.
   * \return the object TypeId
   */
  static TypeId GetTypeId (void);

  TcpSwiftStart (void);
  /**
   * \brief Copy constructor
   * \param sock the object to copy
   */
  TcpSwiftStart (const TcpSwiftStart& sock);
  virtual ~TcpSwiftStart (void);

  /**
   * \brief Protocol variant (Swift Start)
   */
  enum ProtocolType 
  {
    SWIFTSTART
  };

protected:
  virtual void PktsAcked (Ptr<TcpSocketState> tcb, uint32_t segmentsAcked,
                          const Time& rtt);
  virtual Ptr<TcpCongestionOps> Fork ();
private:
  /**
   * Update the total number of acknowledged packets during the current RTT
   *
   * \param [in] acked the number of packets the currently received ACK acknowledges
   */
  void UpdateAckedSegments (int acked);

  /**
   * Estimate the network's bandwidth
   *
   * \param [in] rtt the RTT estimation.
   * \param [in] tcb the socket state.
   */
  double EstimateBW (Ptr<TcpSocketState> tcb, const Time& rtt);

  double EstimateCapacity(Ptr<TcpSocketState> tcb, const Time& rtt, double estBW);

protected:
  TracedValue<double>    m_estBW;
  double                 m_capacity;      
  double                 m_gamma;
  Time                   m_minRtt;                 //!< Minimum RTT
  double                 m_firstAcked;
  double                 m_secondAcked; 
  enum ProtocolType      m_pType;                  //!< 0 for Westwood, 1 for Westwood+
  bool                   m_acked1;
  bool                   m_acked2;
  int                    m_ackedSegments;          //!< The number of segments ACKed between RTTs
  EventId                m_bwEstimateEvent;        //!< The BW estimation event for Westwood+

};

} // namespace ns3

#endif /* TCP_WESTWOOD_H */

