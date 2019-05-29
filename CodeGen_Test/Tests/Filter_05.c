#include <OpenNetK/ARP.h>
#include <OpenNetK/Ethernet.h>
#include <OpenNetK/IPv4.h>
#include <OpenNetK/IPv6.h>
#include <OpenNetK/RegEx.h>
#include <OpenNetK/TCP.h>
#include <OpenNetK/UDP.h>


unsigned short SwapUInt16( unsigned short aIn )
{
    return ( ( aIn >> 8 ) | ( aIn << 8 ) );
}

unsigned int SwapUInt32( unsigned int aIn )
{
    return ( ( aIn >> 24 ) | ( ( aIn >> 8 ) & 0x0000ff00 ) | ( ( aIn << 8 ) & 0x00ff0000 ) | ( aIn << 24 ) );
}

OPEN_NET_FUNCTION_DECLARE( FilterFunction )
{
    OPEN_NET_FUNCTION_BEGIN

        OPEN_NET_GLOBAL unsigned char * lEthernet         = Ethernet_Data( lBase, lPacketInfo );
        unsigned short                  lEthernet_Type_nh = Ethernet_Type( lBase, lPacketInfo );

        OPEN_NET_GLOBAL unsigned char * lIP;
        unsigned int                    lIP_DataSize_byte;
        unsigned char                   lIP_Protocol;

        switch( lEthernet_Type_nh )
        {
        case IPv4_ETHERNET_TYPE_nh :
            lIP               = IPv4_Data    ( lEthernet );
            lIP_DataSize_byte = IPv4_DataSize( lEthernet );
            lIP_Protocol      = IPv4_Protocol( lEthernet );
            break;
        case IPv6_ETHERNET_TYPE_nh :
            lIP               = IPv6_Data    ( lEthernet );
            lIP_DataSize_byte = IPv6_DataSize( lEthernet );
            lIP_Protocol      = IPv6_Protocol( lEthernet );
            break;
        default : lIP = 0;
        }

        int lR;
        lR = 0;
        OPEN_NET_GLOBAL unsigned short * lA;
        lA = Ethernet_Destination( lBase, lPacketInfo );
        if ( ( 0x0201 == lA[ 0 ] ) && ( 0x0403 == lA[ 1 ] ) && ( 0x0605 == lA[ 2 ] ) ) { lR = 1; }
        else
        {
            lA = Ethernet_Source( lBase, lPacketInfo );
            if ( ( 0x0201 == lA[ 0 ] ) && ( 0x0403 == lA[ 1 ] ) && ( 0x0605 == lA[ 2 ] ) ) { lR = 1; }
        }
        if ( lR )
        {
            lR = 0;
            OPEN_NET_GLOBAL unsigned short * lA;

            switch( lEthernet_Type_nh )
            {
            case ARP_ETHERNET_TYPE_nh :
                switch( ARP_Protocol( lEthernet) )
                {
                case IPv4_ETHERNET_TYPE_nh :
                    lA = ARP_Destination( lEthernet );
                    if ( ( 0xa8c0 == lA[ 0 ] ) && ( 0x0100 == lA[ 1 ] ) ) { lR = 1; }
                    else
                    {
                        lA = ARP_Source( lEthernet );
                        if ( ( 0xa8c0 == lA[ 0 ] ) && ( 0x0100 == lA[ 1 ] ) ) { lR = 1; }
                    }
                    break;
                }
                break;
            case IPv4_ETHERNET_TYPE_nh :
                lA = IPv4_Destination( lEthernet );
                if ( ( 0xa8c0 == lA[ 0 ] ) && ( 0x0100 == lA[ 1 ] ) ) { lR = 1; }
                else
                {
                    lA = IPv4_Source( lEthernet );
                    if ( ( 0xa8c0 == lA[ 0 ] ) && ( 0x0100 == lA[ 1 ] ) ) { lR = 1; }
                }
                break;
            }
            lR = ! lR;
        }
        if ( lR )
        {
            lPacketInfo->mSendTo = ( 1 << FORWARD_OUTPUT ) | OPEN_NET_PACKET_EVENT | OPEN_NET_PACKET_PROCESSED;
            lEvents |= OPEN_NET_BUFFER_EVENT;
        }
        else
        {
            lPacketInfo->mSendTo = OPEN_NET_PACKET_PROCESSED;
        }

    OPEN_NET_FUNCTION_END
}
