#!/bin/bash

kill `ps ax |grep chirp.py |grep -v grep |awk '{print $1}'|xargs`

