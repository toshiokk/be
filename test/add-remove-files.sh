
while : ; do
  for todo in add del ; do
    for pref in a z ; do
      for i in $( seq 10 19 ) ; do
        fn=${pref}file$i
        if [ "$todo" = add ]; then
          echo -n "a $fn "
          touch $fn
        else
          echo -n "d $fn "
          rm $fn
        fi
        sleep 0.1
      done
      echo
      sleep 2
    done
  done
done
