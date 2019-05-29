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
        if ( ( 0 != lIP ) && ( UDP_IP_PROTOCOL == lIP_Protocol ) )
        {
            OPEN_NET_GLOBAL char * lData = UDP_Data( lIP );
            unsigned int lDataSize_byte = lIP_DataSize_byte - UDP_HEADER_SIZE_byte;
            if ( (                           16384 - 42 ) < lDataSize_byte ) { lDataSize_byte = (                           16384 - 42 ); }
            if ( ( aBufferHeader->mPacketSize_byte - 42 ) < lDataSize_byte ) { lDataSize_byte = ( aBufferHeader->mPacketSize_byte - 42 ); }
            unsigned char lA_Counters[ 12 ];
            RegEx lA_RegEx;
            OPEN_NET_CONSTANT RegEx_State lA_States[] =
            {
                { REG_EX_GROUP, 1, 255, 0x0003 },
                { '\n', 1,   1, 0x0000 },
                { REG_EX_OK, 0,   0, 0x0000 },
                { 'a', 1,   1, 0x0000 },
                { 'b', 1,   1, 0x0000 },
                { REG_EX_GROUP, 0,   1, 0x0009 },
                { 'a', 1,   1, 0x0000 },
                { 'b', 1,   1, 0x0000 },
                { REG_EX_RETURN, 0,   0, 0x0000 },
                { 'c', 1,   1, 0x0000 },
                { 'd', 1,   1, 0x0000 },
                { REG_EX_RETURN, 0,   0, 0x0005 },
            };
            REG_EX_CREATE( & lA_RegEx, lA_States, lA_Counters );
            unsigned int i;
            for ( i = 0; ( i < lDataSize_byte ) && ( ! lR ); i ++ )
            {
                if ( RegEx_Execute( & lA_RegEx, lData[ i ] ) ) { lR = 1; }
            }
            if ( ! lR )
            {
                if ( RegEx_End( & lA_RegEx ) ) { lR = 1; }
            }
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
