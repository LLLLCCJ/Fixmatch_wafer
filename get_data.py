# -*- coding: utf-8 -*-
"""get_data.ipynb

Automatically generated by Colaboratory.

Original file is located at
    https://colab.research.google.com/drive/1D02Kndld56aiqqtH5Cw8Or-1kIhqlW5K
"""

# Commented out IPython magic to ensure Python compatibility.
# Load in our libraries
import pandas as pd
import numpy as np
import sklearn

import matplotlib.pyplot as plt


import numpy as np


# %matplotlib inline
import torch
import torchvision
from PIL import Image

import torch.nn as nn
import torch.nn.functional as F
import torch.optim as optim
import torch.nn.init
from torch.utils.data import Dataset, DataLoader,TensorDataset,random_split,SubsetRandomSampler, ConcatDataset
import torch
from PIL import Image
import torchvision.transforms as transforms

from sklearn import model_selection
from sklearn.model_selection import train_test_split

import argparse
from args import parse_args

from RandAugment import RandAugment
from RandAugment.augmentations import CutoutDefault
from torch.utils.data import TensorDataset,DataLoader,Dataset
import os
import torchvision.transforms.functional as TF
from PIL import Image

args = parse_args()

#os.chdir('/content/drive/MyDrive/Colab Notebooks/pytorch/WM')

# 비율 유지
def rgb_sw(x): #rgb 추가
  Dim0 = np.size(x, axis=1)
  Dim1 = np.size(x, axis=2)
  new_x = np.zeros((len(x), Dim0, Dim1, 3)) # 0으로만 이루어진 array 생성
  x = torch.unsqueeze(x,-1) # 차원이 1인 차원 생성
  x = x.to(torch.device('cpu'))
  x = x.numpy()
  for w in range(len(x)):
    for i in range(Dim0):
      for j in range(Dim1):
        new_x[w, i, j, int(x[w, i, j])] = 1
  return new_x

def _resize2(data, Dim0, Dim1):
  transTensor = transforms.ToTensor()
  resizer = transforms.Resize((Dim0, Dim1))
  rwm = torch.ones((1, Dim0, Dim1))
  for i in range(len(data)):
    a = transTensor(data[i].astype('int32'))
    a = resizer(a).reshape((1, Dim0, Dim1))
    rwm = torch.cat([rwm, a])
  x = rwm[1:]
  del rwm
  return x

class BasicDataset(Dataset):
  def __init__(self,data,label,transform):
    self.data=data
    self.label=label
    self.transform=transform
  def __len__(self):
    return len(self.label)

  def __getitem__(self, idx):
    sample_X = self.data[idx]
    sample_y = self.label[idx]
    if self.transform:
      sample_X = self.transform(sample_X)
    return sample_X, sample_y, idx

def test_split(labels,test_size):
  labels = np.array(labels)
  train_labeled_idxs = []

  for i in range(9):
      idxs = np.where(labels == i)[0]
      train_labeled_idxs.extend(idxs[:test_size[i]])
  return train_labeled_idxs

def train_labeled_split(labels, n_labeled_per_class):
    labels = np.array(labels)
    train_labeled_idxs = []

    for i in range(9):
        idxs = np.where(labels == i)[0]
        train_labeled_idxs.extend(idxs[:n_labeled_per_class[i]])

    return train_labeled_idxs

def train_unlabeled_split(labels,k):
    labels = np.array(labels)
    train_unlabeled_idxs = []
    idxs=np.where(labels==9)[0]
    train_unlabeled_idxs.extend(idxs[:k])
    return train_unlabeled_idxs


'''
def label_preprocessing(labels):
  labels = labels.apply(lambda x: np.array([x]))
  labels=np.array(labels.tolist())
  if labels[0]!=[9]:
    labels=_to_one_hot(torch.as_tensor(np.int64(labels)))
  else:
    labels=unlabeled_to_one_hot(labels)
  print(labels)
  return labels

#def _to_one_hot(y, num_classes=args.num_class):
def _to_one_hot(y, num_classes=9):
  scatter_dim = len(y.size())
  y_tensor = y.view(*y.size(), -1)
  zeros = torch.zeros(*y.size(), num_classes, dtype=y.dtype)
  return zeros.scatter(scatter_dim, y_tensor, 1)

#def unlabeled_to_one_hot(labels,num_classes=args.num_class):
def unlabeled_to_one_hot(labels,num_classes=9):
  new_labels=[]
  for i in range(len(labels)):
    new_labels.append(np.array([-1 for i in range(num_classes)]))
  labels=torch.as_tensor(np.int64(new_labels))
  return labels
'''

