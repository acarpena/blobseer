#include "Copyright.h"
/* General Form */
/*
#ifdef <ArchiName>
#define ANDPT 0x1fff
COMMENT: and to get  the index of the walk table 
Rule : ANDPT+1= 2*cachesize (measured in 4 bytes words
#endif
*/

#ifdef UII
#define ANDPT 0x1fff
#endif

#ifdef P4
#ifdef WIN
#ifndef  NVC
#define VCP4
#undef  P4
#endif
#endif
#define ANDPT 0xfff
#endif

#ifdef CORE2
#ifdef WIN
#ifndef  NVC
#define VCP4
#undef  P4
#endif
#endif
#define ANDPT 0xfff
#endif

#ifdef PIII
#ifdef WIN
#ifndef  NVC
#define VCPIII
#undef PIII
#endif
#endif
#define ANDPT 0x1fff
#endif

#ifdef UIII
#define ANDPT 0x7fff
#endif

#ifdef ATHLON
#ifdef WIN
#ifndef  NVC
#define VCATHLON
#undef ATHLON
#endif
#endif
#define ANDPT 0x7fff
#endif



#ifdef ITANIUM
#define ANDPT 0x1fff
#define ADR64
#endif

#ifdef G4
#define ANDPT 0x3fff
#endif
