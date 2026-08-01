#pragma once

namespace nsIR
{
    enum IRprotocols2
    {
        UNKNOWN = -1,
        UNUSED = 0,
        RC5,
        RC6,
        NEC,
        SONY,
        PANASONIC, // (5)
        JVC,
        SAMSUNG,
        WHYNTER,
        AIWA_RC_T501,
        LG, // (10)
        SANYO,
        MITSUBISHI,
        DISH,
        SHARP,
        COOLIX, // (15)
        DAIKIN,
        DENON,
        KELVINATOR,
        SHERWOOD,
        MITSUBISHI_AC, // (20)
        RCMM,
        SANYO_LC7461,
        RC5X,
        GREE,
        PRONTO, // Technically not a protocol, but an encoding. (25)
        NEC_LIKE,
        ARGO,
        TROTEC,
        NIKAI,
        RAW,         // Technically not a protocol, but an encoding. (30)
        GLOBALCACHE, // Technically not a protocol, but an encoding.
        TOSHIBA_AC,
        FUJITSU_AC,
        MIDEA,
        MAGIQUEST, // (35)
        LASERTAG,
        CARRIER_AC,
        HAIER_AC,
        MITSUBISHI2,
        HITACHI_AC, // (40)
        HITACHI_AC1,
        HITACHI_AC2,
        GICABLE,
        HAIER_AC_YRW02,
        WHIRLPOOL_AC, // (45)
        SAMSUNG_AC,
        LUTRON,
        ELECTRA_AC,
        PANASONIC_AC,
        PIONEER, // (50)
        LG2,
        MWM,
        DAIKIN2,
        VESTEL_AC,
        TECO, // (55)
        SAMSUNG36,
        TCL112AC,
        LEGOPF,
        MITSUBISHI_HEAVY_88,
        MITSUBISHI_HEAVY_152, // 60
        DAIKIN216,
        SHARP_AC,
        GOODWEATHER,
        INAX,
        DAIKIN160, // 65
        NEOCLIMA,
        DAIKIN176,
        DAIKIN128,
        AMCOR,
        DAIKIN152, // 70
        MITSUBISHI136,
        MITSUBISHI112,
        HITACHI_AC424,
        SONY_38K,
        EPSON, // 75
        SYMPHONY,
        HITACHI_AC3,
        DAIKIN64,
        AIRWELL,
        DELONGHI_AC, // 80
        DOSHISHA,
        MULTIBRACKETS,
        CARRIER_AC40,
        CARRIER_AC64,
        HITACHI_AC344, // 85
        CORONA_AC,
        MIDEA24,
        ZEPEAL,
        SANYO_AC,
        VOLTAS, // 90
        METZ,
        TRANSCOLD,
        TECHNIBEL_AC,
        MIRAGE,
        ELITESCREENS, // 95
        PANASONIC_AC32,
        MILESTAG2,
        ECOCLIM,
        XMP,
        TRUMA, // 100
        HAIER_AC176,
        TEKNOPOINT,
        KELON,
        TROTEC_3550,
        SANYO_AC88, // 105
        BOSE,
        ARRIS,
        RHOSS,
        AIRTON,
        COOLIX48, // 110
        HITACHI_AC264,
        KELON168,
        HITACHI_AC296,
        DAIKIN200,
        HAIER_AC160, // 115
        CARRIER_AC128,
        TOTO,
        CLIMABUTLER,
        TCL96AC,
        BOSCH144, // 120
        SANYO_AC152,
        DAIKIN312,
        GORENJE,
        WOWWEE,
        CARRIER_AC84, // 125
        YORK,
        /* Now not standard IRremote8266 protocol*/
        // SONY_R2 = 1000,
        // MCE_T = 1001,
        //  Add new entries before this one, and update it to point to the last entry.
        //  kLastDecodeType = IR_PROTOCOL_MCE // Not used for now
    };
} // Namespace

