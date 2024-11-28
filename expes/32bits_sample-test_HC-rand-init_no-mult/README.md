
Run the experiment:
```sh
./run.sh | tee run.log
```

Plot the distribution of qualities:
```sh
cat results/<DATE>/*.yaml | grep quality | cut -d: -f2 | ./plot.py
```

