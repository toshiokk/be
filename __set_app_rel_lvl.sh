show_help()
{
echo "change Application Release Level -- APP_REL_LVL definition in appdef.h"
echo "Usage: $0 [0|1|2|3|4]"
echo "  0: experimental"
echo "  1: test-1       release"
echo "  2: test-2       release"
echo "  3: test-3       release"
echo "  4: official     release"
}

sed_define()
{
  mk_backup_file appdefs.h
  sed "s/define APP_REL_LVL ./define APP_REL_LVL $1/g" appdefs.h >appdefs.h~~~~
  if ! diff appdefs.h appdefs.h~~~~; then
    cp -av appdefs.h~~~~ appdefs.h
  fi
}

if [ "$1" = 0 -o "$1" = 1 -o "$1" = 2 -o "$1" = 3 -o "$1" = 4 ]; then
  sed_define $1
else
  if [ -z "$1" ]; then
    show_help
  fi
fi
echo ""
grep "define APP_REL_LVL " appdefs.h
