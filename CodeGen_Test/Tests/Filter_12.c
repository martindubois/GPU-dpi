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
        if ( NULL != lIPv4 )
        {
            switch ( lIPv4_Protocol )
            {
            case UDP_IP_PROTOCOL :
                if ( 0x0100 == UDP_DestinationPort( lIP ) ) { lR = 1; }
                else
                {
                    if ( 0x0100 == UDP_SourcePort( lIP ) ) { lR = 1; }
                }
                break;
            }
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
