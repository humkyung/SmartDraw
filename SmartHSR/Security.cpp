#include "Security.h"

#include "spromeps.h"

SP_STATUS status;
RB_SPRO_APIPACKET packet;
RB_WORD devID = 0x3E8E;		// Change this to match your developer ID.
const int queryLength = 6;	// Change this to the length of byte of your query data.
int QueryPair = 4;		// For displaying the 4th row of the array.


// A three-dimensional query and response pair array.
// Declaration : QueryResponses [ number of pairs (dynamic) ] [2] [ length of byte ]
// For each pair inside the array : { { query data }, { response } }
// Change the values to match your query response pairs.
// You should be able to paste the values from the Query Response Generator.

unsigned char QueryResponses[][2][6] = { 
  { { 0x13, 0x2D, 0x71, 0x9C, 0x25, 0xA2 }, { 0x9F, 0xD3, 0x38, 0xE3, 0x16, 0x52 } },
  { { 0x52, 0x79, 0x51, 0x16, 0x50, 0xB2 }, { 0x0F, 0x5A, 0x5A, 0x78, 0x6B, 0xD5 } },
  { { 0x49, 0x7F, 0x55, 0x1C, 0xDF, 0x7F }, { 0xC9, 0x77, 0x46, 0xAD, 0x1B, 0xC3 } },
  { { 0x51, 0x77, 0x23, 0xF2, 0x7F, 0xA1 }, { 0x3B, 0x4F, 0x1C, 0xD1, 0x19, 0xC7 } },
  { { 0xCC, 0xC3, 0xE9, 0x0A, 0xDA, 0x03 }, { 0x75, 0x5D, 0xC0, 0x26, 0xB8, 0xC6 } }
 };

void formatReturnString ()
{
	if (status == SP_SUCCESS)
		printf ("\r\n SUCCESS.");
	else
		printf ("\r\n ERROR!  Status code : %d.", status);
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
void printQueryResponse (int QueryPair)
{
	printf("\r\n Query:             ");
	for(int a=0;a<6;a++)
		printf("0x%02X ",QueryResponses[QueryPair][0][a]);

	printf("\r\n Expected Response: ");
	for(a=0;a<6;a++)
		printf("0x%02X ",QueryResponses[QueryPair][1][a]);
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
void formatPacket ()
{
	printf ("\r\n Calling 'RNBOsproFormatPacket'...");
	status = RNBOsproFormatPacket (packet, sizeof (packet));
	formatReturnString ();
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
void initialize ()
{
	printf ("\r\n\r\n Calling 'RNBOsproInitialize'...");
	status = RNBOsproInitialize (packet);
	formatReturnString ();
}

void findFirstUnit ()
{
	printf ("\r\n\r\n Calling 'RNBOsproFindFirstUnit'...");
	status = RNBOsproFindFirstUnit (packet, devID);
	formatReturnString ();
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
bool query(int nAddress,int QueryPair)
{
	//int address = 8;		// Change this to the address of the first word of 
					// the algorithm descripter to be used for the query.
	unsigned char *queryData;
	unsigned char response[queryLength];
	unsigned long *response32 = 0;
	bool ret=false;

	queryData = &QueryResponses[QueryPair][0][0];
	status = RNBOsproQuery (packet,nAddress, queryData, &response, response32, queryLength);

	unsigned char *expectedResponse = &QueryResponses[QueryPair][1][0];
	//printQueryResponse (QueryPair);
	if(!memcmp (expectedResponse, response, queryLength))
		// Compare memory of the expected and the actual response.
	{
		ret=true;
	}
	
	return ret;
}

int main(int argc, char* argv[])
{
	printf ("\r\n SentinelSuperPro 6.0 ANSI C Sample Program\r\n");
	printf (" ==========================================\r\n");
	formatPacket ();
	initialize ();
	findFirstUnit ();
	query(8,QueryPair);

	printf ("\r\n\r\n Done.\r\n");

	return 0;
}