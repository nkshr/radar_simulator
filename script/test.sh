#!/bin/sh

rsim module rsim_test test0
rsim set test0 cf 1

rsim module rsim_test test1
rsim set test1 cf 1

rsim module rsim_test test2
rsim set test2 cf 1
rsim run
