# -*- coding: utf-8 -*-
"""args.ipynb

Automatically generated by Colaboratory.

Original file is located at
    https://colab.research.google.com/drive/1ssfgmwCSMBzhUM4kXtE73TvC_1m2oFk8
"""

import argparse

def parse_args():
    parser = argparse.ArgumentParser(description='WM-811k Fixmatch Training')
    parser.add_argument('--dataset', default='WM-811K dataset', help='WM 811k dataset')
    parser.add_argument('--resume', default='', type=str, metavar='PATH', help='path to latest checkpoint (default: none)')
    parser.add_argument('--out', default='result', help='Directory to output the result')
    parser.add_argument('--unlabeled_size', type=int, default=40000, help='unlabeled data size')
    parser.add_argument('--none_size', type=int, default=10000, help='none pattern data size')
    parser.add_argument('--all', type=bool, default=True, help='all data used or not')
    parser.add_argument('--test_size', type=int, default=10000, help='test size')
    parser.add_argument('--batch_size', type=int, default=32, help='test size')
    parser.add_argument('--val-iteration', type=int, default=1, help='Frequency for the evaluation')
    parser.add_argument('--num_max', type=int, default=15000, help='Number of samples in the maximal class')
    parser.add_argument('--imb_ratio_l', type=int, default=2.0, help='Imbalance ratio for labeled data')
    parser.add_argument('--ratio', type=float, default=2.0, help='Relative size between labeled and unlabeled data')
    parser.add_argument('--num_class', type=int, default=9, help='Numbe of class of dataset')
    parser.add_argument('--est', action='store_true', help='Using estimated distribution for unlabeled dataset')
    parser.add_argument('--darp', action='store_true', help='Applying DARP')
    parser.add_argument('--lr', '--learning-rate', default=0.002, type=float, metavar='LR', help='initial learning rate')
    parser.add_argument('--ema-decay', default=0.999, type=float)
    parser.add_argument('--epochs', default=1, type=int, metavar='N', help='number of total epochs to run')
    parser.add_argument('--gpu', default='0', type=str, help='id(s) for CUDA_VISIBLE_DEVICES')
    # Hyperparameters for FixMatch
    parser.add_argument('--tau', default=0.95, type=float, help='hyper-parameter for pseudo-label of FixMatch')

    return parser.parse_args()