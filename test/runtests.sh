for file in ./*.qry
do
  echo "$file"
  ../env/posix/querycsv "$file" > /dev/null
done
