import statsmodels.api as sm
from statsmodels.tsa.vector_ar.vecm import coint_johansen
from arch.unitroot import cointegration
import numpy as np
import pandas as pd

np.random.seed(0)

T = 50

epsilon = np.random.normal(0, 1, T)
A = np.cumsum(epsilon)

noise = np.random.normal(0, 0.5, T)
B = A + noise

C = np.cumsum(np.random.normal(0, 1, T))

df = pd.DataFrame({'A': A, 'B': B, 'C': C})
df.to_csv("data/test_data.csv", index=False)

asset1 = []

data = np.column_stack((df['A'], df['B'], df['C']))

result = coint_johansen(data, det_order=0, k_ar_diff=1)
eigvals, lr1, lr2, cvt, cvm, beta = cointegration.johansen(X, k_ar_diff=1, det_order=0)

print("Eigenvalues:", result.eig)
print("Eigenvalues:", eigvals)
print("Trace statistics:", result.lr1)
print("Max eigenvalue statistics:", result.lr2)
print("Critical values (trace):", result.cvt)
print("Critical values (max eigen):", result.cvm)
print("Cointegration vectors (beta):", result.evec)

