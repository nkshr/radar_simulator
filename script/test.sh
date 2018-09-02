#!/bin/sh

rsim module rsim_test test
rsim memory bool status
rsim connect test mb status
rsim run
