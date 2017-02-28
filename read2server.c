// /home/matthew/Documents/Research/RFID/MercuryAPI/mercuryapi-1.29.2.10

#include <inttypes.h>
#include <time.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "/home/matthew/Documents/Research/RFID/MercuryAPI/mercuryapi-1.29.2.10/c/src/api/tm_reader.h"
#include "/home/matthew/Documents/Research/RFID/MercuryAPI/mercuryapi-1.29.2.10/c/src/api/serial_reader_imp.h"

bool connected;

/* Enable this to use transportListener */
#ifndef USE_TRANSPORT_LISTENER
#define USE_TRANSPORT_LISTENER 0
#endif
#define PRINT_TAG_METADATA 0
#define numberof(x) (sizeof((x))/sizeof((x)[0]))

#define usage() {errx(1, "read readerURL [--ant antenna_list] [--pow read_power]\n"\
                         "Please provide reader URL, such as:\n"\
                         "tmr:///com4 or tmr:///com4 --ant 1,2 --pow 2300\n"\
                         "tmr://my-reader.example.com or tmr://my-reader.example.com --ant 1,2 --pow 2300\n"\
                         );}

int main(int argc, char *argv[])
{
  TMR_Reader r, *rp;
  TMR_Status ret;
  TMR_ReadPlan plan;
  TMR_Region region;
  uint8_t *antennaList = NULL;
#define READPOWER_NULL (-12345)
  int readpower = READPOWER_NULL;
  uint8_t buffer[20];
  uint8_t i;
  uint8_t antennaCount = 0x0;
  TMR_String model;
  char str[64];
  TMR_TRD_MetadataFlag metadata = TMR_TRD_METADATA_FLAG_ALL;
  readpower = 100;

  rp = &r;
  ret = TMR_create(rp, "tmr:///COM4");
  buffer[0] = 1;
  antennaList = buffer;
  antennaCount = 0x01;

#ifndef BARE_METAL
  checkerr(rp, ret, 1, "creating reader");

#if USE_TRANSPORT_LISTENER

  if (TMR_READER_TYPE_SERIAL == rp->readerType)
  {
    tb.listener = serialPrinter;
  }
  else
  {
    tb.listener = stringPrinter;
  }
  tb.cookie = stdout;

  TMR_addTransportListener(rp, &tb);
#endif
#endif

  ret = TMR_connect(rp);

#ifndef BARE_METAL
  checkerr(rp, ret, 1, "connecting reader");
#endif
  region = TMR_REGION_NONE;
  ret = TMR_paramGet(rp, TMR_PARAM_REGION_ID, &region);
#ifndef BARE_METAL
  checkerr(rp, ret, 1, "getting region");
#endif

  if (TMR_REGION_NONE == region)
  {
    TMR_RegionList regions;
    TMR_Region _regionStore[32];
    regions.list = _regionStore;
    regions.max = sizeof(_regionStore)/sizeof(_regionStore[0]);
    regions.len = 0;

    ret = TMR_paramGet(rp, TMR_PARAM_REGION_SUPPORTEDREGIONS, &regions);
#ifndef BARE_METAL
    checkerr(rp, ret, __LINE__, "getting supported regions");

    if (regions.len < 1)
    {
      checkerr(rp, TMR_ERROR_INVALID_REGION, __LINE__, "Reader doesn't support any regions");
    }
#endif
    region = regions.list[0];
    ret = TMR_paramSet(rp, TMR_PARAM_REGION_ID, &region);
#ifndef BARE_METAL
	checkerr(rp, ret, 1, "setting region");
#endif
  }

  if (READPOWER_NULL != readpower)
  {
    int value;

    ret = TMR_paramGet(rp, TMR_PARAM_RADIO_READPOWER, &value);
#ifndef BARE_METAL
	checkerr(rp, ret, 1, "getting read power");
    printf("Old read power = %d dBm\n", value);
#endif
    value = readpower;
    ret = TMR_paramSet(rp, TMR_PARAM_RADIO_READPOWER, &value);
#ifndef BARE_METAL
	checkerr(rp, ret, 1, "setting read power");
#endif
  }

  {
    int value;
    ret = TMR_paramGet(rp, TMR_PARAM_RADIO_READPOWER, &value);
#ifndef BARE_METAL
    checkerr(rp, ret, 1, "getting read power");
    printf("Read power = %d dBm\n", value);
#endif
  }

  model.value = str;
  model.max = 64;
  TMR_paramGet(rp, TMR_PARAM_VERSION_MODEL, &model);
  if (((0 == strcmp("Sargas", model.value)) || (0 == strcmp("M6e Micro", model.value)) ||(0 == strcmp("M6e Nano", model.value)))
    && (NULL == antennaList))
  {
#ifndef BARE_METAL
    fprintf(stdout, "Reader doesn't support antenna detection.  Please provide antenna list.\n");
    usage();
#endif
  }

  /**
  * for antenna configuration we need two parameters
  * 1. antennaCount : specifies the no of antennas should
  *    be included in the read plan, out of the provided antenna list.
  * 2. antennaList  : specifies  a list of antennas for the read plan.
  **/

  if (rp->readerType == TMR_READER_TYPE_SERIAL)
  {
	// Set the metadata flags. Configurable Metadata param is not supported for llrp readers
	// metadata = TMR_TRD_METADATA_FLAG_ANTENNAID | TMR_TRD_METADATA_FLAG_FREQUENCY | TMR_TRD_METADATA_FLAG_PHASE;
	ret = TMR_paramSet(rp, TMR_PARAM_METADATAFLAG, &metadata);
#ifndef BARE_METAL
	checkerr(rp, ret, 1, "Setting Metadata Flags");
#endif
  }

  // initialize the read plan
  ret = TMR_RP_init_simple(&plan, antennaCount, antennaList, TMR_TAG_PROTOCOL_GEN2, 1000);
#ifndef BARE_METAL
  checkerr(rp, ret, 1, "initializing the  read plan");
#endif

  /* Commit read plan */
  ret = TMR_paramSet(rp, TMR_PARAM_READ_PLAN, &plan);
#ifndef BARE_METAL
  checkerr(rp, ret, 1, "setting read plan");
#endif
  ret = TMR_read(rp, 500, NULL);

#ifndef BARE_METAL
if (TMR_ERROR_TAG_ID_BUFFER_FULL == ret)
  {
    /* In case of TAG ID Buffer Full, extract the tags present
    * in buffer.
    */
    fprintf(stdout, "reading tags:%s\n", TMR_strerr(rp, ret));
  }
  else
  {
    checkerr(rp, ret, 1, "reading tags");
  }
#endif
  while (TMR_SUCCESS == TMR_hasMoreTags(rp))
  {
    TMR_TagReadData trd;
    char epcStr[128];
    char timeStr[128];

    ret = TMR_getNextTag(rp, &trd);
#ifndef BARE_METAL
  checkerr(rp, ret, 1, "fetching tag");
#endif
    TMR_bytesToHex(trd.tag.epc, trd.tag.epcByteCount, epcStr);

#ifndef BARE_METAL

    uint8_t shift;
    uint64_t timestamp;
    time_t seconds;
    int micros;
    char* timeEnd;
    char* end;

    shift = 32;
    timestamp = ((uint64_t)trd.timestampHigh<<shift) | trd.timestampLow;
    seconds = timestamp / 1000;
    micros = (timestamp % 1000) * 1000;

    /*
     * Timestamp already includes millisecond part of dspMicros,
     * so subtract this out before adding in dspMicros again
     */
    micros -= trd.dspMicros / 1000;
    micros += trd.dspMicros;

    timeEnd = timeStr + sizeof(timeStr)/sizeof(timeStr[0]);
    end = timeStr;
    end += strftime(end, timeEnd-end, "%FT%H:%M:%S", localtime(&seconds));
    end += snprintf(end, timeEnd-end, ".%06d", micros);
    end += strftime(end, timeEnd-end, "%z", localtime(&seconds));

#endif

    printf("EPC:%s ", epcStr);
// Enable PRINT_TAG_METADATA Flags to print Metadata value
#if PRINT_TAG_METADATA
{
  while(1) {
    uint16_t j = 0;
  	for (j = 0; j < 9; j++)
  	{
  		if ((TMR_TRD_MetadataFlag)trd.metadataFlags & (1<<j))
  		{
  			switch ((TMR_TRD_MetadataFlag)trd.metadataFlags & (1<<j))
  			{
  			case TMR_TRD_METADATA_FLAG_READCOUNT:
  					printf("Read Count : %d ", trd.readCount);
  					break;
  			case TMR_TRD_METADATA_FLAG_RSSI:
  					printf("RSSI : %d ", trd.rssi);
  					break;
  				case TMR_TRD_METADATA_FLAG_ANTENNAID:
  					printf("Antenna ID : %d ", trd.antenna);
  					break;
  				case TMR_TRD_METADATA_FLAG_FREQUENCY:
  					printf("Frequency : %d ", trd.frequency);
  					break;
  				case TMR_TRD_METADATA_FLAG_TIMESTAMP:
  					printf("Timestamp : %s ", timeStr);
  					break;
  				case TMR_TRD_METADATA_FLAG_PHASE:
  					printf("Phase : %d ", trd.phase);
  					break;
  				case TMR_TRD_METADATA_FLAG_PROTOCOL:
  					printf("Protocol : %d ", trd.tag.protocol);
  					break;
  				case TMR_TRD_METADATA_FLAG_DATA:
  					//TODO : Initialize Read Data
  					if (0 < trd.data.len)
  					{
  						char dataStr[255];
  						TMR_bytesToHex(trd.data.list, trd.data.len, dataStr);
  						printf("Data(%d): %s\n", trd.data.len, dataStr);
  					}
  					break;
  				case TMR_TRD_METADATA_FLAG_GPIO_STATUS:
  					{
  						TMR_GpioPin state[16];
  						uint8_t i, stateCount = numberof(state);
  						ret = TMR_gpiGet(rp, &stateCount, state);
  						if (TMR_SUCCESS != ret)
  						{
  							fprintf(stdout, "Error reading GPIO pins:%s\n", TMR_strerr(rp, ret));
  							return ret;
  						}
  						printf("GPIO stateCount: %d\n", stateCount);
  						for (i = 0 ; i < stateCount ; i++)
  						{
  							printf("Pin %d: %s\n", state[i].id, state[i].high ? "High" : "Low");
  						}
  					}
  					break;
  				default:
  					break;
  			}
  		}
  	}
  }
}
#endif
	printf ("\n");
#endif
  }

  TMR_destroy(rp);
  return 0;
}
