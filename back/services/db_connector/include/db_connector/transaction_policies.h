#ifndef H_A9C9D47B_F899_48C4_8899_6F3F35EEC205
#define H_A9C9D47B_F899_48C4_8899_6F3F35EEC205

#include <instrumental/serialized_enum.h>

DEFINE_ENUM_WITH_SERIALIZATION(srv::db, Isolation, ReadCommited, RepeatableRead, Serializable);
DEFINE_ENUM_WITH_SERIALIZATION(srv::db, WritePolicy, ReadOnly, ReadWrite);

#endif  // H_A9C9D47B_F899_48C4_8899_6F3F35EEC205