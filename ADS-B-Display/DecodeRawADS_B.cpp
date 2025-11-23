//---------------------------------------------------------------------------

#pragma hdrstop
#include <vcl.h>
#include "DecodeRawADS_B.h"
#include "DisplayGUI.h"
#include <cstring>
#include <string.h>

//---------------------------------------------------------------------------
#pragma package(smart_init)
static int hex_digit_val (int c);
static int decode_modeS_message (modeS_message *mm, const uint8_t *_msg);
static int modeS_message_len_by_type (int type);
static uint32_t CRC_get (const uint8_t *msg, int bits);
static uint32_t CRC_check (const uint8_t *msg, int bits);
static int fix_two_bits_errors (uint8_t *msg, int bits);
static int fix_single_bit_errors (uint8_t *msg, int bits);
static bool brute_force_AP (const uint8_t *msg, modeS_message *mm);
static int decode_AC12_field (uint8_t *msg, metric_unit_t *unit);
static int decode_AC13_field (const uint8_t *msg, metric_unit_t *unit);
static uint32_t aircraft_get_addr (uint8_t a0, uint8_t a1, uint8_t a2);
static void ICAO_cache_add_address (uint32_t addr);
static bool ICAO_address_recently_seen (uint32_t addr);
static uint32_t ICAO_cache_hash_address (uint32_t a);


static uint32_t         *ICAO_cache=NULL;               /**< Recently seen ICAO addresses. */

/**
 * Parity table for MODE S Messages.
 *
 * The table contains 112 (`MODES_LONG_MSG_BITS`) elements, every element
 * corresponds to a bit set in the message, starting from the first bit of
 * actual data after the preamble.
 *
 * For messages of 112 bit, the whole table is used.
 * For messages of 56 bits only the last 56 elements are used.
 *
 * The algorithm is as simple as XOR-ing all the elements in this table
 * for which the corresponding bit on the message is set to 1.
 *
 * The last 24 elements in this table are set to 0 as the checksum at the
 * end of the message should not affect the computation.
 *
 * \note
 * This function can be used with DF11 and DF17. Other modes have
 * the CRC *XOR-ed* with the sender address as they are replies to interrogations,
 * but a casual listener can't split the address from the checksum.
 */
static const uint32_t checksum_table [MODES_LONG_MSG_BITS] = {
             0x3935EA, 0x1C9AF5, 0xF1B77E, 0x78DBBF, 0xC397DB, 0x9E31E9, 0xB0E2F0, 0x587178,
             0x2C38BC, 0x161C5E, 0x0B0E2F, 0xFA7D13, 0x82C48D, 0xBE9842, 0x5F4C21, 0xD05C14,
             0x682E0A, 0x341705, 0xE5F186, 0x72F8C3, 0xC68665, 0x9CB936, 0x4E5C9B, 0xD8D449,
             0x939020, 0x49C810, 0x24E408, 0x127204, 0x093902, 0x049C81, 0xFDB444, 0x7EDA22,
             0x3F6D11, 0xE04C8C, 0x702646, 0x381323, 0xE3F395, 0x8E03CE, 0x4701E7, 0xDC7AF7,
             0x91C77F, 0xB719BB, 0xA476D9, 0xADC168, 0x56E0B4, 0x2B705A, 0x15B82D, 0xF52612,
             0x7A9309, 0xC2B380, 0x6159C0, 0x30ACE0, 0x185670, 0x0C2B38, 0x06159C, 0x030ACE,
             0x018567, 0xFF38B7, 0x80665F, 0xBFC92B, 0xA01E91, 0xAFF54C, 0x57FAA6, 0x2BFD53,
             0xEA04AD, 0x8AF852, 0x457C29, 0xDD4410, 0x6EA208, 0x375104, 0x1BA882, 0x0DD441,
             0xF91024, 0x7C8812, 0x3E4409, 0xE0D800, 0x706C00, 0x383600, 0x1C1B00, 0x0E0D80,
             0x0706C0, 0x038360, 0x01C1B0, 0x00E0D8, 0x00706C, 0x003836, 0x001C1B, 0xFFF409,
             0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000,
             0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000,
             0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000
           };


void InitDecodeRawADS_B(void)
{
 ICAO_cache =(uint32_t *) calloc (2 * sizeof(uint32_t) * MODES_ICAO_CACHE_LEN, 1);
}


/**
 * Hash the ICAO address to index our cache of MODES_ICAO_CACHE_LEN
 * elements, that is assumed to be a power of two.
 */
