
#ifndef INT_TYPES_H
#define INT_TYPES_H

//vc2008 does not have <stdint.h>:
#if defined(_MSC_VER) && (_MSC_VER <= 1500)
#include <boost/cstdint.hpp>
using boost::int8_t;
using boost::uint8_t;
using boost::int32_t;
using boost::uint32_t;
using boost::int64_t;
using boost::uint64_t;
#else
#include <stdint.h>
#endif

#endif//#ifndef INT_TYPES_H

