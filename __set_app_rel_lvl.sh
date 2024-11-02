show_help()
{
  echo "change Application Release Level -- APP_REL_LVL definition in appdef.h"
  echo "Usage: $0 [0|1|2]"
  echo "  0: experimental release"
  echo "  1: test-1       release"
  echo "  2: official     release"
}

sed_define()
{
  cp -av appdefs.h appdefs.h0
  sed "s/define APP_REL_LVL ./define APP_REL_LVL $1/g" appdefs.h >appdefs.h1
  if ! diff appdefs.h appdefs.h1 >/dev/null; then
    cp -av appdefs.h1 appdefs.h
  fi
}

if [ "$1" = 0 ] || [ "$1" = 1 ] || [ "$1" = 2 ]; then
  grep "define APP_REL_LVL " appdefs.h
  sed_define $1
else
  if [ -n "$1" ]; then
    show_help
  fi
fi
grep "define APP_REL_LVL " appdefs.h
