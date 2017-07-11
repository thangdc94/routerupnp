/**
 * @file mappingrule.h
 * @brief mapping rule struct
 * @details Mapping rule struct is parameters that need for port mapping
 * 
 * @author Pham Ngoc Thang (thangdc94)
 * @bug No known bug
 */

#ifndef __MAPPING_RULE_H_
#define __MAPPING_RULE_H_

/** Using X Macro technique for enum to string */
#define SUPPORTED_PROTOCOLS(X) \
    X(UDP, "UDP")              \
    X(TCP, "TCP")

/** Generate enum */
#define PROTOCOL_ENUM(NAME, TEXT) NAME,

/** UPnP Supported Protocols  */
typedef enum _SupportedProtocol_t {
    SUPPORTED_PROTOCOLS(PROTOCOL_ENUM)
} SupportedProtocol_t;
#undef PROTOCOL_ENUM

/** Mapping input for AddPortMapping */
typedef struct _MappingRule_t
{
    char eport[6];             /**< external port */
    char iport[6];             /**< internal port */
    SupportedProtocol_t proto; /**< protocol will be mapped */
} MappingRule_t;

/** Generate switch case for ::get_proto_str() */
#define PROTOCOL_TEXT(NAME, TEXT) \
    case NAME:                    \
        return TEXT;

/**
 * @brief Get Protocol string from SupportedProtocol_t
 * @details Get Protocol string from SupportedProtocol_t. This function will use
 * define #SUPPORTED_PROTOCOLS(X)
 *
 * @param[in] proto protocol you want to get string
 * @return string of protocol
 */
static inline const char *get_proto_str(SupportedProtocol_t proto)
{
    switch (proto)
    {
        SUPPORTED_PROTOCOLS(PROTOCOL_TEXT)
    }
    return "Unknown";
}
#undef PROTOCOL_TEXT

#endif //__MAPPING_RULE_H_
