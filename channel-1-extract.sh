printf x > temp-dump.txt
cat $1 >> temp-dump.txt
xxd temp-dump.txt | awk '{print $2} {print $4} {print $6} {print $8}' > temp.txt
python3 ./signify.py
rm temp.txt
rm temp-dump.txt
