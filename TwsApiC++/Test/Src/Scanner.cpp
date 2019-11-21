//
// Created by carman on 11/19/19.
//

#include "TwsApiL0.h"
#include "TwsApiDefs.h"

using namespace TwsApi;

bool EndOfScannerData	= false;
bool ErrorForRequest	= false;

//----------------------------------------------------------------------------
// MyEWrapper
//----------------------------------------------------------------------------
class MyEWrapper: public EWrapperL0
{
public:

    explicit MyEWrapper( bool CalledFromThread = true ) : EWrapperL0( CalledFromThread ) {}

    void winError( const IBString& str, int lastError ) override
    {
        fprintf( stderr, "WinError: %d = %s\n", lastError, (const char*)str );
        ErrorForRequest = true;
    }

    void error( const int id, const int errorCode, const IBString errorString ) override
    {
        if (id != -1) {
            fprintf( stderr, "Error for id=%d: %d = %s\n", id, errorCode, (const char*)errorString );
        }
        ErrorForRequest = (id > 0);
    }

    void scannerDataEnd(int reqId) override
    {
        EndOfScannerData = true;
    }

    void scannerData( int reqId, int rank, const ContractDetails &contractDetails, const IBString &distance, const IBString &benchmark, const IBString &projection, const IBString &legsStr) override
    {
        fprintf( stdout, "%s,", contractDetails.summary.symbol.c_str());
    }

    void managedAccounts(const IBString& accountsList ) override
    {
//        fprintf( stdout, "%s", accountsList.c_str());
    }

    void nextValidId( OrderId orderId ) override
    {
//        PrintProcessId,printf( "nextValidId = %ld\n", orderId );
    }

    void connectionClosed() override
    {
//        PrintProcessId,printf( "Connection Closed\n");
    }
};

int main( )
{
    MyEWrapper MW(false);	// no thread
    EClientL0* EC = EClientL0::New(&MW);

    if( EC->eConnect("", 7497, 100))
    {
        ScannerSubscription scanSub;
        scanSub.instrument = "STK";
        scanSub.locationCode = "STK.NASDAQ";
        scanSub.scanCode = "TOP_PERC_GAIN";
        scanSub.numberOfRows = 5;
        scanSub.belowPrice = 15.0;
        scanSub.abovePrice = 0.5;
        scanSub.marketCapAbove = 10000000.0;
        scanSub.marketCapBelow = 500000000.0;
        scanSub.aboveVolume = 100000.0;

        EC->reqScannerSubscription(100, scanSub);

        while (!EndOfScannerData)
            EC->checkMessages();
    }

    EC->eDisconnect();
    delete EC;

    return ErrorForRequest;
}
