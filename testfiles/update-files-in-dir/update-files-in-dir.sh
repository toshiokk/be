while : ; do
  cp -v ../*.log ../*.png .
  sleep 3
  cp -v ../*.txt .
  sleep 3
  rm -v *.txt
  sleep 3
  rm -v *.log *.png
  sleep 3
done
