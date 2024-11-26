#!/usr/bin/env python3

import sys
import seaborn as sb
import pandas as pd
import matplotlib.pyplot as plt

df = pd.read_csv(sys.stdin)

print(df)

sb.displot( data = df , kde = True, rug = True)

plt.title(f"Full avalanche test qualities distribution for {df.size} runs")
plt.xlabel("Quality")

plt.show()
