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
        if ( 0 != lIP )
        {
            OPEN_NET_GLOBAL char * lData = lIP;
            unsigned int lDataSize_byte = lIP_DataSize_byte;
            if ( (                           16384 - 34 ) < lDataSize_byte ) { lDataSize_byte = (                           16384 - 34 ); }
            if ( ( aBufferHeader->mPacketSize_byte - 34 ) < lDataSize_byte ) { lDataSize_byte = ( aBufferHeader->mPacketSize_byte - 34 ); }
            OPEN_NET_CONSTANT char * lA_Data = "Totot";
            unsigned short    lA_Index = 0;
            OPEN_NET_CONSTANT unsigned char lB_Data[ 3 ] = { 0x01, 0x02, 0x03 };
            unsigned short    lB_Index = 0;
            unsigned int i;
            for ( i = 0; ( i < lDataSize_byte ) && ( ! lR ); i ++ )
            {
                if ( lA_Data[ lA_Index ] == lData[ i ] ) { lA_Index ++; }
                else { lA_Index = ( ( lA_Data[ 0 ] == lData[ i ] ) ? 1 : 0 ); }
                if ( 5 <= lA_Index ) { lR = 1; }
                if ( 1 <= i )
                {
                    if ( lB_Data[ lB_Index ] == lData[ i ] ) { lB_Index ++; }
                    else { lB_Index = ( ( lB_Data[ 0 ] == lData[ i ] ) ? 1 : 0 ); }
                    if ( 3 <= lB_Index ) { lR = 1; }
                }
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