// --- Protocol de la librarie 'IRremoteESP8266' ----------------:
// enum decode_type_t {
//   UNKNOWN = -1,
//   UNUSED = 0,
//   RC5,
//   RC6,
//   NEC,
//   SONY,
//   PANASONIC,  // (5)
//   JVC,
//   SAMSUNG,
//   WHYNTER,
//   AIWA_RC_T501,
//   LG,  // (10)
//   SANYO,
//   MITSUBISHI,
//   DISH,
//   SHARP,
//   COOLIX,  // (15)
//   DAIKIN,
//   DENON,
//   KELVINATOR,
//   SHERWOOD,
//   MITSUBISHI_AC,  // (20)
//   RCMM,
//   SANYO_LC7461,
//   RC5X,
//   GREE,
//   PRONTO,  // Technically not a protocol, but an encoding. (25)
//   NEC_LIKE,
//   ARGO,
//   TROTEC,
//   NIKAI,
//   RAW,  // Technically not a protocol, but an encoding. (30)
//   GLOBALCACHE,  // Technically not a protocol, but an encoding.
//   TOSHIBA_AC,
//   FUJITSU_AC,
//   MIDEA,
//   MAGIQUEST,  // (35)
//   LASERTAG,
//   CARRIER_AC,
//   HAIER_AC,
//   MITSUBISHI2,
//   HITACHI_AC,  // (40)
//   HITACHI_AC1,
//   HITACHI_AC2,
//   GICABLE,
//   HAIER_AC_YRW02,
//   WHIRLPOOL_AC,  // (45)
//   SAMSUNG_AC,
//   LUTRON,
//   ELECTRA_AC,
//   PANASONIC_AC,
//   PIONEER,  // (50)
//   LG2,
//   MWM,
//   DAIKIN2,
//   VESTEL_AC,
//   TECO,  // (55)
//   SAMSUNG36,
//   TCL112AC,
//   LEGOPF,
//   MITSUBISHI_HEAVY_88,
//   MITSUBISHI_HEAVY_152,  // 60
//   DAIKIN216,
//   SHARP_AC,
//   GOODWEATHER,
//   INAX,
//   DAIKIN160,  // 65
//   NEOCLIMA,
//   DAIKIN176,
//   DAIKIN128,
//   AMCOR,
//   DAIKIN152,  // 70
//   MITSUBISHI136,
//   MITSUBISHI112,
//   HITACHI_AC424,
//   SONY_38K,
//   EPSON,  // 75
//   SYMPHONY,
//   HITACHI_AC3,
//   DAIKIN64,
//   AIRWELL,
//   DELONGHI_AC,  // 80
//   DOSHISHA,
//   MULTIBRACKETS,
//   CARRIER_AC40,
//   CARRIER_AC64,
//   HITACHI_AC344,  // 85
//   CORONA_AC,
//   MIDEA24,
//   ZEPEAL,
//   SANYO_AC,
//   VOLTAS,  // 90
//   METZ,
//   TRANSCOLD,
//   TECHNIBEL_AC,
//   MIRAGE,
//   ELITESCREENS,  // 95
//   PANASONIC_AC32,
//   MILESTAG2,
//   ECOCLIM,
//   XMP,
//   TRUMA,  // 100
//   HAIER_AC176,
//   TEKNOPOINT,
//   KELON,
//   TROTEC_3550,
//   SANYO_AC88,  // 105
//   BOSE,
//   ARRIS,
//   RHOSS,
//   AIRTON,
//   COOLIX48,  // 110
//   HITACHI_AC264,
//   KELON168,
//   HITACHI_AC296,
//   DAIKIN200,
//   HAIER_AC160,  // 115
//   CARRIER_AC128,
//   TOTO,
//   CLIMABUTLER,
//   TCL96AC,
//   BOSCH144,  // 120
//   SANYO_AC152,
//   DAIKIN312,
//   GORENJE,
//   WOWWEE,
//   CARRIER_AC84,  // 125
//   YORK,
//   // Add new entries before this one, and update it to point to the last entry.
//   kLastDecodeType = YORK,
// };

// --- Protocol de la librarie 'IRremote' ----------------:
// typedef enum {
//     UNKNOWN = 0,
//     PULSE_WIDTH,
//     PULSE_DISTANCE,
//     PULSE_DISTANCE_WIDTH,
//     APPLE,
//     DENON,
//     JVC,
//     LG,
//     LG2,
//     NEC,
//     NEC2, /* 10 NEC with full frame as repeat */
//     ONKYO,
//     PANASONIC,
//     KASEIKYO,
//     KASEIKYO_DENON,
//     KASEIKYO_SHARP,
//     KASEIKYO_JVC,
//     KASEIKYO_MITSUBISHI,
//     RC5,
//     RC6,
//     SAMSUNG, /* 20*/
//     SAMSUNGLG,
//     SAMSUNG48,
//     SHARP,
//     SONY,
//     /* Now the exotic protocols */
//     BANG_OLUFSEN,
//     BOSEWAVE,
//     LEGO_PF,
//     MAGIQUEST,
//     WHYNTER, /* 30 */
//     FAST
// } decode_type_t;