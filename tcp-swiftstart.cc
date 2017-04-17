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
#include "tcp-swiftstart.h"
#include "ns3/log.h"
#include "ns3/simulator.h"
#include "rtt-estimator.h"
#include "tcp-socket-base.h"

NS_LOG_COMPONENT_DEFINE ("TcpSwiftStart");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (TcpSwiftStart);

TypeId
TcpSwiftStart::GetTypeId (void)
{
  static TypeId tid = TypeId("ns3::TcpSwiftStart")
    .SetParent<TcpNewReno>()
    .SetGroupName ("Internet")
    .AddConstructor<TcpSwiftStart>()
    .AddTraceSource("EstimatedBW", "The estimated bandwidth",
                    MakeTraceSourceAccessor(&TcpSwiftStart::m_estBW),
                    "ns3::TracedValueCallback::Double")
  ;
  return tid;
}

TcpSwiftStart::TcpSwiftStart (void) :
  TcpNewReno (),
  m_estBW (0),
  m_capacity (0),
  m_gamma (2),
  m_acked1(false),
  m_acked2(false)
{
  NS_LOG_FUNCTION (this);
}

TcpSwiftStart::TcpSwiftStart (const TcpSwiftStart& sock) :
  TcpNewReno (sock),
  m_estBW (sock.m_estBW),
  m_capacity (sock.m_capacity),
  m_gamma (sock.m_gamma),
  m_acked1(sock.m_acked1),
  m_acked2(sock.m_acked2)
{
  NS_LOG_FUNCTION (this);
  NS_LOG_LOGIC ("Invoked the copy constructor");
}

TcpSwiftStart::~TcpSwiftStart (void)
{
}



void 
TcpSwiftStart::PktsAcked (Ptr<TcpSocketState> tcb, uint32_t segmentsAcked, const Time& rtt)
{
  NS_LOG_FUNCTION (this << tcb << segmentsAcked << rtt);
 
  if (rtt.IsZero ())
    {
      NS_LOG_WARN ("RTT measured is zero!");
      return;
    }

   if(!m_acked1)
     {
       m_firstAcked = Simulator::Now ().GetSeconds ();
       m_acked1 = true;
     }

   else if(!m_acked2)
     {
       m_secondAcked = Simulator::Now ().GetSeconds ();
       
       double estBW = EstimateBW (tcb, rtt);
       double estCap =  EstimateCapacity(tcb, rtt, estBW);

       tcb->m_cWnd = uint32_t(estCap);
       tcb->m_initialCWnd = uint32_t(estCap);
       m_acked2 = true;
     } 
}


double 
TcpSwiftStart::EstimateBW (Ptr<TcpSocketState> tcb, const Time& rtt)
{
  double estBW = (m_secondAcked - m_firstAcked)/tcb->m_segmentSize;

  return estBW;
}


double 
TcpSwiftStart::EstimateCapacity(Ptr<TcpSocketState> tcb, const Time& rtt, double estBW) 
{
  double estCap = estBW * rtt.GetSeconds();

  return estCap / m_gamma;
}





Ptr<TcpCongestionOps>
TcpSwiftStart::Fork ()
{
  return CreateObject<TcpSwiftStart> (*this);
}

} // namespace ns3
