# "update-system-clipboard.sh"
# on Linux
# - Start and continuously Run this script at system startup from GUI process
# - monitor BE clipboard file and update system clipboard if updated
# on WSL
# - Run this script every time you cut/copy text
if [ ! -d /mnt/c ]; then
  # Linux: continue checking update of $HOME/.be/clipboard
  prevsec=0
  while : ; do
    echo -n "."
    sec=`date -r $HOME/.be/clipboard +%s`
    if [ "$prevsec" -lt "$sec" ]; then
      prevsec=$sec
      cat $HOME/.be/clipboard | xclip
      cat $HOME/.be/clipboard
    fi
    sleep 1
  done
else
  # WSL: send text in file $HOME/.be/clipboard to system clipboard
  clip.exe <$HOME/.be/clipboard
  echo -n "Text in the 'BE clipboard file' was copied to the 'system clipboard'"
fi
