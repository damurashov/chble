
/*********************************************************************
 * INCLUDES
 */
#include "config.h"
#include "meshtasticservice.h"

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * CONSTANTS
 */

// Scan Characteristic Lengths
#define SCAN_INTERVAL_WINDOW_CHAR_LEN    4
#define SCAN_PARAM_REFRESH_LEN           1

// Scan Parameter Refresh Values
#define SCAN_PARAM_REFRESH_REQ           0x00

// Callback events
#define SCAN_INTERVAL_WINDOW_SET         1

// Get/Set parameters
#define SCAN_PARAM_PARAM_INTERVAL        0
#define SCAN_PARAM_PARAM_WINDOW          1


/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * GLOBAL VARIABLES
 */
// Meshtastic service
const uint8_t meshtasticServUUID[ATT_UUID_SIZE] = {
    0xfd, 0xea, 0x73, 0xe2, 0xca, 0x5d, 0xa8, 0x9f, 0x1f, 0x46, 0xa8, 0x15, 0x18, 0xb2, 0xa1, 0x6b
};

// Scan interval window characteristic
// TODO DM remove
const uint8_t meshtasticIntervalWindowUUID[ATT_BT_UUID_SIZE] = {
    LO_UINT16(SCAN_INTERVAL_WINDOW_UUID), HI_UINT16(SCAN_INTERVAL_WINDOW_UUID)};

/*********************************************************************
 * EXTERNAL VARIABLES
 */

/*********************************************************************
 * EXTERNAL FUNCTIONS
 */

/*********************************************************************
 * LOCAL VARIABLES
 */

// Application callback
static meshtasticServiceCB_t meshtasticServiceCB;

/*********************************************************************
 * Profile Attributes - variables
 */

// Scan Parameters Service attribute
static const gattAttrType_t meshtasticService = {ATT_UUID_SIZE, meshtasticServUUID};


const uint8_t meshtasticFromRadioCharacteristicUUID[ATT_UUID_SIZE] = {
    0x2, 0x0, 0x12, 0xac, 0x42, 0x2, 0x78, 0xb8, 0xed, 0x11, 0x93, 0x49, 0x9e, 0xe6, 0x55, 0x2c
};
static uint8_t fromradioProps = GATT_PROP_READ;
// TODO DM: is it the correct buffer size?
static uint8_t fromradioBuf[256];

const uint8_t meshtasticToRadioCharacteristicUUID[ATT_UUID_SIZE] = {
    0xe7, 0x1, 0x44, 0x12, 0x66, 0x78, 0xdd, 0xa1, 0xad, 0x4d, 0x9e, 0x12, 0xd2, 0x76, 0x5c, 0xf7
};
static uint8_t toradioProps = GATT_PROP_WRITE;
static uint8_t toradioBuf[256];

/*********************************************************************
 * Profile Attributes - Table
 */

static gattAttribute_t meshtasticAttrTbl[] = {
    // Scan Parameters Service attribute
    {
        {ATT_BT_UUID_SIZE, primaryServiceUUID}, /* type */
        GATT_PERMIT_READ,                       /* permissions */
        0,                                      /* handle */
        (uint8_t *)&meshtasticService            /* pValue */
    },
    // FROMRADIO_UUID declaration (TODO DM I am unsure. The table seems to follow
    // service-declaration-definition-declaration-definition-... structure)
    {
        {ATT_BT_UUID_SIZE, characterUUID},
        GATT_PERMIT_READ,
        0,
        &fromradioProps,
    },
    // FROMRADIO_UUID definition
    {
        {ATT_UUID_SIZE, meshtasticFromRadioCharacteristicUUID},
        // TODO DM XXX READ must require authentication, authorization, and encryption?
        GATT_PERMIT_READ,
        0,
        // TODO DM XXX? Should I also handle it in read/write callbacks somehow?
        &fromradioBuf,
    },
    // TORADIO_UUID declaration
    {
        {ATT_BT_UUID_SIZE, characterUUID},
        GATT_PERMIT_READ,
        0,
        &toradioProps,
    },
    // TORADIO_UUID definition
    {
        {ATT_UUID_SIZE, meshtasticToRadioCharacteristicUUID},
        // TODO DM XXX WRITE must require authentication, authorization, and encryption?
        GATT_PERMIT_WRITE,
        0,
        &toradioBuf,
    }
};

// Attribute index enumeration-- these indexes match array elements above
enum
{
    SCAN_PARAM_SERVICE_IDX,       // Scan Parameters Service
    SCAN_PARAM_INTERVAL_DECL_IDX, // Scan Interval Window declaration
    SCAN_PARAM_INTERVAL_IDX,      // Scan Interval Window characteristic
    SCAN_PARAM_REFRESH_DECL_IDX,  // Scan Parameter Refresh declaration
    SCAN_PARAM_REFRESH_IDX,       // Scan Parameter Refresh characteristic
    SCAN_PARAM_REFRESH_CCCD_IDX   // Scan Parameter Refresh characteristic client characteristic configuration
};

/*********************************************************************
 * LOCAL FUNCTIONS
 */
static bStatus_t meshtasticWriteAttrCB(uint16_t connHandle, gattAttribute_t *pAttr,
                                      uint8_t *pValue, uint16_t len, uint16_t offset, uint8_t method);
