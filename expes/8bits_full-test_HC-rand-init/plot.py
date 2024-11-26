#!/usr/bin/env python3

import sys
import seaborn as sb
import pandas as pd
import matplotlib.pyplot as plt

df = pd.Series([float(i) for i in sys.stdin])

print(df)

imin = df.idxmin()

print("Minimum at seed=", imin+1,"=",df[imin])

sb.displot( data = df , kde = True, rug = True)

plt.title(f"Full avalanche test qualities distribution for {df.size} runs")
plt.xlabel("Quality")

plt.show()

