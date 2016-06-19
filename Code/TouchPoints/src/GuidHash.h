#pragma once
#include <guid.h>

//from:
//https://github.com/graeme-hill/crossguid/issues/4
//if performance becomes an issue, refer to:
//https://github.com/graeme-hill/crossguid/pull/6
//stringstreaming might be more expensive than cstrings
namespace std
{
	template<>
	struct hash<Guid>
	{
		size_t operator()(const Guid &guid) const
		{
			stringstream ss;
			ss << guid;
			return hash<string>()(ss.str());
		}
	};
}
