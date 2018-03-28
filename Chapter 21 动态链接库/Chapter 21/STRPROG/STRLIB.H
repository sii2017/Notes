#ifdef __cplusplus
#define EXPORT extern "C" __declspec(dllexport)
#else
#define EXPORT __declspec(dllexport)
#endif

#define MAX_STRINGS 256
#define MAX_LENGTH 63

//The callback function type definition uses generic strings
typedef BOOL (CALLBACK* GETSTRCB)(PCTSTR, PVOID);
//Each function has ANSI and Unicode versions
EXPORT BOOL CALLBACK AddStringA(PCSTR);
EXPORT BOOL CALLBACK AddStringW(PCWSTR);


EXPORT BOOL CALLBACK DeleteStringA(PCSTR);
EXPORT BOOL CALLBACK DeleteStringW(PCWSTR);

EXPORT int CALLBACK GetStringA(GETSTRCB, PVOID);
EXPORT int CALLBACK GetStringW(GETSTRCB, PVOID);

//Use the correct version depending on the UNICODE identifier
#ifdef UNICODE
#define AddString AdddStringW
#define DeleteString DeleteStringW
#define GetStrings GetStringsW
#else
#define AddString AddStringA
#define DeleteString DeleteStringA
#define GetStrings GetStringA
#endif