echo "change Application Release Level -- APP_REL_LVL definition in appdef.h"
echo "Usage: $0 [0|1|2]"
echo "  0: experimental release"
echo "  1: test         release"
echo "  2: official     release"

sed_define()
{
  mk_backup_file appdefs.h
  sed "s/define APP_REL_LVL ./define APP_REL_LVL $1/g" appdefs.h >appdefs.h~~~~
  if ! diff appdefs.h appdefs.h~~~~; then
    cp -av appdefs.h~~~~ appdefs.h
  fi
}

{
  if [ "$1" = 0 -o "$1" = 1 -o "$1" = 2 ]; then
    sed_define $1
  fi
  grep "define APP_REL_LVL" appdefs.h
} | grep APP_REL_LVL