static uint32_t ICAO_cache_hash_address (uint32_t a)
{
  /* The following three rounds will make sure that every bit affects
   * every output bit with ~ 50% of probability.
   */
  a = ((a >> 16) ^ a) * 0x45D9F3B;
  a = ((a >> 16) ^ a) * 0x45D9F3B;
  a = ((a >> 16) ^ a);
  return (a & (MODES_ICAO_CACHE_LEN - 1));
}


/**
 * Add the specified entry to the cache of recently seen ICAO addresses.
 *
 * Note that we also add a timestamp so that we can make sure that the
 * entry is only valid for `MODES_ICAO_CACHE_TTL` seconds.
 */
static void ICAO_cache_add_address (uint32_t addr)
{
  uint32_t h = ICAO_cache_hash_address (addr);

  ICAO_cache [h*2]   = addr;
  ICAO_cache [h*2+1] = (uint32_t) time (NULL);
}

/**
 * Returns true if the specified ICAO address was seen in a DF format with
 * proper checksum (not XORed with address) no more than
 * `MODES_ICAO_CACHE_TTL` seconds ago.
 * Otherwise returns false.
 */
static bool ICAO_address_recently_seen (uint32_t addr)
{
  uint32_t h_idx = ICAO_cache_hash_address (addr);
  uint32_t _addr = ICAO_cache [2*h_idx];
  uint32_t seen  = ICAO_cache [2*h_idx + 1];

  return (_addr && _addr == addr && (time(NULL) - seen) <= MODES_ICAO_CACHE_TTL);
}

/**
 * Convert 24-bit big-endian (network order) to host order format.
 */
static uint32_t aircraft_get_addr (uint8_t a0, uint8_t a1, uint8_t a2)
{
  return ((a0 << 16) | (a1 << 8) | a2);
}


/**
 * If the message type has the checksum XORed with the ICAO address, try to
 * brute force it using a list of recently seen ICAO addresses.
 *
 * Do this in a brute-force fashion by XORing the predicted CRC with
 * the address XOR checksum field in the message. This will recover the
 * address: if we found it in our cache, we can assume the message is okay.
 *
 * This function expects `mm->msg_type` and `mm->msg_bits` to be correctly
 * populated by the caller.
 *
 * On success the correct ICAO address is stored in the `modeS_message`
 * structure in the `AA [0..2]` fields.
 *
 * \retval true   successfully recovered a message with a correct checksum.
 * \retval false  failed to recover a message with a correct checksum.
 */
static bool brute_force_AP (const uint8_t *msg, modeS_message *mm)
{
  uint8_t aux [MODES_LONG_MSG_BYTES];
  int     msg_type = mm->msg_type;
  int     msg_bits = mm->msg_bits;

  if (msg_type == 0 ||         /* Short air surveillance */
      msg_type == 4 ||         /* Surveillance, altitude reply */
      msg_type == 5 ||         /* Surveillance, identity reply */
      msg_type == 16 ||        /* Long Air-Air Surveillance */
      msg_type == 20 ||        /* Comm-A, altitude request */
      msg_type == 21 ||        /* Comm-A, identity request */
      msg_type == 24)          /* Comm-C ELM */
  {
    uint32_t addr;
    uint32_t CRC;
    int      last_byte = (msg_bits / 8) - 1;

    /* Work on a copy. */
    memcpy (aux, msg, msg_bits/8);

    /* Compute the CRC of the message and XOR it with the AP field
     * so that we recover the address, because:
     *
     * (ADDR xor CRC) xor CRC = ADDR.
     */
    CRC = CRC_check (aux, msg_bits);
    aux [last_byte]   ^= CRC & 0xFF;
    aux [last_byte-1] ^= (CRC >> 8) & 0xFF;
    aux [last_byte-2] ^= (CRC >> 16) & 0xFF;

    /* If the obtained address exists in our cache we consider
     * the message valid.
     */
    addr = aircraft_get_addr (aux[last_byte-2], aux[last_byte-1], aux[last_byte]);
    if (ICAO_address_recently_seen(addr))
    {
      mm->AA [0] = aux [last_byte-2];
      mm->AA [1] = aux [last_byte-1];
      mm->AA [2] = aux [last_byte];
      return (true);
    }
  }
  return (false);
}

/**
 * Decode the 13 bit AC altitude field (in DF 20 and others).
 *
 * \param in  msg   the raw message to work with.
 * \param out unit  set to either `MODES_UNIT_METERS` or `MODES_UNIT_FEETS`.
 * \retval the altitude.
 */
