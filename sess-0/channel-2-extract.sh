printf x > temp-dump.txt
cat $1 >> temp-dump.txt
xxd temp-dump.txt | awk '{print $3} {print $5} {print $7} {print $9}' > temp.txt
python3 ./signify.py
rm temp.txt
rm temp-dump.txt
