
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
    0x6b, 0xa1, 0xb2, 0x18, 0x15, 0xa8, 0x46, 0x1f, 0x9f, 0xa8, 0x5d, 0xca, 0xe2, 0x73, 0xea,
};

// Meshtastic to-radio characteristic
const uint8_t meshtasticToRadioCharacteristicUUID[ATT_UUID_SIZE] = {
    0xf7, 0x5c, 0x76, 0xd2, 0x12, 0x9e, 0x4d, 0xad, 0xa1, 0xdd, 0x78, 0x66, 0x12, 0x44, 0x01, 0xe7
};

const uint8_t meshtasticFromRadioCharacteristicUUID[ATT_UUID_SIZE] = {
    0x2c, 0x55, 0xe6, 0x9e, 0x49, 0x93, 0x11, 0xed, 0xb8, 0x78, 0x02, 0x42, 0xac, 0x12, 0x00, 0x02
};

const uint8_t meshtasticFromNumCharacteristicUUID[ATT_UUID_SIZE] = {
    0xed, 0x9d, 0xa1, 0x8c, 0xa8, 0x00, 0x4f, 0x66, 0xa6, 0x70, 0xaa, 0x75, 0x47, 0xe3, 0x44, 0x53
};

const uint8_t meshtasticLegacyLogradioCharacteristicUUID[ATT_UUID_SIZE] = {
    0x6c, 0x6f, 0xd2, 0x38, 0x78, 0xfa, 0x43, 0x6b, 0xaa, 0xcf, 0x15, 0xc5, 0xbe, 0x1e, 0xf2, 0xe2
};

const uint8_t meshtasticLogradioCharacteristicUUID[ATT_UUID_SIZE] = {
    0x5a, 0x3d, 0x6e, 0x49, 0x06, 0xe6, 0x44, 0x23, 0x99, 0x44, 0xe9, 0xde, 0x8c, 0xdf, 0x95, 0x47
};

// Scan interval window characteristic
// TODO DM remove
const uint8_t meshtasticIntervalWindowUUID[ATT_BT_UUID_SIZE] = {
    LO_UINT16(SCAN_INTERVAL_WINDOW_UUID), HI_UINT16(SCAN_INTERVAL_WINDOW_UUID)};

// Scan parameter refresh characteristic
// TODO DM remove
const uint8_t meshtasticRefreshUUID[ATT_BT_UUID_SIZE] = {
    LO_UINT16(SCAN_REFRESH_UUID), HI_UINT16(SCAN_REFRESH_UUID)};

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
static
const gattAttrType_t meshtasticService = {ATT_UUID_SIZE, meshtasticServUUID};

// Scan Interval Window characteristic
static uint8_t scanIntervalWindowProps = GATT_PROP_WRITE_NO_RSP;
static uint8_t scanIntervalWindow[SCAN_INTERVAL_WINDOW_CHAR_LEN];

// Scan Parameter Refresh characteristic
static uint8_t       meshtasticRefreshProps = GATT_PROP_NOTIFY;
static uint8_t       meshtasticRefresh[SCAN_PARAM_REFRESH_LEN];
static gattCharCfg_t meshtasticRefreshClientCharCfg[GATT_MAX_NUM_CONN];

/*********************************************************************
 * Profile Attributes - Table
 */

static gattAttribute_t meshtasticAttrTbl[] = {
    // Scan Parameters Service attribute
    {
        {ATT_UUID_SIZE, primaryServiceUUID}, /* type */
        GATT_PERMIT_READ,                       /* permissions */
        0,                                      /* handle */
        (uint8_t *)&meshtasticService            /* pValue */
    },
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

    // Initialize Client Characteristic Configuration attributes
    GATTServApp_InitCharCfg(INVALID_CONNHANDLE, meshtasticRefreshClientCharCfg);

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
    switch(param)
    {
        case SCAN_PARAM_PARAM_INTERVAL:
            *((uint16_t *)value) = BUILD_UINT16(scanIntervalWindow[0],
                                                scanIntervalWindow[1]);
            break;

        case SCAN_PARAM_PARAM_WINDOW:
            *((uint16_t *)value) = BUILD_UINT16(scanIntervalWindow[2],
                                                scanIntervalWindow[3]);
            break;

        default:
            ret = INVALIDPARAMETER;
            break;
    }

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
    uint16_t value;

    value = GATTServApp_ReadCharCfg(connHandle, meshtasticRefreshClientCharCfg);
    if(value & GATT_CLIENT_CFG_NOTIFY)
    {
        attHandleValueNoti_t noti;

        noti.pValue = GATT_bm_alloc(connHandle, ATT_HANDLE_VALUE_NOTI,
                                    SCAN_PARAM_REFRESH_LEN, NULL, 0);
        if(noti.pValue != NULL)
        {
            // send notification
            noti.handle = meshtasticAttrTbl[SCAN_PARAM_REFRESH_IDX].handle;
            noti.len = SCAN_PARAM_REFRESH_LEN;
            noti.pValue[0] = SCAN_PARAM_REFRESH_REQ;

            if(GATT_Notification(connHandle, &noti, FALSE) != SUCCESS)
            {
                GATT_bm_free((gattMsg_t *)&noti, ATT_HANDLE_VALUE_NOTI);
            }
        }
    }
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
    uint16_t  uuid;
    bStatus_t status = SUCCESS;

    // Make sure it's not a blob operation (no attributes in the profile are long)
    if(offset > 0)
    {
        return (ATT_ERR_ATTR_NOT_LONG);
    }

    uuid = BUILD_UINT16(pAttr->type.uuid[0], pAttr->type.uuid[1]);

    // Only one writeable attribute
    if(uuid == SCAN_INTERVAL_WINDOW_UUID)
    {
        if(len == SCAN_INTERVAL_WINDOW_CHAR_LEN)
        {
            uint16_t interval = BUILD_UINT16(pValue[0], pValue[1]);
            uint16_t window = BUILD_UINT16(pValue[0], pValue[1]);

            // Validate values
            if(window <= interval)
            {
                tmos_memcpy(pAttr->pValue, pValue, len);

                (*meshtasticServiceCB)(SCAN_INTERVAL_WINDOW_SET);
            }
            else
            {
                status = ATT_ERR_INVALID_VALUE;
            }
        }
        else
        {
            status = ATT_ERR_INVALID_VALUE_SIZE;
        }
    }
    else if(uuid == GATT_CLIENT_CHAR_CFG_UUID)
    {
        status = GATTServApp_ProcessCCCWriteReq(connHandle, pAttr, pValue, len,
                                                offset, GATT_CLIENT_CFG_NOTIFY);
    }
    else
    {
        status = ATT_ERR_ATTR_NOT_FOUND;
    }

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
            GATTServApp_InitCharCfg(connHandle, meshtasticRefreshClientCharCfg);
        }
    }
}