static int decode_AC13_field (const uint8_t *msg, metric_unit_t *unit)
{
  int m_bit = msg[3] & (1 << 6);
  int q_bit = msg[3] & (1 << 4);
  int ret;

  if (!m_bit)
  {
    *unit = MODES_UNIT_FEET;
    if (q_bit)
    {
      /* N is the 11 bit integer resulting from the removal of bit Q and M
       */
      int n = ((msg[2] & 31) << 6)   |
              ((msg[3] & 0x80) >> 2) |
              ((msg[3] & 0x20) >> 1) |
               (msg[3] & 15);

      /**
       * The final altitude is due to the resulting number multiplied by 25, minus 1000.
       */
      ret = 25 * n - 1000;
      if (ret < 0)
         ret = 0;
      return (ret);
    }
    else
    {
      /** \todo Implement altitude where Q=0 and M=0 */
    }
  }
  else
  {
    *unit = MODES_UNIT_METERS;

    /** \todo Implement altitude when meter unit is selected.
     */
  }
  return (0);
}

/**
 * Decode the 12 bit AC altitude field (in DF 17 and others).
 * Returns the altitude or 0 if it can't be decoded.
 */
static int decode_AC12_field (uint8_t *msg, metric_unit_t *unit)
{
  int ret, n, q_bit = msg[5] & 1;

  if (q_bit)
  {
    /* N is the 11 bit integer resulting from the removal of bit Q
     */
    *unit = MODES_UNIT_FEET;
    n = ((msg[5] >> 1) << 4) | ((msg[6] & 0xF0) >> 4);

    /* The final altitude is due to the resulting number multiplied
     * by 25, minus 1000.
     */
    ret = 25 * n - 1000;
    if (ret < 0)
       ret = 0;
    return (ret);
  }
  return (0);
}



/**
 * Try to fix single bit errors using the checksum. On success modifies
 * the original buffer with the fixed version, and returns the position
 * of the error bit. Otherwise if fixing failed, -1 is returned.
 */
static int fix_single_bit_errors (uint8_t *msg, int bits)
{
  int     i;
  uint8_t aux [MODES_LONG_MSG_BITS / 8];

  for (i = 0; i < bits; i++)
  {
    int      byte = i / 8;
    int      mask = 1 << (7-(i % 8));
    uint32_t crc1, crc2;

    memcpy (aux, msg, bits/8);
    aux [byte] ^= mask;   /* Flip j-th bit. */

    crc1 = CRC_get (aux, bits);
    crc2 = CRC_check (aux, bits);

    if (crc1 == crc2)
    {
      /* The error is fixed. Overwrite the original buffer with
       * the corrected sequence, and returns the error bit
       * position.
       */
      memcpy (msg, aux, bits/8);
      return (i);
    }
  }
  return (-1);
}

/**
 * Similar to `fix_single_bit_errors()` but try every possible two bit combination.
 *
 * This is very slow and should be tried only against DF17 messages that
 * don't pass the checksum, and only with `Modes.error_correct_2` setting.
 */
static int fix_two_bits_errors (uint8_t *msg, int bits)
{
  int     j, i;
  uint8_t aux [MODES_LONG_MSG_BITS / 8];

  for (j = 0; j < bits; j++)
  {
    int byte1 = j / 8;
    int mask1 = 1 << (7-(j % 8));

    /* Don't check the same pairs multiple times, so i starts from j+1 */
    for (i = j+1; i < bits; i++)
    {
      int      byte2 = i / 8;
      int      mask2 = 1 << (7 - (i % 8));
      uint32_t crc1, crc2;

      memcpy (aux, msg, bits/8);

      aux [byte1] ^= mask1;  /* Flip j-th bit. */
      aux [byte2] ^= mask2;  /* Flip i-th bit. */

      crc1 = CRC_get (aux, bits);
      crc2 = CRC_check (aux, bits);

      if (crc1 == crc2)
      {
        /* The error is fixed. Overwrite the original buffer with
         * the corrected sequence, and returns the error bit
         * position.
         */
        memcpy (msg, aux, bits/8);

        /* We return the two bits as a 16 bit integer by shifting
         * 'i' on the left. This is possible since 'i' will always
         * be non-zero because i starts from j+1.
         */
        return (j | (i << 8));
      }
    }
  }
  return (-1);
}



