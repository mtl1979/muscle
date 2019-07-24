/* This file is Copyright 2000-2013 Meyer Sound Laboratories Inc.  See the included LICENSE.txt file for details. */

#ifndef MuscleUDPSocketDataIO_h
#define MuscleUDPSocketDataIO_h

#include "support/MuscleSupport.h"

#include "dataio/DataIO.h"
#include "util/NetworkUtilityFunctions.h"

namespace muscle {

/**
 *  Data I/O to and from a UDP socket!
 */
class UDPSocketDataIO : public PacketDataIO, private CountedObject<UDPSocketDataIO>
{
public:
   /**
    *  Constructor.
    *  @param sock The socket to use.
    *  @param blocking specifies whether to use blocking or non-blocking socket I/O.
    *  If you will be using this object with a AbstractMessageIOGateway,
    *  and/or select(), then it's usually better to set blocking to false.
    */
   UDPSocketDataIO(const ConstSocketRef & sock, bool blocking) : _sock(sock), _maxPacketSize(MUSCLE_MAX_PAYLOAD_BYTES_PER_UDP_ETHERNET_PACKET)
   {
      (void) SetBlockingIOEnabled(blocking);
      _sendTo.AddTail();  // so that by default, Write() will just call send() on our socket
   }

   /** Destructor.
    *  Closes the socket descriptor, if necessary.
    */
   virtual ~UDPSocketDataIO()
   {
      // empty
   }

   virtual int32 Read(void * buffer, uint32 size)
   {
      IPAddress tmpAddr = invalidIP;
      uint16 tmpPort = 0;
      int32 ret = ReceiveDataUDP(_sock, buffer, size, _blocking, &tmpAddr, &tmpPort);
      _recvFrom.SetIPAddress(tmpAddr);
      _recvFrom.SetPort(tmpPort);
      return ret;
   }

   virtual int32 Write(const void * buffer, uint32 size)
   {
      int32 ret = 0;
      for (uint32 i=0; i<_sendTo.GetNumItems(); i++)
      {
         const IPAddressAndPort & iap = _sendTo[i];
         int32 r = SendDataUDP(_sock, buffer, size, _blocking, iap.GetIPAddress(), iap.GetPort());
         if (r < (int32)size) return r;
                         else ret = r;
      }
      return ret;
   }

   /** Implemented as a no-op:  UDP sockets are always flushed immediately anyway */
   virtual void FlushOutput() {/* empty */}

   /** Overridden to return the maximum packet size of a UDP packet.
     * Defaults to MUSCLE_MAX_PAYLOAD_BYTES_PER_UDP_ETHERNET_PACKET (aka 1388 bytes),
     * but the returned value can be changed via SetPacketMaximumSize().
     */
   virtual uint32 GetMaximumPacketSize() const {return _maxPacketSize;}

   /** This can be called to change the maximum packet size value returned
     * by GetMaximumPacketSize().  You might call this e.g. if you are on a network
     * that supports Jumbo UDP packets and want to take advantage of that.
     */
   void SetPacketMaximumSize(uint32 maxPacketSize) {_maxPacketSize = maxPacketSize;}

   /**
    * Closes our socket connection
    */
   virtual void Shutdown() {_sock.Reset();}

   virtual const ConstSocketRef & GetReadSelectSocket()  const {return _sock;}
   virtual const ConstSocketRef & GetWriteSelectSocket() const {return _sock;}
   virtual const IPAddressAndPort & GetSourceOfLastReadPacket() const {return _recvFrom;}

   /** Call this to make our Write() method use sendto() with the specified
     * destination address and port.  Calling this with (invalidIP, 0) will
     * revert us to our default behavior of just calling() send on our UDP socket.
     */
   virtual status_t SetPacketSendDestination(const IPAddressAndPort & dest) {(void) _sendTo.EnsureSize(1, true); _sendTo.Head() = dest; return B_NO_ERROR;}

   /** Returns the IP address and port that Write() will send to, e.g. as was
     * previously specified in SetPacketSendDestination().
     */
   virtual const IPAddressAndPort & GetPacketSendDestination() const {return _sendTo.HasItems() ? _sendTo.Head() : _sendTo.GetDefaultItem();}

   /** Call this to make our Write() method use sendto() with the specified destination addresss and ports. */
   void SetPacketSendDestinations(const Queue<IPAddressAndPort> & dests) {_sendTo = dests;}

   /** Returns read/write access to our list of send-destinations. */
   Queue<IPAddressAndPort> & GetPacketSendDestinations() {return _sendTo;}

   /** Returns read-only access to our list of send-destinations. */
   const Queue<IPAddressAndPort> & GetPacketSendDestinations() const {return _sendTo;}

   /**
    * Enables or diables blocking I/O on this socket.
    * If this object is to be used by an AbstractMessageIOGateway,
    * then non-blocking I/O is usually better to use.
    * @param blocking If true, socket is set to blocking I/O mode.  Otherwise, non-blocking I/O.
    * @return B_NO_ERROR on success, B_ERROR on error.
    */
   status_t SetBlockingIOEnabled(bool blocking)
   {
      status_t ret = SetSocketBlockingEnabled(_sock, blocking);
      if (ret == B_NO_ERROR) _blocking = blocking;
      return ret;
   }

   /** Returns true iff our socket is set to use blocking I/O (as specified in
    *  the constructor or in our SetBlockingIOEnabled() method)
    */
   bool IsBlockingIOEnabled() const {return _blocking;}

private:
   ConstSocketRef _sock;
   bool _blocking;

   IPAddressAndPort _recvFrom;
   Queue<IPAddressAndPort> _sendTo;
   uint32 _maxPacketSize;
};
DECLARE_REFTYPES(UDPSocketDataIO);

}; // end namespace muscle

#endif
