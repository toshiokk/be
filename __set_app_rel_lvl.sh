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
  grep "define APP_REL_LVL " applvl.h
  sed "s/define APP_REL_LVL ./define APP_REL_LVL $1/g" applvl.h >applvl.h~
  rm -f applvl.h0
  if ! diff applvl.h applvl.h~ >/dev/null; then
    cp -av applvl.h applvl.h0  # backup
    cp -v applvl.h~ applvl.h
  fi
elif [ "$1" = - ]; then
  if [ -f applvl.h0 ] && ! diff applvl.h applvl.h0 >/dev/null; then
    cp -v applvl.h0 applvl.h  # recover
  fi
else
  if [ -n "$1" ]; then
    show_help
  fi
fi

grep "define APP_REL_LVL " applvl.h
