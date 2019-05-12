#ifndef _STATIC_ALLOC_ACTIVATOR_
#define _STATIC_ALLOC_ACTIVATOR_

template<class T, int N = 0, typename... A>
T& Instance(A... a)
{
	static T* obj;
	static uint8_t data[sizeof(T)];
	if (!obj)
	{
		obj = new (data)T(a...);
	}
	return *obj;
}

template<class T, int N = 0, typename... A>
T& InstanceOnce(A... a)
{
    static uint8_t data[sizeof(T)];
    return *new(data)T(a...);
}

#endif
