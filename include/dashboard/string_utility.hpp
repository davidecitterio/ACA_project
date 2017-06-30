#ifndef DASHBOARD_STRING_UTILITY_HDR
#define DASHBOARD_STRING_UTILITY_HDR

#include <string>


namespace dashboard
{
	template<class T >
	inline typename std::enable_if<std::is_arithmetic<T>::value, std::string>::type
	stringify(const T argument)
	{
		return std::to_string(argument);
	}

	template<class T >
	inline typename std::enable_if<!std::is_arithmetic<T>::value, std::string>::type
	stringify(const T argument)
	{
		return std::string(argument);
	}

	inline std::string stringify(const std::string argument)
	{
		return argument;
	}

	template<class T>
	inline std::string compose(const T last_term)
	{
		return stringify(last_term);
	}

	template<class T, class ...Y>
	inline std::string compose(const T mid_term, const Y... arguments)
	{
		return stringify(mid_term) + compose(arguments...);
	}


}






#endif // DASHBOARD_STRING_UTILITY_HDR
