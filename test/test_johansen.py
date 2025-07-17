import statsmodels.api as sm
from statsmodels.tsa.vector_ar.vecm import coint_johansen
import warnings
import numpy as np
import pandas as pd


stock1 = pd.read_csv("data/test_data.csv");
stock2 = pd.read_csv("data/test_data2.csv");

asset1 = []
data = np.column_stack((stock1['data'], stock2['data']))

with warnings.catch_warnings():
    warnings.simplefilter("ignore")
    for i in range(-1, 2):
        result = coint_johansen(data, det_order=i, k_ar_diff=1)
        print(f"det_order={i}: Eigenvalues:", result.eig)
