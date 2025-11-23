//---------------------------------------------------------------------------

#ifndef DecodeRawADS_BH
#define DecodeRawADS_BH
//---------------------------------------------------------------------------
#define TWO_PI             (2 * M_PI)
#define MODES_PREAMBLE_US             8         /* microseconds */
#define MODES_LONG_MSG_BITS         112
#define MODES_SHORT_MSG_BITS         56
#define MODES_FULL_LEN             (MODES_PREAMBLE_US + MODES_LONG_MSG_BITS)
#define MODES_LONG_MSG_BYTES       (MODES_LONG_MSG_BITS / 8)
#define MODES_SHORT_MSG_BYTES      (MODES_SHORT_MSG_BITS / 8)
#define MODES_MAX_SBS_SIZE          256

#define MODES_ICAO_CACHE_LEN       1024   /* Power of two required. */
#define MODES_ICAO_CACHE_TTL         60   /* Time to live of cached addresses (sec). */

#define error_correct_1 true  /**< Fix 1 bit errors (default: true). */
#define error_correct_2 true  /**< Fix 2 bit errors (default: false). */

/**
 * The `readsb` program will send 5 heart-beats like this
 * in RAW mode.
 */
#define MODES_RAW_HEART_BEAT      "*0000;\n*0000;\n*0000;\n*0000;\n*0000;\n"

typedef enum metric_unit_t {
        MODES_UNIT_FEET   = 1,
        MODES_UNIT_METERS = 2
      } metric_unit_t;

#define UNIT_NAME(unit) (unit == MODES_UNIT_METERS ? "meters" : "feet")


typedef struct modeS_message {
        uint8_t  msg [MODES_LONG_MSG_BYTES]; /**< Binary message. */
        int      msg_bits;                   /**< Number of bits in message. */
        int      msg_type;                   /**< Downlink format #. */
        bool     CRC_ok;                     /**< True if CRC was valid. */
        uint32_t CRC;                        /**< Message CRC. */
        double   sig_level;                  /**< RSSI, in the range [0..1], as a fraction of full-scale power. */
        int      error_bit;                  /**< Bit corrected. -1 if no bit corrected. */
        uint8_t  AA [3];                     /**< ICAO Address bytes 1, 2 and 3 (big-endian). */
        bool     phase_corrected;            /**< True if phase correction was applied. */

        /** DF11
         */
        int ca;                              /**< Responder capabilities. */

        /** DF 17
         */
        int  ME_type;                        /**< Extended squitter message type. */
        int  ME_subtype;                     /**< Extended squitter message subtype. */
        int  heading;                        /**< Horizontal angle of flight. */
        bool heading_is_valid;               /**< We got a valid `heading`. */
        int  aircraft_type;                  /**< Aircraft identification. "Type A..D". */
        int  odd_flag;                       /**< 1 = Odd, 0 = Even CPR message. */
        int  UTC_flag;                       /**< UTC synchronized? */
        int  raw_latitude;                   /**< Non decoded latitude. */
        int  raw_longitude;                  /**< Non decoded longitude. */
        char flight [9];                     /**< 8 chars flight number. */
        int  EW_dir;                         /**< 0 = East, 1 = West. */
        int  EW_velocity;                    /**< E/W velocity. */
        int  NS_dir;                         /**< 0 = North, 1 = South. */
        int  NS_velocity;                    /**< N/S velocity. */
        int  vert_rate_source;               /**< Vertical rate source. */
        int  vert_rate_sign;                 /**< Vertical rate sign. */
        int  vert_rate;                      /**< Vertical rate. */
        int  velocity;                       /**< Computed from EW and NS velocity. */

        /** DF4, DF5, DF20, DF21
         */
        int flight_status;                   /**< Flight status for DF4, 5, 20 and 21. */
        int DR_status;                       /**< Request extraction of downlink request. */
        int UM_status;                       /**< Request extraction of downlink request. */
        int identity;                        /**< 13 bits identity (Squawk). */

        /** Fields used by multiple message types.
         */
        int           altitude;
        metric_unit_t unit;
      } modeS_message;


typedef enum
{
  HaveMsg=0,
  MsgHeartBeat=1,
  CRCError=2,
  BadMessageHighLow=3,
  BadMessageTooLong=4,
  BadMessageFormat1=5,
  BadMessageFormat2=6,
  BadMessageEmpty1=7,
  BadMessageEmpty2=8
} TDecodeStatus;

TDecodeStatus decode_RAW_message(AnsiString MsgIn,modeS_message *mm);
void InitDecodeRawADS_B(void);
#endif
