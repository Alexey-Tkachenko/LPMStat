#ifndef _PLACEMENT_NEW_
#define _PLACEMENT_NEW_

inline void* operator new (unsigned size, void* ptr) noexcept
{
	return ptr;
}

#endif
