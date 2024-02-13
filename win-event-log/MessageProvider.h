 // Header section.
 // The following are the categories of events.
 // MessageIdTypedef=WORD
 // MessageId=0x1
 // SymbolicName=ANY_CATEGORY
 // Language=English
 // Any Events
 // .
 // 
 // The following are the message definitions.
//
//  Values are 32 bit values laid out as follows:
//
//   3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1
//   1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
//  +---+-+-+-----------------------+-------------------------------+
//  |Sev|C|R|     Facility          |               Code            |
//  +---+-+-+-----------------------+-------------------------------+
//
//  where
//
//      Sev - is the severity code
//
//          00 - Success
//          01 - Informational
//          10 - Warning
//          11 - Error
//
//      C - is the Customer code flag
//
//      R - is a reserved bit
//
//      Facility - is the facility code
//
//      Code - is the facility's status code
//
//
// Define the facility codes
//
#define FACILITY_SYSTEM                  0x0
#define FACILITY_RUNTIME                 0x2
#define FACILITY_STUBS                   0x3
#define FACILITY_IO_ERROR_CODE           0x4


//
// Define the severity codes
//
#define STATUS_SEVERITY_SUCCESS          0x0
#define STATUS_SEVERITY_INFORMATIONAL    0x1
#define STATUS_SEVERITY_WARNING          0x2
#define STATUS_SEVERITY_ERROR            0x3


//
// MessageId: MSG_SUCCESS_MESSAGE
//
// MessageText:
//
// %1
//
#define MSG_SUCCESS_MESSAGE              ((DWORD)0x00020100L)

//
// MessageId: MSG_INFO_MESSAGE
//
// MessageText:
//
// %1
//
#define MSG_INFO_MESSAGE                 ((DWORD)0x00020101L)

//
// MessageId: MSG_WARNING_MESSAGE
//
// MessageText:
//
// %1
//
#define MSG_WARNING_MESSAGE              ((DWORD)0x00020102L)

//
// MessageId: MSG_ERROR_MESSAGE
//
// MessageText:
//
// %1
//
#define MSG_ERROR_MESSAGE                ((DWORD)0xC0020103L)

