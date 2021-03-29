import numpy as np

from sklearn import model_selection
from sklearn.tree import DecisionTreeClassifier
from sklearn.ensemble import RandomForestClassifier
from sklearn.svm import SVC
from sklearn.multiclass import OneVsRestClassifier
from sklearn.metrics import accuracy_score
from sklearn.metrics import confusion_matrix
from sklearn.metrics import r2_score
from sklearn.metrics import mean_squared_error

import matplotlib.pyplot as plt
import matplotlib.dates as mdates
import pandas as pd

import datetime
from math import sqrt

# Actual range of data: 0 - 500ish
def map_to_labels(x):
    if x in range(0, 70):
        return 1
    elif x in range(70, 150):
        return 2
    else:
        return 3

# Read in data as a pandas DataFrame
df = pd.read_csv("cyclists_vs_temperature.csv")
df["Labels"] = df["Cyclists"].map(map_to_labels)

# Parse dates to datetime format
df["Date"] = pd.to_datetime(df["Date"], format="%Y-%m-%d %H:%M")
df["Date"] = df["Date"].dt.strftime("%d.%m.%Y %H")

# Parse separate features from datetime
df["hour"] = pd.DatetimeIndex(df["Date"]).hour
df["year"] = pd.DatetimeIndex(df["Date"]).year
df["month"] = pd.DatetimeIndex(df["Date"]).month
df["day"] = pd.DatetimeIndex(df["Date"]).day
df["weekday"] = pd.DatetimeIndex(df["Date"]).weekday

# Create dummy encoding to avoid duplicate data
df = pd.get_dummies(df, columns=["hour"], drop_first=True, prefix="hour")
df = pd.get_dummies(df, columns=["year"], drop_first=True, prefix="year")
df = pd.get_dummies(df, columns=["month"], drop_first=True, prefix="month")
df = pd.get_dummies(df, columns=["day"], drop_first=True, prefix="day")
df = pd.get_dummies(df, columns=["weekday"], drop_first=True, prefix="wday")

# Drop unparsed datetime, absolute cyclist values and null rows
df = df.drop(["Date"], axis = 1)
df = df.drop(["Cyclists"], axis = 1)
df = df.dropna(how="any", axis = 0)

print(df.info())

df_1 = df.iloc[:55000,:]
df_2 = df.iloc[55001:,:]

# Training and validation
y = df_1["Labels"]
X = df_1.drop(["Labels"], axis = 1)

X_train, X_val, y_train, y_val = model_selection.train_test_split(X, y)

# Testing
y_test = df_2["Labels"]
X_test = df_2.drop(["Labels"], axis = 1)


def train_and_predict(clf_type):
    
    if clf_type == "dtree":
        clf = DecisionTreeClassifier(criterion="entropy", random_state=3)
    else:
        clf = RandomForestClassifier(criterion="entropy", n_estimators=200, oob_score=True, random_state=69, n_jobs=4, verbose=1)

    clf.fit(X_train, y_train)
    y_train_pred = clf.predict(X_train)
    y_val_pred = clf.predict(X_val)
    y_test_pred = clf.predict(X_test)

    print(clf_type)
    print("-----------------------------------------------------------------------")
    print(pd.DataFrame(
        confusion_matrix(y_val, y_val_pred),
        columns=["Predicted 1", "Predicted 2", "Predicted 3"],
        index=["True 1", "True 2", "True 3"]
    ))
    print("Train accuracy:")
    print(accuracy_score(y_train, y_train_pred))
    print("Validation accuracy:")
    print(accuracy_score(y_val, y_val_pred))
    print("Test accuracy:")
    print(accuracy_score(y_test, y_test_pred))
    print("-----------------------------------------------------------------------")
    

train_and_predict("dtree")
train_and_predict("rforest")
