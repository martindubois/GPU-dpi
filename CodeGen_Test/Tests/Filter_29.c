#include <OpenNetK/Ethernet.h>
#include <OpenNetK/IPv4.h>
#include <OpenNetK/IPv6.h>
#include <OpenNetK/TCP.h>
#include <OpenNetK/UDP.h>

OPEN_NET_FUNCTION_DECLARATION( FilterFunction )
{
    OPEN_NET_FUNCTION_BEGIN

        OPEN_NET_GLOBAL unsigned char * lEthernet      = Etherent_Data( lBase, lPacketInfo );
        unsigned short                  lEthernet_Type = Ethernet_Type( lBase, lPacketInfo );

        OPEN_NET_GLOBAL unsigned char * lIP;
        unsigned char                   lIP_Protocol;

        switch ( lEthernet_Type )
        {
        case IPv4_ETHERNET_TYPE_nh :
            lIP          = IPv4_Data    ( lEthernet );
            lIP_Protocol = IPv4_Protocol( lEthernet );
            break; 
        case IPv6_ETHERNET_TYPE_nh :
            lIP          = IPv6_Data    ( lEthernet );
            lIP_Protocol = IPv6_Protocol( lEthernet );
            break; 
        default : lIP = NULL;
        }

        int lR;
        lR = 0;
        OPEN_NET_GLOBAL unsigned short * lA;

        switch(lEthernet_Type)
        {
        case IPv4_ETHERNET_TYPE:
            lA = IPv4_Destination( lEthernet );
            if ( ( 0x007f == lA[ 0 ] ) && ( 0x0100 == lA[ 1 ] ) ) { lR = 1; }
            lA = IPv4_Source( lEthernet );
            if ( ( 0x007f == lA[ 0 ] ) && ( 0x0100 == lA[ 1 ] ) ) { lR = 1; }
            break;
        case IPv6_ETHERNET_TYPE:
            lA = IPv6_Destination( lEthernet );
            if ( ( 0x0000 == lA[ 0 ] ) && ( 0x0000 == lA[ 1 ] ) && ( 0x0000 == lA[ 2 ] ) && (0x0000 == lA[ 3 ] ) && ( 0x0000 == lA[ 4 ] ) && ( 0x0000 == lA[ 5 ] ) && ( 0x0000 == lA[ 6 ] ) && (0x0001 == lA[ 7 ] ) ) { lR = 1; }
            lA = IPv6_Source( lEthernet );
            if ( ( 0x0000 == lA[ 0 ] ) && ( 0x0000 == lA[ 1 ] ) && ( 0x0000 == lA[ 2 ] ) && (0x0000 == lA[ 3 ] ) && ( 0x0000 == lA[ 4 ] ) && ( 0x0000 == lA[ 5 ] ) && ( 0x0000 == lA[ 6 ] ) && (0x0001 == lA[ 7 ] ) ) { lR = 1; }
            break;
        }
        if ( lR )
        {
            lPacketInfo->mSendTo = ( 1 << OUTPUT_ADAPTER ) | ( 1 << FORWARD_ADAPTER ) | OPEN_NET_PACKET_PROCESSED;
        }
        else
        {
            lPacketInfo->mSendTo = ( 1 << FORWARD_ADAPTER ) | OPEN_NET_PACKET_PROCESSED;
        }

    OPEN_NET_FUNCTION_END( 0 )
}
