# ran in linux with python 3.8.5, but it can be run on any python3 virtual env
virtualenv sandbox

pip install -r requirements.txt

cd sandbox/src

django-admin startproject app