static bStatus_t meshtasticReadAttrCB(uint16_t connHandle, gattAttribute_t *pAttr,
                                     uint8_t *pValue, uint16_t *pLen, uint16_t offset, uint16_t maxLen, uint8_t method);

/*********************************************************************
 * PROFILE CALLBACKS
 */

// Service Callbacks
gattServiceCBs_t meshtasticCBs = {
    meshtasticReadAttrCB,  // Read callback function pointer
    meshtasticWriteAttrCB, // Write callback function pointer
    NULL                  // Authorization callback function pointer
};

/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/*********************************************************************
 * @fn      Meshtastic_AddService
 *
 * @brief   Initializes the Battery Service by registering
 *          GATT attributes with the GATT server.
 *
 * @return  Success or Failure
 */
bStatus_t Meshtastic_AddService(void)
{
    uint8_t status = SUCCESS;

    // Register GATT attribute list and CBs with GATT Server App
    status = GATTServApp_RegisterService(meshtasticAttrTbl, GATT_NUM_ATTRS(meshtasticAttrTbl), GATT_MAX_ENCRYPT_KEY_SIZE,
                                         &meshtasticCBs);

    return (status);
}

/*********************************************************************
 * @fn      Meshtastic_Register
 *
 * @brief   Register a callback function with the Battery Service.
 *
 * @param   pfnServiceCB - Callback function.
 *
 * @return  None.
 */
extern void Meshtastic_Register(meshtasticServiceCB_t pfnServiceCB)
{
    meshtasticServiceCB = pfnServiceCB;
}

/*********************************************************************
 * @fn      Meshtastic_SetParameter
 *
 * @brief   Set a Battery Service parameter.
 *
 * @param   param - Profile parameter ID
 * @param   len - length of data to right
 * @param   value - pointer to data to write.  This is dependent on
 *          the parameter ID and WILL be cast to the appropriate
 *          data type (example: data type of uint16_t will be cast to
 *          uint16_t pointer).
 *
 * @return  bStatus_t
 */
bStatus_t Meshtastic_SetParameter(uint8_t param, uint8_t len, void *value)
{
    bStatus_t ret = SUCCESS;

    switch(param)
    {
        default:
            ret = INVALIDPARAMETER;
            break;
    }

    return (ret);
}

/*********************************************************************
 * @fn      Meshtastic_GetParameter
 *
 * @brief   Get a Battery Service parameter.
 *
 * @param   param - Profile parameter ID
 * @param   value - pointer to data to get.  This is dependent on
 *          the parameter ID and WILL be cast to the appropriate
 *          data type (example: data type of uint16_t will be cast to
 *          uint16_t pointer).
 *
 * @return  bStatus_t
 */
bStatus_t Meshtastic_GetParameter(uint8_t param, void *value)
{
    bStatus_t ret = SUCCESS;

    // TODO DM
    return (ret);
}

/*********************************************************************
 * @fn      Meshtastic_RefreshNotify
 *
 * @brief   Notify the peer to refresh the scan parameters.
 *
 * @param   connHandle - connection handle
 *
 * @return  None
 */
void Meshtastic_RefreshNotify(uint16_t connHandle)
{
}

/*********************************************************************
 * @fn          meshtasticReadAttrCB
 *
 * @brief       GATT read callback.
 *
 * @param       connHandle - connection message was received on
 * @param       pAttr - pointer to attribute
 * @param       pValue - pointer to data to be read
 * @param       pLen - length of data to be read
 * @param       offset - offset of the first octet to be read
 * @param       maxLen - maximum length of data to be read
 *
 * @return      Success or Failure
 */
static bStatus_t meshtasticReadAttrCB(uint16_t connHandle, gattAttribute_t *pAttr,
                                     uint8_t *pValue, uint16_t *pLen, uint16_t offset, uint16_t maxLen, uint8_t method)
{
    bStatus_t status = SUCCESS;

    return (status);
}

/*********************************************************************
 * @fn      meshtasticWriteAttrCB
 *
 * @brief   Validate attribute data prior to a write operation
 *
 * @param   connHandle - connection message was received on
 * @param   pAttr - pointer to attribute
 * @param   pValue - pointer to data to be written
 * @param   len - length of data
 * @param   offset - offset of the first octet to be written
 *
 * @return  Success or Failure
 */
static bStatus_t meshtasticWriteAttrCB(uint16_t connHandle, gattAttribute_t *pAttr,
                                      uint8_t *pValue, uint16_t len, uint16_t offset, uint8_t method)
{
    bStatus_t status = SUCCESS;

    return (status);
}

/*********************************************************************
 * @fn          Meshtastic_HandleConnStatusCB
 *
 * @brief       Service link status change handler function.
 *
 * @param       connHandle - connection handle
 * @param       changeType - type of change
 *
 * @return      none
 */
void Meshtastic_HandleConnStatusCB(uint16_t connHandle, uint8_t changeType)
{
    // Make sure this is not loopback connection
    if(connHandle != LOOPBACK_CONNHANDLE)
    {
        // Reset Client Char Config if connection has dropped
        if((changeType == LINKDB_STATUS_UPDATE_REMOVED) ||
           ((changeType == LINKDB_STATUS_UPDATE_STATEFLAGS) &&
            (!linkDB_Up(connHandle))))
        {
            // GATTServApp_InitCharCfg(connHandle, meshtasticRefreshClientCharCfg);
        }
    }
}
