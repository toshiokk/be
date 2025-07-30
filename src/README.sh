echo "converting markdown to html ..."
rm -f README.html
pandoc -f markdown -t html -c github.css README.md >README.html
if [ -f README.html ]; then
  echo "README.html made"
fi