def label_preprocessing(labels):
  labels = labels.apply(lambda x: np.array(x))
  labels=np.array(labels)

  labels=labels.astype(np.int64)
  return labels

def data_preprocessing(data):
  img=[]
  data=data.tolist()
  resized_images2= _resize2(data, 64, 64)
  count = (resized_images2 == 2).sum().item()
  resized_images2 = rgb_sw(resized_images2)
  data = torch.Tensor(np.transpose(resized_images2, (0, 3, 1, 2)))
  for i in data:
    img.append(TF.to_pil_image(i))

  return img



class TransformTwice:
    def __init__(self, transform, transform2):
        self.transform = transform
        self.transform2 = transform2

    def __call__(self, inp):
        out1 = self.transform(inp)
        out2 = self.transform2(inp)
        out3 = self.transform2(inp)
        return out1, out2, out3



def get_WM_data(N_SAMPLES_PER_CLASS,unlabeled_size,test_size):
  df=pd.read_pickle("LSWMD.pkl")
  WM_mean = (0.4914, 0.4822, 0.4465) # equals np.mean(train_set.train_data, axis=(0,1,2))/255
  WM_std = (0.2471, 0.2435, 0.2616) # equals np.std(train_set.train_data, axis=(0,1,2))/255
  transform_train = transforms.Compose([
        transforms.RandomCrop(64, padding=4),
        transforms.RandomHorizontalFlip(),
        transforms.ToTensor(),
        transforms.Normalize(WM_mean, WM_std)
    ])

  transform_val = transforms.Compose([
      transforms.ToTensor(),
      transforms.Normalize(WM_mean, WM_std)
  ])
  transform_strong = transforms.Compose([
      transforms.RandomCrop(64, padding=4),
      transforms.RandomHorizontalFlip(),
      transforms.ToTensor(),
      transforms.Normalize(WM_mean, WM_std)
  ])
  transform_strong.transforms.insert(0, RandAugment(3, 4))
  transform_strong.transforms.append(CutoutDefault(16))

  # 사이즈가 너무 커서
  df=df.iloc[:2000]
  for i in range(len(df)):
    if list(df.loc[i].failureType)==[]:
      df.loc[i,'failureType']=9
  df['failureNum'] = df.failureType
  mapping_type={'Center':0,'Donut':1,'Edge-Loc':2,'Edge-Ring':3,'Loc':4,'Random':5,'Scratch':6,'Near-full':7,'none':8}
  df=df.replace({'failureNum':mapping_type})

  labels = df['failureNum'].to_numpy()

  train_labeled_idxs=train_labeled_split(labels,N_SAMPLES_PER_CLASS)
  train_unlabeled_idxs=train_unlabeled_split(labels,unlabeled_size)
  test_list_per_class=[test_size for i in range(args.num_class)]
  test_idxs=test_split(labels,test_list_per_class)

  train_labeled_data=df.iloc[train_labeled_idxs].waferMap
  train_labeled_data_label=df.iloc[train_labeled_idxs].failureNum
  train_unlabeled_data=df.iloc[train_unlabeled_idxs].waferMap
  train_unlabeled_data_label=df.iloc[train_unlabeled_idxs].failureNum
  test_data=df.iloc[test_idxs].waferMap
  test_data_label=df.iloc[test_idxs].failureNum

  train_labeled_data_label_pre=label_preprocessing(train_labeled_data_label)
  train_unlabeled_data_label_pre=label_preprocessing(train_unlabeled_data_label)
  test_data_label_pre=label_preprocessing(test_data_label)

  train_labeled_data_pre=data_preprocessing(train_labeled_data)
  train_unlabeled_data_pre=data_preprocessing(train_unlabeled_data)
  test_data_pre=data_preprocessing(test_data)


  train_labeled_dataset=BasicDataset(train_labeled_data_pre,train_labeled_data_label_pre,transform=transform_train)
  train_unlabeled_dataset=BasicDataset(train_unlabeled_data_pre,train_unlabeled_data_label_pre,transform=TransformTwice(transform_train,transform_strong))
  test_dataset=BasicDataset(test_data_pre,test_data_label_pre,transform=transform_val)

  return train_labeled_dataset,train_unlabeled_dataset,test_dataset