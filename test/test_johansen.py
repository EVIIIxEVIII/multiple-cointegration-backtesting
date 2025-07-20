import statsmodels.api as sm
from statsmodels.tsa.vector_ar.vecm import coint_johansen
import warnings
import numpy as np
import pandas as pd


stock1 = pd.read_csv("data/BTC_USD.csv");
stock2 = pd.read_csv("data/ETH_USD.csv");

asset1 = []
data = np.column_stack((stock1['close'], stock2['close']))

result = coint_johansen(data, det_order=1, k_ar_diff=1)
print(f"Eigenvalues:", result.eig)
print("Trace statistics:", result.lr1)
print("Max eigenvalue statistics:", result.lr2)
print("Critical values (trace):", result.cvt)
print("Critical values (max eigen):", result.cvm)
print("Cointegration vectors (beta):", result.evec)
