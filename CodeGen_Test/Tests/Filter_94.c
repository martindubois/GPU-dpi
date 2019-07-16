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

unsigned int ReadUInt32( OPEN_NET_GLOBAL unsigned short * aAddr )
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
        OPEN_NET_GLOBAL unsigned char * lData = Ethernet_Data( lBase, lPacketInfo );
        unsigned int lDataSize_byte = Ethernet_DataSize( lBase, lPacketInfo );
        if ( (                           16384 - 14 ) < lDataSize_byte ) { lDataSize_byte = (                           16384 - 14 ); }
        if ( ( aBufferHeader->mPacketSize_byte - 14 ) < lDataSize_byte ) { lDataSize_byte = ( aBufferHeader->mPacketSize_byte - 14 ); }
        OPEN_NET_CONSTANT unsigned char lA_Data[ 11 ] = { 0x10, 0x11, 0x00, 0x00, 0x01, 0x02, 0x03, 0x04, 0x01, 0x02, 0x03 };
        unsigned short    lA_Index = 0;
        unsigned int i;
        for ( i = 0; ( i < lDataSize_byte ) && ( ! lR ); i ++ )
        {
            if ( lA_Data[ lA_Index ] == lData[ i ] ) { lA_Index ++; }
            else { lA_Index = ( ( lA_Data[ 0 ] == lData[ i ] ) ? 1 : 0 ); }
            if ( 11 <= lA_Index ) { lR = 1; }
        }
        if ( lR )
        {
            lPacketInfo->mSendTo = OPEN_NET_PACKET_EVENT | OPEN_NET_PACKET_PROCESSED;
            lEvents |= OPEN_NET_BUFFER_EVENT;
        }
        else
        {
            lPacketInfo->mSendTo = OPEN_NET_PACKET_PROCESSED;
        }

    OPEN_NET_FUNCTION_END
}
