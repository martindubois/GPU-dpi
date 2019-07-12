#include <OpenNetK/ARP.h>
#include <OpenNetK/Ethernet.h>
#include <OpenNetK/IPv4.h>
#include <OpenNetK/IPv6.h>
#include <OpenNetK/RegEx.h>
#include <OpenNetK/TCP.h>
#include <OpenNetK/UDP.h>


OPEN_NET_DEVICE unsigned short SwapUInt16( unsigned short aIn )
{
    return ( ( aIn >> 8 ) | ( aIn << 8 ) );
}

OPEN_NET_DEVICE unsigned int ReadUInt32( OPEN_NET_GLOBAL unsigned short * aAddr )
{
    return ( ( SwapUInt16( aAddr[ 0 ] ) << 16 ) | SwapUInt16( aAddr[ 1 ] ) );
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

        switch( lEthernet_Type_nh )
        {
        case ARP_ETHERNET_TYPE_nh :
            switch( ARP_Protocol( lEthernet) )
            {
            case IPv4_ETHERNET_TYPE_nh :
                lA = ARP_Destination( lEthernet );
                if ( ( 0x00c0 & 0x00fe ) == ( lA[ 0 ] & 0x00fe ) ) { lR = 1; }
                else
                {
                    lA = ARP_Source( lEthernet );
                    if ( ( 0x00c0 & 0x00fe ) == ( lA[ 0 ] & 0x00fe ) ) { lR = 1; }
                }
                break;
            }
            break;
        case IPv4_ETHERNET_TYPE_nh :
            lA = IPv4_Destination( lEthernet );
            if ( ( 0x00c0 & 0x00fe ) == ( lA[ 0 ] & 0x00fe ) ) { lR = 1; }
            else
            {
                lA = IPv4_Source( lEthernet );
                if ( ( 0x00c0 & 0x00fe ) == ( lA[ 0 ] & 0x00fe ) ) { lR = 1; }
            }
            break;
        }
        if ( lR )
        {
            lPacketInfo->mSendTo = ( 1 << OUTPUT_ADAPTER ) | OPEN_NET_PACKET_PROCESSED;
        }
        else
        {
            lPacketInfo->mSendTo = ( 1 << FORWARD_ADAPTER ) | OPEN_NET_PACKET_PROCESSED;
        }

    OPEN_NET_FUNCTION_END
}
