// Counts the number of arguments that return true (up to 8)
#define COUNT_TRUE(...) COUNT_TRUE_IMPL(__VA_ARGS__, 0,0,0,0,0,0,0,0)

#define COUNT_TRUE_IMPL(a,b,c,d,e,f,g,h,...) \
 ( \
  ((a)?1:0) + \
  ((b)?1:0) + \
  ((c)?1:0) + \
  ((d)?1:0) + \
  ((e)?1:0) + \
  ((f)?1:0) + \
  ((g)?1:0) + \
  ((h)?1:0) \
 )