static uint32_t CRC_check (const uint8_t *msg, int bits)
{
  uint32_t crc = 0;
  int      offset = 0;
  int      j;

  if (bits != MODES_LONG_MSG_BITS)
     offset = MODES_LONG_MSG_BITS - MODES_SHORT_MSG_BITS;

  for (j = 0; j < bits; j++)
  {
    int byte = j / 8;
    int bit  = j % 8;
    int mask = 1 << (7 - bit);

    /* If bit is set, XOR with corresponding table entry.
     */
    if (msg[byte] & mask)
       crc ^= checksum_table [j + offset];
  }
  return (crc); /* 24 bit checksum. */
}


/**
 * Given the Downlink Format (DF) of the message, return the
 * message length in bits.
 */
static int modeS_message_len_by_type (int type)
{
  if (type == 16 || type == 17 || type == 19 || type == 20 || type == 21)
     return (MODES_LONG_MSG_BITS);
  return (MODES_SHORT_MSG_BITS);
}


static int hex_digit_val (int c)
{
  c = tolower (c);
  if (c >= '0' && c <= '9')
     return (c - '0');
  if (c >= 'a' && c <= 'f')
     return (c - 'a' + 10);
  return (-1);
}

/*
 * Return the CRC in a message.
 * CRC is always the last three bytes.
 */
static uint32_t CRC_get (const uint8_t *msg, int bits)
{
  uint32_t CRC = ((uint32_t) msg [(bits / 8) - 3] << 16) |
                 ((uint32_t) msg [(bits / 8) - 2] << 8) |
                  (uint32_t) msg [(bits / 8) - 1];
  return (CRC);
}

TDecodeStatus decode_RAW_message (AnsiString MsgIn,modeS_message *mm)
{
  uint8_t       bin_msg [MODES_LONG_MSG_BYTES];
  int           len, j, msg_len;
  uint8_t      *hex, *end;
  uint8_t       msg[512];

  strcpy((char *)msg,MsgIn.c_str());
  strcat((char *)msg,"\n");
  msg_len=strlen((char *)msg);

  if (msg_len == 0)  /* all was consumed */
     return (BadMessageEmpty1);

  end = (uint8_t *)memchr (msg, '\n', msg_len);
  if (!end)
  {
    return (BadMessageFormat1);
  }

  *end++ = '\0';
  hex = msg;
  len = end - msg - 1;

  if (msg_len >= 2 && end[-2] == '\r')
  {
    end[-2] = '\0';
    len--;
  }

  /* Remove spaces on the left and on the right.
   */
  if (!strcmp((const char*)hex, MODES_RAW_HEART_BEAT))
  {
    // Got heart-beat signal
    return (MsgHeartBeat);
  }

  while (len && isspace(hex[len-1]))
  {
    hex [len-1] = '\0';
    len--;
  }
  while (isspace(*hex))
  {
    hex++;
    len--;
  }

  /* Check it's format.
   */
  if (len < 2)
  {
    // Got Empty Message
    return (BadMessageEmpty2);
  }

  if (hex[0] != '*' || !memchr(msg, ';', len))
  {
    // Got Bad Message
    return (BadMessageFormat2);
  }

  /* Turn the message into binary.
   */
  hex++;     /* Skip `*` and `;` */
  len -= 2;

  if (len > 2*MODES_LONG_MSG_BYTES)   /* Too long message (> 28 bytes)... broken. */
  {
    // Message Too Long
    return (BadMessageTooLong);
  }

  for (j = 0; j < len; j += 2)
  {
    int high = hex_digit_val (hex[j]);
    int low  = hex_digit_val (hex[j+1]);

    if (high == -1 || low == -1)
    {
      // Got Bad Message High Low
      return (BadMessageHighLow);
    }
    bin_msg[j/2] = (high << 4) | low;
  }

  decode_modeS_message (mm, bin_msg);
  if (mm->CRC_ok) return HaveMsg;

  return (CRCError);
 }

 /**
 * Decode a raw Mode S message demodulated as a stream of bytes by `detect_modeS()`.
 *
 * And split it into fields populating a `modeS_message` structure.
 */
