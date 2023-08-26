# -*- coding: utf-8 -*-
"""train.ipynb

Automatically generated by Colaboratory.

Original file is located at
    https://colab.research.google.com/drive/1ch4OduN5N-3vDYHDpzoXkWavwMmY0Ze7
"""

import os
os.chdir("C:/Users/lcj/desktop/Fix")

import argparse
import numpy as np
from args import parse_args
import torch

from fixmatch import trains, make_imb_data,SemiLoss

from get_data import *
from fixmatch import *
from args import parse_args

import warnings
warnings.filterwarnings('ignore')

def main():
  global best_acc
  args = parse_args()
  state = {k: v for k, v in args._get_kwargs()}
  args.out = args.dataset + '@N_' + str(args.all) +"_"+ str(args.unlabeled_size)+ "_" + str(args.epochs)
  if not os.path.isdir(args.out):
    mkdir_p(args.out)
  else:
    while os.path.isdir(args.out):
      args.out=args.out+'_1'
    mkdir_p(args.out)

  print(args.out)

  N_SAMPLES_PER_CLASS = make_imb_data(args.num_max, args.num_class, args.imb_ratio_l)

  N_SAMPLES_PER_CLASS_T = torch.Tensor(N_SAMPLES_PER_CLASS)


  train_labeled_set, train_unlabeled_set, test_set = get_WM_data(args,N_SAMPLES_PER_CLASS)

  print(11111111111111111111111111111111111111111111111111111111111111)
  return
  labeled_trainloader = data.DataLoader(train_labeled_set, batch_size=args.batch_size, shuffle=True, num_workers=2,
                                            drop_last=True)

  unlabeled_trainloader = data.DataLoader(train_unlabeled_set, batch_size=args.batch_size, shuffle=True, num_workers=2,
                                              drop_last=True)

  test_loader = data.DataLoader(test_set, batch_size=args.batch_size, shuffle=False, num_workers=2)

  # Model
  print("==> creating WRN-28-2")
  def create_model(ema=False):
    model = models.WRN(2, args.num_class)
    model = model.cuda()
    if ema:
      for param in model.parameters():
        param.detach_()
    return model

  os.environ['CUDA_VISIBLE_DEVICES'] = args.gpu
  use_cuda = torch.cuda.is_available()
  model=create_model()
  ema_model=create_model(ema=True)

  cudnn.benchmark=True
  print('    Total params: %.2fM' % (sum(p.numel() for p in model.parameters())/1000000.0))

  train_criterion=SemiLoss()
  criterion = nn.CrossEntropyLoss()
  optimizer=optim.Adam(model.parameters(),lr=args.lr)
  ema_optimizer=WeightEMA(model,ema_model,lr=args.lr,alpha=args.ema_decay)
  start_epoch=0

  title=args.dataset

  if args.resume:
    print('==> Resuming from checkpoing..')
    assert os.path.isfile(args.resume), 'Error: no checkpoing directory found'
    args.out=os.path.dirname(args.resume)
    checkpoint=torch.load(args.resume)
    start_epoch=checkpoint['epoch']
    model.load_state_dict(checkpoint['state_dict'])
    ema_moodel.load_state_dict(checkpoint['ema_state_dict'])
    optimizer.load_state_dict(checkpoint['optimizer'])
    logger=Logger(os.path.join(args.out,'log.txt'),title=title,resume=True)
  else:
    logger=Logger(os.path.join(args.out,'log.txt'),title=title)
    logger.set_names(['Train_Loss', 'Train Loss X','Train Loss U','Test Loss','Test Acc.','Test GM.'])

  test_accs=[]
  test_gms=[]

  emp_distb_u=torch.ones(args.num_class)/args.num_class
  pseudo_orig=torch.ones(len(train_unlabeled_set.data),args.num_class)/args.num_class
  pseudo_refine=torch.ones(len(train_unlabeled_set.data),args.num_class)/args.num_class

  #MAIN FUNCTION
  for epoch in range(start_epoch,args.epochs):
    print('\n EPOCH : [%d | %d] LR: %f'%(epoch+1,args.epochs,state['lr']))
    #Use the estimated distribution of unlabeled data
    if args.est:
      est_name = './estimation/WM@_{}_estim.npy'.format(args.imb_ratio_l, args.imb_ratio_u)
      est_disb=np.load(est_name)
      target_disb=len(train_unlabeled_set.data)*torch.Tensor(est_disb)/np.sum(est_disb)
    else:
      target_disb = N_SAMPLES_PER_CLASS_T * len(train_unlabeled_set.data) / sum(N_SAMPLES_PER_CLASS)

    train_loss, train_loss_x, train_loss_u, emp_distb_u, pseudo_orig, pseudo_refine = trains(args, labeled_trainloader,
                                                                                             unlabeled_trainloader,
                                                                                             model, optimizer,
                                                                                             ema_optimizer,
                                                                                             train_criterion,
                                                                                             epoch, use_cuda,
                                                                                             target_disb, emp_distb_u,
                                                                                             pseudo_orig, pseudo_refine)

    # Evaluation part
    test_loss, test_acc, test_cls, test_gm = validate(test_loader, ema_model, criterion, use_cuda,
                                                      mode='Test Stats', num_class=args.num_class)

    # Append logger file
    logger.append([train_loss, train_loss_x, train_loss_u, test_loss, test_acc, test_gm])

    # Save models
    save_checkpoint({
        'epoch': epoch + 1,
        'state_dict': model.state_dict(),
        'ema_state_dict': ema_model.state_dict(),
        'optimizer': optimizer.state_dict(),
    }, epoch + 1, args.out)
    test_accs.append(test_acc)
    test_gms.append(test_gm)

  logger.close()

  # Print the final results
  print('Mean bAcc:')
  print(np.mean(test_accs[-20:]))

  print('Mean GM:')
  print(np.mean(test_gms[-20:]))

  print('Name of saved folder:')
  print(args.out)

if __name__ == '__main__':
  main()