cd ..
while : ; do
  mv -f __error~ z__error~
  mv -f __todo.txt~ z__todo.txt~
  echo -n "z"
  sleep 1
  mv -f z__error~ __error~
  mv -f z__todo.txt~ __todo.txt~
  echo -n "."
  sleep 1
done