static int decode_modeS_message (modeS_message *mm, const uint8_t *_msg)
{
  uint32_t    CRC;   /* Computed CRC, used to verify the message CRC. */
  const char *AIS_charset = "?ABCDEFGHIJKLMNOPQRSTUVWXYZ????? ???????????????0123456789??????";
  uint8_t    *msg;
  bool        check_imf = false;

  memset (mm, '\0', sizeof(*mm));

  /* Work on our local copy
   */
  memcpy (mm->msg, _msg, sizeof(mm->msg));
  msg = mm->msg;

  /* Get the message type ASAP as other operations depend on this
   */
  mm->msg_type = msg[0] >> 3;    /* Downlink Format */
  mm->msg_bits = modeS_message_len_by_type (mm->msg_type);
  mm->CRC      = CRC_get (msg, mm->msg_bits);
  CRC = CRC_check (msg, mm->msg_bits);

  /* Check CRC and fix single bit errors using the CRC when
   * possible (DF 11 and 17).
   */
  mm->error_bit = -1;    /* No error */
  mm->CRC_ok = (mm->CRC == CRC);

  if (!mm->CRC_ok && error_correct_1 && (mm->msg_type == 11 || mm->msg_type == 17))
  {
    mm->error_bit = fix_single_bit_errors (msg, mm->msg_bits);
    if (mm->error_bit != -1)
    {
      mm->CRC    = CRC_check (msg, mm->msg_bits);
      mm->CRC_ok = true;
      // Removed DP Modes.stat.single_bit_fix++;
    }
    else if (error_correct_2 && mm->msg_type == 17 && (mm->error_bit = fix_two_bits_errors(msg, mm->msg_bits)) != -1)
    {
      mm->CRC    = CRC_check (msg, mm->msg_bits);
      mm->CRC_ok = true;
      // Removed DP Modes.stat.two_bits_fix++;
    }
  }

  /* Note: most of the other computation happens **after** we fix the single bit errors.
   * Otherwise we would need to recompute the fields again.
   */
  mm->ca = msg[0] & 7;        /* Responder capabilities. */

  /* ICAO address
   */
  mm->AA [0] = msg [1];
  mm->AA [1] = msg [2];
  mm->AA [2] = msg [3];

  /* DF17 type (assuming this is a DF17, otherwise not used)
   */
  mm->ME_type    = msg[4] >> 3;      /* Extended squitter message type. */
  mm->ME_subtype = msg[4] & 7;       /* Extended squitter message subtype. */

  /* Fields for DF4,5,20,21
   */
  mm->flight_status = msg[0] & 7;         /* Flight status for DF4,5,20,21 */
  mm->DR_status = msg[1] >> 3 & 31;       /* Request extraction of downlink request. */
  mm->UM_status = ((msg[1] & 7) << 3) |   /* Request extraction of downlink request. */
                  (msg[2] >> 5);

  /*
   * In the squawk (identity) field bits are interleaved like this:
   * (message bit 20 to bit 32):
   *
   * C1-A1-C2-A2-C4-A4-ZERO-B1-D1-B2-D2-B4-D4
   *
   * So every group of three bits A, B, C, D represent an integer
   * from 0 to 7.
   *
   * The actual meaning is just 4 octal numbers, but we convert it
   * into a base ten number that happens to represent the four octal numbers.
   *
   * For more info: http://en.wikipedia.org/wiki/Gillham_code
   */
  {
    int a, b, c, d;

    a = ((msg[3] & 0x80) >> 5) |
        ((msg[2] & 0x02) >> 0) |
        ((msg[2] & 0x08) >> 3);
    b = ((msg[3] & 0x02) << 1) |
        ((msg[3] & 0x08) >> 2) |
        ((msg[3] & 0x20) >> 5);
    c = ((msg[2] & 0x01) << 2) |
        ((msg[2] & 0x04) >> 1) |
        ((msg[2] & 0x10) >> 4);
    d = ((msg[3] & 0x01) << 2) |
        ((msg[3] & 0x04) >> 1) |
        ((msg[3] & 0x10) >> 4);
    mm->identity = a*1000 + b*100 + c*10 + d;
  }

  /* DF 11 & 17: try to populate our ICAO addresses whitelist.
   * DFs with an AP field (XORed addr and CRC), try to decode it.
   */
  if (mm->msg_type != 11 && mm->msg_type != 17)
  {
    /* Check if we can check the checksum for the Downlink Formats where
     * the checksum is XORed with the aircraft ICAO address. We try to
     * brute force it using a list of recently seen aircraft addresses.
     */
    if (brute_force_AP(msg, mm))
    {
      /* We recovered the message, mark the checksum as valid.
       */
      mm->CRC_ok = true;
    }
    else
      mm->CRC_ok = false;
  }
  else
  {
    /* If this is DF 11 or DF 17 and the checksum was ok, we can add this address
     * to the list of recently seen addresses.
     */
    if (mm->CRC_ok && mm->error_bit == -1)
       ICAO_cache_add_address (aircraft_get_addr(mm->AA[0], mm->AA[1], mm->AA[2]));
  }

  /* Decode 13 bit altitude for DF0, DF4, DF16, DF20
   */
  if (mm->msg_type == 0 || mm->msg_type == 4 || mm->msg_type == 16 || mm->msg_type == 20)
     mm->altitude = decode_AC13_field (msg, &mm->unit);

  /** Decode extended squitter specific stuff.
   */
  if (mm->msg_type == 17)
  {
    /* Decode the extended squitter message.
     */
    if (mm->ME_type >= 1 && mm->ME_type <= 4)
    {
      /* Aircraft Identification and Category
       */
      mm->aircraft_type = mm->ME_type - 1;
      mm->flight [0] = AIS_charset [msg[5] >> 2];
      mm->flight [1] = AIS_charset [((msg[5] & 3) << 4) | (msg[6] >> 4)];
      mm->flight [2] = AIS_charset [((msg[6] & 15) <<2 ) | (msg[7] >> 6)];
      mm->flight [3] = AIS_charset [msg[7] & 63];
      mm->flight [4] = AIS_charset [msg[8] >> 2];
      mm->flight [5] = AIS_charset [((msg[8] & 3) << 4) | (msg[9] >> 4)];
      mm->flight [6] = AIS_charset [((msg[9] & 15) << 2) | (msg[10] >> 6)];
      mm->flight [7] = AIS_charset [msg[10] & 63];
      mm->flight [8] = '\0';

      char *p = mm->flight + 7;
      while (*p == ' ')    /* Remove trailing spaces */
        *p-- = '\0';

    }
    else if (mm->ME_type >= 9 && mm->ME_type <= 18)
    {
      /* Airborne position Message
       */
      mm->odd_flag = msg[6] & (1 << 2);
      mm->UTC_flag = msg[6] & (1 << 3);
      mm->altitude = decode_AC12_field (msg, &mm->unit);
      mm->raw_latitude  = ((msg[6] & 3) << 15) | (msg[7] << 7) | (msg[8] >> 1); /* Bits 23 - 39 */
      mm->raw_longitude = ((msg[8] & 1) << 16) | (msg[9] << 8) | msg[10];       /* Bits 40 - 56 */
    }
    else if (mm->ME_type == 19 && mm->ME_subtype >= 1 && mm->ME_subtype <= 4)
    {
      /* Airborne Velocity Message
       */
      if (mm->ME_subtype == 1 || mm->ME_subtype == 2)
      {
        mm->EW_dir           = (msg[5] & 4) >> 2;
        mm->EW_velocity      = ((msg[5] & 3) << 8) | msg[6];
        mm->NS_dir           = (msg[7] & 0x80) >> 7;
        mm->NS_velocity      = ((msg[7] & 0x7F) << 3) | ((msg[8] & 0xE0) >> 5);
        mm->vert_rate_source = (msg[8] & 0x10) >> 4;
        mm->vert_rate_sign   = (msg[8] & 0x08) >> 3;
        mm->vert_rate        = ((msg[8] & 7) << 6) | ((msg[9] & 0xFC) >> 2);

        /* Compute velocity and angle from the two speed components.
         * hypot(x,y) == sqrt(x*x+y*y)
         */
        mm->velocity = (int) hypot ((double)mm->NS_velocity, (double)mm->EW_velocity);

        if (mm->velocity)
        {
          int    ewV = mm->EW_velocity;
          int    nsV = mm->NS_velocity;
          double heading;

          if (mm->EW_dir)
             ewV *= -1;
          if (mm->NS_dir)
             nsV *= -1;
          heading = atan2 (ewV, nsV);

          /* Convert to degrees.
           */
          mm->heading = (int) (heading * 360 / TWO_PI);
          mm->heading_is_valid = true;

          /* We don't want negative values but a [0 .. 360> scale.
           */
          if (mm->heading < 0)
             mm->heading += 360;
        }
        else
          mm->heading = 0;
      }
      else if (mm->ME_subtype == 3 || mm->ME_subtype == 4)
      {
        mm->heading_is_valid = msg[5] & (1 << 2);
        mm->heading = (int) (360.0/128) * (((msg[5] & 3) << 5) | (msg[6] >> 3));
      }
    }
    else if (mm->ME_type == 19 && mm->ME_subtype >= 5 && mm->ME_subtype <= 8)
    {
      //Removed DP decode_ES_surface_position (mm, check_imf);
    }
  }
  mm->phase_corrected = false;  /* Set to 'true' by the caller if needed. */
  return (mm->CRC_ok);
}
