#ifndef __NONCOPYABLE_H__
#define __NONCOPYABLE_H__

class noncopyable
{
	public:
		noncopyable() {}
		~noncopyable() {}
		noncopyable(const noncopyable& ) = delete; /* since c++11 */
		noncopyable& operator=(const noncopyable& ) = delete; /* since c++11 */
};

#endif
