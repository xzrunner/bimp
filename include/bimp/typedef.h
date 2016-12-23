#ifndef _BIMP_DEFINES_H_
#define _BIMP_DEFINES_H_

namespace bimp
{

#ifndef ALIGN_4BYTE
	#define ALIGN_4BYTE(x) (((x) + 3) & ~3)
#endif // ALIGN_4BYTE

}

#endif // _BIMP_DEFINES_H_