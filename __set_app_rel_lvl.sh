## change release level in 'appdefs.h'

show_help()
{
  echo "change Application Release Level -- APP_REL_LVL definition in appdef.h"
  echo "Usage: $0 [0|1|2|-]"
  echo " e.g.: $0 0        # set to experimental-release"
  echo " e.g.: $0 1        # set to test-release"
  echo " e.g.: $0 2        # set to official-release"
  echo " e.g.: $0 -        # recover release level"
}

if [ "$1" = 0 ] || [ "$1" = 1 ] || [ "$1" = 2 ]; then
  grep "define APP_REL_LVL " appdefs.h
  sed "s/define APP_REL_LVL ./define APP_REL_LVL $1/g" appdefs.h >appdefs.h~
  rm -f appdefs.h0
  if ! diff appdefs.h appdefs.h~ >/dev/null; then
    cp -av appdefs.h appdefs.h0  # backup
    cp -av appdefs.h~ appdefs.h
  fi
elif [ "$1" = - ]; then
  if [ -f appdefs.h0 ] && ! diff appdefs.h appdefs.h0 >/dev/null; then
    cp -av appdefs.h0 appdefs.h  # recover
  fi
else
  if [ -n "$1" ]; then
    show_help
  fi
fi

grep "define APP_REL_LVL " appdefs.h
