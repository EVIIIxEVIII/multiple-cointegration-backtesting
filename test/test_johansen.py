import statsmodels.api as sm
from statsmodels.tsa.vector_ar.vecm import coint_johansen
import warnings
import numpy as np
import pandas as pd


stock1 = pd.read_csv("data/test_data.csv");
stock2 = pd.read_csv("data/test_data2.csv");

asset1 = []
data = np.column_stack((stock1['data'], stock2['data']))

result = coint_johansen(data, det_order=0, k_ar_diff=1)
print("Critical values (trace):", result.cvt)
print("Critical values (max eigen):", result.cvm)
print("----------------------------------------");
